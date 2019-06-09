#include <fx.h>
#include <vector>

typedef std::vector<FX::FXString> vfxstring_t;
class ProjectXML;

struct compiler_t {
  enum platform_t { Unknown = 0x0, Windows = 0x1, Unix = 0x2, Mac = 0x4, All = 0x7, };
  enum kind_t {
    unknown=0, gcc, cygwin, bcc, dmc, dmd, gdc, arm_elf_gcc, tricoregcc, ppcgcc,
    msp430gcc, icc, msvctk, msvc8, ow, sdcc, tcc, COMPILER_KIND_LAST,
  };
  int          platforms;//platform_t flagovi
  kind_t       which;
  vfxstring_t  cswitches;
  vfxstring_t  cdirectories;
  vfxstring_t  lswitches;
  vfxstring_t  ldirectories;
  vfxstring_t  libraries;
  vfxstring_t  rdirectories;
  vfxstring_t  pre;//pre-build step
  vfxstring_t  post;//post-build step
  bool         post_allways;//even if project is up-to-date
  FX::FXString mcmd_build;
  FX::FXString mcmd_compile;
  FX::FXString mcmd_clean;
  FX::FXString mcmd_distclean;

  static FX::FXString compiler_shortname[COMPILER_KIND_LAST];

  static int which_platform(FX::FXString const& str);
  static kind_t which_compiler(FX::FXString const& str);
};

struct target_t {
  enum precedence_t { ProjectOnly, TargetOnly, TargetBefore, TargetAfter, };
  enum kind_t { GUI=0, Console, Static, Dynamic, Extra, Sys, };
  enum flags_t {
    AutoOutPfx  = 0x01, AutoOutExt  = 0x02, ExeTermPause= 0x04,//Console pause on end of execution
    CreateDEF   = 0x08,/*Windows*/ CreateImport= 0x10,/*Windows*/ AllFlagsOn  = 0x1f,
  };
  int          flags;
  FX::FXString title;
  FX::FXString ofname;
  FX::FXString odir;
  FX::FXString wdir;//working directory
  vfxstring_t  dep_libs;//dependency libraries
  vfxstring_t  dep_files;//other dependency outputs
  kind_t       kind;
  compiler_t   compiler;
  FX::FXString exswitches;
  FX::FXString exhost;//for dynamic libraries
  precedence_t order_cswitches;
  precedence_t order_cincludes;
  precedence_t order_lswitches;
  precedence_t order_ldirectories;
  precedence_t order_rdirectories;
  FX::FXString script_name;
  static precedence_t valid_precedence(int val) {
    if(val>=target_t::ProjectOnly && val<=target_t::TargetAfter) return (precedence_t)val;
    return TargetAfter;
  }
  static kind_t valid_kind(int val) {
    if(val>=target_t::GUI && val<=target_t::Sys) return (kind_t)val;
    return Console;
  }
};

struct unit_t {
  enum tool_t { CPP, C, WINDRES };
  enum flags_t { DoCompile = 0x1, DoLink = 0x2, AllFlagsOn = 0x3};
  FX::FXString filename;
  tool_t       tool;
  int          flags;
  unsigned int weight;
  FX::FXString view;
  vfxstring_t  targets;
  FX::FXString subdir(void) const { return filename.left(filename.rfind(PATHSEP)); }
  static tool_t which_tool(FX::FXString const& str);
};

struct project_t {
  FX::FXString             path;
  FX::FXString             fname;
  FX::FXbool               ondiskview;

  FX::FXString             title;
  enum {
    hdr_subdir = 0, obj_dir = 1, hdr_dir = 2,
  }                        pch_mode;
  FX::FXString             default_target;
  FX::FXString             active_target;
  vfxstring_t              views;//virtual folders
  enum flags_t { NotMakefile = 0x1, ExtendedNames = 0x2, ShowNotes = 0x4, AllFlagsOn = 0x7, };
  int                      flags;
  compiler_t               compiler;
  FX::FXString             mfilename;//makefile
  vfxstring_t              valiases;//virtual targets aliases
  std::vector<vfxstring_t> vrealtargets;//virtual targets real targets
  FX::FXString             script_name;
  std::vector<target_t>    targets;
  std::vector<unit_t>      units;

  project_t(void) : ondiskview(false) { }
  int read(FX::FXString const& xfilename);
private:
  int fill_common(ProjectXML& xfile);
  int fill_targets(ProjectXML& xfile);
  int fill_units(ProjectXML& xfile);
};

