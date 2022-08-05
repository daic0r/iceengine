//
//  TextManager.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Fonts/TextManager.h>
#include <fstream>
#include <sstream>
#include <System/Config.h>
#include <System/SystemServices.h>
#include <Utils/FinalAction.h>
#include <Interfaces/ITextRenderer.h>
#include <iostream>

#ifdef RENDERER_OPEN_GL
#include <Renderer/OpenGL/TextRendererGL.h>
#endif

namespace Ice {

namespace GUI {

bool TextManager::m_bInstanced{ false };

TextManager::TextManager() {
    assert(!m_bInstanced);
    m_bInstanced = true;

//#ifdef RENDERER_OPEN_GL
//    m_pTextRenderer = std::make_unique<TextRendererGL>();
//#endif
}

void TextManager::registerFont(const std::string &strName) {
    std::string strFileName = systemServices.getConfig()->prependAssetPath("Fonts/" + strName);
//    std::string strFileName{ Config::BasePath };
//    strFileName += "Assets/Fonts/" + strName;

//    auto pTexMan = systemServices.getTextureManager();
//    auto pTex = pTexMan->loadTextureAtlas(strName, strFileName + ".png");
    
    FontInfo info = readFontFile(strFileName + ".fnt");
    //info.pTexture = pTex;
    info.pTexture = systemServices.getTextRenderer()->registerFontTexture(strName, strFileName + ".png");
    
    for (auto& [ch, ci] : info.characters) {
        std::tie(ci.x, ci.y) = info.pTexture->getTexCoord(ci.pixX, ci.pixY);
        ci.texWidth = info.pTexture->texelWidth() * static_cast<float>(ci.width);
        ci.texHeight = info.pTexture->texelHeight() * static_cast<float>(ci.height);
    }
    
    m_mFonts[info.name] = info;
}

const FontInfo& TextManager::getFont(const std::string& strName) const {
    if (m_mFonts.find(strName) == m_mFonts.end()) {
        throw std::runtime_error("Font not found");
    }
    return m_mFonts.at(strName);
}

FontInfo TextManager::readFontFile(const std::string& strFile) {
    std::ifstream f{ strFile };
    
    if (!f.good())
        throw std::runtime_error("Font file could not be opened");

    auto defer = finally([&f] {
        if (f.is_open())
            f.close();
    });
    
    bool bFirstLine{ true };

    std::string strLine;
    std::string strTemp, strTok, strValue;
    
    FontInfo ret;
    int nTexHeight{ -1 }, nTexWidth{ -1 };

    auto extractToken = [](std::istringstream& iss, const std::string& strDelim = "=") {
        std::string strTok;
        iss >> strTok;
		if (strTok.empty())
			return std::make_pair(std::string{}, std::string{});
        std::string strName = strTok.substr(0, strTok.find(strDelim));
        std::string strValue = strTok.substr(strTok.find(strDelim)+1);
        if (strValue.front() == '"' && strValue.back() != '"') {
            std::string strTemp = strValue;
            while (strTemp.back() != '"') {
                iss >> strTemp;
                strValue += " ";
                strValue += strTemp;
            }
        }
        if (strValue[0] == '"')
            strValue = strValue.substr(1, strValue.length()-2);
        return std::make_pair(strName, strValue);
    };
    
    
    while (std::getline(f, strLine)) {
        std::istringstream iss{ strLine };

        if (bFirstLine) {
            iss >> strTemp;

            if (strTemp != "info")
                throw std::runtime_error("Unexpected token in font file");
            
            ret.name = extractToken(iss).second;
            while (iss) {
                std::tie(strTemp, strValue) = extractToken(iss);
                if (strTemp == "size")
                    ret.size = std::stoi(strValue);
                else
                if (strTemp == "bold")
                    ret.bold = std::stoi(strValue);
                else
                if (strTemp == "italic")
                    ret.italic = std::stoi(strValue);
                else
                if (strTemp == "padding") {
                    int pos = 0;
                    int nSideForPadding = 0;
                    
                    for (nSideForPadding = 0; nSideForPadding < 4; ++nSideForPadding) {
                        std::string strPaddingValue;
                        while (pos < strValue.length() && strValue[pos] != ',') {
                            strPaddingValue += strValue[pos++];
                        }
                        const int nIntValue = std::stoi(strPaddingValue);
                        switch (nSideForPadding) {
                            case 0:
                                ret.padding.top = nIntValue;
                                break;
                            case 1:
                                ret.padding.right = nIntValue;
                                break;
                            case 2:
                                ret.padding.bottom = nIntValue;
                                break;
                            case 3:
                                ret.padding.left = nIntValue;
                                break;
                        }
                        ++pos;
                    }
                }
            }
            
            
            std::getline(f, strTemp);
            iss = std::istringstream{ strTemp };
            
            iss >> strTemp;

            if (strTemp != "common")
                throw std::runtime_error("Unexpected token in font file");
            
//            iss >> strTemp; iss >> strTemp;
//            iss >> strTok;
//            strValue = strTok.substr(strTok.find("=")+1);
//            nTexWidth = std::stoi(strValue);
//            iss >> strTok;
//            strValue = strTok.substr(strTok.find("=")+1);
//            nTexHeight = std::stoi(strValue);
            
            while (iss) {
                std::tie(strTemp, strValue) = extractToken(iss);
                if (strTemp == "scaleW")
                    nTexWidth = std::stoi(strValue);
                else
                if (strTemp == "scaleH")
                    nTexHeight = std::stoi(strValue);
                else
                if (strTemp == "lineHeight")
                    ret.lineHeight = std::stoi(strValue);
                else
                if (strTemp == "base")
                    ret.baselineHeight = std::stoi(strValue);
            }
            
            //if (nTexWidth != pTex->width() || nTexHeight != pTex->height())
            //    throw std::runtime_error("Dimensions in .fnt file do not match those of the corresponding image file");
            
            ret.lineHeight -= (ret.padding.top + ret.padding.bottom);
            
            std::getline(f, strTemp);
            std::getline(f, strTemp);
            bFirstLine = false;
            continue;
        }
        
        FontCharacterInfo ci;
        
        iss >> strTemp;
        if (strTemp != "char")
            throw std::runtime_error("Unexpected token in font file");
        
        int x{ -1 }, y{ - 1};

        while (iss) {
            std::tie(strTemp, strValue) = extractToken(iss);
            if (!iss)
                break;
            int nVal = std::stoi(strValue);
            
            if (strTemp == "id")
                ci.id = nVal;
            else
            if (strTemp == "x")
                ci.pixX = nVal;
            else
            if (strTemp == "y")
                ci.pixY = nVal;
            else
            if (strTemp == "width")
                ci.width = nVal;
            else
            if (strTemp == "height")
                ci.height = nVal;
            else
            if (strTemp == "xoffset")
                ci.xoffset = nVal;
            else
            if (strTemp == "yoffset")
                ci.yoffset = nVal;
            else
            if (strTemp == "xadvance")
                ci.xadvance = nVal;
        } // for
        
        // Undo the padding and use the desired padding instead
        // Since we're using singned distance fields, the padding is used in the font file
        // so the bold characters with their distance fields won't overlap in the texture file.
        // If we render the characters with a way smaller thickness than provided by the distance fields,
        // (which we probably will), we will not need to accomodate characters this bold when rendering.
        // Therefore, we undo the padding introduced by Hiero and use our own padding,
        // which will probably be less, instead. If we don't, the large padding with the lesser-thickness
        // rendered characters will make the characters spaced too far apart.
        ci.pixX += ret.padding.left - DESIRED_PADDING;
        ci.pixY += ret.padding.top - DESIRED_PADDING;
        ci.xoffset += ret.padding.left - DESIRED_PADDING;
        ci.yoffset += ret.padding.top - DESIRED_PADDING;
        ci.width -= (ret.padding.right + ret.padding.left) - (2 * DESIRED_PADDING);
        ci.height -= (ret.padding.bottom + ret.padding.top) - (2 * DESIRED_PADDING);
        ci.xadvance -= ret.padding.left + ret.padding.right;
        
        ret.characters[ci.id] = ci;
    } // while
    
    return ret;
}

}

}
