#include "SourceLanguage.h"
#include <fx.h>
#include <Scintilla.h>
using FX::FXScrollArea;
using FX::FXString;
using FX::FXSelector;
using FX::FXint;
using FX::FXuint;
#include <FXScintilla.h>
#include "SearchReplaceDialog.h"

class search_bounds_t;

class ScintillaEditor : protected FXScintilla {
  FXDECLARE(ScintillaEditor)
  SourceLanguage srclng;
  SearchReplaceDialog srdlg;
protected:
  ScintillaEditor();
private:
  int find_once(int first, int last, int searchFlags, FX::FXString const& what);
  int find_text(search_bounds_t& b, FX::FXuint mode);
public:
  enum {
    ID_EDITUNDO = FXScintilla::ID_LAST,
    ID_EDITREDO,
    ID_EDITCUT,
    ID_EDITCOPY,
    ID_EDITPASTE,
    ID_EDITCLEAR,
    ID_EDITSHLEFT,
    ID_EDITSHRIGHT,
    ID_EDITSEARCH,
    ID_EDITSEARCHPREV,
    ID_EDITSEARCHNEXT,
    ID_LAST,
  };
  ScintillaEditor(FXComposite * p, FXObject * tgt = NULL, FXSelector sel = 0,
    FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);
  long select_length(void);
  long length(void);

  long onCmdUndoRedo(FX::FXObject *, FX::FXSelector, void *);
  long onCmdCutCopyPasteClear(FX::FXObject *, FX::FXSelector, void *);
  long onCmdIndent(FX::FXObject *, FX::FXSelector, void *);
  long onCmdSearchReplace(FX::FXObject *, FX::FXSelector, void *);
  sptr_t can_undo(void);
  sptr_t can_redo(void);

  sptr_t getDoc(void);
  void selectDoc(sptr_t pdoc);
  sptr_t createDoc(void);
  void referenceDoc(sptr_t pdoc);
  void unreferenceDoc(sptr_t pdoc);

  FX::FXbool getOvertype(void);
  FX::FXbool getReadOnly(void);

  void setTabWidth(FX::FXint w = 4);
  FX::FXint getTabWidth(void);

  void setTabIndents(FX::FXbool indents = true);//no tab character its used for indenting
  FX::FXbool getTabIndents(void);

  void setSpacesForTab(FX::FXbool spaces = true);
  FX::FXbool getSpacesForTab(void);

  int load_file(FX::FXString const& path, FX::FXString const& filename, bool insert = false);
  void apply_style(FX::FXString const& path, FX::FXString const& filename);
  int save_file(FX::FXString const& path, FX::FXString const& filename, bool copied = false);
};
