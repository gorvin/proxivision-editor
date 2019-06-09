
#include "DocumentList.h"

// Map
FXDEFMAP(DocumentList) DocumentListMap[]={
//  FXMAPFUNC(FX::SEL_MOTION,0,GuidedSplitter::onMotion),
};

// Object implementation
FXIMPLEMENT(DocumentList, FX::FXTable, DocumentListMap, ARRAYNUMBER(DocumentListMap))

DocumentList::DocumentList(FX::FXComposite *p, FX::FXObject* tgt, FX::FXSelector sel,
  FX::FXuint opts, FX::FXint x, FX::FXint y, FX::FXint w, FX::FXint h) :
   FX::FXTable(p,tgt,sel,opts|FX::VSCROLLER_ALWAYS|FX::TABLE_NO_COLSELECT|FX::TABLE_COL_SIZABLE|FX::TABLE_READONLY,x,y,w,h)
{
  setRowHeaderWidth(0);
  insertColumns(getNumColumns(), 3);
  setColumnText(0, "*"); setColumnWidth(0, 15); setColumnJustify(0, FX::FXHeaderItem::LEFT);
  setColumnText(1, "File"); setColumnWidth(1, 150); setColumnJustify(1, FX::FXHeaderItem::LEFT);
  setColumnText(2, "Path"); setColumnWidth(2, 300); setColumnJustify(2, FX::FXHeaderItem::LEFT);
}

DocumentList::~DocumentList()
{
  for(FX::FXint row = 0; row<nrows; row++) delete getFileData(row);
}

void DocumentList::add_doc(FX::FXString const& path, FX::FXString const& filename, sptr_t pdoc, FX::FXTreeItem* item)
{
  FX::FXint nrows=getNumRows();
  insertRows(nrows);

  setItemText(nrows, 0, "N"); setItemJustify(nrows, 0, FX::FXTableItem::LEFT);

  setItemText(nrows, 1, filename); setItemJustify(nrows, 1, FX::FXTableItem::LEFT);
  setItemData(nrows, 1, new opened_doc_t(pdoc, item));

  setItemText(nrows, 2, path); setItemJustify(nrows, 2, FX::FXTableItem::LEFT);

  layout();//da se uzme u obzir novi red pri racunanju rasporeda u makePositionVisible
  selectRow(nrows);
  makePositionVisible(nrows, 0);
}

sptr_t DocumentList::selectDoc(FX::FXString const& fpath, FX::FXString const& fname)
{
  FX::FXString fullpath(fpath); fullpath.append(PATHSEP).append(fname);
  for(FX::FXint row = 0; row<nrows; row++) {
    FX::FXString rowfull(getFilePath(row)); rowfull.append(PATHSEP).append(getFileName(row));
    if(FX::compare(rowfull, fullpath)) continue;
    selectRow(row);
    makePositionVisible(row, 0);
    return getFileData(row)->pdoc;
  }
  return 0;
}

//sptr_t DocumentList::selectDoc(FX::FXTreeItem* item)
//{
//  for(FX::FXint row = 0; row<nrows; row++) {
//    opened_doc_t const* pod = getFileData(row);
//    if(pod->item == item) { selectRow(row); makePositionVisible(row, 0); return pod->pdoc; }
//  }
//  return 0;
//}

void DocumentList::selectDoc(sptr_t pdoc)
{
  for(FX::FXint row = 0; row<nrows; row++) {
    opened_doc_t const* pod = getFileData(row);
    if(pod->pdoc == pdoc) { selectRow(row); makePositionVisible(row, 0);}
  }
}

void DocumentList::setModified(sptr_t pdoc, bool modified)
{
  FX::FXint row = getRow(pdoc);
  if(row == -1) return;
  setItemText(row, 0, modified ? "*" : " ");
//  for(FX::FXint row = 0; row<nrows; row++) {
//    if(getFileData(row)->pdoc == pdoc) { setItemText(row, 0, modified ? "*" : " "); return; }
//  }
}

FX::FXint DocumentList::getRow(sptr_t pdoc) const
{
  for(FX::FXint row = 0; row<nrows; row++) { if(getFileData(row)->pdoc == pdoc) return row; }
  return -1;
}

void DocumentList::removeDoc(sptr_t pdoc)
{
  for(FX::FXint row = 0; row<nrows; row++) {
    opened_doc_t const* data = getFileData(row);
    if(data->pdoc == pdoc) { delete data; removeRows(row); layout(); return; }
  }
}

void DocumentList::layout()
{
  FX::FXint sz = width - getColumnWidth(0) - getColumnWidth(1) - vertical->getWidth() - 1;
  if(getColumnWidth(2) < sz) setColumnWidth(2, sz);
  FX::FXTable::layout();
}
