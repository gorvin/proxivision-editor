#include "xincs.h"
#include "fxver.h"
#include "fxdefs.h"
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
#include "FXFont.h"
#include "FXFrame.h"
#include "FXLabel.h"
#include "FXButton.h"
#include "FXPacker.h"
#include "FXHorizontalFrame.h"
#include "FXTextField.h"
#include "FXVerticalFrame.h"
#include "FXCheckButton.h"
#include "SearchReplaceDialog.h"

// Map
FXDEFMAP(SearchReplaceDialog) SearchReplaceDialogMap[]={
  FXMAPFUNC(FX::SEL_UPDATE,SearchReplaceDialog::ID_WRAPMODE,SearchReplaceDialog::onUpdWrap),
  FXMAPFUNC(FX::SEL_COMMAND,SearchReplaceDialog::ID_WRAPMODE,SearchReplaceDialog::onCmdWrap),
  };


// Object implementation
FXIMPLEMENT(SearchReplaceDialog,FX::FXReplaceDialog,SearchReplaceDialogMap,ARRAYNUMBER(SearchReplaceDialogMap))


// File Open Dialog
SearchReplaceDialog::SearchReplaceDialog(FX::FXWindow* owner, const FX::FXString& caption,
 FX::FXIcon* ic, FX::FXuint opts, FX::FXint x, FX::FXint y, FX::FXint w, FX::FXint h):
  FXReplaceDialog(owner,caption,ic,opts,x,y,w,h){
  FX::FXVerticalFrame* entry = dynamic_cast<FX::FXVerticalFrame*>(searchlabel->getParent());
  if(!entry) throw "Invalid assumption about base FXReplaceDialog";
  FX::FXHorizontalFrame* options2=new FX::FXHorizontalFrame(entry,FX::LAYOUT_FILL_X,0,0,0,0, 0,0,0,0);
  new FX::FXCheckButton(options2, tr("&Wrap At End of File"), this, ID_WRAPMODE,
    FX::ICON_BEFORE_TEXT|FX::LAYOUT_CENTER_X);
  }


// Cleanup
SearchReplaceDialog::~SearchReplaceDialog() { }


// Change search direction
long SearchReplaceDialog::onCmdWrap(FX::FXObject*,FX::FXSelector,void*){
  searchmode^=FX::SEARCH_WRAP;
  return 1;
  }


// Update search direction
long SearchReplaceDialog::onUpdWrap(FX::FXObject* sender,FX::FXSelector,void*){
  sender->handle(this,FXSEL(FX::SEL_COMMAND,(searchmode&FX::SEARCH_WRAP)? ID_CHECK:ID_UNCHECK),NULL);
  return 1;
  }

