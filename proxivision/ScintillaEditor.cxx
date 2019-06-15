#include "ScintillaEditor.h"
#include "Proxivision.h"
#include <errno.h>

// Map
FXDEFMAP(ScintillaEditor) ScintillaEditorMap[]={
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITUNDO, ScintillaEditor::onCmdUndoRedo),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITREDO, ScintillaEditor::onCmdUndoRedo),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITCUT, ScintillaEditor::onCmdCutCopyPasteClear),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITCOPY, ScintillaEditor::onCmdCutCopyPasteClear),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITPASTE, ScintillaEditor::onCmdCutCopyPasteClear),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITCLEAR, ScintillaEditor::onCmdCutCopyPasteClear),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITSHLEFT, ScintillaEditor::onCmdIndent),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITSHRIGHT, ScintillaEditor::onCmdIndent),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITSEARCH, ScintillaEditor::onCmdSearchReplace),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITSEARCHNEXT, ScintillaEditor::onCmdSearchReplace),
  FXMAPFUNC(FX::SEL_COMMAND, ScintillaEditor::ID_EDITSEARCHPREV, ScintillaEditor::onCmdSearchReplace),
};

// Object implementation
FXIMPLEMENT(ScintillaEditor,FXScintilla,ScintillaEditorMap,ARRAYNUMBER(ScintillaEditorMap))

ScintillaEditor::ScintillaEditor() : srdlg(FXScintilla::getShell(), "Search & Replace") { }

ScintillaEditor::ScintillaEditor(FXComposite * p, FXObject * tgt, FXSelector sel, FXuint opts,
 FXint x, FXint y, FXint w, FXint h) : FXScintilla(p, tgt, sel, opts, x, y, w, h),
 srdlg(FXScintilla::getShell(), "Search & Replace")
{
  sendMessage(SCI_SETTABINDENTS, true, 0);
}

long ScintillaEditor::onCmdUndoRedo(FX::FXObject*, FX::FXSelector sel,void*)
{
  printf("onCmdUndoRedo\n");
  if(FXSELID(sel) == ID_EDITUNDO)
    sendMessage(SCI_UNDO, 0, 0);
  else if(FXSELID(sel) == ID_EDITREDO)
    sendMessage(SCI_REDO, 0, 0);
  else
    return 0;

  return 1;
}

long ScintillaEditor::onCmdCutCopyPasteClear(FX::FXObject *, FX::FXSelector sel, void *)
{
  if(FXSELID(sel) == ID_EDITCUT)
    sendMessage(SCI_CUT, 0, 0);
  else if(FXSELID(sel) == ID_EDITCOPY)
    sendMessage(SCI_COPY, 0, 0);
  else if(FXSELID(sel) == ID_EDITPASTE)
    sendMessage(SCI_PASTE, 0, 0);
  else if(FXSELID(sel) == ID_EDITCLEAR)
    sendMessage(SCI_CLEAR, 0, 0);
  else
    return 0;
  return 1;
}

long ScintillaEditor::onCmdIndent(FX::FXObject *, FX::FXSelector sel, void *)
{
  //printf("onCmdIndent\n");
  if(!select_length()) {
    FX::FXint oldpos = sendMessage(SCI_GETCURRENTPOS, 0, 0);
    FX::FXint line = sendMessage(SCI_LINEFROMPOSITION, oldpos, 0);
    FX::FXint linestart = sendMessage(SCI_POSITIONFROMLINE, line, 0);
    if(oldpos != linestart) {
      FX::FXint lineend = sendMessage(SCI_GETLINEENDPOSITION, line , 0);
      sendMessage(SCI_SETSEL, linestart, lineend+1);
    }
    //TODO: popravi da sa zadnjim redom dokumenta radi kako treba
  }
  if(FXSELID(sel) == ID_EDITSHLEFT)
    sendMessage(SCI_BACKTAB, 0, 0);
  else if(FXSELID(sel) == ID_EDITSHRIGHT)
    sendMessage(SCI_TAB, 0, 0);
  else {
    return 0;
  }
  return 1;
}

class search_bounds_t {
  int start;
  int end;
  int middle;
  int lower;
  int higher;
  FX::FXuint modeprev;

public:
  search_bounds_t(void) : start(-1), end(-1), middle(-1), lower(-1), higher(-1), modeprev(-1) { }

  void restart(int newstart, int newend, int newmiddle);
  void update_search(FX::FXuint mode, int hi_ext);
  FX::FXbool update_search_on_wrap(int pos, FX::FXuint mode);
  void next(FX::FXuint mode, int pos, int hi_ext);

  int const& high(void) const { return higher; }
  int const& low(void) const { return lower; }
};

void search_bounds_t::restart(int newstart, int newend, int newmiddle)
{
  start = newstart; end = newend; middle = newmiddle;
  lower = newstart; higher = newend; modeprev = 0;
  if(middle > -1) printf("middle = %u\n", middle);
}

void search_bounds_t::update_search(FX::FXuint mode, int hi_ext)
{
  if(modeprev && ((modeprev&FX::SEARCH_BACKWARD) ^ (mode&FX::SEARCH_BACKWARD))) {
    if(mode&FX::SEARCH_BACKWARD) {//sa forward na backward
      higher = lower-1; lower = start;
    } else {//sa backward na forward
      lower = higher + hi_ext; higher = end;
    }
  }
  modeprev = (-1 & ~FX::SEARCH_BACKWARD) | (mode&FX::SEARCH_BACKWARD);

  if(middle == -1) return;
  if(mode&FX::SEARCH_BACKWARD) higher = middle; else lower = middle;
  middle = -1;
}

void search_bounds_t::next(FX::FXuint mode, int pos, int hi_ext)
{
  if(mode&FX::SEARCH_BACKWARD) higher = pos; else lower = pos + hi_ext;
}

FX::FXbool search_bounds_t::update_search_on_wrap(int pos, FX::FXuint mode)
{
  if(pos == -1 && mode&FX::SEARCH_WRAP) {
    lower = start; higher = end;
    printf("Wrapped: l %u, h %u, s %u, e %u\n", lower, higher, start, end);
    return true;
  }
  return false;
}

long ScintillaEditor::onCmdSearchReplace(FX::FXObject *, FX::FXSelector sel, void *)
{
  printf("onCmdSearchReplace\n");
  static sptr_t pdocprev(0);
  static search_bounds_t bound;
  if(!pdocprev || pdocprev != getDoc() || FXSELID(sel) == ID_EDITSEARCH) {
    int start = 0; int end = length(); int middle = -1;
    if(FXSELID(sel) == ID_EDITSEARCH) {
      if(select_length()) {
        start = sendMessage(SCI_GETSELECTIONSTART,0,0); end = sendMessage(SCI_GETSELECTIONEND,0,0);
      } else
        middle = sendMessage(SCI_GETCURRENTPOS,0,0);
    }
    bound.restart(start, end, middle);
  }
  pdocprev = getDoc();

  FX::FXint code;
  if(FXSELID(sel) == ID_EDITSEARCH) {
    printf("ID_EDITSEARCH\n");
    FX::FXuint placement(FX::PLACEMENT_OWNER);

    printf("Searching ...");
    while((code = srdlg.execute(placement)) == FX::FXReplaceDialog::ID_SEARCH_NEXT) {
      printf("... continue search\n");
      placement = FX::PLACEMENT_DEFAULT;
      FX::FXuint mode = srdlg.getSearchMode();
      bound.update_search(mode, select_length());
      if(find_text(bound, mode) == -1) { printf("... end search\n"); srdlg.destroy(); break; }
    }
  } else if(FXSELID(sel) == ID_EDITSEARCHNEXT) {
    printf("ID_EDITSEARCHNEXT\n");
    FX::FXint mode = (srdlg.getSearchMode() & ~FX::SEARCH_BACKWARD) | FX::SEARCH_WRAP;
    bound.update_search(mode, select_length());//TODO: select_length se moze promijeniti izmedju poziva
    if(find_text(bound, mode) == -1) { printf("...end of file\n"); }
  } else if(FXSELID(sel) == ID_EDITSEARCHPREV){
    printf("ID_EDITSEARCHPREV\n");
    FX::FXint mode = srdlg.getSearchMode() | FX::SEARCH_BACKWARD | FX::SEARCH_WRAP;
    bound.update_search(mode, select_length());//TODO: select_length se moze promijeniti izmedju poziva
    if(find_text(bound, mode) == -1) { printf("...end of file\n"); }
  } else {
    printf("Unknown ID\n");
    return 0;
  }
  return 1;
}

int ScintillaEditor::find_once(int iFirst, int iLast, int searchFlags, FX::FXString const& what)
{
  sendMessage(SCI_SETSEARCHFLAGS, searchFlags, 0);
  sendMessage(SCI_SETTARGETSTART, iFirst, 0);
  sendMessage(SCI_SETTARGETEND, iLast, 0);
  return sendMessage(SCI_SEARCHINTARGET, what.length(), (sptr_t)what.text());
}

int ScintillaEditor::find_text(search_bounds_t& b, FX::FXuint mode)
{
  FX::FXbool const mback(mode&FX::SEARCH_BACKWARD);
  FX::FXString const ssearch = srdlg.getSearchText();

  int searchFlags(0);
  if(!(mode&FX::SEARCH_IGNORECASE)) searchFlags |= SCFIND_MATCHCASE;
  if(mode&FX::SEARCH_REGEX) searchFlags |= SCFIND_REGEXP;
  //FX::FXuint replace_target_cmd = mode&FX::SEARCH_REGEX ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;

  int pos = find_once(mback?b.high():b.low(), mback?b.low():b.high(), searchFlags, ssearch);
  if(b.update_search_on_wrap(pos, mode))
    pos = find_once(mback?b.high():b.low(), mback?b.low():b.high(), searchFlags, ssearch);
  printf("first %u, last %u, pos %u\n", mback?b.high():b.low(), mback?b.low():b.high(), pos);

  if(pos > -1) {
    sendMessage(SCI_SETSELECTIONSTART, sendMessage(SCI_GETTARGETSTART, 0, 0), 0);
    sendMessage(SCI_SETSELECTIONEND, sendMessage(SCI_GETTARGETEND, 0, 0), 0);
    b.next(mode, pos, select_length());
  }
  return pos;
}

sptr_t ScintillaEditor::can_undo(void) { return sendMessage(SCI_CANUNDO, 0, 0); }
sptr_t ScintillaEditor::can_redo(void) { return sendMessage(SCI_CANREDO, 0, 0); }

sptr_t ScintillaEditor::getDoc(void) { return sendMessage(SCI_GETDOCPOINTER, 0, 0); }
sptr_t ScintillaEditor::createDoc(void) { return sendMessage(SCI_CREATEDOCUMENT, 0, 0); }
void ScintillaEditor::selectDoc(sptr_t pdoc) { sendMessage(SCI_SETDOCPOINTER, 0, pdoc); }
void ScintillaEditor::referenceDoc(sptr_t pdoc) { sendMessage(SCI_ADDREFDOCUMENT, 0, pdoc); }
void ScintillaEditor::unreferenceDoc(sptr_t pdoc) { sendMessage(SCI_RELEASEDOCUMENT, 0, pdoc); }

FX::FXbool ScintillaEditor::getOvertype(void) { return sendMessage(SCI_GETOVERTYPE, 0, 0); }
FX::FXbool ScintillaEditor::getReadOnly(void) { return sendMessage(SCI_GETREADONLY, 0, 0); }

FX::FXint ScintillaEditor::getTabWidth(void)
 { return sendMessage(SCI_GETTABWIDTH, 0, 0); }

void ScintillaEditor::setTabWidth(FX::FXint w)
 { sendMessage(SCI_SETTABWIDTH, w, 0); }

FX::FXbool ScintillaEditor::getTabIndents(void)
 { return sendMessage(SCI_GETTABINDENTS, 0, 0); }

void ScintillaEditor::setTabIndents(FX::FXbool indts)
 { sendMessage(SCI_SETTABINDENTS, indts, 0); }

FX::FXbool ScintillaEditor::getSpacesForTab(void)
 { return sendMessage(SCI_SETUSETABS, 0, 0) ? false: true; }

void ScintillaEditor::setSpacesForTab(FX::FXbool spc)
 { sendMessage(SCI_SETUSETABS, spc ? false : true, 0); }

inline int RetVal0(char const* msg) { printf("%s", msg); return 0; }
inline int RetVal1(char const* msg) { printf("%s", msg); return 1; }

long ScintillaEditor::length(void) { return sendMessage(SCI_GETLENGTH,0,0); }

long ScintillaEditor::select_length(void)
{
  //NOTE: Moglo bi ovako: return sendMessage(SCI_GETSELTEXT,0,0);
  long length = sendMessage(SCI_GETSELECTIONEND,0,0) - sendMessage(SCI_GETSELECTIONSTART,0,0);
  return (length>=0) ? length : -length;
}

int ScintillaEditor::load_file(FX::FXString const& path, FX::FXString const& filename, bool insert)
{
  FX::FXString fullpath(path);
  fullpath.append(PATHSEP).append(filename);
  printf("Loading %s ...", fullpath.text());

  FX::FXFile file(fullpath, FX::FXFile::Reading);
  if (!file.isOpen())
    return RetVal0("(FAIL open)\n");

  file.position(0,FX::FXIO::Begin);
  long pos=0;

  if (insert) {
    sendMessage(SCI_BEGINUNDOACTION, 0, 0);
    if (select_length()>0) sendMessage(SCI_REPLACESEL,0,0);//clear
    pos = sendMessage(SCI_GETCURRENTPOS, 0,0);
  } else {
    sendMessage(SCI_CLEARALL,0,0);
  }
  const int BUFSIZE=1025;
  char buf[BUFSIZE];

  do {
    FX::FXival n=file.readBlock(buf,BUFSIZE);
    if (n<0) return RetVal0("(FAIL read)\n");
    if (insert) {
      sendMessage(SCI_INSERTTEXT,pos,(sptr_t)buf);
      pos+=n;
    } else {
      sendMessage(SCI_APPENDTEXT,n,(sptr_t)buf);
    }
  } while (!file.eof());
  if(!file.size()) {
    sendMessage(SCI_APPENDTEXT, 1,(sptr_t)"$");//samo da posalje SCN_SAVEPOINTLEFT
    sendMessage(SCI_CLEARALL,0,0);
  }
  file.close();

  if (insert) {
    sendMessage(SCI_GOTOPOS,pos,0);
    sendMessage(SCI_CONVERTEOLS,sendMessage(SCI_GETEOLMODE,0,0),0);
    sendMessage(SCI_ENDUNDOACTION,0,0);
  } else {
    sendMessage(SCI_EMPTYUNDOBUFFER,0,0);
    //sendMessage(SCI_SETSAVEPOINT,0,0);
  }
  printf("(opened)\n");
  //apply_style(path, filename);
  return 1;
}

void ScintillaEditor::apply_style(FX::FXString const& path, FX::FXString const& filename)
{
  sendMessage(SCI_STYLECLEARALL, 0, 0);
  sendMessage(SCI_STYLESETFONT, STYLE_CALLTIP, (sptr_t) FX::FXApp::instance()->getNormalFont()->getName().text());
  sendMessage(SCI_STYLESETSIZE, STYLE_CALLTIP, FX::FXApp::instance()->getNormalFont()->getSize()/10);
  sendMessage(SCI_CALLTIPUSESTYLE,32,1);

  auto spLang = srclng.find(filename);
  if(!spLang) {
      printf("Unexpected error: not found any lexer\n");
      return;
  }
  //printf("Selected lexer [%u]\n", li.id);

  // //int nbits=sendMessage(SCI_GETSTYLEBITSNEEDED,0,0);
  // size_t imax=32;//1<<nbits;
  // if(imax>32) 
  //   imax=32;//TODO: dozvoli i indekse vece od 32 za neke egzoticne lexere
  
  ;
  for(size_t idxStyle = 0; idxStyle<spLang->styles.size(); ++idxStyle) {
    auto style = spLang->styles[idxStyle]->getStyle();
    sendMessage(SCI_STYLESETFORE, idxStyle, style.ulFg);
    sendMessage(SCI_STYLESETBACK, idxStyle, style.ulBg);
    sendMessage(SCI_STYLESETBOLD, idxStyle, style.bBold ? 1 : 0);
    sendMessage(SCI_STYLESETITALIC, idxStyle, style.bItalic ? 1 : 0);
    printf("Adding style %lu fg=0x%08lx fg=0x%08lx\n", idxStyle, style.ulFg, style.ulBg);
  }
  sendMessage(SCI_SETTABWIDTH, 4, 0);

  bool usetabs;
  switch(spLang->getTabPolicy()) {
    case langdescr_t::TAB_USETAB: 
      usetabs = true;
      break;
    case langdescr_t::TAB_SPACES:
      usetabs = false;
      break;
    case langdescr_t::TAB_AUTO:
      usetabs = false; //TODO: TAB_CHAR_AUTO utvrdi iz \n\t u dokumentu
      break;
    // default: {
    //   usetabs = Proxivision::instance()->defaults_edit.flags&edit_prm_t::FUseTabs;
  }
  sendMessage(SCI_SETUSETABS, usetabs, 0);
  sendMessage(SCI_SETTABWIDTH, Proxivision::instance()->defaults_edit.TabWidth, 0); // TODO: is cfg json-a
  sendMessage(SCI_SETLEXER, spLang->id, 0);
  // sendMessage(SCI_SETSTYLEBITS, sendMessage(SCI_GETSTYLEBITSNEEDED,0,0), 0);

  for(size_t idxWord = 0; idxWord<KEYWORDSET_MAX; ++idxWord) {
    sendMessage(SCI_SETKEYWORDS, idxWord, (sptr_t)"");
  }
  for(size_t idxWord = 0; idxWord<spLang->keywords.size(); ++idxWord) {
    auto& words = spLang->keywords[idxWord];
    sendMessage(SCI_SETKEYWORDS, idxWord, (sptr_t)(words.get()) );
    printf("Adding keywordset %lu %s\n", idxWord, words.get());
  }

  sendMessage(SCI_SETPROPERTY, (sptr_t)"lexer.cpp.track.preprocessor", (sptr_t)"0");
  sendMessage(SCI_SETPROPERTY, (sptr_t)"lexer.cpp.update.preprocessor", (sptr_t)"0");

  //sendMessage(SCI_SETLEXER, spLang->id, 0);
	sendMessage(SCI_COLOURISE, 0, -1);
}

/*struct Sci_CharacterRange { long cpMin; long cpMax; };
struct Sci_TextRange { struct Sci_CharacterRange chrg; char *lpstrText; };*/
int ScintillaEditor::save_file(FX::FXString const& path, FX::FXString const& filename, bool copied)
{
  FX::FXString fullpath(path); fullpath.append("/").append(filename);
  printf("Saving %s ...", fullpath.text());

  FX::FXFile file(fullpath, FX::FXFile::Writing);
  if (!file.isOpen())
      return RetVal0("(failed opening)\n");

  const int BUFSIZE = 1025;
  char buf[BUFSIZE];
  Sci_TextRange txrng = { {0, BUFSIZE}, buf};//stavlja \0 na kraju
  FX::FXint len = sendMessage(SCI_GETLENGTH,0,0);
  FX::FXival accwrote = 0;
  while(accwrote != len) {
      if(txrng.chrg.cpMax > len) txrng.chrg.cpMax = len;
      if(txrng.chrg.cpMin >= txrng.chrg.cpMax) break;
      FX::FXival n = sendMessage(SCI_GETTEXTRANGE, 0, (sptr_t)&txrng);//vraca broj procitanih bez \0 na kraju
      FX::FXival wrote = file.writeBlock(buf, n);
      accwrote += wrote;
      if(wrote < n) break;
      txrng.chrg.cpMin += n; txrng.chrg.cpMax += n;
  }
  if(!file.close() || (accwrote!=len)) {
      printf("(%s)\n", strerror(errno));
      return 0;
  }
  if(!copied) 
      sendMessage(SCI_SETSAVEPOINT, 0, 0);
  return RetVal1("(saved)\n");
}
