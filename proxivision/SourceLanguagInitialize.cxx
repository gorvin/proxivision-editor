#include "SourceLanguage.h"
#include <picojson.h>
#include <FXSystem.h>
#include <fstream>
#include <windows.h>

//#include <SciLexer.h>
//#include <assert.h>

// #define COMMNT_FG MYRGB(0xcc,0xcc,0xcc)
// #define PREPRC_FG MYRGB(0x00,0x80,0x00)
// #define NUMBER_FG MYRGB(0xcc,0x00,0xff)
// #define OPERTR_FG MYRGB(0x88,0x00,0xff)
// #define STRING_FG MYRGB(0x88,0xc0,0x00)
// #define STREOL_FG MYRGB(0xff,0x00,0x00)
// #define _WORD1_FG MYRGB(0x00,0x00,0xff)
// #define _WORD2_FG MYRGB(0x00,0x55,0xff)
// #define _WORD3_FG MYRGB(0x00,0x99,0xff)
// #define SCALAR_FG MYRGB(0x00,0xbb,0xbb)
// #define __ORANGE_ MYRGB(0xff,0x80,0x00)
// #define __PURPLE_ MYRGB(0xcc,0x00,0xcc)
// #define ___BLUE__ MYRGB(0x00,0x00,0xee)
// #define __GREEN__ MYRGB(0x00,0xcc,0x00)
// #define __DKRED__ MYRGB(0xcc,0x60,0x60)
// #define _HERE_BG_ MYRGB(0xff,0xff,0xee)

// #define _DEFLT_BG MYRGB(0xff,0xff,0xff)
// #define _DEFLT_FG MYRGB(0,0,0)

unsigned long getRGB(std::string const& val, std::map<std::string, unsigned long> const* namedColors = nullptr)
{
    do {
        if(val.empty())
            break;
        size_t idx = 0;
        int base = 10;
        if('#' == val[0]) {
            idx = 1;
            base = 16;
        }
        if(val.empty())
            break;
        try {
          unsigned long ulVal = std::stoul(val.substr(idx), nullptr, base);
          unsigned long ulRGB = (ulVal & 0x00FF0000) >> 16 | (ulVal & 0x0000FF00) | (ulVal & 0x000000FF) << 16;
          return ulRGB;
        } catch(std::invalid_argument const& ex) {
          break;
        }
    } while(0);

    if (nullptr != namedColors) {
        auto it = namedColors->find(val);
        if(namedColors->end() != it)
            return it->second;
    }
    return 0;
}

std::string getCurrentExecutablePath() {
    char szPath[260] = {0};
#ifdef _WIN32
    int bytes = GetModuleFileNameA(NULL, szPath, _countof(szPath));
#else // _WIN32
    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = MIN(readlink(szTmp, szPath, _countof(szPath)), _countof(szPath) - 1);
#endif // _WIN32
    if(bytes >= 0) {
        szPath[bytes] = '\0';
        return std::string(szPath);
    }
    return std::string();
}

SourceLanguage::SourceLanguage(void)
{
    auto exeDir = FX::FXPath::directory(getCurrentExecutablePath().c_str());
    auto jsonFPath(exeDir + PATHSEPSTRING + "ScintillaLangStyles.json");
    std::ifstream fsJson(jsonFPath.text());
    picojson::value jsDoc;
    std::string err = picojson::parse(jsDoc, fsJson);
    if (!err.empty()) {
        std::cerr << err << std::endl;
        throw std::runtime_error("JSON syntax error");
    }

    // check if the type of the value is "object"
    if (!jsDoc.is<picojson::object>()) {
        throw std::runtime_error("Root JSON is not an object");
    }
    auto& jsRoot = jsDoc.get<picojson::object>();
    
    {
        auto itColors = jsRoot.find("colors");
        if (jsRoot.end() != itColors) {
            auto& jsarrColors = itColors->second;
            if (!jsarrColors.is<picojson::object>()) {
                throw std::runtime_error("Colors JSON value is not an object");
            }
            for (auto& entry: jsarrColors.get<picojson::object>()) {
                if (!entry.second.is<std::string>()) {
                    throw std::runtime_error("Color JSON value is not an string");
                }
                namedColors[entry.first] = getRGB(entry.second.get<std::string>());
            }
        }
    }
    {
        auto itClasses = jsRoot.find("classes");
        if (jsRoot.end() != itClasses) {
            auto& jsClassses = itClasses->second;
            if (!jsClassses.is<picojson::object>()) {
                throw std::runtime_error("Classes JSON value is not an object");
            }

            for (auto& entry: jsClassses.get<picojson::object>()) {
                if (!entry.second.is<picojson::object>()) {
                    throw std::runtime_error("Global class JSON value is not an object");
                }
                auto& jsStyle = entry.second.get<picojson::object>();
                auto itFg = jsStyle.find("fg");
                if(jsStyle.end() == itFg || !itFg->second.is<std::string>()) {
                    throw std::runtime_error("Style FG JSON value not found");
                }
                auto itBg = jsStyle.find("bg");
                if(jsStyle.end() == itBg || !itBg->second.is<std::string>()) {
                    throw std::runtime_error("Style BG JSON value not found");
                }
                auto itBold = jsStyle.find("bold");
                if(jsStyle.end() == itBold || !itBold->second.is<bool>()) {
                    throw std::runtime_error("Style Bold JSON value not found");
                }
                auto itItalic = jsStyle.find("italic");
                if(jsStyle.end() == itItalic || !itItalic->second.is<bool>()) {
                    throw std::runtime_error("Style Italic JSON value not found");
                }
                styleClasses[entry.first].reset(new style_class_t{
                    getRGB(itFg->second.get<std::string>(), &namedColors),
                    getRGB(itBg->second.get<std::string>(), &namedColors),
                    itBold->second.get<bool>(),
                    itItalic->second.get<bool>()
                });
            }
        }      
    }

    //bool useTabs = false; // tabs usage policy (do replace with spaces or not)
    {
        auto itUseTabs = jsRoot.find("use_tabs");
        if (jsRoot.end() != itUseTabs) {
            auto& jsVal = itUseTabs->second;
            if (jsVal.is<picojson::null>()) {
                spbUseTabsGlobal.reset();
            } else if (!jsVal.is<bool>()) {
                throw std::runtime_error("Tabs JSON value is not an boolean");
            }
            spbUseTabsGlobal.reset(new bool(jsVal.get<bool>()));
        }
    }

    {
        auto itLangs = jsRoot.find("scintilla_langs");
        if (jsRoot.end() != itLangs) {
            auto& jsLangs = itLangs->second;
            if (!jsLangs.is<picojson::object>()) {
                throw std::runtime_error("Langs JSON value is not an object");
            }

            for (auto& entry: jsLangs.get<picojson::object>()) {
                std::string const sLangName = entry.first;
                langdescr_ptr spLd(new langdescr_t);
                spLd->name = entry.first;
                spLd->id = -1;
                spLd->spbUseTabsGlobal = spbUseTabsGlobal;
                if (!entry.second.is<picojson::object>()) {
                    throw std::runtime_error("Lang type JSON value is not an object");
                }
                auto& jsOne = entry.second.get<picojson::object>();
                auto itId = jsOne.find("id");
                if(jsOne.end() == itId || !itId->second.is<double>()) {
                    throw std::runtime_error("Lang id JSON value not found");
                }
                spLd->id = static_cast<size_t>(itId->second.get<double>());
                auto itExtensions = jsOne.find("extensions");
                if(jsOne.end() == itExtensions || !itExtensions->second.is<std::string>()) {
                    throw std::runtime_error("Lang extensions JSON value not found");
                }
                spLd->extensions = itExtensions->second.get<std::string>();
                auto itUseTabs = jsOne.find("use_tabs");
                if(jsOne.end() != itUseTabs) {
                    auto& jsVal = itUseTabs->second;
                    if (jsVal.is<picojson::null>()) {
                        spLd->spbUseTabs.reset(nullptr);
                    } 
                    else if (jsVal.is<bool>()) {
                        spLd->spbUseTabs.reset(new bool(jsVal.get<bool>()));
                    }
                    else {
                        throw std::runtime_error("Tabs JSON value is not an boolean");
                    }
                }
                auto itStyles = jsOne.find("styles");
                if(jsOne.end() != itStyles) {
                    if (!itStyles->second.is<picojson::object>()) {
                        throw std::runtime_error("Style JSON value is not an object");
                    }
                    auto& jsStyles = itStyles->second.get<picojson::object>();
                    auto itSubcls = jsStyles.find("subclasses");
                    if(jsStyles.end() != itSubcls) {
                        auto& jsarrSubcls = itSubcls->second;
                        if (!jsarrSubcls.is<picojson::array>()) {
                            throw std::runtime_error("Styles list JSON value is not an array");
                        }
                        for(auto& onestyle: jsarrSubcls.get<picojson::array>()) {
                            if (!onestyle.is<std::string>()) {
                                throw std::runtime_error("One style name JSON value is not an string");
                            }
                            srcstyle_ptr spStyle(new srcstyle_t);
                            spStyle->name = onestyle.get<std::string>();
                            spStyle->styleClassName = "GDEFAULT";
                            spStyle->styleClass = styleClasses["GDEFAULT"];
                            auto itOneStyle = jsStyles.find(spStyle->name);
                            if(jsStyles.end() != itOneStyle) {
                                if (!itOneStyle->second.is<picojson::object>()) {
                                    throw std::runtime_error("One style specialization JSON value is not an object");
                                }
                                auto& jsOneStyle = itOneStyle->second.get<picojson::object>();
                                auto itOnesClass = jsOneStyle.find("class");
                                if(jsOneStyle.end() != itOnesClass) {
                                    if(itOnesClass->second.is<std::string>()) {
                                        auto sParentName = itOnesClass->second.get<std::string>();
                                        auto itParent = styleClasses.find(sParentName);
                                        if(styleClasses.end() != itParent) {
                                            spStyle->styleClassName = sParentName;
                                            spStyle->styleClass = itParent->second;
                                        }
                                    }
                                }
                                auto itFg = jsOneStyle.find("fg");
                                if(jsOneStyle.end() != itFg && itFg->second.is<std::string>()) {
                                    spStyle->spFg.reset(new unsigned long(getRGB(itFg->second.get<std::string>(), &namedColors)));
                                }
                                auto itBg = jsOneStyle.find("bg");
                                if(jsOneStyle.end() != itBg && itBg->second.is<std::string>()) {
                                    spStyle->spBg.reset(new unsigned long(getRGB(itBg->second.get<std::string>(), &namedColors)));
                                }
                                auto itBold = jsOneStyle.find("bold");
                                if(jsOneStyle.end() != itBold && !itBold->second.is<bool>()) {
                                    spStyle->spbBold.reset(new bool(itBold->second.get<bool>()));
                                }
                                auto itItalic = jsOneStyle.find("italic");
                                if(jsOneStyle.end() != itItalic && itItalic->second.is<bool>()) {
                                    spStyle->spbItalic.reset(new bool(itItalic->second.get<bool>()));
                                }
                            }   

                            spLd->styles.push_back(spStyle);
                        }
                    }

                }
                auto itKeywords = jsOne.find("keywords");
                if(jsOne.end() != itKeywords) {
                    if (!itKeywords->second.is<picojson::array>()) {
                        throw std::runtime_error("Keywords JSON value is not an array");
                    }
                    auto& jsarrKeywords = itKeywords->second.get<picojson::array>();
                    for(auto& onewords: jsarrKeywords) {
                        if(onewords.is<std::string>()) {
                            auto& swords = onewords.get<std::string>();
                            auto pWords = new char[swords.size() + 1];
                            std::memcpy(pWords, swords.data(), swords.size());
                            pWords[swords.size()] = '\0';
                            spLd->keywords.push_back(std::shared_ptr<char[]>(pWords));
                        } else if(onewords.is<picojson::array>()) {
                            std::string concatParts;
                            for(auto& part: onewords.get<picojson::array>()) {
                                if(part.is<std::string>()) {
                                    concatParts += part.get<std::string>();
                                }
                            }
                            auto pWords = new char[concatParts.size() + 1];
                            std::memcpy(pWords, concatParts.data(), concatParts.size());
                            pWords[concatParts.size()] = '\0';
                            spLd->keywords.push_back(std::shared_ptr<char[]>(pWords));
                        } else {
                            throw std::runtime_error("Keywords JSON value is not an either string or array");
                        }
                    }
                }
                languages[sLangName] = spLd;
            }
        }
    }
}
