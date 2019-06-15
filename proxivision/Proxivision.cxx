#include "Proxivision.h"
#include "ProxivisionWindow.h"
//#include "IconCache.h"
#include "StringPartitioner.h"
#include <string>

// #ifndef _WIN32
// # include "xsettings-client.h" //theme
// #endif

#include <fx.h>

#ifndef _MSC_VER
# include <unistd.h>
# include <signal.h>
#else
# include <io.h>
#endif
#include <Scintilla.h> //DEFAULT_EOL

#include <vector>

#ifdef _WIN32
#define DEFAULT_EOL SC_EOL_CRLF
#else
#define DEFAULT_EOL SC_EOL_LF
#endif


#define FILE_FILTERS "All Files (*)|"\
              "C/C++ files (*.[ch],*.[ch]pp,*.[ch]xx,*.[CH],*.cc,*.hh)|"\
              "Script files (*.sh,*.lua,*.rb,*.pl,*.py)|"\
              "Config files (config*,*cfg,*conf,*.ini,*rc,.*)|"\
              "Makefiles ([Mm]akefile*,*.mk,CMake*,*.cmake*,Jam*)|"\
              "Web files (*html,*.htm,*.php*)|"\
              "Text files (*.txt)|"

env_prm_t::env_prm_t(void) : flags(FAllEnvFlagsOn), OutputPaneHeight(64), MaxFiles(128),
 AutosaveInterval(60), WheelLines(3), SearchOptions(0), SearchWrapAsk(true),
 ShellCommand("/bin/sh -c"), FileFilters(FILE_FILTERS) {
  FileFilters.substitute("|", "\n", true);
}

FX::FXbool LocaleIsUTF8(){
#if FOX_MAJOR*10+FOX_MINOR <=16
//# ifdef _WIN32
//    return GetACP()==CP_UTF8;
//# else
    const FX::FXchar* str;
    if((str=getenv("LC_CTYPE"))!=NULL || (str=getenv("LC_ALL"))!=NULL || (str=getenv("LANG"))!=NULL){
      return (strstr(str,"utf")!=NULL || strstr(str,"UTF")!=NULL);
    }
    return false;
//# endif
#else
  return FX::FXSystem::localeIsUTF8();
#endif
}

edit_prm_t::edit_prm_t(void) : flags(FAllEditFlagsOn), TabWidth(4),
 CaretWidth(1), RightEdgeColumn(80), ZoomFactor(0), AutoIndentSmart(true),
 DefaultEOL(DEFAULT_EOL), FontSize(120), FontName("Fixed [Misc]")
{
  if(!LocaleIsUTF8()) flags&=~FSingleByte;
}

// Map
FXDEFMAP(Proxivision) ProximalNebulaMap[]={
  FXMAPFUNC(FX::SEL_SIGNAL,Proxivision::ID_CLOSEALL,Proxivision::onCmdCloseAll),
  FXMAPFUNC(FX::SEL_COMMAND,Proxivision::ID_CLOSEALL,Proxivision::onCmdCloseAll),
};

FXIMPLEMENT(Proxivision,FX::FXApp,ProximalNebulaMap,ARRAYNUMBER(ProximalNebulaMap))

Proxivision::Proxivision(void) { }

Proxivision::Proxivision(const FX::FXString& name)
 : FX::FXApp(name,FX::FXString::null)
{
#ifndef DEBUG
//If interrupt happens, quit gracefully;we may want to save edit buffer contents
//w/o asking if display gets disconnected or if hangup signal is received.
  addSignal(SIGINT,this,ID_CLOSEALL);
#ifndef _WIN32
  addSignal(SIGQUIT,this,ID_CLOSEALL);
  addSignal(SIGHUP,this,ID_CLOSEALL);
  addSignal(SIGPIPE,this,ID_CLOSEALL);
#endif
#endif
}

void Proxivision::init(int& argc,char** argv,bool connect)
{
  FX::FXApp::init(argc,argv,connect);
  // After init, the registry has been loaded and we know the icon search path
  //associations->setIconPath(reg().readStringEntry("SETTINGS","iconpath",FX::FXIconDictionary::defaultIconPath));

  //TODO: koristi svoje mimetypes-ikone u svom pathu, ako neku nemas onda koristi sistemsku
  printf("Icon path = ");
  appIconPath = FX::FXPath::expand("~/.local/share/icons");
  StringPartitioner part(FX::FXIconCache::defaultIconPath, PATHLISTSEP);
  while(part.next()) {
    FX::FXString pathpart(FX::FXPath::expand(part.mid()));
    if(!FX::FXStat::isDirectory(pathpart)) { 
      printf("(invalid %s)", pathpart.text()); 
      continue; 
    }
    appIconPath.append(PATHLISTSEP).append(pathpart);
  }
  printf("%s\n", appIconPath.text());
  //aicodict.reset(new FX::FXIconCache(this, myiconpath));

// #ifndef _WIN32
//   XSettingsClient * client = xsettings_client_new((Display*)getDisplay(), 0, NULL, NULL, NULL);
//   XSettingsSetting* setting=0;
//   xsettings_client_get_setting (client, "Net/IconThemeName", &setting);
//   //printf("Get xsettings status = %u\n", val);

//   if(!setting) {
//     printf("Didn't get X client settings\n");
//   } else {
//     theme = setting->data.v_string;
//     printf("Tema = %s\n", setting->data.v_string);
//     xsettings_setting_free(setting);
//   }
//   xsettings_client_destroy(client);
// #endif
  // aicache.reset(new IconCache(theme, "16x16", appIconPath/*aicodict.get()->getIconPath()*/));
}

long Proxivision::onCmdCloseAll(FX::FXObject*,FX::FXSelector,void*){
  //while(0<windowlist.no() && windowlist[0]->close(true));
// TODO: Close all windows
  return 1;
}

Proxivision::~Proxivision(){ }

// FX::FXIcon* Proxivision::getIcon(FX::FXString const& iconNameNoExt)
// {
//   FX::FXString iname(aicache->find_icon(iconNameNoExt));
//   if(iname.empty())
//     return NULL;
//   // return aicodict->insert(iname.text());
//   return getIconFromFile(iname);
// }

FX::FXIcon* Proxivision::getMimeIcon(FX::FXString const& for_fname)
{
  if(knownIcons.has(for_fname))
    return knownIcons[for_fname];
  //FX::FXIcon* icon = aicache->find_mimetype_icon(this, for_fname);

  std::string const sName(for_fname.text());
  std::string sExt(sName.substr(sName.rfind(".") + 1));
  if(sExt == "cxx")
    sExt = "cpp";
  if(sExt == ".hxx")
    sExt = "hpp";
  
  std::string const fType(std::string() + "filetype-" + sExt);
  if(knownIcons.has(fType.c_str()))
    return knownIcons[fType.c_str()];

  FX::FXIcon* icon = getEmbeddedIcon(this, (fType.c_str()));
  if(nullptr != icon)
    knownIcons[fType.c_str()] = icon;
  return icon;
}

FX::FXIcon* Proxivision::getIconFromFile(FX::FXString const& filepath)
{
   if(knownIcons.has(filepath))
    return knownIcons[filepath];
 // return aicodict->insert(filepath.text());
  FX::FXIcon* icon = FX::FXIconSource::defaultIconSource.loadIconFile(this, filepath.text());
  if(nullptr != icon)
    knownIcons[filepath] = icon;
  return icon;
}

FX::FXIcon* Proxivision::getMenuIcon(FX::FXString const& action_name)
{
  if(knownIcons.has(action_name))
    return knownIcons[action_name];
  return nullptr;
}


// FX::FXint Proxivision::size_of_icondict(void) const { return aicodict->no(); }

class StdoutRedirect : public FX::FXThread {
  FX::FXObject* const target;
  bool& finish;
  int fdbkp;
  FX::FXFile fstdout;
  FX::FXFile fredirect;
  FX::FXint run(void);
public:
  StdoutRedirect(FX::FXObject* tgt, bool& finish_thread, char const* pipename);
  ~StdoutRedirect(void);
};

StdoutRedirect::StdoutRedirect(FX::FXObject* tgt, bool& finish_thread, char const* pipename) :
 target(tgt), finish(finish_thread), fdbkp(dup(STDOUT_FILENO)), fstdout(pipename, FX::FXIO::Writing),
 fredirect(pipename, FX::FXIO::Reading)
{
  if(!tgt) exit(-1);
  if(fstdout.handle() == FX::FXInputHandle(-1)) exit(-1);
  dup2((intptr_t)fstdout.handle(), STDOUT_FILENO);
  //printf("This is stdout to file\n"); fflush(stdout);
}

StdoutRedirect::~StdoutRedirect(void)
{
  dup2(fdbkp, STDOUT_FILENO);
  close(fdbkp);
  printf("Back to console\n"); fflush(stdout);
}

FX::FXint StdoutRedirect::run(void)
{
  static const ssize_t BUF_LEN=1024;
  char buf[BUF_LEN];
  FX::FXApp* const app = FX::FXApp::instance();
  FX::FXSelector const selector = FXSEL(FX::SEL_COMMAND,ProxivisionWindow::ID_REDIRECT);
  FX::FXString sbuf;
  while(!finish) {
    ssize_t rcvd=0;
    FX::FXThread::sleep(1000000000ul);//da pocne FXApp::run
    while((rcvd=fredirect.readBlock(buf,BUF_LEN))>0 && !finish) {
      sbuf.assign(buf, rcvd);
      app->mutex().lock();
      target->tryHandle(app, selector, &sbuf);
      app->mutex().unlock();
      if (rcvd<BUF_LEN) { break; }
    }
  }
  finish = false;
  //FX::FXThread::sleep(90000000000ul);
  return 0;
}

typedef std::vector<FX::FXString> vfxstring_t;

void at_semicolon(vfxstring_t& v, FX::FXString const& in)
 { StringPartitioner part(in, ';'); while(part.next()) v.push_back(part.mid()); }

int main(int argc, char* argv[])
{
//    FX::FXApp app("Proximal Vision");
//    app.init(argc,argv);
//    FX::FXMessageBox::information(&app, FX::FXMessageBox::ID_CLICKED_OK, "Hello", "There");
//
  Proxivision app("Proximal Vision");
  app.init(argc,argv);
//    FX::FXMessageBox::information(&app, FX::FXMessageBox::ID_CLICKED_OK, "Hello", "There");
//    return 0;
  FX::FXIcon* pvicon = getEmbeddedIcon(&app, EMBED_ICON::PROXYVISION_APP);
  ProxivisionWindow* win = new ProxivisionWindow(&app, pvicon);
  app.create();

  //TODO: Ovo bi sve moglo u Proxivision::run
  bool finish=false;
  char const* pipename = "proxivision_stdout.pipe";
  //TODO: obrisati postojeci pipename u slucaju da nije fifo
  FX::FXPipe::create(pipename);
  StdoutRedirect thr(win, finish, pipename);

  thr.start();
  int status = app.run();

  finish = true;
  printf("Finishing, releasing resources\n");
  while(finish) FX::FXThread::sleep(1000000);

  return status;
}
