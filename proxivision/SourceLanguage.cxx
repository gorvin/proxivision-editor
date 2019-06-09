#include "SourceLanguage.h"
#include "stdutils.h"
#include "Proxivision.h"
#include <fx.h>
#include <SciLexer.h>

#ifndef _WIN32
# include <xdgmime.h>
#else // _WIN32
# include <urlmon.h>
# include <locale>
# include <codecvt>
#endif // _WIN32

#include <string>
#include <vector>
#include <map>
#include <memory>

style_class_t srcstyle_t::getStyle() const
{
    style_class_t style;
    style.ulFg = spFg ? *spFg : styleClass->ulFg;
    style.ulBg = spBg ? *spBg : styleClass->ulBg;
    style.bBold = spbBold ? *spbBold : styleClass->bBold;
    style.bItalic = spbItalic ? *spbItalic : styleClass->bItalic;
    return style;
}

#ifdef FOX_1_6
# define FXPATH_MATCH(pattern,file) \
 FX::FXPath::match(pattern,file,FX::FILEMATCH_FILE_NAME|FX::FILEMATCH_CASEFOLD)
#else
# define FXPATH_MATCH(pattern,file) \
 FX::FXPath::match(file,pattern,FX::FXPath::PathName|FX::FXPath::CaseFold)
#endif

langdescr_t const* SourceLanguage::find(FX::FXString const& filename)
{
    std::string const sFilename(filename.text());
    std::string const sBasename(sFilename, 1 + sFilename.rfind("/"));
    FX::FXString basename(sBasename.c_str());
    for (auto it: languages) {
        auto const& ld = it.second;
        if (!ld->extensions.empty() && FXPATH_MATCH(ld->extensions.c_str(), basename))
            return ld.get();
    }
    return nullptr;
}

/* Nesikoristeni lexeri:
 SCLEX_FREEBASIC SCLEX_BATCH SCLEX_CAML SCLEX_CONF SCLEX_COBOL
 SCLEX_ERRORLIST SCLEX_FLAGSHIP SCLEX_MARKDOWN SCLEX_METAPOST
SCLEX_PROPERTIES SCLEX_R SCLEX_SMALLTALK SCLEX_VERILOG*/
// int SourceLanguage::from_mime(FX::FXString const& mimetype)
// {
//   if(mimetype.find("text/css")>-1) return SCLEX_CSS;

//   if(mimetype.find("text/html")>-1) return SCLEX_HTML;

//   if(mimetype.find("text/x-asm")>-1) return SCLEX_ASM;

//   if(mimetype.find("text/x-adasrc")>-1) return SCLEX_ADA;

//   if(mimetype.find("text/x-csrc")>-1) return SCLEX_CPP;
//   if(mimetype.find("text/x-c++src")>-1) return SCLEX_CPP;
//   if(mimetype.find("text/x-chdr")>-1) return SCLEX_CPP;
//   if(mimetype.find("text/x-c++hdr")>-1) return SCLEX_CPP;
//   if(mimetype.find("text/x-java")>-1) return SCLEX_CPP;
//   if(mimetype.find("application/x-java")>-1) return SCLEX_CPP;
//   if(mimetype.find("text/x-csharp")>-1) return SCLEX_CPP;

//   if(mimetype.find("text/x-emacs-lisp")>-1) return SCLEX_LISP;
//   if(mimetype.find("text/x-script.elisp")>-1) return SCLEX_LISP;
//   if(mimetype.find("text/x-script.lisp")>-1) return SCLEX_LISP;
//   if(mimetype.find("application/x-lisp")>-1) return SCLEX_LISP;

//   if(mimetype.find("text/x-erlang")>-1) return SCLEX_ERLANG;

//   if(mimetype.find("text/x-fortran")>-1) return SCLEX_F77;//SCLEX_FORTRAN

//   if(mimetype.find("text/x-haskell")>-1) return SCLEX_HASKELL;

//   if(mimetype.find("text/x-lua")>-1) return SCLEX_LUA;

//   if(mimetype.find("text/x-makefile")>-1) return SCLEX_MAKEFILE;

//   if(mimetype.find("text/x-cmake")>-1) return SCLEX_CMAKE;

//   if(mimetype.find("text/x-pascal")>-1) return SCLEX_PASCAL;

//   if(mimetype.find("text/x-patch")>-1) return SCLEX_DIFF;

//   if(mimetype.find("text/x-python")>-1) return SCLEX_PYTHON;
//   if(mimetype.find("text/x-script.phyton")>-1) return SCLEX_PYTHON;

//   if(mimetype.find("text/x-sql")>-1) return SCLEX_SQL;

//   if(mimetype.find("text/x-tcl")>-1) return SCLEX_TCL;
//   if(mimetype.find("text/x-script.tcl")>-1) return SCLEX_TCL;

//   if(mimetype.find("text/x-tex")>-1) return SCLEX_TEX;// SCLEX_LATEX;

//   if(mimetype.find("text/x-vhdl")>-1) return SCLEX_VHDL;

//   if(mimetype.find("application/postscript")>-1) return SCLEX_PS;

//   if(mimetype.find("application/xml")>-1) return SCLEX_XML;

//   if(mimetype.find("application/x-perl")>-1) return SCLEX_PERL;
//   if(mimetype.find("text/x-script.perl")>-1) return SCLEX_PERL;
//   if(mimetype.find("text/x-script.perl-module")>-1) return SCLEX_PERL;

//   if(mimetype.find("application/x-ruby")>-1) return SCLEX_RUBY;

//   if(mimetype.find("application/x-shellscript")>-1) return SCLEX_BASH;
//   if(mimetype.find("application/x-csh")>-1) return SCLEX_BASH;
//   if(mimetype.find("application/x-sh")>-1) return SCLEX_BASH;
//   if(mimetype.find("application/x-ksh")>-1) return SCLEX_BASH;
//   if(mimetype.find("text/x-script.ksh")>-1) return SCLEX_BASH;
//   if(mimetype.find("text/x-script.csh")>-1) return SCLEX_BASH;
//   if(mimetype.find("text/x-script.sh")>-1) return SCLEX_BASH;
//   if(mimetype.find("text/x-script.zsh")>-1) return SCLEX_BASH;

//   if(mimetype.find("text/x-dsrc")>-1) return SCLEX_D;

//   //if(mimetype.find("text/plain")>-1) return SCLEX_NULL;
//   //if(mimetype.find("text/x-matlab")>-1) return SCLEX_MATLAB;
//   //if(mimetype.find("application/x-php")>-1) return

//   return SCLEX_NULL;
// }

// #ifndef _WIN32

// # define MIMEBUF_MAXBY xdg_mime_get_max_buffer_extents()

// inline std::string MimeTypeFromData(const char* pszFileName, const void* data, size_t bytes_read)
// {
//   int result_prio;
//   pdzFileName = pszFileName;
//   char const* mt = xdg_mime_get_mime_type_for_data (data, bytes_read, &result_prio);
//   retunr nullptr != mt ? mt : "";
// }

// #else // _WIN32

// # define MIMEBUF_MAXBY 2048

// static std::string MimeTypeFromData(const char* pszFileName, const void* data, size_t bytes_read)
// {
//     std::wstring sFilename( std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(pszFileName) );
//     LPWSTR pwzMimeOut = NULL;
//     HRESULT hr = ::FindMimeFromData(NULL, sFilename.c_str(), const_cast<LPVOID>(data), bytes_read,
//                                    NULL, FMFD_URLASFILENAME, &pwzMimeOut, 0x0 );
//     if ( SUCCEEDED( hr ) ) {
//         std::string strResult( std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(pwzMimeOut) );
//         // Despite the documentation stating to call operator delete, the
//         // returned string must be cleaned up using CoTaskMemFree
//         ::CoTaskMemFree( pwzMimeOut );
//         return strResult;
//     }

//     return std::string();
// }
// #endif // _WIN32

// lexer_info SourceLanguage::find(FX::FXString const& dir, FX::FXString const& filename)
// {
//   FX::FXString path(dir);
//   path.append(PATHSEP).append(filename);
//   //char path[1024]; snprintf (path, 1024, "%s"PATHSEPSTRING"%s", dir, filename);
//   int max_extent = MIMEBUF_MAXBY;
//   std::unique_ptr<char[]> data(new char[max_extent]);
//   if(!data) {
//     printf ("Out of memory for file %s\n", path.text());
//     return lexer_info(&languages[SCLEX_NULL], SCLEX_NULL);
//   }

//   FILE* file = fopen (path.text(), "r");
//   if(!file) {
//     printf ("Could not open %s\n", path.text());
//     return lexer_info(&languages[SCLEX_NULL], SCLEX_NULL);
//   }
//   int bytes_read = fread (data.get(), 1, max_extent, file);
//   const bool bFileReadFailed = 0 == ferror(file) || 0 == bytes_read;
//   fclose(file);

//   FX::FXString mt;
//   if(bFileReadFailed) {
//     printf ("Error reading file %s\n", path.text());
//   }
  
//   mt = MimeTypeFromData (path.text(), data.get(), bytes_read).c_str();
//   if(!mt.empty())
//     printf("%s\t%s (data)\n", path.text(), mt.text());
//   int id = SourceLanguage::from_mime(mt);
//   return lexer_info(&languages[id], id);
// }
