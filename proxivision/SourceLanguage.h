#include <fx.h>
#include <memory>
#include <vector>
#include <map>

#define MYRGB(r,g,b) ((r) | (g<<8) | (b<<16))

struct lexer_item_t {
  FX::FXColor fore;      // Foreground color, in #RRGGBB format
  FX::FXColor back;      // Background color, in #RRGGBB format
  enum {
    FFontBold   = 0x1,
    FFontItalic = 0x2,
  };
  int font_style; // Bitmask of Normal | Bold | Italic
  lexer_item_t(FX::FXColor fg, FX::FXColor bg, int flags = 0) :
    fore(fg), back(bg), font_style(flags) { }
  lexer_item_t(void) :
    fore(MYRGB(0,0,0)), back(MYRGB(0xff,0xff,0xff)), font_style(0) { }
};

//NOTE: MAX_NKEYWORDSETS je isto kao KEYWORDSET_MAX+1 u Scintilla.h
#define MAX_NKEYWORDSETS 9
//NOTE: SCLEX_ID_MAX je za 1 vece od najveceg ID lexera u SciLexer.h
#define SCLEX_ID_MAX 99

// //Clan niza ciji je indeks Scintilla SCLEX_* identifikator
// struct lexer_t {
//   lexer_item_t  styles[32];   // Array of style definitions
//   char const*   keywords[MAX_NKEYWORDSETS];//keyword lists
//   char const*   extensions;        // Pipe-delimited list of wildcards
//   enum {
//     TAB_CHAR_DEFAULT,  // Decide by preferences setting
//     TAB_CHAR_ALWAYS,   // Always use tabs
//     TAB_CHAR_NEVER,    // Always use spaces
//     TAB_CHAR_AUTO      // Decide by document content, else fallback to TABS_DEFAULT
//   }             tab_policy;   // How to determine use of tabs or spaces.
//   lexer_t(void) : extensions(0), tab_policy(TAB_CHAR_DEFAULT){
//     for(size_t i=0; i<MAX_NKEYWORDSETS; i++) keywords[i] = 0;
//   }
// };

// struct lexer_info {
//   lexer_t*  l;
//   int       id;
//   lexer_info(lexer_t* lex, int lid) : l(lex), id(lid) { }
// };

struct style_class_t;
typedef std::shared_ptr<style_class_t> style_class_ptr;

struct style_class_t {
    unsigned long ulFg;      // Foreground color, in #RRGGBB format
    unsigned long ulBg;      // Background color, in #RRGGBB format
    bool          bBold;
    bool          bItalic;
};

struct srcstyle_t;
typedef std::shared_ptr<srcstyle_t> srcstyle_ptr;

struct srcstyle_t {
    std::string                    name;
    std::string                    styleClassName;
    style_class_ptr                styleClass;
    std::unique_ptr<unsigned long> spFg;
    std::unique_ptr<unsigned long> spBg;
    std::unique_ptr<bool>          spbBold;
    std::unique_ptr<bool>          spbItalic;
    style_class_t getStyle() const;
};

struct langdescr_t;
typedef std::shared_ptr<langdescr_t> langdescr_ptr;
typedef std::shared_ptr<char[]> cstr_ptr;

//Clan niza ciji je indeks Scintilla SCLEX_* identifikator
struct langdescr_t {
    std::string               name;
    size_t                    id;
    std::vector<srcstyle_ptr> styles;     // Array of style definitions
    std::vector<cstr_ptr>     keywords;   // keyword lists
    std::string               extensions; // Pipe-delimited list of wildcards
    std::unique_ptr<bool>     spbUseTabs; // null means Auto, otherwise true/false
    std::shared_ptr<bool>     spbUseTabsGlobal;
    
    enum eTABPOLICY{
        TAB_USETAB,   // Always use tabs
        TAB_SPACES,   // Always use spaces
        TAB_AUTO      // Decide by document content, else fallback to TABS_DEFAULT
    };
    eTABPOLICY getTabPolicy(void) const {
        if (!spbUseTabs) {
            if (!spbUseTabsGlobal)
                return TAB_AUTO;
            return *spbUseTabsGlobal
                ? TAB_USETAB
                : TAB_SPACES;
        }
        return *spbUseTabs ? TAB_USETAB : TAB_SPACES;
    }
};

class SourceLanguage {
    std::map<std::string, unsigned long>   namedColors;
    std::map<std::string, style_class_ptr> styleClasses;
    std::map<std::string, langdescr_ptr>   languages;
    std::shared_ptr<bool>                  spbUseTabsGlobal; // null means Auto, otherwise true/false
    
    //static int from_mime(FX::FXString const& mimetype);

public:
    langdescr_t const* find(FX::FXString const& filename_to_match);
    // lexer_info find(FX::FXString const& dir, FX::FXString const& filename);
    SourceLanguage(void);
};
