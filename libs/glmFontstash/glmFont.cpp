//
//  glmFont.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFont.h"

#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstash.h"

glmFont::glmFont(): m_bLoaded(false) {
    m_fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
}

glmFont::~glmFont(){
    unload();
}

void glmFont::unload(){
    m_bLoaded = false;
    
    if(m_fs != NULL) {
        glfonsDelete(m_fs);
    }
}

bool glmFont::loadFont(std::string _filename, float _fontsize, float _depth, bool _bUsePolygons){
    _fontsize *= 2;
    
    m_font = fonsAddFont(m_fs, "sans", _filename.c_str());
    if(m_font == FONS_INVALID) {
        std::cerr << "Could not add font normal" << std::endl;
        glfonsDelete(m_fs);
        m_fs = NULL;
        
        m_bLoaded = false;
    } else {
        m_effect = FONS_EFFECT_DISTANCE_FIELD;
        
        fonsSetBlur(m_fs, 4.5);
        fonsSetBlurType(m_fs, m_effect);
        
        fonsSetSize(m_fs, _fontsize);
        fonsSetFont(m_fs, m_font);
        
        glfonsSetSDFProperties(m_fs, 0.25, 0.35, 0.38, 0.55, 0.7);
        
        fonsVertMetrics(m_fs, &m_ascender, &m_descender, &m_lineh);
        
        fonsSetErrorCallback(m_fs, glmFont::handleFontstashError, this);
        
        m_bLoaded = true;
    }
    
    return m_bLoaded;
}

void glmFont::handleFontstashError(void* uptr, int error, int val) {
    glmFont* ref = (glmFont*) uptr;
    
    switch (error) {
        case FONS_ATLAS_FULL:
            int width, height;
            fonsGetAtlasSize(ref->m_fs, &width, &height);
            if(width <= ATLAS_MAX_SIZE && height <= ATLAS_MAX_SIZE) {
                fonsExpandAtlas(ref->m_fs, width * 2, height * 2, 0);
            } else {
                std::cerr << "Atlas couldn't expand more than " << ATLAS_MAX_SIZE << std::endl;
            }
            break;
        default:
            break;
    }
}

bool glmFont::isLoaded(){
    return m_bLoaded;
}

glmRectangle glmFont::getStringBoundingBox(unsigned int id) {
    if(m_bLoaded){
        glm::vec4 bbox;
        glfonsGetBBox(m_fs, id, &bbox[0], &bbox[1], &bbox[2], &bbox[3]);
        return glmRectangle(bbox);
    }
    return glmRectangle();
}

void glmFont::drawString(unsigned int _id, float _alpha) {
    glfonsSetColor(m_fs, fontColor.r, fontColor.g, fontColor.b, _alpha);
    glfonsSetOutlineColor(m_fs, outlineColor.r, outlineColor.g, outlineColor.b, _alpha);
    glfonsScale(m_fs, 1, -1);
    glfonsDrawText(m_fs, _id);
}

void glmFont::drawString(unsigned int _id, const glm::vec3 &_pos, float _alpha) {
    glfonsPushMatrix(m_fs);
    
    glfonsTranslate(m_fs, _pos.x, _pos.y);
    
    glfonsSetColor(m_fs, fontColor.r, fontColor.g, fontColor.b, _alpha);
    glfonsSetOutlineColor(m_fs, outlineColor.r, outlineColor.g, outlineColor.b, _alpha);
    glfonsDrawText(m_fs, _id);
    
    glfonsPopMatrix(m_fs);
}

void glmFont::drawString(unsigned int _id, const glmRectangle &_rect, float _alpha) {
    glm::vec3 p = _rect.getBottomLeft();
    glfonsPushMatrix(m_fs);
    
    glfonsTranslate(m_fs, p.x, p.y);
    
    glfonsSetColor(m_fs, fontColor.r, fontColor.g, fontColor.b, _alpha);
    glfonsSetOutlineColor(m_fs, outlineColor.r, outlineColor.g, outlineColor.b, _alpha);
    glfonsDrawText(m_fs, _id);
    
    glfonsPopMatrix(m_fs);
}

void glmFont::drawSubString(unsigned int _id, unsigned int _i, bool _offset, float _alpha) {
    if(!_offset) {
        float xglyphOffset = glfonsGetGlyphOffset(m_fs, _id, _i);
        glfonsTranslate(m_fs, -xglyphOffset, 0.0);
    }
    drawSubString(_id, _i, _i, _alpha);
}

void glmFont::drawSubString(unsigned int _id, unsigned int _from, unsigned int _to, float _alpha) {
    glfonsScale(m_fs, 1, -1);
    
    glfonsSetColor(m_fs, fontColor.r, fontColor.g, fontColor.b, _alpha);
    glfonsSetOutlineColor(m_fs, outlineColor.r, outlineColor.g, outlineColor.b, _alpha);
    
    glfonsDrawText(m_fs, _id, _from, _to);
}

FONScontext* glmFont::getContext() const {
    return m_fs;
}

FONSeffectType glmFont::getEffect() const {
    return m_effect;
}

float glmFont::getAscender() const {
    return m_ascender;
}

float glmFont::getDescender() const {
    return m_descender;
}

float glmFont::getLineH() const {
    return m_lineh;
}