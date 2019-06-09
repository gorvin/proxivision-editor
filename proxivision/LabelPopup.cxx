#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
#include "fxkeys.h"
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
#include "FXAccelTable.h"
#include "FXMutex.h"
#include "FXEvent.h"
#include "FXWindow.h"
#include "FXApp.h"
#include "FXDCWindow.h"
#include "FXFont.h"
#include "FXIcon.h"
#include "LabelPopup.h"
#include "FXMenuPane.h"
#include "FXObjectList.h"
#include "FXList.h"
#define MENUBUTTON_MASK (MENUBUTTON_AUTOGRAY|MENUBUTTON_AUTOHIDE|MENUBUTTON_TOOLBAR)

/*******************************************************************************/

// Map
FXDEFMAP(LabelPopup) LabelPopupMap[]={
  FXMAPFUNC(FX::SEL_UPDATE,0,LabelPopup::onUpdate),
  FXMAPFUNC(FX::SEL_ENTER,0,LabelPopup::onEnter),
  FXMAPFUNC(FX::SEL_LEAVE,0,LabelPopup::onLeave),
  FXMAPFUNC(FX::SEL_MOTION,0,LabelPopup::onMotion),
  FXMAPFUNC(FX::SEL_FOCUSIN,0,LabelPopup::onFocusIn),
  FXMAPFUNC(FX::SEL_FOCUSOUT,0,LabelPopup::onFocusOut),
  FXMAPFUNC(FX::SEL_UNGRABBED,0,LabelPopup::onUngrabbed),
  FXMAPFUNC(FX::SEL_LEFTBUTTONPRESS,0,LabelPopup::onLeftBtnPress),
  FXMAPFUNC(FX::SEL_LEFTBUTTONRELEASE,0,LabelPopup::onLeftBtnRelease),
  FXMAPFUNC(FX::SEL_KEYPRESS,0,LabelPopup::onKeyPress),
  FXMAPFUNC(FX::SEL_KEYRELEASE,0,LabelPopup::onKeyRelease),
  FXMAPFUNC(FX::SEL_KEYPRESS,FX::FXWindow::ID_HOTKEY,LabelPopup::onHotKeyPress),
  FXMAPFUNC(FX::SEL_KEYRELEASE,FX::FXWindow::ID_HOTKEY,LabelPopup::onHotKeyRelease),
  FXMAPFUNC(FX::SEL_COMMAND,FX::FXWindow::ID_POST,LabelPopup::onCmdPost),
  FXMAPFUNC(FX::SEL_COMMAND,FX::FXWindow::ID_UNPOST,LabelPopup::onCmdUnpost),
  FXMAPFUNC(FX::SEL_UPDATE, LabelPopup::ID_LIST, LabelPopup::onListUpdate),
  FXMAPFUNC(FX::SEL_CLICKED, LabelPopup::ID_LIST, LabelPopup::onListClicked),
  FXMAPFUNC(FX::SEL_COMMAND, LabelPopup::ID_LIST, LabelPopup::onListClicked),
  FXMAPFUNC(FX::SEL_CHANGED, LabelPopup::ID_LIST, LabelPopup::onListChanged),
  };


// Object implementation
FXIMPLEMENT(LabelPopup,FX::FXLabel,LabelPopupMap,ARRAYNUMBER(LabelPopupMap))


// Deserialization
LabelPopup::LabelPopup(){
  list=(FX::FXList*)-1L;
  pane=(FX::FXPopup*)-1L;
  offsetx=0;
  offsety=0;
  state=false;
  }


// Make a check button
LabelPopup::LabelPopup(FX::FXComposite* p, const FX::FXString& text, FX::FXIcon* ic,
 FX::FXuint opts, FX::FXint x, FX::FXint y, FX::FXint w, FX::FXint h,
 FX::FXint pl, FX::FXint pr, FX::FXint pt, FX::FXint pb):
  FX::FXLabel(p,text,ic,opts,x,y,w,h,pl,pr,pt,pb){
  pane=new FX::FXPopup(this, FX::FRAME_LINE);
  list=new FX::FXList(pane, this, LabelPopup::ID_LIST, FX::LIST_BROWSESELECT|FX::LIST_AUTOSELECT|
       FX::LAYOUT_FILL|FX::SCROLLERS_TRACK|FX::VSCROLLING_OFF|FX::HSCROLLING_OFF);
  offsetx=0;
  offsety=0;
  state=false;
  }


// Create window
void LabelPopup::create(){
  FX::FXLabel::create();
  if(pane) pane->create();
  }


// Detach window
void LabelPopup::detach(){
  FX::FXLabel::detach();
  if(pane) pane->detach();
  }


// If window can have focus
bool LabelPopup::canFocus() const { return true; }


// Implement auto-hide or auto-gray modes
long LabelPopup::onUpdate(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  if(!FX::FXLabel::onUpdate(sender,sel,ptr)){
    if(options&MENUBUTTON_AUTOHIDE){if(shown()){hide();recalc();}}
    if(options&MENUBUTTON_AUTOGRAY){disable();}
    }
  return 1;
  }


// Gained focus
long LabelPopup::onFocusIn(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXLabel::onFocusIn(sender,sel,ptr);
  update(border,border,width-(border<<1),height-(border<<1));
  return 1;
  }


// Lost focus
long LabelPopup::onFocusOut(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXLabel::onFocusOut(sender,sel,ptr);
  update(border,border,width-(border<<1),height-(border<<1));
  return 1;
  }


// Inside the button
long LabelPopup::onEnter(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXLabel::onEnter(sender,sel,ptr);
  if(isEnabled()){
    if(options&MENUBUTTON_TOOLBAR) update();
    }
  return 1;
  }


// Outside the button
long LabelPopup::onLeave(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXLabel::onLeave(sender,sel,ptr);
  if(isEnabled()){
    if(options&MENUBUTTON_TOOLBAR) update();
    }
  return 1;
  }


// Pressed left button
long LabelPopup::onLeftBtnPress(FX::FXObject*,FX::FXSelector,void* ptr){
  flags&=~FLAG_TIP;
  handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr);
  if(isEnabled()){
    if(target && target->tryHandle(this,FXSEL(FX::SEL_LEFTBUTTONPRESS,message),ptr)) return 1;
    if(isEnabled()) handle(this,FXSEL(FX::SEL_COMMAND, state ? ID_UNPOST : ID_POST),NULL);
    return 1;
    }
  return 0;
  }


// Released left button
long LabelPopup::onLeftBtnRelease(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXEvent* ev=(FX::FXEvent*)ptr;
  if(isEnabled()){
    if(target && target->tryHandle(this,FXSEL(FX::SEL_LEFTBUTTONRELEASE,message),ptr)) return 1;
    if(ev->moved){ handle(this,FXSEL(FX::SEL_COMMAND,ID_UNPOST),NULL); }
    return 1;
    }
  return 0;
  }


// If we moved over the pane, we'll ungrab again, or re-grab
// when outside of the plane
long LabelPopup::onMotion(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXEvent* ev=(FX::FXEvent*)ptr;
  if(state){
    if(pane){
      if(pane->contains(ev->root_x,ev->root_y)){
        if(grabbed()) ungrab();
        }
      else{
        if(!grabbed()) grab();
        }
      return 1;
      }
    }
  return 0;
  }


// The widget lost the grab for some reason
long LabelPopup::onUngrabbed(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXLabel::onUngrabbed(sender,sel,ptr);
  handle(this,FXSEL(FX::SEL_COMMAND,ID_UNPOST),NULL);
  return 1;
  }


// Keyboard press; forward to menu pane, or handle it here
long LabelPopup::onKeyPress(FX::FXObject*,FX::FXSelector sel,void* ptr){
  FX::FXEvent* event=(FX::FXEvent*)ptr;
  flags&=~FLAG_TIP;
  if(pane && pane->shown() && pane->handle(pane,sel,ptr)) return 1;
  if(isEnabled()){
    if(target && target->tryHandle(this,FXSEL(FX::SEL_KEYPRESS,message),ptr)) return 1;
    if(event->code==FX::KEY_space || event->code==FX::KEY_KP_Space){
      if(isEnabled()) handle(this,FXSEL(FX::SEL_COMMAND, state ? ID_UNPOST : ID_POST),NULL);
      return 1;
      }
    }
  return 0;
  }


// Keyboard release; forward to menu pane, or handle here
long LabelPopup::onKeyRelease(FX::FXObject*,FX::FXSelector sel,void* ptr){
  FX::FXEvent* event=(FX::FXEvent*)ptr;
  if(pane && pane->shown() && pane->handle(pane,sel,ptr)) return 1;
  if(isEnabled()){
    if(target && target->tryHandle(this,FXSEL(FX::SEL_KEYRELEASE,message),ptr)) return 1;
    if(event->code==FX::KEY_space || event->code==FX::KEY_KP_Space){
      return 1;
      }
    }
  return 0;
  }


// Hot key combination pressed
long LabelPopup::onHotKeyPress(FX::FXObject*,FX::FXSelector,void* ptr){
  FXTRACE((200,"%s::onHotKeyPress %p\n",getClassName(),this));
  flags&=~FLAG_TIP;
  handle(this,FXSEL(FX::SEL_FOCUS_SELF,0),ptr);
  if(isEnabled()) handle(this,FXSEL(FX::SEL_COMMAND, state ? ID_UNPOST : ID_POST),NULL);
  return 1;
  }


// Hot key combination released
long LabelPopup::onHotKeyRelease(FX::FXObject*,FX::FXSelector,void*){
  FXTRACE((200,"%s::onHotKeyRelease %p\n",getClassName(),this));
  return 1;
  }


// Post the menu
long LabelPopup::onCmdPost(FX::FXObject*,FX::FXSelector,void*){
  if(!state){
    if(pane){
      if(target) target->tryHandle(this, FXSEL(FX::SEL_COMMAND,ID_QUERY_MENU),list);
      if(list->getNumItems() == 0) return 0;
      FX::FXint x,y,w,h;
      translateCoordinatesTo(x,y,getRoot(),0,0);
//      w=pane->getShrinkWrap() ? pane->getDefaultWidth() : pane->getWidth();
//      h=pane->getShrinkWrap() ? pane->getDefaultHeight() : pane->getHeight();
      w=pane->getDefaultWidth();
      h=pane->getDefaultHeight();
      //x=x+(width-w)/2;
      x+=offsetx;
      y-=offsety+h;
      pane->popup(this,x,y,w,h);
      if(!grabbed()) grab();
      }
    flags&=~FLAG_UPDATE;
    state=true;
    update();
    }
  return 1;
  }


// Unpost the menu
long LabelPopup::onCmdUnpost(FX::FXObject*,FX::FXSelector,void*){
  if(state){
    if(pane){
      pane->popdown();
      if(grabbed()) ungrab();
      }
    flags|=FLAG_UPDATE;
    state=false;
    update();
    }
  return 1;
  }


// Forward GUI update of list to target; but only if pane is not popped
long LabelPopup::onListUpdate(FX::FXObject*,FX::FXSelector,void*){
  return target && pane->shown() && target->tryHandle(this,FXSEL(FX::SEL_UPDATE,message),NULL);
  }


// Item in list widget changed
long LabelPopup::onListChanged(FX::FXObject*,FX::FXSelector,void* ptr){
  return target && target->tryHandle(this,FXSEL(FX::SEL_CHANGED,message),ptr);
  }


// Forward clicked message from list to target
long LabelPopup::onListClicked(FX::FXObject*,FX::FXSelector sel,void* ptr){
  if(isEnabled()) handle(this,FXSEL(FX::SEL_COMMAND,ID_UNPOST),NULL);
  if(FXSELTYPE(sel)==FX::SEL_COMMAND){
    setText(list->getItemText((FX::FXint)(FX::FXival)ptr));
    setIcon(list->getItemIcon((FX::FXint)(FX::FXival)ptr));
    if(target) target->tryHandle(this,FXSEL(FX::SEL_COMMAND,message),ptr);
    }
  return 1;
  }


// Out of focus chain
void LabelPopup::killFocus(){
  FX::FXLabel::killFocus();
  handle(this,FXSEL(FX::SEL_COMMAND,ID_UNPOST),NULL);
  }


// Logically inside pane
bool LabelPopup::contains(FX::FXint parentx,FX::FXint parenty) const {
  if(pane && pane->shown() && pane->contains(parentx,parenty)) return true;
  return false;
  }



// Set icon positioning
void LabelPopup::setButtonStyle(FX::FXuint style){
  FX::FXuint opts=(options&~MENUBUTTON_MASK) | (style&MENUBUTTON_MASK);
  if(options!=opts){
    options=opts;
    update();
    }
  }


// Get icon positioning
FX::FXuint LabelPopup::getButtonStyle() const {
  return (options&MENUBUTTON_MASK);
  }


// Save object to stream
void LabelPopup::save(FX::FXStream& store) const {
  FX::FXLabel::save(store);
  store << pane;
  store << list;
  store << offsetx;
  store << offsety;
  }


// Load object from stream
void LabelPopup::load(FX::FXStream& store){
  FX::FXLabel::load(store);
  store >> pane;
  store >> list;
  store >> pane;
  store >> offsety;
  }


// Delete it
LabelPopup::~LabelPopup(){
  delete pane;
  list=(FX::FXList*)-1L;
  pane=(FX::FXPopup*)-1L;
  }
