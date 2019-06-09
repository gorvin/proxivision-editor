#include "TabDockBar.h"

// Map
FXDEFMAP(TabDockTitle) TabDockTitleMap[]={
  FXMAPFUNC(FX::SEL_LEFTBUTTONPRESS,0,TabDockTitle::onLeftBtnPress),
  FXMAPFUNC(FX::SEL_LEFTBUTTONRELEASE,0,TabDockTitle::onLeftBtnRelease),
};

// Object implementation
FXIMPLEMENT(TabDockTitle,FX::FXDockTitle,TabDockTitleMap,ARRAYNUMBER(TabDockTitleMap))

// Pressed mouse button
long TabDockTitle::onLeftBtnPress(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  //if(!FX::FXDockHandler::onLeftBtnPress(sender,sel,ptr)){
  FX::FXDockHandler::onLeftBtnPress(sender,sel,ptr);{
    if(isEnabled()){
      getParent()->handle(this,FXSEL(FX::SEL_COMMAND,FX::FXTabBar::ID_OPEN_ITEM),ptr);
      flags|=FLAG_PRESSED;
      flags&=~FLAG_UPDATE;
      return 1;
      }
    }
  return 0;
  }

// Released mouse button
long TabDockTitle::onLeftBtnRelease(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  if(!FX::FXDockHandler::onLeftBtnRelease(sender,sel,ptr)){
    if(isEnabled()){
      flags|=FLAG_UPDATE;
      flags&=~FLAG_PRESSED;
      return 1;
      }
    }
  return 0;
  }


TabDockTitle::TabDockTitle(FX::FXComposite* p,const FX::FXString& text,FX::FXObject* tgt,
 FX::FXSelector sel,FX::FXuint opts,int x,int y,int w,int h,int pl,int pr,
 int pt,int pb) : FX::FXDockTitle(p,text,tgt,sel,opts,x,y,w,h,pl,pr,pt,pb)
{

}

/*******************************************************************************/

// Map
FXDEFMAP(TabDockBar) TabDockBarMap[]={
  FXMAPFUNC(FX::SEL_FOCUS_PREV,0,TabDockBar::onFocusLeft),
  FXMAPFUNC(FX::SEL_FOCUS_NEXT,0,TabDockBar::onFocusRight),
  FXMAPFUNC(FX::SEL_BEGINDRAG,TabDockBar::ID_TOOLBARGRIP,TabDockBar::onBeginDragGrip),
  FXMAPFUNC(FX::SEL_ENDDRAG,TabDockBar::ID_TOOLBARGRIP,TabDockBar::onEndDragGrip),
  FXMAPFUNC(FX::SEL_DRAGGED,TabDockBar::ID_TOOLBARGRIP,TabDockBar::onDraggedGrip),
  FXMAPFUNC(FX::SEL_TIMEOUT,TabDockBar::ID_TIMER,TabDockBar::onDockTimer),
  };


// Object implementation
FXIMPLEMENT(TabDockBar,FX::FXPacker,TabDockBarMap,ARRAYNUMBER(TabDockBarMap))


// Deserialization
TabDockBar::TabDockBar():drydock(NULL),wetdock(NULL){
  flags|=FLAG_ENABLED;
  gripx=0;
  gripy=0;
  }


// Make a dockable and, possibly, floatable toolbar
TabDockBar::TabDockBar(FX::FXComposite* p,FX::FXComposite* q,FX::FXuint opts,int x,int y,int w,int h,int pl,int pr,int pt,int pb,int hs,int vs):
  FXPacker(p,opts,x,y,w,h,pl,pr,pt,pb,hs,vs),drydock(p),wetdock(q){
  flags|=FLAG_ENABLED;
  gripx=0;
  gripy=0;
  }


// Return true if toolbar is docked
FX::FXbool TabDockBar::isDocked() const {
  return (getParent()!=wetdock);
  }


// Set parent when docked, if it was docked it will remain docked
void TabDockBar::setDryDock(FX::FXComposite* dry){
  if(dry && dry->id() && getParent()==drydock){
    reparent(dry,NULL);
    }
  drydock=dry;
  }


// Set parent when floating
void TabDockBar::setWetDock(FX::FXComposite* wet){
  if(wet && wet->id() && getParent()==wetdock){
    reparent(wet,NULL);
    }
  wetdock=wet;
  }


// Undock the bar
void TabDockBar::undock(int rootx,int rooty,FX::FXbool notify){
  FX::FXTabBook* docksite=dynamic_cast<FX::FXTabBook*>(getParent());
  if(wetdock && isDocked()){
    //if(docksite) docksite->undockToolBar(this);
    FX::FXWindow *tabitem = docksite->childAtIndex(docksite->indexOfChild(this)-1);
    reparent(wetdock);
    //wetdock->position(rootx,rooty,wetdock->getDefaultWidth(),wetdock->getDefaultHeight());
    wetdock->position(rootx,rooty,docksite->getWidth(),docksite->getHeight());
    wetdock->show();
    tabitem->reparent(this,getFirst());
    if(notify && target){target->tryHandle(this,FXSEL(FX::SEL_FLOATED,message),docksite);}
    }
  }


// Dock the bar near position in dock site
void TabDockBar::dock(FX::FXTabBook* docksite,FX::FXbool notify){
  if(docksite && getParent()!=docksite){
    setDryDock(docksite);
    getFirst()->reparent(docksite,NULL);//grip/title
    reparent(docksite,NULL);
    wetdock->hide();
    if(notify && target){target->tryHandle(this,FXSEL(FX::SEL_DOCKED,message),docksite);}
    }
  }


// Search siblings of drydock for dock opportunity near given coordinates
FX::FXTabBook* TabDockBar::findDockNear(int rootx,int rooty){

  if(drydock){
    register FX::FXWindow* top;
    // Translate without pain; assumes position of the top window is correct
    top=drydock;
    while(top->getParent()!=getRoot()) top=top->getParent();

    register FX::FXWindow *child;
    register FX::FXTabBook *docksite;
    int dockx,docky;

    child = top;
    while(child) {
      docksite=dynamic_cast<FX::FXTabBook*>(child);
      if(docksite && docksite->shown()){
        return docksite;
        }
      child->translateCoordinatesFrom(dockx,docky,getRoot(),rootx,rooty);
      child = child->getChildAt(dockx,docky);
      }
    }
  return NULL;
  }


// Tool bar grip drag started; the grip widget can be at any level under this dock bar
long TabDockBar::onBeginDragGrip(FX::FXObject* sender,FX::FXSelector,void* ptr){
  FX::FXWindow *grip=static_cast<FX::FXWindow*>(sender);
  FX::FXEvent* event=static_cast<FX::FXEvent*>(ptr);
  FX::FXTabBook* book=dynamic_cast<FX::FXTabBook*>(getParent());
  if(book){
    book->setCurrent(getParent()->indexOfChild(grip));
    }
  if(dynamic_cast<FX::FXTabBook*>(drydock)){
    getRoot()->translateCoordinatesFrom(gripx,gripy,grip->getParent(),grip->getX(),grip->getY());
    gripx = event->root_x-gripx;//TODO: ovo treba obaviti za SEL_LEFTBUTTONCLICK
    gripy = event->root_y-gripy;
    raise();
    return 1;
    }
  return 0;
  }


// Tool bar grip dragged
long TabDockBar::onDraggedGrip(FX::FXObject* /*sender*/,FX::FXSelector,void* ptr){
  FX::FXTabBook *toolbardock=dynamic_cast<FX::FXTabBook*>(getParent());
  FX::FXToolBarShell *toolbarshell=dynamic_cast<FX::FXToolBarShell*>(getParent());
  FX::FXEvent* event=static_cast<FX::FXEvent*>(ptr);
  int rootx,rooty;

  // Root position
  rootx=event->root_x-gripx;
  rooty=event->root_y-gripy;

  // Stop dock timer
  getApp()->removeTimeout(this,ID_TIMER);

  // We are docked
  if(toolbardock){

    int dockevx,dockevy;
    toolbardock->translateCoordinatesFrom(dockevx,dockevy,getRoot(),event->root_x,event->root_y);
    TabDockTitle* dockitem = dynamic_cast<TabDockTitle*>(toolbardock->getChildAt(dockevx,dockevy));
    if(dockitem) {
        FX::FXWindow* selfdockitem = getPrev();
        if(dockitem != selfdockitem) {
          if(toolbardock->indexOfChild(dockitem)>toolbardock->indexOfChild(selfdockitem)) {
            FX::FXWindow* dockitem_assoc = dockitem->getNext();
            dockitem->reparent(toolbardock,selfdockitem);
            dockitem_assoc->reparent(toolbardock,selfdockitem);
            toolbardock->setCurrent(toolbardock->getCurrent()+1);
            }
          else {
            selfdockitem->reparent(toolbardock,dockitem);
            reparent(toolbardock,dockitem);
            toolbardock->setCurrent(toolbardock->getCurrent()-1);
            }
          }
      }

    if(findDockNear(rootx,rooty) != toolbardock){
      undock(rootx,rooty,true);
      }
    }

  // We are floating
  else if(toolbarshell){

    // We're near a dock, if we hover around we'll dock there
    if(!(event->state&FX::CONTROLMASK)){
      //toolbardock=findDockNear(rootx,rooty);
      //if(toolbardock) getApp()->addTimeout(this,ID_TIMER,300,toolbardock);
      getApp()->addTimeout(this,ID_TIMER,2000,event);
      }

    // Move around freely
    wetdock->move(rootx,rooty);
    }

  return 1;
  }


// Hovered near dock site:- dock it!
long TabDockBar::onDockTimer(FX::FXObject* /*sender*/,FX::FXSelector sel,void* ptr){
  //FX::FXTabBook *toolbardock=static_cast<FX::FXTabBook*>(ptr);
  //dock(toolbardock,true);
  TabDockTitle* title_to_tabitem = dynamic_cast<TabDockTitle*>(getFirst());
  onEndDragGrip(title_to_tabitem,sel,ptr);
  //onBeginDragGrip(title_to_tabitem,sel,ptr);
  if(title_to_tabitem)
    title_to_tabitem->onLeftBtnPress(title_to_tabitem,sel,ptr);
  return 1;
  }


// Tool bar grip drag ended
long TabDockBar::onEndDragGrip(FX::FXObject*,FX::FXSelector,void* ptr){
  FX::FXToolBarShell *toolbarshell=dynamic_cast<FX::FXToolBarShell*>(getParent());
  FX::FXEvent* event=static_cast<FX::FXEvent*>(ptr);
  FX::FXTabBook *toolbardock;
  int rootx,rooty;
  getApp()->removeTimeout(this,ID_TIMER);
  if(toolbarshell){
    if(!(event->state&FX::CONTROLMASK)){
      rootx=event->root_x-gripx;
      rooty=event->root_y-gripy;
      toolbardock=findDockNear(rootx,rooty);
      if(toolbardock){
        dock(toolbardock,true);
        }
      }
    }
  return 1;
  }


// Save data
void TabDockBar::save(FX::FXStream& store) const {
  FXPacker::save(store);
  store << drydock;
  store << wetdock;
  }


// Load data
void TabDockBar::load(FX::FXStream& store){
  FXPacker::load(store);
  store >> drydock;
  store >> wetdock;
  }


// Destroy
TabDockBar::~TabDockBar(){
  getApp()->removeTimeout(this,ID_TIMER);
  drydock=(FX::FXComposite*)-1L;
  wetdock=(FX::FXComposite*)-1L;
  }
