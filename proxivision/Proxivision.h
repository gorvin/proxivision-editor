#include "stdutils.h"
#include <fx.h>

// class IconCache;

struct env_prm_t {
  enum flag_t {
    FToolbar      = 0x01u,
    FStatusBar    = 0x02u,
    FOutputPane   = 0x04u,
    FWrapToolbar  = 0x08u,
    FTabsBelow    = 0x10u,
    FWatchChanges = 0x20u,
    FAutosave     = 0x40u,
    FAllEnvFlagsOn= 0x7fu,
  };
  int flags;
  int OutputPaneHeight;
  int MaxFiles;
  int AutosaveInterval;
  int WheelLines;
  int SearchOptions;
  bool SearchWrapAsk;
  FX::FXString ShellCommand;
  FX::FXString FileFilters;
  env_prm_t(void);
};

struct edit_prm_t {
  enum flag_t {
    FSingleByte   = 0x001u,
    FUseTabs      = 0x002u,
    FSmartHome    = 0x004u,
    FBraceMatch   = 0x008u,
    FLineNumbers  = 0x010u,
    FRightEdge    = 0x020u,
    FIndentGuides = 0x040u,
    FSmoothScroll = 0x080u,
    FAutoIndent   = 0x100u,
    FSearchWrap   = 0x200u,
    FAllEditFlagsOn=0x3ffu,
  };
  int flags;
  int TabWidth;
  int CaretWidth;
  int RightEdgeColumn;
  int ZoomFactor;
  bool AutoIndentSmart;
  int DefaultEOL;
  int FontSize;//TODO: Trebali bi razliciti fontovi za okruzenje i editor
  FX::FXString FontName;
  edit_prm_t(void);
};

class Proxivision : public FX::FXApp {
  FXDECLARE(Proxivision)
  FX::FXString                        theme;
  FX::FXString                        appIconPath;
  //stdutils::auto_ptr<FX::FXIconCache> aicodict;
  FX::FXIconDictionary                knownIcons;
  // stdutils::auto_ptr<IconCache>       aicache;
public:
  //FX::FXint DefaultFileFormat;
  env_prm_t  defaults_env;
  edit_prm_t defaults_edit;
private:
  Proxivision();
  explicit Proxivision(const Proxivision&);
  Proxivision& operator=(const Proxivision&);
public:
  enum{
    ID_CLOSEALL=FXApp::ID_LAST,
    ID_LAST
    };
public:
  long onCmdCloseAll(FX::FXObject*,FX::FXSelector,void*);
public:
  Proxivision(const FX::FXString& name);
  virtual void init(int& argc,char** argv,bool connect=true);
  // FX::FXIcon* getIcon(FX::FXString const& fname_noext);
  FX::FXIcon* getMimeIcon(FX::FXString const& for_fname);
  FX::FXIcon* getIconFromFile(FX::FXString const& filepath);
  FX::FXIcon* getMenuIcon(FX::FXString const& action_name);
//  FX::FXint size_of_icondict(void) const;
  virtual ~Proxivision();
  static inline Proxivision* instance() { return dynamic_cast<Proxivision*>(FX::FXApp::instance());}
  };
