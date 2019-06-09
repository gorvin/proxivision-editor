//#include "ScintillaEditor.h"
#include <fx.h>

//namespace FX {
//  class FXToolBarShell;
//  class FXIconDict;
//  class FXTabBook;
//  class FXText;
//};
class ProjectList;
class ScintillaEditor;
class TabDockBar;
class DocumentList;
class LabelPopup;

struct EMBED_ICON {
  enum eID {
    PROXYVISION_APP = 0,
    DOCUMENT_NEW,
    DOCUMENT_OPEN,
    DOCUMENT_SAVE,
    DOCUMENT_SAVE_AS,
    DOCUMENT_SAVE_ALL,
    EDIT_CUT,
    EDIT_COPY,
    EDIT_DELETE,
    EDIT_PASTE,
    EDIT_REDO,
    EDIT_UNDO,
    FORMAT_SHIFT_LEFT,
    FORMAT_SHIFT_RIGHT,
    SEARCH,
    SEARCH_NEXT,
    SEARCH_PREV,
    GO_BACKWARD,
    GO_FORWARD,
    FONTS,

    COUNT
  };
};

FX::FXIcon* getEmbeddedIcon(FX::FXApp* app, EMBED_ICON::eID idIcon);
FX::FXString getEmbeddedIconName(FX::FXApp* app, EMBED_ICON::eID idIcon);
FX::FXIcon* getEmbeddedIcon(FX::FXApp* app, FX::FXString const& name); 

/* bezvezni komentar */
/** ovo je doxy key \ref */
/*! ovo je doxy key \ref */
/// ovo je doxy key \ref

class ProxivisionWindow : public FX::FXMainWindow {
FXDECLARE(ProxivisionWindow)

private:
  ProjectList*        dirlist;                 // Directory view
  FX::FXText*         textout;
  ScintillaEditor*    scint;
  DocumentList*       lopened;
  FX::FXToolBar*      toolbar;
  FX::FXToolBarShell* dragshelltabs1;
  FX::FXToolBarShell* dragshelltabs2;
  FX::FXStatusBar*    statusbar;
  LabelPopup*      popup_targets;
  //FX::FXLabel*        showactivetgt;
//  FX::FXToolBarShell *dragshelltabs3;
  ProxivisionWindow();
  int load_file(FX::FXString const& path, FX::FXString const& filename, FX::FXTreeItem*);

  TabDockBar* add(FX::FXTabBook*, FX::FXString const&, FX::FXToolBarShell*);
  void add(EMBED_ICON::eID actionid, FX::FXObject* tgt, FX::FXSelector sel, char const* text);
  static FX::FXString save_filename(FX::FXWindow* wshell, FX::FXString const& path);
  static FX::FXString open_filename(FX::FXWindow* wshell, FX::FXString const& path);
  FX::FXButton* find_button(FX::FXToolBar* tbar, FX::FXSelector sel);

public:
  enum{
    ID_REDIRECT=FXMainWindow::ID_LAST,
    ID_OPEN_TREE,//ProjectList
    ID_OPEN_OLIST,//DocumentList
    ID_EDITOR,//ScintillaEditor
    ID_ABOUT,
    ID_READONLY,
    ID_OVERTYPE,
    ID_TARGET,
    ID_DOCNEW,
    ID_DOCOPEN,
    ID_DOCSAVE,
    ID_DOCSAVEAS,
    ID_DOCSAVEALL,
    //ID_EDITUNDO,
    //ID_EDITREDO,
    //ID_EDITCUT,
    //ID_EDITCOPY,
    //ID_EDITPASTE,
    //ID_EDITDELETE,
    //ID_EDITLSHIFT,
    //ID_EDITRSHIFT,
    //ID_EDITSEARCH,
    //ID_EDITSEARCHPREV,
    //ID_EDITSEARCHNEXT,
    ID_GOPREV,
    ID_GONEXT,
    ID_FONTCHANGE,
    ID_LAST,
  };
  long onUpdate(FX::FXObject*, FX::FXSelector, void*);
  long onCmdOpenTree(FX::FXObject*, FX::FXSelector, void*);
  long onCmdOpenOlist(FX::FXObject*, FX::FXSelector, void*);
  long onCmdEditor(FX::FXObject *, FX::FXSelector, void *);
  long onData(FX::FXObject*, FX::FXSelector, void*);
  long onCmdDocNew(FX::FXObject *, FX::FXSelector, void *);
  long onCmdDocOpen(FX::FXObject *, FX::FXSelector, void *);
  long onCmdDocSave(FX::FXObject *, FX::FXSelector, void *);
  long onCmdDocSaveAs(FX::FXObject *, FX::FXSelector, void *);
  long onUpdStatusbar(FX::FXObject *, FX::FXSelector, void*);
  long onClickTargetList(FX::FXObject*, FX::FXSelector, void*);
  long onCmdTargetList(FX::FXObject*, FX::FXSelector, void*);
  ProxivisionWindow(FX::FXApp *a, FX::FXIcon*i);
  virtual ~ProxivisionWindow();
  virtual void create();
  virtual void detach();
  virtual FX::FXbool close(FX::FXbool notify=false);
};

