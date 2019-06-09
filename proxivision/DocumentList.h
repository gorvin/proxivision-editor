
#include <fx.h>
#include <Scintilla.h>

//class HeaderIntermediate : public virtual FX::FXHeader { };
//class ListIntermediate: public virtual FX::FXTable { };

struct opened_doc_t {
  sptr_t          pdoc;//scinitilla document
  FX::FXTreeItem* item;//ProjectList item
  opened_doc_t(sptr_t pd = 0, FX::FXTreeItem* it = 0) : pdoc(pd), item(it) { }
};

class DocumentList : protected FX::FXTable {
  FXDECLARE(DocumentList)
protected:
  DocumentList(){}
  FX::FXString getFileStatus(FX::FXint row) const { return (row>-1) ? getItemText(row, 0) : FX::FXString::null; }
  FX::FXString getFileName(FX::FXint row) const { return (row>-1) ? getItemText(row, 1) : FX::FXString::null; }
  FX::FXString getFilePath(FX::FXint row) const { return (row>-1) ? getItemText(row, 2) : FX::FXString::null; }
  void setFileStatus(FX::FXint row, FX::FXString const& t) { setItemText(row, 0, t); }
  void setFileName(FX::FXint row, FX::FXString const& t) { setItemText(row, 1, t); }
  void setFilePath(FX::FXint row, FX::FXString const& t) { setItemText(row, 2, t); }
public:
  void layout();
  void add_doc(FX::FXString const& path, FX::FXString const& filename, sptr_t pdoc, FX::FXTreeItem* item);

  opened_doc_t const* getFileData(FX::FXint row) const { return (opened_doc_t*)getItemData(row, 1); }
  FX::FXint getRow(sptr_t pdoc) const;
  void removeDoc(sptr_t pdoc);
  FX::FXint getNumRows() const { return FX::FXTable::nrows; }

  FX::FXString getFileStatus(sptr_t pdoc) const { return getFileStatus(getRow(pdoc)); }
  FX::FXString getFileName(sptr_t pdoc) const { return getFileName(getRow(pdoc)); }
  FX::FXString getFilePath(sptr_t pdoc) const { return getFilePath(getRow(pdoc)); }
  void setFileStatus(sptr_t pdoc, FX::FXString const& t) { setFileStatus(getRow(pdoc), t); }
  void setFileName(sptr_t pdoc, FX::FXString const& t) { setFileName(getRow(pdoc), t); }
  void setFilePath(sptr_t pdoc, FX::FXString const& t) { setFilePath(getRow(pdoc), t); }

  //FX::FXint isOK(void) const { return isRowSelected(getCurrentRow()); }
  //sptr_t selectDoc(FX::FXTreeItem* item);
  sptr_t selectDoc(FX::FXString const& fpath, FX::FXString const& fname);
  void selectDoc(sptr_t pdoc);
  void setModified(sptr_t pdoc, bool modified=true);
  //sptr_t getDoc(FX::FXint row) const { return getFileData(row)->pdoc; }
  //FX::FXTreeItem* getTreeItem(FX::FXint row) const { return getFileData(row)->item; }

  DocumentList(FX::FXComposite *p, FX::FXObject* tgt=NULL, FX::FXSelector sel=0,
   FX::FXuint opts=FX::LIST_NORMAL, FX::FXint x=0, FX::FXint y=0, FX::FXint w=0, FX::FXint h=0);
  ~DocumentList();
};
