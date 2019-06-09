#ifndef SEARCHREPLACEDIALOG_H
#define SEARCHREPLACEDIALOG_H

#ifndef FXREPLACEDIALOG_H
#include "FXReplaceDialog.h"
#endif

/// Text search dialog
class FXAPI SearchReplaceDialog : public FX::FXReplaceDialog {
  FXDECLARE(SearchReplaceDialog)
protected:
  SearchReplaceDialog(){}
private:
  SearchReplaceDialog(const SearchReplaceDialog&);
  SearchReplaceDialog &operator=(const SearchReplaceDialog&);

public:
  long onCmdWrap(FX::FXObject*,FX::FXSelector sel,void*);
  long onUpdWrap(FX::FXObject*,FX::FXSelector sel,void*);
  enum {
    ID_WRAPMODE = FX::FXReplaceDialog::ID_LAST,
    ID_LAST,
  };
  /// Construct search dialog box
  SearchReplaceDialog(FX::FXWindow* owner, const FX::FXString& caption, FX::FXIcon* ic=NULL,
    FX::FXuint opts=0, FX::FXint x=0, FX::FXint y=0, FX::FXint w=0, FX::FXint h=0);

  /// Destructor
  ~SearchReplaceDialog();
  };


#endif //SEARCHREPLACEDIALOG_H
