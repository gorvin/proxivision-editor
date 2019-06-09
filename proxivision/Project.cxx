#include "Project.h"
#include "ProjectXML.h"

void at_semicolon(vfxstring_t& v, FX::FXString const& in);

int compiler_t::which_platform(FX::FXString const& str)
{
  int platforms = compiler_t::Unknown;
  if(str.find("Windows") != -1) platforms |= Windows;
  if(str.find("Unix") != -1) platforms |= Unix;
  if(str.find("Mac") != -1) platforms |= Mac;
  if(str.find("All") != -1) platforms = All;

  return platforms;
}

unit_t::tool_t unit_t::which_tool(FX::FXString const& str)
{
  if(FX::compare(str, "CPP") == 0) return CPP;
  if(FX::compare(str, "C") == 0) return C;
  return WINDRES;
}

compiler_t::kind_t compiler_t::which_compiler(FX::FXString const& str)
{
  for(int i=0; i<COMPILER_KIND_LAST; i++) {
    if(FX::compare(str, compiler_shortname[i]) == 0) return (compiler_t::kind_t)i;
  }
  return unknown;
}

FX::FXString compiler_t::compiler_shortname[compiler_t::COMPILER_KIND_LAST] = {
  "unknown",
  "gcc",        //GNU GCC
  "cygwin",     //Cygwin GCC
  "bcc",        //Borland C++ Compiler 5.5
  "dmc",        //Digital Mars Compiler
  "dmd",        //Digital Mars D Compiler
  "gdc",        //GDC D Compiler
  "arm_elf_gcc",//GNU ARM GCC Compiler
  "tricoregcc", //GNU TriCore GCC Compiler
  "ppchc",      //GNU PowerPC GCC Compiler
  "msp430gcc",  //GNU MSP430 GCC Compiler
  "icc",        //Intel C/C++ Compiler
  "msvctk",     //Microsoft Visual C++ Toolkit 2003
  "msvc8",      //Microsoft Visual C++ 2005
  "ow",         //OpenWatcom (W32) Compiler
  "sdcc",       //SDCC Compiler
  "tcc",        //Tiny C Compiler
};

//Code::Blocks fajlovi *.cbp su opisani na http://wiki.codeblocks.org/index.php?title=Project_file
int project_t::fill_targets(ProjectXML& xfile)
{
  int nnodes = xfile.nsubnodes("//Project/Build/Target");
  if(!nnodes) return 0;

  FX::FXString stmp;
  targets.resize(nnodes);

  for (int i=0; i < nnodes; i++) {
    targets[i].flags = target_t::AllFlagsOn;

    xfile.read(targets[i].title, "//Project/Build/Target[%u]/@title", i+1);
    if(targets[i].title.empty())  continue;

    xfile.read(stmp, "//Project/Build/Target[%u]/Option/@platforms", i+1);
    //NOTE: Vise platformi se zadaje kao semicolon separated lista
    targets[i].compiler.platforms = compiler_t::which_platform(stmp);

    xfile.reset(targets[i].flags, target_t::AutoOutPfx, "//Project/Build/Target[%u]/Option/@prefix_auto", i+1);
    xfile.reset(targets[i].flags, target_t::AutoOutExt, "//Project/Build/Target[%u]/Option/@extension_auto", i+1);

    xfile.read(targets[i].ofname, "//Project/Build/Target[%u]/Option/@output", i+1);
    xfile.read(targets[i].wdir, "//Project/Build/Target[%u]/Option/@working_dir", i+1);
    xfile.read(targets[i].odir, "//Project/Build/Target[%u]/Option/@object_output", i+1);

    //NOTE: deprecated: "//Project/Build/Target[%u]/Option/@deps_output"
    xfile.read(stmp, "//Project/Build/Target[%u]/Option/@external_deps", i+1);
    targets[i].dep_libs.clear();
    at_semicolon(targets[i].dep_libs, stmp);

    xfile.read(stmp, "//Project/Build/Target[%u]/Option/@additional_output", i+1);
    targets[i].dep_files.clear();
    at_semicolon(targets[i].dep_files, stmp);

    targets[i].kind = target_t::valid_kind(xfile.number("//Project/Build/Target[%u]/Option/@type", i+1));

    xfile.read(stmp, "//Project/Build/Target[%u]/Option/@compiler", i+1);
    targets[i].compiler.which = compiler_t::which_compiler(stmp);

    xfile.reset(targets[i].flags, target_t::ExeTermPause, "//Project/Build/Target[%u]/Option/@use_console_runner", i+1);
    xfile.read(targets[i].exswitches, "//Project/Build/Target[%u]/Option/@parameters", i+1);
    xfile.read(targets[i].exhost, "//Project/Build/Target[%u]/Option/@host_application", i+1);

    xfile.reset(targets[i].flags, target_t::CreateDEF, "//Project/Build/Target[%u]/Option/@createDefFile", i+1);
    xfile.reset(targets[i].flags, target_t::CreateImport, "//Project/Build/Target[%u]/Option/@createStaticLib", i+1);

    targets[i].order_cswitches = target_t::valid_precedence(xfile.number("//Project/Build/Target[%u]/Option/@projectCompilerOptionsRelation", i+1));
    targets[i].order_lswitches = target_t::valid_precedence(xfile.number("//Project/Build/Target[%u]/Option/@projectLinkerOptionsRelation", i+1));
    targets[i].order_cincludes = target_t::valid_precedence(xfile.number("//Project/Build/Target[%u]/Option/@projectIncludeDirsRelation", i+1));
    targets[i].order_rdirectories = target_t::valid_precedence(xfile.number("//Project/Build/Target[%u]/Option/@projectResourceIncludeDirsRelation", i+1));
    targets[i].order_ldirectories = target_t::valid_precedence(xfile.number("//Project/Build/Target[%u]/Option/@projectLibDirsRelation", i+1));
    xfile.read(targets[i].script_name, "//Project/Build/Target[%u]/Script/@file", i+1);
    xfile.vread(targets[i].compiler.cswitches, "//Project/Build/Target[%u]/Compiler/Add/@option", i+1);
    xfile.vread(targets[i].compiler.cdirectories, "//Project/Build/Target[%u]/Compiler/Add/@directory", i+1);
    xfile.vread(targets[i].compiler.rdirectories, "//Project/Build/Target[%u]/ResourceCompiler/Add/@directory", i+1);
    xfile.vread(targets[i].compiler.lswitches, "//Project/Build/Target[%u]/Linker/Add/@option", i+1);
    xfile.vread(targets[i].compiler.ldirectories, "//Project/Build/Target[%u]/Linker/Add/@directory", i+1);
    xfile.vread(targets[i].compiler.libraries, "//Project/Build/Target[%u]/Linker/Add/@library", i+1);
    xfile.vread(targets[i].compiler.pre, "//Project/Build/Target[%u]/ExtraCommands/Add/@before", i+1);
    xfile.vread(targets[i].compiler.post, "//Project/Build/Target[%u]/ExtraCommands/Add/@after", i+1);
    xfile.read(stmp, "//Project/Build/Target[%u]/ExtraCommands/Mode/@after", i+1);
    targets[i].compiler.post_allways = (FX::compare(stmp, "1") == 0);
    xfile.read(targets[i].compiler.mcmd_build, "//Project/Build/Target[%u]/MakeCommands/Build/@command", i+1);
    xfile.read(targets[i].compiler.mcmd_compile, "//Project/Build/Target[%u]/MakeCommands/CompileFile/@command", i+1);
    xfile.read(targets[i].compiler.mcmd_clean, "//Project/Build/Target[%u]/MakeCommands/Clean/@command", i+1);
    xfile.read(targets[i].compiler.mcmd_distclean, "//Project/Build/Target[%u]/MakeCommands/DistClean/@command", i+1);
  }
  for(std::vector<target_t>::iterator it = targets.begin(); it != targets.end(); ++it) {
    if(it->title.empty()) { targets.erase(it); it = targets.begin(); }
  }
  return 1;
}

int project_t::fill_units(ProjectXML& xfile)
{
  int nnodes = xfile.nsubnodes("//Project/Unit");
  if(!nnodes) return 0;

  FX::FXString stmp;
  units.resize(nnodes);

  for (int i=0; i < nnodes; i++) {
    units[i].flags = unit_t::AllFlagsOn;

    xfile.read(units[i].filename, "//Project/Unit[%u]/@filename", i+1);
    if(units[i].filename.empty()) continue;

    xfile.read(stmp, "//Project/Unit[%u]/Option/@compilerVar", i+1);
    units[i].tool = unit_t::which_tool(stmp);
    xfile.reset(units[i].flags, unit_t::DoCompile, "//Project/Unit[%u]/Option/@compile", i+1);
    xfile.reset(units[i].flags, unit_t::DoLink, "//Project/Unit[%u]/Option/@link", i+1);
    units[i].weight=xfile.number("//Project/Unit[%u]/Option/@weight", i+1);//= xmlXPathCastNodeToNumber(node);
    xfile.read(units[i].view, "//Project/Unit[%u]/Option/@virtualFolder", i+1);
    xfile.vread(units[i].targets, "//Project/Unit[%u]/Option/@target", i+1);
  }
  for(std::vector<unit_t>::iterator it = units.begin(); it != units.end(); ++it) {
    if(it->filename.empty()) { units.erase(it); it = units.begin(); }
  }
  return 1;
}


int project_t::fill_common(ProjectXML& xfile)
{
  int nnodes = xfile.nsubnodes("//Project/Unit");
  if(!nnodes) return 0;

  FX::FXString stmp;

  flags = AllFlagsOn;

  xfile.read(title, "//Project/Option/@title");

  xfile.read(stmp, "//Project/Option/@platforms");
  compiler.platforms = compiler_t::which_platform(stmp);

  xfile.read(mfilename, "//Project/Option/@makefile");
  xfile.reset(flags, NotMakefile, "//Project/Option/@makefile_is_custom");

  switch((int)xfile.number("//Project/Option/@pch_mode")) {
    case hdr_subdir: pch_mode = hdr_subdir; break;
    case obj_dir: pch_mode = obj_dir; break;
    default: pch_mode = hdr_dir;
  }
  xfile.read(default_target, "//Project/Option/@default_target");

  xfile.read(stmp, "//Project/Option/@compiler");
  compiler.which = compiler_t::which_compiler(stmp);

  xfile.read(stmp, "//Project/Option/@virtualFolders");
  at_semicolon(views, stmp);

  xfile.reset(flags, ExtendedNames, "//Project/Option/@extended_obj_names");
  xfile.reset(flags, ShowNotes, "//Project/Option/@show_notes");

  //"//Project/Option/notes/![CDATA[]]",

  xfile.read(compiler.mcmd_build, "//Project/MakeCommands/Build/@command");
  xfile.read(compiler.mcmd_compile, "//Project/MakeCommands/CompileFile/@command");
  xfile.read(compiler.mcmd_clean, "//Project/MakeCommands/Clean/@command");
  xfile.read(compiler.mcmd_distclean, "//Project/MakeCommands/DistClean/@command");

  xfile.read(script_name, "//Project/Script/@file");

  //prvo valiases iskoristis za pridruzene real targets a zatim prema namjeni.
  xfile.vread(valiases, "//Project/Build/VirtualTargets/Add/@targets");
  vrealtargets.resize(valiases.size());
  for(size_t i=0; i<valiases.size(); i++) {
    at_semicolon(vrealtargets[i], valiases[i]);
  }
  xfile.vread(valiases, "//Project/Build/VirtualTargets/Add/@alias");

  xfile.vread(compiler.cswitches, "//Project/Build/Target[%u]/Compiler/Add/@option");
  xfile.vread(compiler.cdirectories, "//Project/Build/Target[%u]/Compiler/Add/@directory");
  xfile.vread(compiler.rdirectories, "//Project/Build/Target[%u]/ResourceCompiler/Add/@directory");
  xfile.vread(compiler.lswitches, "//Project/Build/Target[%u]/Linker/Add/@option");
  xfile.vread(compiler.ldirectories, "//Project/Build/Target[%u]/Linker/Add/@directory");
  xfile.vread(compiler.libraries, "//Project/Build/Target[%u]/Linker/Add/@library");
  xfile.vread(compiler.pre, "//Project/Build/Target[%u]/ExtraCommands/Add/@before");
  xfile.vread(compiler.post, "//Project/Build/Target[%u]/ExtraCommands/Add/@after");
  xfile.read(stmp, "//Project/Build/Target[%u]/ExtraCommands/Mode/@after");
  compiler.post_allways = (FX::compare(stmp, "1") == 0);

  //TODO: "//Project/Build/Extensions/code_completion",
  //TODO: "//Project/Build/Extensions/debugger",
  return 1;
}

int project_t::read(FX::FXString const& xfilename)
{
  ProjectXML xfile(xfilename);
  if(!fill_common(xfile)) return 0;
  if(!fill_targets(xfile)) return 0;
  if(!fill_units(xfile)) return 0;
  return 1;
}
