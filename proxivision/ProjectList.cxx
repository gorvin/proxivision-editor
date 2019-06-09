#include "ProjectList.h"
#include "Proxivision.h"
#include "SourceLanguage.h"
#include "StringPartitioner.h"

// Map
FXDEFMAP(ProjectList) ProjectListMap[]={
  FXMAPFUNC(FX::SEL_DRAGGED,0,ProjectList::onDragged),
  FXMAPFUNC(FX::SEL_DND_ENTER,0,ProjectList::onDNDEnter),
  FXMAPFUNC(FX::SEL_DND_LEAVE,0,ProjectList::onDNDLeave),
  FXMAPFUNC(FX::SEL_DND_DROP,0,ProjectList::onDNDDrop),
  FXMAPFUNC(FX::SEL_DND_MOTION,0,ProjectList::onDNDMotion),
  FXMAPFUNC(FX::SEL_DND_REQUEST,0,ProjectList::onDNDRequest),
  FXMAPFUNC(FX::SEL_BEGINDRAG,0,ProjectList::onBeginDrag),
  FXMAPFUNC(FX::SEL_ENDDRAG,0,ProjectList::onEndDrag),
};

// Object implementation
FXIMPLEMENT(ProjectList,FX::FXTreeList,ProjectListMap,ARRAYNUMBER(ProjectListMap))

// FX::FXIcon* ProjectList::ohiericon(hierarchy_t type)
// {
//   FX::FXString nico;
//   switch(type) {
//   case ROOT_FOLDER: nico = "pv-folder-open"; break;
//   case SUB_FOLDER: nico = "pv-folder-open"; break;
//   case VIEW_FOLDER: nico = "pv-view-open"; break;
//   default: nico = "text-x-preview";
//   }
//   return Proxivision::instance()->getIcon(nico);
// }

// FX::FXIcon* ProjectList::chiericon(hierarchy_t type)
// {
//   FX::FXString nico;
//   switch(type) {
//   case ROOT_FOLDER: nico = "pv-folder"; break;
//   case SUB_FOLDER: nico = "pv-folder"; break;
//   case VIEW_FOLDER: nico = "pv-view"; break;
//   default: nico = "text-x-preview";
//   }
//   return Proxivision::instance()->getIcon(nico);
// }

// For serialization
ProjectList::ProjectList()
{
  dropEnable();
  sortfunc=ProjectList::ascending;
  dropaction=FX::DRAG_MOVE;
}

#define PROJECTLIST_SELECTOR (FX::TREELIST_BROWSESELECT| FX::TREELIST_SHOWS_LINES|\
 FX::TREELIST_SHOWS_BOXES|FX::TREELIST_ROOT_BOXES)

// Directory List Widget
ProjectList::ProjectList(FX::FXComposite *p, FX::FXObject* tgt, FX::FXSelector sel, FX::FXuint opts,
 FX::FXint x, FX::FXint y, FX::FXint w, FX::FXint h): FX::FXTreeList(p, tgt, sel,
  opts|PROJECTLIST_SELECTOR, x, y, w, h)
{
  dropEnable();
  sortfunc=ProjectList::ascending;
  dropaction=FX::DRAG_MOVE;
}

// Create X window
void ProjectList::create()
{
  FX::FXTreeList::create();
  if(!deleteType){deleteType=getApp()->registerDragType(deleteTypeName);}
  if(!urilistType){urilistType=getApp()->registerDragType(urilistTypeName);}
}

// Detach disconnects the icons
void ProjectList::detach()
{
  FX::FXTreeList::detach();
  deleteType=0;
  urilistType=0;
}

// Destroy zaps the icons
void ProjectList::destroy()
{
  FX::FXTreeList::destroy();
}

int ProjectList::scan(FX::FXTreeItem* iroot)
{
  if(iroot->getParent()) return 0;//nije root
  printf("Scanning...\n");
  project_t const&    p(project(iroot));
  FX::FXString const  rootpfx(p.title+PATHSEP);
  FX::FXbool ondiskview(onDiskView(iroot));
  if(!ondiskview) for(size_t i=0; i<p.views.size(); i++) {//TODO: Mozda treba i hijerarhija pogleda
    FX::FXTreeItem* it = appendItem(iroot, p.views[i], nullptr/*ohiericon(VIEW_FOLDER)*/, nullptr/*chiericon(VIEW_FOLDER)*/, 0);
    it->setHasItems(true);
  }
  for(size_t i=0; i<p.units.size(); i++) {
    unit_t const&   punit(p.units[i]);
    FX::FXString    viewroot(rootpfx);
    FX::FXTreeItem* iview(0);
    if(!ondiskview && !punit.view.empty()) {
      viewroot += punit.view; iview = getPathnameItem(viewroot);
    }
    if(!iview) iview = iroot;
    //TODO: if(!punit.view.empty() && iview == root) iz project_t ukloni taj nepostojeci view za unit
    printf("[%s]",punit.filename.text());
    FX::FXint    endsep(punit.filename.rfind(PATHSEP));
    if(endsep > 0) {
      FX::FXString      subpath(punit.filename.left(endsep));
      StringPartitioner part(subpath, PATHSEP);
      while(part.next()) {
        FX::FXTreeItem* viewex = getPathnameItem(viewroot + part.left());
        if(!viewex) {
          viewex = appendItem(iview, part.mid(), nullptr/*ohiericon(SUB_FOLDER)*/, nullptr/*chiericon(SUB_FOLDER)*/, 0);
          viewex->setHasItems(true);
        }
        iview = viewex;
      }
    }
    FX::FXString name(punit.filename); if(endsep > 0) name = name.right(name.length() - endsep - 1);
    FX::FXIcon* uicon = Proxivision::instance()->getMimeIcon(name.text());
    appendItem(iview, name, uicon, uicon, (void*)&punit);
  }//NOTE: tranparentnost png indexed ikona je bolja nego za rgb

  return 1;
}

int ProjectList::add(FX::FXString const& ppath, FX::FXString const& pfname)
{
  printf("Add %s" PATHSEPSTRING "%s\n", ppath.text(), pfname.text());
  project_t* p = new project_t;
  p->path = ppath;
  p->fname = pfname;
  if(!p->read(ppath + PATHSEPSTRING + pfname)) { delete p; return 0; }
  p->active_target = p->default_target.empty() ? p->targets[0].title : p->default_target;

  FX::FXTreeItem* iroot = appendItem(NULL, p->title, nullptr/*ohiericon(ROOT_FOLDER)*/, nullptr/*chiericon(ROOT_FOLDER)*/, p);
  iroot->setHasItems(true);
  //setViewType(iroot, true);
  scan(iroot);
  return 1;
}

void ProjectList::setViewType(FX::FXTreeItem* iroot, FX::FXbool ondiskview)
{
  if(iroot->getParent()) return;//nije root
  ((project_t*)iroot->getData())->ondiskview = ondiskview;
}

FX::FXString ProjectList::project_subdir(FX::FXTreeItem const* item) const
{
  if(item->hasItems()) return FX::FXString::null;
  return ((unit_t const*)item->getData())->subdir();
}

project_t const& ProjectList::project(FX::FXTreeItem const* item) const
{
  while(item->getParent()) item = item->getParent();
  return *((project_t*)item->getData());
}

FX::FXString ProjectList::active_target(FX::FXTreeItem const* item) const
{
  if(!item) return FX::FXString::null;
  while(item->getParent()) item = item->getParent();
  return project(item).active_target;
}

void ProjectList::active_target(FX::FXTreeItem* item, FX::FXString const& active)
{
  while(item->getParent()) item = item->getParent();
  ((project_t*)item->getData())->active_target = active;
}

FX::FXbool ProjectList::onDiskView(FX::FXTreeItem const* item) const
{
  while(item->getParent()) item = item->getParent();
  return project(item).ondiskview;
}

// Sort ascending order, keeping directories first
FX::FXint ProjectList::ascending(const FX::FXTreeItem* a,const FX::FXTreeItem* b)
{
  FX::FXint diff=0; if(b->hasItems()) diff++; if(a->hasItems()) diff--;
  return diff ? diff : FX::FXTreeList::ascending(a, b);
}

/*******************************************************************************/


// Handle drag-and-drop enter
long ProjectList::onDNDEnter(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXTreeList::onDNDEnter(sender,sel,ptr);//FX::FXWindow salje do target
  return 1;
}

// Handle drag-and-drop leave
long ProjectList::onDNDLeave(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  stopAutoScroll();
  FX::FXTreeList::onDNDLeave(sender,sel,ptr);//FX::FXWindow salje do target
  return 1;
}


// Handle drag-and-drop motion
long ProjectList::onDNDMotion(FX::FXObject* sender,FX::FXSelector sel,void* ptr){
  FX::FXEvent *event=(FX::FXEvent*)ptr;
  if(startAutoScroll(event,false)) return 1;
  if(FX::FXTreeList::onDNDMotion(sender,sel,ptr)) return 1;//FX::FXWindow salje do target

  if(offeredDNDType(FX::FROM_DRAGNDROP,urilistType)){// Dropping list of filenames
    FX::FXTreeItem* item=getItemAt(event->win_x,event->win_y);//drop place
    if(item && item->hasItems()){// We can drop in a directory
      dropdirectory=getItemPathname(item);//za onDNDDrop
      dropaction=inquireDNDAction();//move,copy,link, za onDNDDrop
      FXTRACE((100,"accepting drop on %s\n",dropdirectory.text()));
      acceptDrop(FX::DRAG_ACCEPT);//indikacija a nastavak je onDNDDrop
    }
    return 1;
  }
  return 0;
}

bool doDNDDrop(int dropaction, FX::FXString const& src, FX::FXString const& dst)
{
  switch(dropaction) {
  case FX::DRAG_MOVE: return FX::FXFile::moveFiles(src,dst);
  case FX::DRAG_COPY: return FX::FXFile::copyFiles(src,dst);
  case FX::DRAG_LINK: return FX::FXFile::symlink(src,dst);
  }
  return true;
}

// Handle drag-and-drop drop
long ProjectList::onDNDDrop(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  stopAutoScroll();
  if(FX::FXTreeList::onDNDDrop(sender,sel,ptr)) return 1;

  FX::FXString dropfiles;//uri-list of files
  if(!getDNDData(FX::FROM_DRAGNDROP,urilistType,dropfiles)) return 0;
  dropFinished(FX::DRAG_ACCEPT);// Tell drag source we got it

  for(FX::FXint beg = 0,end; beg < dropfiles.length(); beg = end + 2) {
    if((end = dropfiles.find_first_of("\r\n",beg))<0) end = dropfiles.length();
    FX::FXString url = dropfiles.mid(beg, end - beg);// File url
    FX::FXString filesrc = FX::FXURL::decode(FX::FXURL::fileFromURL(url));
    FX::FXString filedst = dropdirectory + PATHSEPSTRING + FX::FXPath::name(filesrc);
    if(!doDNDDrop(dropaction, filesrc, filedst)) getApp()->beep();
  }
  return 1;
}

// Somebody wants our dragged data
long ProjectList::onDNDRequest(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  if(FX::FXTreeList::onDNDRequest(sender,sel,ptr)) return 1;

  FX::FXEvent *event=(FX::FXEvent*)ptr;
  if(event->target==urilistType){//Return list of filenames as a uri-list
    setDNDData(FX::FROM_DRAGNDROP,event->target,dragfiles); return 1;
  }
  if(event->target==deleteType) { return 1;}//TODO: Delete files DND data
  return 0;
}

long ProjectList::onBeginDrag(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  if(FX::FXTreeList::onBeginDrag(sender,sel,ptr)) return 1;
  if(!beginDrag(&urilistType,1)) return 0;

  dragfiles=FX::FXString::null;
  //Through hierarchy from first root item
  for(FX::FXTreeItem *item=firstitem; item; item = item->getBelow()) {
    if(!item->isSelected()) continue;
    if(!dragfiles.empty()) dragfiles+="\r\n";
    dragfiles+=FX::FXURL::encode(FX::FXURL::fileToURL(getItemPathname(item)));
  }
  return 1;
}

long ProjectList::onEndDrag(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  if(FX::FXTreeList::onEndDrag(sender,sel,ptr)) return 1;
  endDrag((didAccept()!=FX::DRAG_REJECT));
  setDragCursor(getDefaultCursor());
  return 1;
}

// Dragged stuff around
long ProjectList::onDragged(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{
  if(FX::FXTreeList::onDragged(sender,sel,ptr)) return 1;

  FX::FXEvent* event=(FX::FXEvent*)ptr;
  FX::FXDragAction action = FX::DRAG_MOVE;
  FX::FXDefaultCursor cursor_type = FX::DEF_DNDMOVE_CURSOR;
  //modifikacija drag akcije tasterima CTRL, SHIFT, ALT
  if(event->state&FX::CONTROLMASK) { action=FX::DRAG_COPY; cursor_type = FX::DEF_DNDCOPY_CURSOR; }
  if(event->state&FX::SHIFTMASK) { action=FX::DRAG_MOVE; cursor_type = FX::DEF_DNDMOVE_CURSOR; }
  if(event->state&FX::ALTMASK) { action=FX::DRAG_LINK; cursor_type = FX::DEF_DNDLINK_CURSOR; }

  handleDrag(event->root_x,event->root_y,action);

  if(didAccept()==FX::DRAG_REJECT) cursor_type = FX::DEF_DNDSTOP_CURSOR;
  setDragCursor(getApp()->getDefaultCursor(cursor_type));

  return 1;
}


/*******************************************************************************/

// Open item
FX::FXbool ProjectList::openItem(FX::FXTreeItem* item, FX::FXbool notify)
{
  if(item==NULL){ FX::fxerror("%s::openItem: NULL argument.\n", getClassName()); }
  if(!item->isOpened() && item->isExpanded()){
    item->setOpened(true);
    updateItem(item);
    if(notify && target){target->tryHandle(this,FXSEL(FX::SEL_OPENED,message),(void*)item);}
    return true;
  }
  return false;
}

// Close item
FX::FXbool ProjectList::closeItem(FX::FXTreeItem* item, FX::FXbool notify)
{
  if(item==NULL){ FX::fxerror("%s::closeItem: NULL argument.\n",getClassName()); }
  if(item->isOpened() && !item->isExpanded()){
    item->setOpened(false);
    updateItem(item);
    if(notify && target){target->tryHandle(this,FXSEL(FX::SEL_CLOSED,message),(void*)item);}
    return true;
  }
  return false;
}

// Expand tree
FX::FXbool ProjectList::expandTree(FX::FXTreeItem* tree,FX::FXbool notify)
{
  if(FX::FXTreeList::expandTree(tree,notify)) {
    if(tree->hasItems()){ recalc(); sortChildItems(tree); }
    openItem(tree, notify);
    return true;
  }
  return false;
}

// Collapse tree
FX::FXbool ProjectList::collapseTree(FX::FXTreeItem* tree,FX::FXbool notify)
{
  if(FX::FXTreeList::collapseTree(tree,notify)){
    if(tree->hasItems()) { }
    closeItem(tree, notify);
    return true;
  }
  return false;
}

// Return absolute pathname of item
FX::FXString ProjectList::getItemPathname(const FX::FXTreeItem* item) const
{
  FX::FXString pathname;
  for( ; item; item=item->getParent()) {
    pathname.prepend(item->getText());
    if(item->getParent()) pathname.prepend(PATHSEP);
  }
  return pathname;
}

FX::FXint ProjectList::path_length(const FX::FXTreeItem* item)
{
  FX::FXint pathlen = 0;
  for( ; item; item=item->getParent()) {
    pathlen += item->getText().length();
    if(item->getParent()) pathlen++;
  }
  return pathlen;
}

// Return the item from the absolute pathname
FX::FXTreeItem* ProjectList::getPathnameItem(const FX::FXString& path)
{
  if(path.empty() || ISPATHSEP(path[0])) return NULL;
  FX::FXint pathlen = path.length();
  //brute force through hierarchy from first root item
  for(FX::FXTreeItem *item=firstitem; item; item = item->getBelow()) {
    if(path_length(item) != pathlen) continue;
    if(compare(path, getItemPathname(item)) == 0) return item;
  }
  return NULL;
}

// Save data
void ProjectList::save(FX::FXStream& store) const
{
  FX::FXTreeList::save(store);
}

// Load data
void ProjectList::load(FX::FXStream& store)
{
  FX::FXTreeList::load(store);
}

// Cleanup
ProjectList::~ProjectList()
{
  for(FX::FXTreeItem *it = firstitem; it; it = it->getNext()) { delete (project_t*)it->getData(); }
  clearItems();
}
