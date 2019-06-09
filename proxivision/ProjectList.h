#include "fx.h"
#include "Project.h"

class FXAPI ProjectList : public FX::FXTreeList {
  FXDECLARE(ProjectList)
  enum hierarchy_t { ROOT_FOLDER, SUB_FOLDER, VIEW_FOLDER, PROJECT_FILE, };
protected:
  FX::FXString      dropdirectory;        // Drop directory
  FX::FXDragAction  dropaction;           // Drop action
  FX::FXString      dragfiles;            // Dragged files
protected:
  ProjectList();
private:
  explicit ProjectList(const ProjectList&);//Ne implementiraj!!!
  ProjectList &operator=(const ProjectList&);
  // static FX::FXIcon* ohiericon(hierarchy_t);
  // static FX::FXIcon* chiericon(hierarchy_t);
public:
  long onBeginDrag(FX::FXObject*,FX::FXSelector,void*);
  long onEndDrag(FX::FXObject*,FX::FXSelector,void*);
  long onDragged(FX::FXObject*,FX::FXSelector,void*);
  long onDNDEnter(FX::FXObject*,FX::FXSelector,void*);
  long onDNDLeave(FX::FXObject*,FX::FXSelector,void*);
  long onDNDMotion(FX::FXObject*,FX::FXSelector,void*);
  long onDNDDrop(FX::FXObject*,FX::FXSelector,void*);
  long onDNDRequest(FX::FXObject*,FX::FXSelector,void*);
public:
  static FX::FXint ascending(const FX::FXTreeItem* a,const FX::FXTreeItem* b);

  /// Construct a directory list
  ProjectList(FX::FXComposite *p,FX::FXObject* tgt=NULL,FX::FXSelector sel=0,FX::FXuint opts=0,FX::FXint x=0,FX::FXint y=0,FX::FXint w=0,FX::FXint h=0);

  project_t const& project(FX::FXTreeItem const*) const;
  void setViewType(FX::FXTreeItem* iroot, FX::FXbool ondiskview = false);
  int scan(FX::FXTreeItem* iroot);
  int add(FX::FXString const& ppath, FX::FXString const& pfname);
  FX::FXString project_subdir(FX::FXTreeItem const* item) const;
//  unit_t const* project_unit(FX::FXTreeItem const* item) const
//   { return item->hasItems() ? 0 : (unit_t const*) item->getData(); }
  FX::FXString active_target(FX::FXTreeItem const*) const;
  void active_target(FX::FXTreeItem*, FX::FXString const& active);
  FX::FXbool onDiskView(FX::FXTreeItem const*) const;

  void create();
  void detach();
  void destroy();

  FX::FXbool openItem(FX::FXTreeItem* item, FX::FXbool notify);
  FX::FXbool closeItem(FX::FXTreeItem* item, FX::FXbool notify);
  /// Expand tree
  FX::FXbool expandTree(FX::FXTreeItem* tree,FX::FXbool notify=false);
  FX::FXbool collapseTree(FX::FXTreeItem* tree,FX::FXbool notify);

  /// Return absolute pathname of item
  FX::FXString getItemPathname(const FX::FXTreeItem* item) const;

  FX::FXint path_length(const FX::FXTreeItem* item);

  /// Return the item from the absolute pathname
  FX::FXTreeItem* getPathnameItem(const FX::FXString& path);

  /// Save to stream
  void save(FX::FXStream& store) const;

  /// Load from stream
  void load(FX::FXStream& store);

  /// Destructor
  ~ProjectList();
};
