#ifndef FUIDEDSPLITTER_H
#define FUIDEDSPLITTER_H

#ifndef FXCOMPOSITE_H
#include "FXComposite.h"
#endif




/**
* Splitter window is used to interactively repartition
* two or more subpanels.
* Space may be subdivided horizontally (SPLITTER_HORIZONTAL, which
* the default) or vertically (SPLITTER_VERTICAL option).
* When the splitter is itself resized, the right-most (bottom-most)
* child window will be resized unless the splitter window is reversed;
* if the splitter is reversed, the left-most (top-most) child window
* will be resized instead.
* The splitter widget sends a SEL_CHANGED to its target
* during the resizing of the panels; at the end of the resize interaction,
* it sends a SEL_COMMAND to signify that the resize operation is complete.
* Normally, children are resizable from 0 upwards; however, if the child
* in a horizontally oriented splitter has LAYOUT_FILL_X in combination with
* LAYOUT_FIX_WIDTH, it will not be made smaller than its default width,
* except when the child is the last visible widget (or first when the option
* SPLITTER_REVERSED has been passed to the splitter).
* In a vertically oriented splitter, children with LAYOUT_FILL_Y and
* LAYOUT_FIX_HEIGHT behave analogously.
*/
class GuidedSplitter : public FX::FXComposite {
  FXDECLARE(GuidedSplitter)
private:
  FX::FXWindow *window;         // Window being resized
  FX::FXint     split;          // Split value
  FX::FXint     offset;         // Mouse offset
  FX::FXint     barsize;        // Size of the splitter bar
  FX::FXWindow* stretcher_explicit;
protected:
  GuidedSplitter();
  void adjustHLayout();
  void adjustVLayout();
  void moveHSplit(FX::FXint amount);
  void moveVSplit(FX::FXint amount);
  void drawHSplit(FX::FXint pos);
  void drawVSplit(FX::FXint pos);
  FX::FXWindow* findHSplit(FX::FXint pos);
  FX::FXWindow* findVSplit(FX::FXint pos);
private:
  GuidedSplitter(const GuidedSplitter&);
  GuidedSplitter& operator=(const GuidedSplitter&);
  FX::FXWindow* getFirstShown(void);
  FX::FXWindow* getLastShown(void);
  FX::FXint childGoodHeight(FX::FXWindow* child);
  FX::FXint childGoodWidth(FX::FXWindow* child);
public:
  long onLeftBtnPress(FX::FXObject*,FX::FXSelector,void*);
  long onLeftBtnRelease(FX::FXObject*,FX::FXSelector,void*);
  long onMotion(FX::FXObject*,FX::FXSelector,void*);
  long onFocusNext(FX::FXObject*,FX::FXSelector,void*);
  long onFocusPrev(FX::FXObject*,FX::FXSelector,void*);
  long onFocusUp(FX::FXObject*,FX::FXSelector,void*);
  long onFocusDown(FX::FXObject*,FX::FXSelector,void*);
  long onFocusLeft(FX::FXObject*,FX::FXSelector,void*);
  long onFocusRight(FX::FXObject*,FX::FXSelector,void*);
public:

  /// Splitter options
  enum {
    SPLITTER_HORIZONTAL = 0,                  /// Split horizontally
    SPLITTER_VERTICAL   = 0x00008000,         /// Split vertically
    SPLITTER_REVERSED   = 0x00010000,         /// Reverse-anchored
    SPLITTER_TRACKING   = 0x00020000,         /// Track continuous during split
    SPLITTER_NORMAL     = SPLITTER_HORIZONTAL
    };

  /// Construct new splitter widget
  GuidedSplitter(FX::FXComposite* p,FX::FXuint opts=SPLITTER_NORMAL,FX::FXint x=0,FX::FXint y=0,FX::FXint w=0,FX::FXint h=0);

  /// Construct new splitter widget, which will notify target about size changes
  GuidedSplitter(FX::FXComposite* p,FX::FXObject* tgt,FX::FXSelector sel,FX::FXuint opts=SPLITTER_NORMAL,FX::FXint x=0,FX::FXint y=0,FX::FXint w=0,FX::FXint h=0);

  /// Get default width
  virtual FX::FXint getDefaultWidth();

  /// Get default height
  virtual FX::FXint getDefaultHeight();

  /// Perform layout
  virtual void layout();

  /// Return size of the panel at index
  FX::FXint getSplit(FX::FXint index) const;

  /// Change the size of panel at the given index
  void setSplit(FX::FXint index,FX::FXint size);

  /// Change splitter style
  void setSplitterStyle(FX::FXuint style);

  /// Return current splitter style
  FX::FXuint getSplitterStyle() const;

  /// Change splitter bar size
  void setBarSize(FX::FXint bs);

  /// Return current bar size
  FX::FXint getBarSize() const { return barsize; }

  void SetStretcher(FX::FXWindow* w);

  /// Save to stream
  virtual void save(FX::FXStream& store) const;

  /// Load from stream
  virtual void load(FX::FXStream& store);

  /// Destroy splitter
  virtual ~GuidedSplitter();
  };

#endif
