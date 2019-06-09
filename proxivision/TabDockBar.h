#include <fx.h>

class TabDockTitle : public FX::FXDockTitle {
  FXDECLARE(TabDockTitle)
protected:
  TabDockTitle() {}
public:
  long onLeftBtnPress(FX::FXObject*,FX::FXSelector,void*);
  long onLeftBtnRelease(FX::FXObject*,FX::FXSelector,void*);
  long onMapEvent(FX::FXObject*,FX::FXSelector,void*);

  TabDockTitle(FX::FXComposite* p,const FX::FXString& text,
   FX::FXObject* tgt=NULL,FX::FXSelector sel=0,
   FX::FXuint opts=FX::FRAME_NORMAL|FX::JUSTIFY_CENTER_X|FX::JUSTIFY_CENTER_Y,
   int x=0,int y=0,int w=0,int h=0,int pl=0,int pr=0,int pt=0,int pb=0);
};

class TabDockBar : public FX::FXPacker {
  FXDECLARE(TabDockBar)
protected:
  FX::FXComposite *drydock;	        // Parent when docked
  FX::FXComposite *wetdock;	        // Parent when floating
  int              gripx;           // Grip offset x
  int              gripy;           // Grip offset y

protected:
  TabDockBar();

private:
  TabDockBar(const TabDockBar&);
  TabDockBar &operator=(const TabDockBar&);

public:
  long onBeginDragGrip(FX::FXObject*,FX::FXSelector,void*);
  long onEndDragGrip(FX::FXObject*,FX::FXSelector,void*);
  long onDraggedGrip(FX::FXObject*,FX::FXSelector,void*);
  long onDockTimer(FX::FXObject*,FX::FXSelector,void*);

public:
  enum {
    ID_TOOLBARGRIP=FX::FXPacker::ID_LAST,    /// Tool bar grip
    ID_TIMER,
    ID_LAST
    };

  /**
  * Construct a floatable dock bar, with a default parent p and an
  * alternate parent q.  To allow docking and dragging the default parent
  * p must be of type FXDockSite, and the alternate parent q must be of
  * type FXToolBarShell.
  * Normally, the dock bar is docked under a window p of type FXDockSite.
  * When floated, the toolbar can be docked under window q, which is
  * usually an kind of FXToolBarShell window.
  */
  TabDockBar(FX::FXComposite* p,FX::FXComposite* q,FX::FXuint opts=FX::LAYOUT_TOP|FX::LAYOUT_LEFT|FX::LAYOUT_FILL_X,int x=0,int y=0,int w=0,int h=0,int pl=3,int pr=3,int pt=2,int pb=2,int hs=FX::DEFAULT_SPACING,int vs=FX::DEFAULT_SPACING);

  /// Return true if docked
  FX::FXbool isDocked() const;

  /**
  * Set parent when docked.
  * If it was docked, reparent under the new docking window.
  */
  void setDryDock(FX::FXComposite* dry);

  /**
  * Set parent when floating.
  * If it was undocked, then reparent under the new floating window.
  */
  void setWetDock(FX::FXComposite* wet);

  /// Return parent when docked
  FX::FXComposite* getDryDock() const { return drydock; }

  /// Return parent when floating
  FX::FXComposite* getWetDock() const { return wetdock; }

  /// Search for dock close to coordinates rootx, rooty
  FX::FXTabBook* findDockNear(int rootx,int rooty);

  /**
  * Dock the bar against the given side, near the given position relative
  * to the toolbar dock's origin.
  */
  virtual void dock(FX::FXTabBook* docksite,FX::FXbool notify);

  /**
  * Undock or float the bar.
  * The initial position of the wet dock is a few pixels
  * below and to the right of the original docked position.
  */
  virtual void undock(int rootx,int rooty,FX::FXbool notify=false);

  /// Save toolbar to a stream
  virtual void save(FX::FXStream& store) const;

  /// Load toolbar from a stream
  virtual void load(FX::FXStream& store);

  /// Destroy
  virtual ~TabDockBar();
  };
