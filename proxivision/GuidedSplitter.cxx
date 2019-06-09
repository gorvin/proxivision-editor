#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "FXElement.h"
#include "FXHash.h"
#include "FXAutoThreadStorageKey.h"
#include "FXThread.h"
#include "FXStream.h"
#include "FXString.h"
#include "FXStringDictionary.h"
#include "FXSize.h"
#include "FXPoint.h"
#include "FXRectangle.h"
#include "FXRegistry.h"
#include "FXMutex.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXApp.h"
#include "FXDCWindow.h"
#include "GuidedSplitter.h"


/*
  Notes:
  - Reversed split option starts parting out from right [bottom].
  - Minimal partition of a split is 0 pixels.
  - Minimum width of horizontal splitter is determined by sum of
    all visible children's default widths, and height by maximum
    of all visible children's default heights; analogous for vertical
    splitter of course.
    [This can be done because the default width (height) does not depend
    on the widget's current user-specified actual width (height)].
  - For convenience, width/height of <=1 replaced by minimum width/height,
    but only if both width and height of child is <=1 at the same time.
  - Should we send SEL_CHANGED and SEL_COMMAND also when splitter arrangement
    was changed programmatically?  When bar size changed?
  - GuidedSplitter interprets layout hints on some of the children so that certain
    children have a minimum width (height) constraint on them.
    Note that this rule must have one exception: the last (or first if the option
    SPLITTER_REVERSED has been passed) must necessarily be allowed to be
    any size.
  - If we're just re-sizing a split, do we need to incur a GUI-Update?
  - Do we need to somehow insure that the sum of the sizes of all
    partitions never exceeds the size of the splitter itself?
  - Should we drop default parameters on 2nd constructor so as to
    prevent conflicts on compilers where NULL is defined as 0 instead
    of __null?
*/

// Splitter styles
#define SPLITTER_MASK     (SPLITTER_REVERSED|SPLITTER_VERTICAL|SPLITTER_TRACKING)

/*******************************************************************************/

// Map
FXDEFMAP(GuidedSplitter) GuidedSplitterMap[]={
  FXMAPFUNC(FX::SEL_MOTION,0,GuidedSplitter::onMotion),
  FXMAPFUNC(FX::SEL_LEFTBUTTONPRESS,0,GuidedSplitter::onLeftBtnPress),
  FXMAPFUNC(FX::SEL_LEFTBUTTONRELEASE,0,GuidedSplitter::onLeftBtnRelease),
  FXMAPFUNC(FX::SEL_FOCUS_NEXT,0,GuidedSplitter::onFocusNext),
  FXMAPFUNC(FX::SEL_FOCUS_PREV,0,GuidedSplitter::onFocusPrev),
  FXMAPFUNC(FX::SEL_FOCUS_UP,0,GuidedSplitter::onFocusUp),
  FXMAPFUNC(FX::SEL_FOCUS_DOWN,0,GuidedSplitter::onFocusDown),
  FXMAPFUNC(FX::SEL_FOCUS_LEFT,0,GuidedSplitter::onFocusLeft),
  FXMAPFUNC(FX::SEL_FOCUS_RIGHT,0,GuidedSplitter::onFocusRight),
  };


// Object implementation
FXIMPLEMENT(GuidedSplitter,FX::FXComposite,GuidedSplitterMap,ARRAYNUMBER(GuidedSplitterMap))


// Make a splitter
GuidedSplitter::GuidedSplitter(){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;
  stretcher_explicit=0;
  }


// Make a splitter; it has no interior padding, and no borders
GuidedSplitter::GuidedSplitter(FX::FXComposite* p,FX::FXuint opts,FX::FXint x,FX::FXint y,FX::FXint w,FX::FXint h):
  FX::FXComposite(p,opts,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  defaultCursor=(options&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(FX::DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(FX::DEF_HSPLIT_CURSOR);
  dragCursor=defaultCursor;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;
  stretcher_explicit=0;
  }


// Make a splitter; it has no interior padding, and no borders
GuidedSplitter::GuidedSplitter(FX::FXComposite* p,FX::FXObject* tgt,FX::FXSelector sel,FX::FXuint opts,FX::FXint x,FX::FXint y,FX::FXint w,FX::FXint h):
  FX::FXComposite(p,opts,x,y,w,h){
  flags|=FLAG_ENABLED|FLAG_SHOWN;
  defaultCursor=(options&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(FX::DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(FX::DEF_HSPLIT_CURSOR);
  dragCursor=defaultCursor;
  target=tgt;
  message=sel;
  window=NULL;
  split=0;
  offset=0;
  barsize=4;
  stretcher_explicit=0;
  }


// Get default width
FX::FXint GuidedSplitter::getDefaultWidth(){
  register FX::FXWindow* child;
  register FX::FXint wmax,w,numc;
  wmax=numc=0;
  if(options&SPLITTER_VERTICAL){
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        w=child->getDefaultWidth();
        if(wmax<w) wmax=w;
        }
      }
    }
  else{
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        wmax+=child->getDefaultWidth();
        numc++;
        }
      }
    if(numc>1) wmax+=(numc-1)*barsize;
    }
  return wmax;
  }


// Get default height
FX::FXint GuidedSplitter::getDefaultHeight(){
  register FX::FXWindow* child;
  register FX::FXint hmax,h,numc;
  hmax=numc=0;
  if(options&SPLITTER_VERTICAL){
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        hmax+=child->getDefaultHeight();
        numc++;
        }
      }
    if(numc>1) hmax+=(numc-1)*barsize;
    }
  else{
    for(child=getFirst(); child; child=child->getNext()){
      if(child->shown()){
        h=child->getDefaultHeight();
        if(hmax<h) hmax=h;
        }
      }
    }
  return hmax;
  }

FX::FXWindow* GuidedSplitter::getFirstShown(void)
{
  if(stretcher_explicit->isChildOf(this) && stretcher_explicit->shown()) return stretcher_explicit;
  FX::FXWindow* w=getFirst(); while(w && !w->shown()) w=w->getNext();
  return w;
}

FX::FXWindow* GuidedSplitter::getLastShown(void)
{
  if(stretcher_explicit->isChildOf(this) && stretcher_explicit->shown()) return stretcher_explicit;
  FX::FXWindow* w=getLast(); while(w && !w->shown()) w=w->getPrev();
  return w;
}

FX::FXint GuidedSplitter::childGoodWidth(FX::FXWindow* child)
{
  static FX::FXuint const mask=FX::LAYOUT_FILL_X|FX::LAYOUT_FIX_WIDTH;
  FX::FXint w=child->getWidth();
  if((child->getLayoutHints()&mask)==mask && child->getDefaultWidth()>w) w=child->getDefaultWidth();
  if(w<=1 && child->getHeight()<=1) w=child->getDefaultWidth();
  return w;
}

FX::FXint GuidedSplitter::childGoodHeight(FX::FXWindow* child)
{
  static FX::FXuint const mask=FX::LAYOUT_FILL_Y|FX::LAYOUT_FIX_HEIGHT;
  FX::FXint h=child->getHeight();
  if((child->getLayoutHints()&mask)==mask && child->getDefaultHeight()>h) h=child->getDefaultHeight();
  if(h<=1 && child->getWidth()<=1) h=child->getDefaultHeight();
  return h;
}

// Recompute layout
void GuidedSplitter::layout(){
  FX::FXWindow *child,*stretcher;
  FX::FXint pos;
  if(options&SPLITTER_VERTICAL){          // Vertical
    FX::FXint h;
    if(options&SPLITTER_REVERSED){
      FX::FXint last_pos;
      pos=height;
      stretcher = getFirstShown();
      bool pass1=true;
      child=getLast();
      while(child){
        if(!child->shown()) continue;
        h=childGoodHeight(child);
        if(pass1) {
          if(child==stretcher) {
            pass1 = false; last_pos=pos; pos=0; child=getFirst(); continue;
          }
          child->position(0,pos-h,width,h);
          pos-=h+barsize;
          child=child->getPrev();
        } else {
          if(child==stretcher){ h=last_pos-pos; if(h<0) h=0; }
          child->position(0,pos,width,h);
          pos+=h+barsize;
          if(child==stretcher) break;
          child=child->getNext();
        }
      }
    } else {
      FX::FXint last_pos;
      pos=0;
      stretcher = getLastShown();
      bool pass1=true;
      child=getFirst();
      while(child){
        if(!child->shown()) continue;
        h=childGoodHeight(child);
        if(pass1) {
          if(child==stretcher) {
            pass1 = false; last_pos=pos; pos=height; child=getLast(); continue;
          }
          child->position(0,pos,width,h);
          pos+=h+barsize;
          child=child->getNext();
        } else {
          if(child==stretcher){ h=pos-last_pos; if(h<0) h=0; }
          child->position(0,pos-h,width,h);
          pos-=h+barsize;
          if(child==stretcher) break;
          child=child->getPrev();
        }
      }
    }
  } else {                                   // Horizontal
    FX::FXint w;
    if(options&SPLITTER_REVERSED){
      FX::FXint last_pos;
      pos=width;
      stretcher = getFirstShown();
      bool pass1=true;
      child=getLast();
      while(child){
        if(!child->shown()) continue;
        w=childGoodWidth(child);
        if(pass1) {
          if(child==stretcher) {
            pass1 = false; last_pos=pos; pos=0; child=getFirst(); continue;
          }
          child->position(pos-w,0,w,height);
          pos-=w+barsize;
          if(child==stretcher) break;
          child=child->getPrev();
        } else {
          if(child==stretcher){ w=last_pos-pos; if(w<0) w=0; }
          child->position(pos,0,w,height);
          pos+=w+barsize;
          child=child->getNext();
        }
      }
    } else {
      FX::FXint last_pos;
      pos=0;
      stretcher = getLastShown();
      bool pass1=true;
      child=getFirst();
      while(child){
        if(!child->shown()) continue;
        w=childGoodWidth(child);
        if(pass1) {
          if(child==stretcher) {
            pass1 = false; last_pos=pos; pos=width; child=getLast(); continue;
          }
          child->position(pos,0,w,height);
          pos+=w+barsize;
          child=child->getNext();
        } else {
          if(child==stretcher){ w=pos-last_pos; if(w<0) w=0; }
          child->position(pos-w,0,w,height);
          pos-=w+barsize;
          if(child==stretcher) break;
          child=child->getPrev();
        }
      }
    }
  }
  flags&=~FLAG_DIRTY;
  }


// Adjust horizontal layout
void GuidedSplitter::adjustHLayout(){
  FXASSERT(window);
  if(options&SPLITTER_REVERSED){
    FX::FXint pos=window->getX()+window->getWidth();
    FX::FXuint wnextdiff=window->getWidth()-pos+split;
    window->position(split,0,pos-split,height);
    FX::FXWindow *child = window->getPrev();
    if(child) {
        child->position(child->getX(), 0, child->getWidth()+wnextdiff, height);
      }
    }
  else{
    FX::FXint pos=window->getX();
    FX::FXuint wprevdiff=window->getWidth()-split+pos;
    window->position(pos,0,split-pos,height);
    FX::FXWindow *child = window->getNext();
    if(child) {
        child->position(child->getX()-wprevdiff, 0, child->getWidth()+wprevdiff, height);
      }
    }
  }


// Adjust vertical layout
void GuidedSplitter::adjustVLayout(){
  FXASSERT(window);
  if(options&SPLITTER_REVERSED){
    FX::FXint pos=window->getY()+window->getHeight();
    FX::FXuint hnextdiff=window->getHeight()-pos+split;
    window->position(0,split,width,pos-split);
    FX::FXWindow *child = window->getPrev();
    if(child) {
        child->position(0, child->getY(), width, child->getHeight()+hnextdiff);
      }
    }
  else{
    FX::FXint pos=window->getY();
    FX::FXuint hnextdiff=window->getHeight()-split+pos;
    window->position(0,pos,width,split-pos);
    FX::FXWindow *child = window->getNext();
    if(child) {
        child->position(0, child->getY()-hnextdiff, width, child->getHeight()+hnextdiff);
      }
    }
  }


// Find child just before split
FX::FXWindow* GuidedSplitter::findHSplit(FX::FXint pos){
  register FX::FXWindow* child=getFirst();
  if(options&SPLITTER_REVERSED){
    while(child){
      if(child->shown()){
        if(child->getX()-barsize<=pos && pos<child->getX()) return child;
        }
      child=child->getNext();
      }
    }
  else{
    while(child){
      if(child->shown()){
        if(child->getX()+child->getWidth()<=pos && pos<child->getX()+child->getWidth()+barsize) return child;
        }
      child=child->getNext();
      }
    }
  return NULL;
  }


// Find child just before split
FX::FXWindow* GuidedSplitter::findVSplit(FX::FXint pos){
  register FX::FXWindow* child=getFirst();
  if(options&SPLITTER_REVERSED){
    while(child){
      if(child->shown()){
        if(child->getY()-barsize<=pos && pos<child->getY()) return child;
        }
      child=child->getNext();
      }
    }
  else{
    while(child){
      if(child->shown()){
        if(child->getY()+child->getHeight()<=pos && pos<child->getY()+child->getHeight()+barsize) return child;
        }
      child=child->getNext();
      }
    }
  return NULL;
  }


// Move the horizontal split intelligently
void GuidedSplitter::moveHSplit(FX::FXint pos){
  register FX::FXint smin,smax;
  register FX::FXuint hints;
  FXASSERT(window);
  hints=window->getLayoutHints();
  if(options&SPLITTER_REVERSED){
    smin=barsize;
    smax=window->getX()+window->getWidth();
    if((hints&FX::LAYOUT_FILL_X)&&(hints&FX::LAYOUT_FIX_WIDTH)) smax-=window->getDefaultWidth();
    }
  else{
    smin=window->getX();
    smax=width-barsize;
    if((hints&FX::LAYOUT_FILL_X)&&(hints&FX::LAYOUT_FIX_WIDTH)) smin+=window->getDefaultWidth();
    }
  split=pos;
  if(split<smin) split=smin;
  if(split>smax) split=smax;
  }


// Move the vertical split intelligently
void GuidedSplitter::moveVSplit(FX::FXint pos){
  register FX::FXint smin,smax;
  register FX::FXuint hints;
  FXASSERT(window);
  hints=window->getLayoutHints();
  if(options&SPLITTER_REVERSED){
    smin=barsize;
    smax=window->getY()+window->getHeight();
    if((hints&FX::LAYOUT_FILL_Y)&&(hints&FX::LAYOUT_FIX_HEIGHT)) smax-=window->getDefaultHeight();
    }
  else{
    smin=window->getY();
    smax=height-barsize;
    if((hints&FX::LAYOUT_FILL_Y)&&(hints&FX::LAYOUT_FIX_HEIGHT)) smin+=window->getDefaultHeight();
    }
  split=pos;
  if(split<smin) split=smin;
  if(split>smax) split=smax;
  }


// Button being pressed
long GuidedSplitter::onLeftBtnPress(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXEvent* ev=(FX::FXEvent*)ptr;
  if(isEnabled()){
    grab();
    if(target && target->tryHandle(this,FXSEL(FX::SEL_LEFTBUTTONPRESS,message),ptr)) return 1;
    if(options&SPLITTER_VERTICAL){
      window=findVSplit(ev->win_y);
      if(window){
        if(options&SPLITTER_REVERSED)
          split=window->getY();
        else
          split=window->getY()+window->getHeight();
        offset=ev->win_y-split;
        if(!(options&SPLITTER_TRACKING)){
          drawVSplit(split);
          }
        flags|=FLAG_PRESSED;
        flags&=~FLAG_UPDATE;
        }
      }
    else{
      window=findHSplit(ev->win_x);
      if(window){
        if(options&SPLITTER_REVERSED)
          split=window->getX();
        else
          split=window->getX()+window->getWidth();
        offset=ev->win_x-split;
        if(!(options&SPLITTER_TRACKING)){
          drawHSplit(split);
          }
        flags|=FLAG_PRESSED;
        flags&=~FLAG_UPDATE;
        }
      }
    return 1;
    }
  return 0;
  }


// Button being released
long GuidedSplitter::onLeftBtnRelease(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXuint flgs=flags;
  if(isEnabled()){
    ungrab();
    flags|=FLAG_UPDATE;
    flags&=~FLAG_CHANGED;
    flags&=~FLAG_PRESSED;
    if(target && target->tryHandle(this,FXSEL(FX::SEL_LEFTBUTTONRELEASE,message),ptr)) return 1;
    if(flgs&FLAG_PRESSED){
      if(!(options&SPLITTER_TRACKING)){
        if(options&SPLITTER_VERTICAL){
          drawVSplit(split);
          adjustVLayout();
          }
        else{
          drawHSplit(split);
          adjustHLayout();
          }
        if(flgs&FLAG_CHANGED){
          if(target) target->tryHandle(this,FXSEL(FX::SEL_CHANGED,message),window);
          }
        }
      if(flgs&FLAG_CHANGED){
        if(target) target->tryHandle(this,FXSEL(FX::SEL_COMMAND,message),window);
        }
      }
    return 1;
    }
  return 0;
  }


// Button being released
long GuidedSplitter::onMotion(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXEvent* ev=(FX::FXEvent*)ptr;
  FX::FXint oldsplit;
  if(flags&FLAG_PRESSED){
    oldsplit=split;
    if(options&SPLITTER_VERTICAL){
      moveVSplit(ev->win_y-offset);
      if(split!=oldsplit){
        if(!(options&SPLITTER_TRACKING)){
          drawVSplit(oldsplit);
          drawVSplit(split);
          }
        else{
          adjustVLayout();
          if(target) target->tryHandle(this,FXSEL(FX::SEL_CHANGED,message),window);
          }
        flags|=FLAG_CHANGED;
        }
      }
    else{
      moveHSplit(ev->win_x-offset);
      if(split!=oldsplit){
        if(!(options&SPLITTER_TRACKING)){
          drawHSplit(oldsplit);
          drawHSplit(split);
          }
        else{
          adjustHLayout();
          if(target) target->tryHandle(this,FXSEL(FX::SEL_CHANGED,message),window);
          }
        flags|=FLAG_CHANGED;
        }
      }
    return 1;
    }
  return 0;
  }


// Focus moved to next
long GuidedSplitter::onFocusNext(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  return (options&SPLITTER_VERTICAL) ? onFocusDown(sender,sel,ptr) : onFocusRight(sender,sel,ptr);
  }


// Focus moved to previous
long GuidedSplitter::onFocusPrev(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  return (options&SPLITTER_VERTICAL) ? onFocusUp(sender,sel,ptr) : onFocusLeft(sender,sel,ptr);
  }


// Focus moved up
long GuidedSplitter::onFocusUp(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXWindow *child=getLast();
  if(getFocus()){
    if(getFocus()->handle(this,FXSEL(FX::SEL_FOCUS_UP,0),ptr)) return 1;
    if(!(options&SPLITTER_VERTICAL)) return 0;
    child=getFocus()->getPrev();
    }
  while(child){
    if(child->shown()){
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr)) return 1;
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_UP,0),ptr)) return 1;
      }
    child=child->getPrev();
    }
  return 0;
  }


// Focus moved down
long GuidedSplitter::onFocusDown(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXWindow *child=getFirst();
  if(getFocus()){
    if(getFocus()->handle(this,FXSEL(FX::SEL_FOCUS_DOWN,0),ptr)) return 1;
    if(!(options&SPLITTER_VERTICAL)) return 0;
    child=getFocus()->getNext();
    }
  while(child){
    if(child->shown()){
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr)) return 1;
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_DOWN,0),ptr)) return 1;
      }
    child=child->getNext();
    }
  return 0;
  }


// Focus moved to left
long GuidedSplitter::onFocusLeft(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXWindow *child=getLast();
  if(getFocus()){
    if(getFocus()->handle(this,FXSEL(FX::SEL_FOCUS_LEFT,0),ptr)) return 1;
    if(options&SPLITTER_VERTICAL) return 0;
    child=getFocus()->getPrev();
    }
  while(child){
    if(child->shown()){
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr)) return 1;
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_LEFT,0),ptr)) return 1;
      }
    child=child->getPrev();
    }
  return 0;
  }


// Focus moved to right
long GuidedSplitter::onFocusRight(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXWindow *child=getFirst();
  if(getFocus()){
    if(getFocus()->handle(this,FXSEL(FX::SEL_FOCUS_RIGHT,0),ptr)) return 1;
    if(options&SPLITTER_VERTICAL) return 0;
    child=getFocus()->getNext();
    }
  while(child){
    if(child->shown()){
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr)) return 1;
      if(child->handle(this,FXSEL(FX::SEL_FOCUS_RIGHT,0),ptr)) return 1;
      }
    child=child->getNext();
    }
  return 0;
  }


// Draw the horizontal split
void GuidedSplitter::drawHSplit(FX::FXint pos){
  FX::FXDCWindow dc(this);
  dc.clipChildren(false);
  dc.setFunction(FX::BLT_NOT_DST);
  dc.fillRectangle(pos,0,barsize,height);
  }


// Draw the vertical split
void GuidedSplitter::drawVSplit(FX::FXint pos){
  FX::FXDCWindow dc(this);
  dc.clipChildren(false);
  dc.setFunction(FX::BLT_NOT_DST);
  dc.fillRectangle(0,pos,width,barsize);
  }


// Return size of the panel at index
FX::FXint GuidedSplitter::getSplit(FX::FXint index) const {
  FX::FXWindow *win=childAtIndex(index);
  if(win){
    if(options&SPLITTER_VERTICAL){
      return win->getHeight();
      }
    else{
      return win->getWidth();
      }
    }
  return 0;
  }


// Change the size of panel at the given index
void GuidedSplitter::setSplit(FX::FXint index,FX::FXint size){
  FX::FXWindow *win=childAtIndex(index);
  if(win){
    if(options&SPLITTER_VERTICAL){
      win->setHeight(size);
      }
    else{
      win->setWidth(size);
      }
    win->recalc();
    }
  }


// Return splitter style
FX::FXuint GuidedSplitter::getSplitterStyle() const {
  return (options&SPLITTER_MASK);
  }


// Set horizontal or vertical
void GuidedSplitter::setSplitterStyle(FX::FXuint style){
  FX::FXuint opts=(options&~SPLITTER_MASK) | (style&SPLITTER_MASK);
  if(options!=opts){

    // Split direction changed; need re-layout of everything
    if((opts&SPLITTER_VERTICAL)!=(options&SPLITTER_VERTICAL)){
      for(FX::FXWindow *child=getFirst(); child; child=child->getNext()){
        if(child->shown()){
          child->setWidth(0);
          child->setHeight(0);
          }
        }
      setDefaultCursor((opts&SPLITTER_VERTICAL) ? getApp()->getDefaultCursor(FX::DEF_VSPLIT_CURSOR) : getApp()->getDefaultCursor(FX::DEF_HSPLIT_CURSOR));
      setDragCursor(getDefaultCursor());
      recalc();
      }

    // Split mode reversal; re-layout first and last only
    if((opts&SPLITTER_REVERSED)!=(options&SPLITTER_REVERSED)){
      if(getFirst()){
        getFirst()->setWidth(0);
        getFirst()->setHeight(0);
        getLast()->setWidth(0);
        getLast()->setHeight(0);
        }
      recalc();
      }
    options=opts;
    }
  }


// Change bar size
void GuidedSplitter::setBarSize(FX::FXint bs){
  if(bs!=barsize){
    barsize=bs;
    recalc();
    }
  }

void GuidedSplitter::SetStretcher(FX::FXWindow* w)
{
  if(!w) {
    stretcher_explicit = 0;
    return;
  }

  for(FX::FXWindow* stretcher=getFirst(); stretcher; stretcher=stretcher->getNext()) {
    if(stretcher == w) {
      stretcher_explicit = w;
      return;
    }
  }
  throw "Given window is not child of this GuidedSplitter";
}


// Save object to stream
void GuidedSplitter::save(FX::FXStream& store) const {
  FX::FXComposite::save(store);
  store << barsize;
  }


// Load object from stream
void GuidedSplitter::load(FX::FXStream& store){
  FX::FXComposite::load(store);
  store >> barsize;
  }


// Zap it
GuidedSplitter::~GuidedSplitter(){
  window=(FX::FXWindow*)-1L;
  }
