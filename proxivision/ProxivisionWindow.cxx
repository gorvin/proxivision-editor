#include "ProxivisionWindow.h"
#include "TabDockBar.h"
#include "ProjectList.h"
#include "ProjectXML.h"
#include "GuidedSplitter.h"
#include "Proxivision.h"
#include "ScintillaEditor.h"
#include "DocumentList.h"
#include "LabelPopup.h"

#include <string>
#include <stdio.h>
#include <errno.h>


// extern unsigned int proxivision_png_len;
extern unsigned char const* icon_proxivision_ptr;
extern unsigned char const* icon_document_new_ptr;
extern unsigned char const* icon_document_open_ptr;
extern unsigned char const* icon_document_save_ptr;
extern unsigned char const* icon_document_save_as_ptr;
extern unsigned char const* icon_document_save_all_ptr;
extern unsigned char const* icon_edit_cut_ptr;
extern unsigned char const* icon_edit_copy_ptr;
extern unsigned char const* icon_edit_delete_ptr;
extern unsigned char const* icon_edit_paste_ptr;
extern unsigned char const* icon_edit_redo_ptr;
extern unsigned char const* icon_edit_undo_ptr;
extern unsigned char const* icon_format_shift_left_ptr;
extern unsigned char const* icon_format_shift_right_ptr;
extern unsigned char const* icon_search_ptr;
extern unsigned char const* icon_search_next_ptr;
extern unsigned char const* icon_search_prev_ptr;
extern unsigned char const* icon_go_backward_ptr;
extern unsigned char const* icon_go_forward_ptr;
extern unsigned char const* icon__blank_ptr;
extern unsigned char const* icon__page_ptr;
extern unsigned char const* icon_aac_ptr;
extern unsigned char const* icon_ai_ptr;
extern unsigned char const* icon_aiff_ptr;
extern unsigned char const* icon_avi_ptr;
extern unsigned char const* icon_bmp_ptr;
extern unsigned char const* icon_c_ptr;
extern unsigned char const* icon_cpp_ptr;
extern unsigned char const* icon_css_ptr;
extern unsigned char const* icon_csv_ptr;
extern unsigned char const* icon_dat_ptr;
extern unsigned char const* icon_dmg_ptr;
extern unsigned char const* icon_doc_ptr;
extern unsigned char const* icon_dotx_ptr;
extern unsigned char const* icon_dwg_ptr;
extern unsigned char const* icon_dxf_ptr;
extern unsigned char const* icon_eps_ptr;
extern unsigned char const* icon_exe_ptr;
extern unsigned char const* icon_flv_ptr;
extern unsigned char const* icon_gif_ptr;
extern unsigned char const* icon_h_ptr;
extern unsigned char const* icon_hpp_ptr;
extern unsigned char const* icon_html_ptr;
extern unsigned char const* icon_ics_ptr;
extern unsigned char const* icon_iso_ptr;
extern unsigned char const* icon_java_ptr;
extern unsigned char const* icon_jpg_ptr;
extern unsigned char const* icon_js_ptr;
extern unsigned char const* icon_key_ptr;
extern unsigned char const* icon_less_ptr;
extern unsigned char const* icon_mid_ptr;
extern unsigned char const* icon_mp3_ptr;
extern unsigned char const* icon_mp4_ptr;
extern unsigned char const* icon_mpg_ptr;
extern unsigned char const* icon_odf_ptr;
extern unsigned char const* icon_ods_ptr;
extern unsigned char const* icon_odt_ptr;
extern unsigned char const* icon_otp_ptr;
extern unsigned char const* icon_ots_ptr;
extern unsigned char const* icon_ott_ptr;
extern unsigned char const* icon_pdf_ptr;
extern unsigned char const* icon_php_ptr;
extern unsigned char const* icon_png_ptr;
extern unsigned char const* icon_ppt_ptr;
extern unsigned char const* icon_psd_ptr;
extern unsigned char const* icon_py_ptr;
extern unsigned char const* icon_qt_ptr;
extern unsigned char const* icon_rar_ptr;
extern unsigned char const* icon_rb_ptr;
extern unsigned char const* icon_rtf_ptr;
extern unsigned char const* icon_sass_ptr;
extern unsigned char const* icon_scss_ptr;
extern unsigned char const* icon_sql_ptr;
extern unsigned char const* icon_tga_ptr;
extern unsigned char const* icon_tgz_ptr;
extern unsigned char const* icon_tiff_ptr;
extern unsigned char const* icon_txt_ptr;
extern unsigned char const* icon_wav_ptr;
extern unsigned char const* icon_xls_ptr;
extern unsigned char const* icon_xlsx_ptr;
extern unsigned char const* icon_xml_ptr;
extern unsigned char const* icon_yml_ptr;
extern unsigned char const* icon_zip_ptr;


static struct {
  char const* name;
  unsigned char const* data;
  FX::FXIcon* icon;
} embeddedIcons[] = {
  { "proxivision", icon_proxivision_ptr, nullptr },
  { "document-new", icon_document_new_ptr, nullptr },
  { "document-open", icon_document_open_ptr, nullptr },
  { "document-save", icon_document_save_ptr, nullptr },
  { "document-save-as", icon_document_save_as_ptr, nullptr },
  { "document-save-all", icon_document_save_all_ptr, nullptr },
  { "edit-cut", icon_edit_cut_ptr, nullptr },
  { "edit-copy", icon_edit_copy_ptr, nullptr },
  { "edit-delete", icon_edit_delete_ptr, nullptr },
  { "edit-paste", icon_edit_paste_ptr, nullptr },
  { "edit-redo", icon_edit_redo_ptr, nullptr },
  { "edit-undo", icon_edit_undo_ptr, nullptr },
  { "format-shift-left", icon_format_shift_left_ptr, nullptr },
  { "format-shift-right", icon_format_shift_right_ptr, nullptr },
  { "search", icon_search_ptr, nullptr },
  { "search-next", icon_search_next_ptr, nullptr },
  { "search-prev", icon_search_prev_ptr, nullptr },
  { "go-backward", icon_go_backward_ptr, nullptr },
  { "go-forward", icon_go_forward_ptr, nullptr },
  { "fonts", icon_go_forward_ptr, nullptr },
  { "filetype-_blank", icon__blank_ptr, nullptr },
  { "filetype-_page", icon__page_ptr, nullptr },
  { "filetype-aac", icon_aac_ptr, nullptr },
  { "filetype-ai", icon_ai_ptr, nullptr },
  { "filetype-aiff", icon_aiff_ptr, nullptr },
  { "filetype-avi", icon_avi_ptr, nullptr },
  { "filetype-bmp", icon_bmp_ptr, nullptr },
  { "filetype-c", icon_c_ptr, nullptr },
  { "filetype-cpp", icon_cpp_ptr, nullptr },
  { "filetype-css", icon_css_ptr, nullptr },
  { "filetype-csv", icon_csv_ptr, nullptr },
  { "filetype-dat", icon_dat_ptr, nullptr },
  { "filetype-dmg", icon_dmg_ptr, nullptr },
  { "filetype-doc", icon_doc_ptr, nullptr },
  { "filetype-dotx", icon_dotx_ptr, nullptr },
  { "filetype-dwg", icon_dwg_ptr, nullptr },
  { "filetype-dxf", icon_dxf_ptr, nullptr },
  { "filetype-eps", icon_eps_ptr, nullptr },
  { "filetype-exe", icon_exe_ptr, nullptr },
  { "filetype-flv", icon_flv_ptr, nullptr },
  { "filetype-gif", icon_gif_ptr, nullptr },
  { "filetype-h", icon_h_ptr, nullptr },
  { "filetype-hpp", icon_hpp_ptr, nullptr },
  { "filetype-html", icon_html_ptr, nullptr },
  { "filetype-ics", icon_ics_ptr, nullptr },
  { "filetype-iso", icon_iso_ptr, nullptr },
  { "filetype-java", icon_java_ptr, nullptr },
  { "filetype-jpg", icon_jpg_ptr, nullptr },
  { "filetype-js", icon_js_ptr, nullptr },
  { "filetype-key", icon_key_ptr, nullptr },
  { "filetype-less", icon_less_ptr, nullptr },
  { "filetype-mid", icon_mid_ptr, nullptr },
  { "filetype-mp3", icon_mp3_ptr, nullptr },
  { "filetype-mp4", icon_mp4_ptr, nullptr },
  { "filetype-mpg", icon_mpg_ptr, nullptr },
  { "filetype-odf", icon_odf_ptr, nullptr },
  { "filetype-ods", icon_ods_ptr, nullptr },
  { "filetype-odt", icon_odt_ptr, nullptr },
  { "filetype-otp", icon_otp_ptr, nullptr },
  { "filetype-ots", icon_ots_ptr, nullptr },
  { "filetype-ott", icon_ott_ptr, nullptr },
  { "filetype-pdf", icon_pdf_ptr, nullptr },
  { "filetype-php", icon_php_ptr, nullptr },
  { "filetype-png", icon_png_ptr, nullptr },
  { "filetype-ppt", icon_ppt_ptr, nullptr },
  { "filetype-psd", icon_psd_ptr, nullptr },
  { "filetype-py", icon_py_ptr, nullptr },
  { "filetype-qt", icon_qt_ptr, nullptr },
  { "filetype-rar", icon_rar_ptr, nullptr },
  { "filetype-rb", icon_rb_ptr, nullptr },
  { "filetype-rtf", icon_rtf_ptr, nullptr },
  { "filetype-sass", icon_sass_ptr, nullptr },
  { "filetype-scss", icon_scss_ptr, nullptr },
  { "filetype-sql", icon_sql_ptr, nullptr },
  { "filetype-tga", icon_tga_ptr, nullptr },
  { "filetype-tgz", icon_tgz_ptr, nullptr },
  { "filetype-tiff", icon_tiff_ptr, nullptr },
  { "filetype-txt", icon_txt_ptr, nullptr },
  { "filetype-wav", icon_wav_ptr, nullptr },
  { "filetype-xls", icon_xls_ptr, nullptr },
  { "filetype-xlsx", icon_xlsx_ptr, nullptr },
  { "filetype-xml", icon_xml_ptr, nullptr },
  { "filetype-yml", icon_yml_ptr, nullptr },
  { "filetype-zip", icon_zip_ptr, nullptr },
};
 
FX::FXIcon* getEmbeddedIcon(FX::FXApp* app, EMBED_ICON::eID idIcon) 
{
  FX::FXIcon*& icon(embeddedIcons[idIcon].icon);
  if(nullptr == icon)
    icon = FX::FXIconSource::defaultIconSource.loadIconData(app, embeddedIcons[idIcon].data/*, "png"*/);
  return icon;
}

FX::FXString getEmbeddedIconName(FX::FXApp* app, EMBED_ICON::eID idIcon) 
{
  return embeddedIcons[idIcon].name;
}

FX::FXIcon* getEmbeddedIcon(FX::FXApp* app, FX::FXString const& name) 
{
  for (size_t i=0; i<_countof(embeddedIcons); ++i) {
    if (name == embeddedIcons[i].name) {
      FX::FXIcon*& icon(embeddedIcons[i].icon);
      if(nullptr == icon)
        icon = FX::FXIconSource::defaultIconSource.loadIconData(app, embeddedIcons[i].data/*, "png"*/);
      return icon;
    }
  }
  return nullptr;
}

FXDEFMAP(ProxivisionWindow) MyWindowMap[]={
  FXMAPFUNC(FX::SEL_UPDATE, 0, ProxivisionWindow::onUpdate),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_REDIRECT, ProxivisionWindow::onData),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_OPEN_TREE, ProxivisionWindow::onCmdOpenTree),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_OPEN_OLIST, ProxivisionWindow::onCmdOpenOlist),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_EDITOR, ProxivisionWindow::onCmdEditor),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_DOCNEW, ProxivisionWindow::onCmdDocNew),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_DOCOPEN, ProxivisionWindow::onCmdDocOpen),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_DOCSAVE, ProxivisionWindow::onCmdDocSave),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_DOCSAVEAS, ProxivisionWindow::onCmdDocSaveAs),
  FXMAPFUNC(FX::SEL_UPDATE, ProxivisionWindow::ID_OVERTYPE, ProxivisionWindow::onUpdStatusbar),
  FXMAPFUNC(FX::SEL_UPDATE, ProxivisionWindow::ID_READONLY, ProxivisionWindow::onUpdStatusbar),
  FXMAPFUNC(FX::SEL_UPDATE, ProxivisionWindow::ID_TARGET, ProxivisionWindow::onUpdStatusbar),
  FXMAPFUNC(FX::SEL_COMMAND, FX::FXWindow::ID_QUERY_MENU, ProxivisionWindow::onClickTargetList),
  FXMAPFUNC(FX::SEL_COMMAND, ProxivisionWindow::ID_TARGET, ProxivisionWindow::onCmdTargetList),
};

FXIMPLEMENT(ProxivisionWindow,FX::FXMainWindow,MyWindowMap,ARRAYNUMBER(MyWindowMap))

long ProxivisionWindow::onData(FXObject*o,FXSelector sel, void* p)
{
  FX::FXString* sbuf(static_cast<FX::FXString*>(p));
  if(!sbuf) {
    FX::FXString snotext("Unexpected error [ProxivisionWindow::onData]: NULL string.");
    return onData(o, sel, &snotext);
  }
  textout->appendText(*sbuf);
  textout->makePositionVisible(textout->getLength());

  return 1;
}

FX::FXButton* ProxivisionWindow::find_button(FX::FXToolBar* tbar, FX::FXSelector sel)
{
  for(FX::FXWindow* c=tbar->getFirst(); c; c=c->getNext()) {
    if(c->getSelector() == sel) return (FX::FXButton*)c;
  }
  return 0;
}

long ProxivisionWindow::onUpdate(FX::FXObject* o, FX::FXSelector s, void* p)
{
  FX::FXButton* btn = find_button(toolbar, ID_DOCSAVE);
  if(!btn) { printf("Not found ID_DOCSAVE button!\n"); return 0; }
  if(lopened->getNumRows() > 0){
    FX::FXString docstatus(lopened->getFileStatus(scint->getDoc()));
    if(docstatus.find("*") > -1 || docstatus.find("N") > -1) btn->enable(); else btn->disable();
  } else
    btn->disable();

  btn = find_button(toolbar, ScintillaEditor::ID_EDITUNDO);
  if(!btn) { printf("Not found ID_EDITUNDO button!\n"); return 0; }
  if(scint->can_undo()) btn->enable(); else btn->disable();

  btn = find_button(toolbar, ScintillaEditor::ID_EDITREDO);
  if(!btn) { printf("Not found ID_EDITREDO button!\n"); return 0; }
  if(scint->can_redo()) btn->enable(); else btn->disable();

  btn = find_button(toolbar, ScintillaEditor::ID_EDITCUT);
  if(!btn) { printf("Not found ID_EDITCUT button!\n"); return 0; }
  if(scint->select_length()) btn->enable(); else btn->disable();

  btn = find_button(toolbar, ScintillaEditor::ID_EDITCOPY);
  if(!btn) { printf("Not found ID_EDITCOPY button!\n"); return 0; }
  if(scint->select_length()) btn->enable(); else btn->disable();

  return FX::FXMainWindow::onUpdate(o, s, p);
}

inline int RetVal0(char const* msg) { printf("%s", msg); return 0; }
inline int RetVal1(char const* msg) { printf("%s", msg); return 1; }

// Command from the tree list
long ProxivisionWindow::onCmdOpenTree(FX::FXObject*,FX::FXSelector,void* ptr)
{
  FX::FXTreeItem *item=(FX::FXTreeItem*)ptr;
  if(!item) return RetVal0("No item\n");
  if(item->hasItems()) return RetVal0("Not file\n");
  //if(!saveChanges()) return 1;
  FX::FXString fpath(dirlist->project(item).path); {
    FX::FXString ssub(dirlist->project_subdir(item));
    if(!ssub.empty()) fpath.append(PATHSEP).append(ssub);
  }

  load_file(fpath, item->getText(), item);
  return 1;
}

long ProxivisionWindow::onCmdOpenOlist(FX::FXObject* obj, FX::FXSelector sel, void* ptr)
{
  FX::FXTablePos* tpos=(FX::FXTablePos*)ptr;
  opened_doc_t const* pod = lopened->getFileData(tpos->row);
  if(!pod) return RetVal0("No list item Data\n");
  if(!pod->item) {
    scint->selectDoc(pod->pdoc);//Untitled ili eksterni dokument
    lopened->selectDoc(pod->pdoc);
    return 1;
  }
  dirlist->setCurrentItem(pod->item);
  dirlist->makeItemVisible(pod->item);
  return /*load_file(pod->item);*/onCmdOpenTree(obj, sel, pod->item);
}

int ProxivisionWindow::load_file(FX::FXString const& path, FX::FXString const& filename, FX::FXTreeItem* item)
{
  sptr_t pdoc_prev = scint->getDoc();
  sptr_t pdoc = lopened->selectDoc(path, filename);
  
  if(pdoc) {
    scint->selectDoc(pdoc);//postojeci dokument
    scint->apply_style(path, filename);//postojeci dokument
    return RetVal1("(existing)\n");
  }

  pdoc = scint->createDoc(); //novi dokument + referenca
  scint->selectDoc(pdoc);
  //NOTE: Radi konzistentnosti obrade obavjestenja SCN_SAVEPOINTLEFT odmah se pdoc pridruzuje u
  //lopened. U slucaju greske ucitavanja mora se odmah ukloniti iz lopened
  lopened->add_doc(path, filename, pdoc, item);

  if(!scint->load_file(path, filename, false)) {
    lopened->removeDoc(pdoc);
    scint->unreferenceDoc(pdoc);
    if(lopened->getNumRows()>0)
      scint->selectDoc(pdoc_prev);
    return 0;
  }

  lopened->setModified(pdoc, false);//status "N" -> " "
  scint->apply_style(path, filename);

  return 1;
}

long ProxivisionWindow::onCmdEditor(FX::FXObject*, FX::FXSelector, void* ptr)
{
  static int count = 0;
  SCNotification* notify = (SCNotification*)ptr;
  if(!notify)
    return RetVal1("No notify message\n");
  //printf("onCmdEditor [notify->nmhdr.code=%u]\n", notify->nmhdr.code);

  if(notify->nmhdr.code == SCN_CHARADDED) {
//    printf("%u SCN_CHARADDED\n", count++);
  }

  if(notify->nmhdr.code == SCN_MODIFIED) {
    if(notify->modificationType&SC_MOD_INSERTTEXT) {
//      printf("%u SCN_MODIFIED, SC_MOD_INSERTTEXT\n", count++);
//      printf("pdoc %lu, position %u, length %u\n", scint->getDoc(), notify->position, notify->length);
    }
    if(notify->modificationType&SC_PERFORMED_UNDO) {
//      printf("%u SCN_MODIFIED, SC_PERFORMED_UNDO\n", count++);
//      if(!scint->can_undo()) {
//        printf("From now document is unmodified\n");
//      }
    }
  }
  //postojeci "N" -> nista ne radi, to je novi nesacuvan dokument
  //postojeci " " -> "*"
  //nepostojeci -> pridruzi lopened + "N"
  //NOTE: ovo se salje i za novi dokument kad se ucitava njegov tekst
  if(notify->nmhdr.code == SCN_SAVEPOINTLEFT) {
    printf("\n%u Notify: SCN_SAVEPOINTLEFT\n", count++);
    sptr_t pdoc = scint->getDoc();
    FX::FXint row = lopened->getRow(pdoc);
    if(row > -1) {//postojeci
      if(lopened->getFileStatus(pdoc).find(" ") > -1)
        lopened->setModified(pdoc);
    } else {
      lopened->add_doc("", "Untitled", pdoc, NULL);
      scint->referenceDoc(pdoc);
    }
  }
  if(notify->nmhdr.code == SCN_SAVEPOINTREACHED) {
    printf("%u Notify: SCN_SAVEPOINTREACHED\n", count++);
    sptr_t pdoc = scint->getDoc();
    FX::FXint row = lopened->getRow(pdoc);
    if(row > -1) {//postojeci
      if(lopened->getFileStatus(pdoc).find("*") > -1) 
        lopened->setModified(pdoc, false);
      if(lopened->getFileStatus(pdoc).find("N") > -1) {
        scint->unreferenceDoc(pdoc);
        lopened->removeDoc(pdoc);
      }
    }
  }
  return 1;
}

// Update box for overstrike mode display
long ProxivisionWindow::onUpdStatusbar(FX::FXObject* sender, FX::FXSelector sel, void*)
{
  FXString label("     ");

  if(FXSELID(sel) == ID_OVERTYPE) {
    label = scint->getOvertype() ? "OVR" : "INS";
  } else if(FXSELID(sel) == ID_READONLY) {
    label = scint->getReadOnly() ? "RO" : "RW";
  } else if(FXSELID(sel) == ID_TARGET) {
    sptr_t pdoc = scint->getDoc();
    FX::FXint row = lopened->getRow(pdoc);
    if(pdoc && row > -1) 
      label = dirlist->active_target(lopened->getFileData(row)->item);
  } else {
    return 0;
  }
  sender->handle(this, FXSEL(FX::SEL_COMMAND, ID_SETSTRINGVALUE), (void*)&label);
  return 1;
}

//openning popup list of targets
long ProxivisionWindow::onClickTargetList(FX::FXObject*, FX::FXSelector, void* ptr)
{
  printf("onClickTargetList\n"); //return 0;
  FX::FXTreeItem* it = dirlist->getCurrentItem(); 
  if(!it) 
    return 0;
  printf("Current tree item %s\n", it->getText().text());
  FX::FXList* list = (FX::FXList*)ptr;
  list->clearItems();
  project_t const& p = dirlist->project(it);
  for(size_t i=0; i<p.targets.size(); i++) 
    list->appendItem(p.targets[i].title);
  list->setCurrentItem(list->findItem(dirlist->active_target(it)));
  return 1;
}

//Clicked item on popup list of targets
long ProxivisionWindow::onCmdTargetList(FX::FXObject* sender, FX::FXSelector, void*)
{
  LabelPopup* lb = dynamic_cast<LabelPopup*>(sender);
  if(!lb)
    return 0;
  dirlist->active_target(dirlist->getCurrentItem(), lb->getText());
  printf("Choosen target: %s\n", dirlist->active_target(dirlist->getCurrentItem()).text());
  return 1;
}

long ProxivisionWindow::onCmdDocNew(FX::FXObject*, FX::FXSelector,void*)
{
  if(lopened->getRow(scint->getDoc()) == -1)
    return 1;//trenutno aktivni je vec potencijalni Untitled

  sptr_t pdoc = scint->createDoc();//novi dokument + referenca
  scint->selectDoc(pdoc);
  scint->unreferenceDoc(pdoc);//referenca ide tek za SCN_SAVEPOINTLEFT
  return 1;
}

long ProxivisionWindow::onCmdDocOpen(FX::FXObject*, FX::FXSelector,void*)
{
  FX::FXString fullpath = open_filename(getShell(), FX::FXPath::upLevel(
                            dirlist->project(dirlist->getFirstItem()).path));
  if(fullpath.empty())
    return 0;
  printf("Openning %s\n", fullpath.text());
  FXString fpath = FX::FXPath::directory(fullpath);
  FXString fname = FX::FXPath::name(fullpath);
  //TODO: provjeri da li fajl pripada nekom od otvorenih projekata jer tada ne ide apsolutni path
  load_file(fpath, fname, NULL); //extern file
  return 1;
}

long ProxivisionWindow::onCmdDocSaveAs(FX::FXObject*, FX::FXSelector,void*)
{
  sptr_t pdoc = scint->getDoc();

  FX::FXString fullpath = save_filename(getShell(), FX::FXPath::upLevel(
                          dirlist->project(dirlist->getFirstItem()).path));
  if(fullpath.empty())
    return 0;
  FXString fpath = FX::FXPath::directory(fullpath);
  FXString fname = FX::FXPath::name(fullpath);
  scint->save_file(fpath.text(), fname.text(), true);//ostaje modified ako je bio, ovo je kopija
  if(lopened->getFileStatus(pdoc).find("N") != -1) {
    lopened->setFilePath(pdoc, fpath);
    lopened->setFileName(pdoc, fname);
    lopened->setModified(pdoc, false);
  }
  return 1;
}

long ProxivisionWindow::onCmdDocSave(FX::FXObject*, FX::FXSelector,void*)
{
  sptr_t pdoc = scint->getDoc();
  FXString fpath;
  FXString fname;
  if(lopened->getFileStatus(pdoc).find("N") != -1) {
    FX::FXString fullpath = save_filename(getShell(), FX::FXPath::upLevel(
                            dirlist->project(dirlist->getFirstItem()).path));
    if(fullpath.empty())
      return 0;
    fpath = FX::FXPath::directory(fullpath);
    fname = FX::FXPath::name(fullpath);
    lopened->setFilePath(pdoc, fpath);
    lopened->setFileName(pdoc, fname);
  } else {
    fpath = lopened->getFilePath(pdoc);
    fname = lopened->getFileName(pdoc);
  }
  scint->save_file(fpath.text(), fname.text());
  lopened->setModified(pdoc, false);
  return 1;
}

ProxivisionWindow::ProxivisionWindow() {}

TabDockBar* ProxivisionWindow::add(FX::FXTabBook* book, FX::FXString const& title, FX::FXToolBarShell* tbshell)
{
  if(!book) { printf("Error: Can't add tab to null tab-book"); return 0; }

  TabDockTitle* ttab = new TabDockTitle(book, title, 0,
    TabDockBar::ID_TOOLBARGRIP, FX::FRAME_RAISED | FX::LAYOUT_FILL_X, 0,0,0,0,2,2,1,1);
  TabDockBar *tbar = new TabDockBar(book, tbshell, FX::LAYOUT_FILL|FX::FRAME_NONE);
  ttab->setTarget(tbar);

  return tbar;
}

#define BUTTON_STYLE (FX::ICON_ABOVE_TEXT|FX::BUTTON_TOOLBAR|FX::FRAME_RAISED|\
FX::LAYOUT_TOP|FX::LAYOUT_LEFT)

void ProxivisionWindow::add(EMBED_ICON::eID actionid, FX::FXObject* tgt,
 FX::FXSelector sel, char const* text)
{
  FX::FXString actionname = getEmbeddedIconName(getApp(), actionid);
  printf("[%s]", actionname.text());
  FX::FXIcon* aicon = getEmbeddedIcon(getApp(), actionid);
  // TODO: load gif and remember in icon cache for later cleanup
  new FX::FXButton(toolbar, text, aicon, tgt, sel, BUTTON_STYLE);
}

ProxivisionWindow::ProxivisionWindow(FX::FXApp *a, FX::FXIcon* pvicon) :
 FX::FXMainWindow(a, "Proximal Vision", pvicon, pvicon, FX::DECOR_ALL, 0, 0, 800, 600)
{
  FX::FXVerticalFrame* base_splitter = new FX::FXVerticalFrame(this, FX::LAYOUT_SIDE_TOP|FX::LAYOUT_FILL);

  GuidedSplitter* splitter = new GuidedSplitter(base_splitter, FX::LAYOUT_SIDE_TOP|FX::LAYOUT_FILL |
    GuidedSplitter::SPLITTER_TRACKING);

  //Field left
  FX::FXVerticalFrame *lfr = new FX::FXVerticalFrame(splitter, FX::LAYOUT_FILL|FX::FRAME_SUNKEN,
    0, 0, 170, 0, 1, 1, 1, 1);
  new FX::FXDockTitle(lfr, "Tabovi 1", 0, 0, FX::LAYOUT_FILL_X|FX::FRAME_RAISED);
  FX::FXTabBook* tbdock = new FX::FXTabBook(lfr, this, ID_LAST, FX::LAYOUT_FILL|FX::FRAME_RAISED); {
    dragshelltabs1=new FX::FXToolBarShell(this, FX::FRAME_LINE|FX::DECOR_RESIZE);
    TabDockBar* tbar = add(tbdock, "1. tab", dragshelltabs1);
    dirlist=new ProjectList(tbar, this, ID_OPEN_TREE, FX::LAYOUT_FILL);
  }{
    dragshelltabs2=new FX::FXToolBarShell(this, FX::FRAME_LINE|FX::DECOR_RESIZE);
    TabDockBar* tbar = add(tbdock, "2. tab", dragshelltabs2);
    new FX::FXText(tbar, this, ID_LAST, FX::LAYOUT_FILL|FX::TEXT_SHOWACTIVE);
  }
  //Field central
  GuidedSplitter* splitter_central=new GuidedSplitter(splitter, FX::LAYOUT_SIDE_TOP|FX::LAYOUT_FILL|
    GuidedSplitter::SPLITTER_VERTICAL|GuidedSplitter::SPLITTER_TRACKING);

  //    Field top
  FX::FXTabBook *tbtop = new FX::FXTabBook(splitter_central, this, ID_LAST, FX::LAYOUT_FILL|
    FX::FRAME_SUNKEN, 0, 0, 0, 120); {
    new FX::FXTabItem(tbtop, "Output", 0, FX::TAB_TOP_NORMAL, 0, 0, 0, 0, 2, 2, 1, 1);
    textout = new FX::FXText(tbtop, this, ID_LAST, FX::LAYOUT_FILL|FX::TEXT_SHOWACTIVE,
      0, 0, 0, 120, 1, 1, 1, 1);
  }{
    new FX::FXTabItem(tbtop, "DocList", 0, FX::TAB_TOP_NORMAL, 0, 0, 0, 0, 2, 2, 1, 1);
    lopened = new DocumentList(tbtop, this, ID_OPEN_OLIST);
  }
  //    Field bottom
  // FX::FXVerticalFrame* scibox = new FX::FXVerticalFrame(splitter_central, FX::LAYOUT_FILL|
  //  FX::FRAME_SUNKEN, 0, 0, 450, 0, 1, 1, 1, 1);
  toolbar=new FX::FXToolBar(splitter_central, FX::LAYOUT_TOP|FX::LAYOUT_LEFT|FX::LAYOUT_FILL_X|FX::FRAME_RAISED, 0, 0, 0, 30);
	scint = new ScintillaEditor(splitter_central, this, ID_EDITOR, FX::LAYOUT_FILL|FX::FRAME_RAISED);
  //scint->setTabIndents();
//  scint->sendMessage(SCI_SETMULTIPLESELECTION,false,0);
//  scint->sendMessage(SCI_SETADDITIONALSELECTIONTYPING,true,0);

  //Field right
  FX::FXVerticalFrame *rfr = new FX::FXVerticalFrame(splitter, FX::LAYOUT_FILL|FX::FRAME_SUNKEN,
    0, 0, 170, 0, 1, 1, 1, 1);
  new FX::FXDockTitle(rfr, "Tabovi 2", 0, 0, FX::LAYOUT_FILL_X|FX::FRAME_RAISED);
  new FX::FXTabBook(rfr, 0, 0, FX::LAYOUT_FILL|FX::FRAME_RAISED);

  splitter->SetStretcher(splitter_central);

  statusbar = new FX::FXStatusBar(base_splitter, FX::LAYOUT_SIDE_BOTTOM|FX::LAYOUT_FILL_X|
    FX::STATUSBAR_WITH_DRAGCORNER|FX::FRAME_RAISED);

  // splitter->setBarSize(420);

  // Info about the editor
  //FX::FXIcon* pvicon = FX::FXIconSource::defaultIconSource.loadIconData(a, ___proxivision_proxivision_ptr, "png");
  //Proxivision::instance()->getIconFromFile(
   //                    "/home/igor/Projects/ProxivisionWorkspace/proxivision/proxivision.png");
  new FX::FXButton(statusbar, "\tAbout Proxivision\tAbout the Proxivision IDE.",
    pvicon, this, ID_ABOUT, FX::LAYOUT_FILL_Y|FX::LAYOUT_RIGHT);

  // Show readonly state in status bar
  FX::FXLabel* readonly = new FX::FXLabel(statusbar,"RWPlace", NULL, FX::FRAME_SUNKEN|
    FX::JUSTIFY_RIGHT|FX::LAYOUT_RIGHT|FX::LAYOUT_CENTER_Y,0,0,0,0,2,2,1,1);
  readonly->setTarget(this);
  readonly->setSelector(ID_READONLY);

  // Show insert mode in status bar
  FX::FXLabel* overtype = new FX::FXLabel(statusbar, "INS/REPLPlace", NULL, FX::FRAME_SUNKEN|
    FX::LAYOUT_RIGHT|FX::LAYOUT_CENTER_Y,0,0,0,0,2,2,1,1);
  overtype->setTarget(this);
  overtype->setSelector(ID_OVERTYPE);

  dirlist->add(FX::FXPath::expand("proxivision"), "proxivision.cbp");
  dirlist->add(FX::FXPath::expand("fox"), "fox.cbp");
  //dirlist->add("/home/igor/tmp/test", "test.cbp");
  //dirlist->add("/home/igor/tmp/test2", "test2.cbp");
  dirlist->expandTree(dirlist->getFirstItem());
  dirlist->expandTree(dirlist->getFirstItem()->getNext());

  // Show active target in status bar
//  popup_targets = new LabelPopup(statusbar, this, ID_TARGET, FX::FRAME_SUNKEN|
//    FX::LAYOUT_RIGHT|FX::LAYOUT_CENTER_Y);
  popup_targets = new LabelPopup(statusbar, "Mjesto", NULL, FX::FRAME_SUNKEN|FX::LAYOUT_RIGHT|FX::LAYOUT_CENTER_Y,0,0,0,0,2,2,1,1);
  popup_targets->setTarget(this);
  popup_targets->setSelector(ID_TARGET);

  // Toobar buttons: File manipulation
  add(EMBED_ICON::DOCUMENT_NEW, this, ID_DOCNEW, "\tNew\tCreate new document.");
  add(EMBED_ICON::DOCUMENT_OPEN, this, ID_DOCOPEN, "\tOpen\tOpen document file.");
  add(EMBED_ICON::DOCUMENT_SAVE, this, ID_DOCSAVE, "\tSave\tSave document.");
  add(EMBED_ICON::DOCUMENT_SAVE_AS, this, ID_DOCSAVEAS, "\tSave As\tSave document to another file.");
  add(EMBED_ICON::DOCUMENT_SAVE_ALL, this, ID_DOCSAVEALL, "\tSave All\tSave all documents and projects.");
  // Editing
  new FX::FXSeparator(toolbar,FX::SEPARATOR_GROOVE);
  add(EMBED_ICON::EDIT_UNDO, (FX::FXObject*)scint, ScintillaEditor::ID_EDITUNDO, "\tUndo\tUndo last change.");
  add(EMBED_ICON::EDIT_REDO, (FX::FXObject*)scint, ScintillaEditor::ID_EDITREDO, "\tRedo\tRedo last undo.");
  add(EMBED_ICON::EDIT_CUT, (FX::FXObject*)scint, ScintillaEditor::ID_EDITCUT, "\tCut\tCut selection to clipboard.");
  add(EMBED_ICON::EDIT_COPY, (FX::FXObject*)scint, ScintillaEditor::ID_EDITCOPY, "\tCopy\tCopy selection to clipboard.");
  add(EMBED_ICON::EDIT_PASTE, (FX::FXObject*)scint, ScintillaEditor::ID_EDITPASTE, "\tPaste\tPaste clipboard.");
  add(EMBED_ICON::EDIT_DELETE, (FX::FXObject*)scint, ScintillaEditor::ID_EDITCLEAR, "\tDelete\t\tDelete selection.");
  add(EMBED_ICON::FORMAT_SHIFT_LEFT, (FX::FXObject*)scint, ScintillaEditor::ID_EDITSHLEFT, "\tShift left\tShift text left by one.");
  add(EMBED_ICON::FORMAT_SHIFT_RIGHT, (FX::FXObject*)scint, ScintillaEditor::ID_EDITSHRIGHT, "\tShift right\tShift text right by one.");
  // Search
  new FX::FXSeparator(toolbar,FX::SEPARATOR_GROOVE);
  add(EMBED_ICON::SEARCH, (FX::FXObject*)scint, ScintillaEditor::ID_EDITSEARCH, "\tSearch\tSearch text.");
  add(EMBED_ICON::SEARCH_NEXT, (FX::FXObject*)scint, ScintillaEditor::ID_EDITSEARCHNEXT, "\tSearch Next Selected\tSearch next occurrence of selected text.");
  add(EMBED_ICON::SEARCH_PREV, (FX::FXObject*)scint, ScintillaEditor::ID_EDITSEARCHPREV, "\tSearch Previous Selected\tSearch previous occurrence of selected text.");
  add(EMBED_ICON::GO_BACKWARD, (FX::FXObject*)scint, ID_GOPREV, "\tPrevious View\tGoto previous view.");
  add(EMBED_ICON::GO_FORWARD, (FX::FXObject*)scint, ID_GONEXT, "\tNext View\tGoto next view.");
  // Font
  new FX::FXSeparator(toolbar,FX::SEPARATOR_GROOVE);
  add(EMBED_ICON::FONTS, this, ID_FONTCHANGE, "\tFonts\tDisplay font dialog.");

  // printf("Icon dictionary size = %u\n", Proxivision::instance()->size_of_icondict());
}

ProxivisionWindow::~ProxivisionWindow()
{
  for(FX::FXint row = 0; row<lopened->getNumRows(); row++) {
    opened_doc_t const* pod = lopened->getFileData(row);
    scint->unreferenceDoc(pod->pdoc);
  }
  delete dragshelltabs2;
  delete dragshelltabs1;
}

void ProxivisionWindow::create()
{
  FX::FXMainWindow::create();
  dragshelltabs1->create();
  dragshelltabs2->create();
  statusbar->show();
  show(FX::PLACEMENT_SCREEN);
}

FX::FXbool ProxivisionWindow::close(FX::FXbool notify)
{
//  if(!saveChanges()) return false;// Prompt to save changes
//  writeRegistry();

  // Perform normal close stuff
  return FXMainWindow::close(notify);
}

void ProxivisionWindow::detach()
{
  FX::FXMainWindow::detach();
  dragshelltabs2->detach();
  dragshelltabs1->detach();
}

FX::FXString ProxivisionWindow::save_filename(FX::FXWindow* wshell, FX::FXString const& path)
{
  FX::FXFileDialog savedialog(wshell, "Save file as");
  savedialog.setSelectMode(FX::SELECTFILE_ANY);
  savedialog.setDirectory(path);
  savedialog.setPatternList("All Files (*)");
  savedialog.setCurrentPattern(0);
  do {
    if(!savedialog.execute()) return FXString::null;

    FX::FXString fullpath = savedialog.getFilename();
    if(fullpath.empty()) return FX::FXString::null;
    if(!FX::FXStat::exists(fullpath)) return fullpath;

    FX::FXuint force = FX::FXMessageBox::question(wshell, FX::MBOX_YES_NO_CANCEL, "Overwrite?",
              "%s:\n%s\n\n%s", "File exists", fullpath.text(), "Do you want to replace it?");
    switch(force) {
      case FX::MBOX_CLICKED_CANCEL: return FX::FXString::null;
      case FX::MBOX_CLICKED_YES: return fullpath;
    }
  } while(1);
}

FX::FXString ProxivisionWindow::open_filename(FX::FXWindow* wshell, FX::FXString const& path)
{
  FX::FXFileDialog opendialog(wshell, "Open file");
  opendialog.setSelectMode(FX::SELECTFILE_EXISTING);
  opendialog.setDirectory(path);
  opendialog.setPatternList("All Files (*)");
  opendialog.setCurrentPattern(0);

  if(!opendialog.execute()) return FXString::null;

  FXString filename = opendialog.getFilename();
  if(!FX::FXStat::isFile(filename)) return FXString::null;
  return filename;
}

