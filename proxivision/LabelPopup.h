#ifndef LABELPOPUP_H
#define LABELPOPUP_H

#ifndef FXLABEL_H
#include "FXLabel.h"
#endif

namespace FX {
  class FXPopup;
  class FXList;
}

// Menu button options
enum {
  MENUBUTTON_AUTOGRAY      = 0x00800000,                                      /// Automatically gray out when no target
  MENUBUTTON_AUTOHIDE      = 0x01000000,                                      /// Automatically hide when no target
  MENUBUTTON_TOOLBAR       = 0x02000000,                                      /// Toolbar style
//  MENUBUTTON_DOWN          = 0,                                               /// Popup window appears below menu button
//  MENUBUTTON_UP            = 0x04000000,                                      /// Popup window appears above menu button
//  MENUBUTTON_LEFT          = 0x08000000,                                      /// Popup window to the left of the menu button
//  MENUBUTTON_RIGHT         = MENUBUTTON_LEFT|MENUBUTTON_UP,                   /// Popup window to the right of the menu button
  };



/**
* A menu button posts a popup menu when clicked.
* There are many ways to control the placement where the popup will appear;
* first, the popup may be placed on either of the four sides relative to the
* menu button; this is controlled by the flags MENUBUTTON_DOWN, etc.
* Next, there are several attachment modes; the popup's left/bottom edge may
* attach to the menu button's left/top edge, or the popup's right/top edge may
* attach to the menu button's right/bottom edge, or both.
* Also, the popup may apear centered relative to the menu button.
* Finally, a small offset may be specified to displace the location of the
* popup by a few pixels so as to account for borders and so on.
* Normally, the menu button shows an arrow pointing to the direction where
* the popup is set to appear; this can be turned off by passing the option
* MENUBUTTON_NOARROWS.
*/
class FXAPI LabelPopup : public FX::FXLabel {
  FXDECLARE(LabelPopup)
protected:
  FX::FXPopup *pane;                  // Pane to pop up
  FX::FXList  *list;
  FX::FXint    offsetx;               // Shift attachment point x
  FX::FXint    offsety;               // Shift attachment point y
  FX::FXbool   state;                 // Pane was popped
protected:
  LabelPopup();
private:
  LabelPopup(const LabelPopup&);
  LabelPopup &operator=(const LabelPopup&);
public:
  //long onPaint(FX::FXObject*,FX::FXSelector,void*);
  long onUpdate(FX::FXObject*,FX::FXSelector,void*);
  long onEnter(FX::FXObject*,FX::FXSelector,void*);
  long onLeave(FX::FXObject*,FX::FXSelector,void*);
  long onFocusIn(FX::FXObject*,FX::FXSelector,void*);
  long onFocusOut(FX::FXObject*,FX::FXSelector,void*);
  long onUngrabbed(FX::FXObject*,FX::FXSelector,void*);
  long onMotion(FX::FXObject*,FX::FXSelector,void*);
  long onLeftBtnPress(FX::FXObject*,FX::FXSelector,void*);
  long onLeftBtnRelease(FX::FXObject*,FX::FXSelector,void*);
  long onKeyPress(FX::FXObject*,FX::FXSelector,void*);
  long onKeyRelease(FX::FXObject*,FX::FXSelector,void*);
  long onHotKeyPress(FX::FXObject*,FX::FXSelector,void*);
  long onHotKeyRelease(FX::FXObject*,FX::FXSelector,void*);
  //Prikazuje panel ali prije salje SEL_COMMAND FX::FXWindow::ID_QUERY_MENU da target popuni panel
  long onCmdPost(FX::FXObject*,FX::FXSelector,void*);
  long onCmdUnpost(FX::FXObject*,FX::FXSelector,void*);
  long onListUpdate(FX::FXObject*,FX::FXSelector,void*);
  long onListChanged(FX::FXObject*,FX::FXSelector,void* ptr);
  long onListClicked(FX::FXObject*,FX::FXSelector sel,void* ptr);

public:
  enum {
    ID_LIST = FX::FXLabel::ID_LAST,
    ID_LAST,
  };
  /// Constructor
  LabelPopup(FX::FXComposite* p, const FX::FXString& text, FX::FXIcon* ic = NULL,
   FX::FXuint opts = FX::JUSTIFY_NORMAL|FX::ICON_BEFORE_TEXT,
   FX::FXint x=0, FX::FXint y=0, FX::FXint w=0, FX::FXint h=0, FX::FXint pl = FX::DEFAULT_PAD,
   FX::FXint pr = FX::DEFAULT_PAD, FX::FXint pt = FX::DEFAULT_PAD, FX::FXint pb = FX::DEFAULT_PAD);

  /// Create server-side resources
  virtual void create();

  /// Detach server-side resources
  virtual void detach();

  /// Returns true because a menu button can receive focus
  virtual bool canFocus() const;

  /// Remove the focus from this window
  virtual void killFocus();

  /// Return true if window logically contains the given point
  virtual bool contains(FX::FXint parentx,FX::FXint parenty) const;

  /// Change the popup menu
  //void setMenu(FX::FXPopup *pup);

  /// Return current popup menu
  //FX::FXPopup* getMenu() const { return pane; }

  /// Set X offset where menu pops up relative to button
  void setXOffset(FX::FXint offx){ offsetx=offx; }

  /// Return current X offset
  FX::FXint getXOffset() const { return offsetx; }

  /// Set Y offset where menu pops up relative to button
  void setYOffset(FX::FXint offy){ offsety=offy; }

  /// Return current Y offset
  FX::FXint getYOffset() const { return offsety; }

  /// Change menu button style
  void setButtonStyle(FX::FXuint style);

  /// Get menu button style
  FX::FXuint getButtonStyle() const;

  /// Save menu button to a stream
  virtual void save(FX::FXStream& store) const;

  /// Load menu button from a stream
  virtual void load(FX::FXStream& store);

  /// Destructor
  virtual ~LabelPopup();
  };

#endif //LABELPOPUP_H
