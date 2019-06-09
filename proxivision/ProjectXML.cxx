#include "ProjectXML.h"
#include "Proxivision.h"
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "stdutils_impl.h"

ProjectXML::ProjectXML(FX::FXString const& filename, char const* docfail) :
 doc(xmlParseFile(filename.text()), docfail, xmlFreeDoc), result1(0, "Error: xmlXPathEvalExpression\n",
  xmlXPathFreeObject), result2(0, "Error: xmlXPathEvalExpression\n", xmlXPathFreeObject) { }

ProjectXML::~ProjectXML(void) { xmlCleanupParser(); };

int ProjectXML::nsubnodes(char const* xpathfull)
{
  xmlXPathContext* context = xmlXPathNewContext(doc.get());
	if (context == NULL) { printf("Error in xmlXPathNewContext\n"); return 0; }
  result1.reset(xmlXPathEvalExpression((xmlChar const*)xpathfull, context));
	xmlXPathFreeContext(context);
  stdutils::auto_ptr_check(result1);// result1.check();

  xmlNodeSet* nodes = result1->nodesetval;
  if(nodes == NULL || nodes->nodeTab == NULL) {
    printf("Empty nodeset: %s\n", (char const*) xpathfull); return 0;
  }
  return nodes->nodeNr;
}

xmlNodeSet* ProjectXML::allnodes(char const* xpath_base, int num)
{
  sprintf((char*)xpath, xpath_base, num);

  xmlXPathContext* context = xmlXPathNewContext(doc.get());
	if (context == NULL) { printf("Error in xmlXPathNewContext\n"); return 0; }

  result2.reset(xmlXPathEvalExpression(xpath, context));
	xmlXPathFreeContext(context);
  stdutils::auto_ptr_check(result2);//result2.check();

  xmlNodeSet* nodes = result2->nodesetval;
  if(xmlXPathNodeSetIsEmpty(nodes)){
    //printf("Empty nodeset: %s\n", (char const*) xpath);
    return 0;
  }
  return nodes;
}

double ProjectXML::number(char const* xpath_base, int num)
{
  xmlNodeSet* nodes = allnodes(xpath_base, num);
  if(!nodes) return -1;
  if(nodes->nodeNr>1) { printf("\n Unexpected multiple attributes"); }
  if(!nodes->nodeTab[0]->xmlChildrenNode) return -1;

  return xmlXPathCastNodeToNumber(nodes->nodeTab[0]->xmlChildrenNode);
}

//! Cisti i postavlja \ref sval iz xpath vrijednost
void ProjectXML::read(FX::FXString& sval, char const* xpath_base, int num)
{
  sval.clear();

  xmlNodeSet* nodes = allnodes(xpath_base, num);
  if(!nodes) return;
  if(nodes->nodeNr>1) { printf("\n Unexpected multiple attributes"); }
  if(!nodes->nodeTab[0]->xmlChildrenNode) return;

  xmlChar* key = xmlNodeListGetString(doc.get(), nodes->nodeTab[0]->xmlChildrenNode, 1);
  if(key) { sval = (char const*)key; xmlFree(key); }
}

void ProjectXML::vread(vfxstring_t& sval, char const* xpath_base, int num)
{
  sval.clear();

  xmlNodeSet*  nodes = allnodes(xpath_base, num);
  if(!nodes) return;
  sval.resize(nodes->nodeNr);

  xmlChar* key=0;
  for(int i=0; i<nodes->nodeNr; i++) {
    if(!nodes->nodeTab[i]->xmlChildrenNode) continue;
    key = xmlNodeListGetString(doc.get(), nodes->nodeTab[i]->xmlChildrenNode, 1);
    if(key) { sval[i] = (char const*)key; xmlFree(key); }
  }
}

//! Resetuje \ref flag_which u \ref flags ako je vrijednost u xpathu "0"
void ProjectXML::reset(int& flags, int flag_which, char const* xpath_base, int num)
{
  xmlNodeSet* nodes = allnodes(xpath_base, num);
  if(!nodes) return;
  if(nodes->nodeNr>1) { printf("\n Unexpected multiple attributes"); }
  if(!nodes->nodeTab[0]->xmlChildrenNode) return;

  int val = (int)xmlXPathCastNodeToNumber(nodes->nodeTab[0]->xmlChildrenNode);
  if(val == 0) { flags &= ~flag_which; }
}
