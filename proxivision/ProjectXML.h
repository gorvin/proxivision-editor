#include <fx.h>
#include <vector>
#include "stdutils.h"

typedef struct _xmlXPathObject xmlXPathObject;
typedef struct _xmlDoc xmlDoc;
typedef struct _xmlNodeSet xmlNodeSet;
typedef unsigned char xmlChar;

typedef stdutils::auto_ptr<xmlXPathObject> auto_xmlXPathObject_ptr;
typedef stdutils::auto_ptr<xmlDoc> auto_xmlDoc_ptr;

typedef std::vector<FX::FXString> vfxstring_t;

class ProjectXML {
 	auto_xmlDoc_ptr         doc;
  auto_xmlXPathObject_ptr result1;
  auto_xmlXPathObject_ptr result2;
  xmlChar                 xpath[256];
  explicit ProjectXML(void);//Ne implementiraj!!!
  xmlNodeSet* allnodes(char const* xpath_base, int num);

public:
  ProjectXML(FX::FXString const& filename, char const* docfail = "Document parsing failed. \n");
  int nsubnodes(char const* xpathfull);

  double number(char const* xpath_base, int num);
  double number(char const* xpathfull) { return number(xpathfull, 0); }

  //! Cisti i postavlja \ref sval iz xpath vrijednost
  void read(FX::FXString& sval, char const* xpath_base, int num);
  void read(FX::FXString& sval, char const* xpathfull) { read(sval, xpathfull, 0); }

  void vread(vfxstring_t& sval, char const* xpath_base, int num);
  void vread(vfxstring_t& sval, char const* xpathfull) { vread(sval, xpathfull, 0); }

  //! Resetuje \ref flag_which u \ref flags ako je vrijednost u xpathu "0"
  void reset(int& flags, int flag_which, char const* xpath_base, int num);
  void reset(int& flags, int flag_which, char const* xpathfull) { reset(flags, flag_which, xpathfull, 0); }

  ~ProjectXML(void);
};
