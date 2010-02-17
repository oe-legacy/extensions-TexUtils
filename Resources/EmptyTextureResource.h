// Empty texture resource.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _EMPTY_TEXTURE_RESOURCE_H_
#define _EMPTY_TEXTURE_RESOURCE_H_

#include <boost/serialization/weak_ptr.hpp>
#include <Resources/ITextureResource.h>

#include <cstring> // includes memcpy

using namespace OpenEngine::Resources;

//namespace OpenEngine {
//    namespace Resources {

class EmptyTextureResource;

typedef boost::shared_ptr<EmptyTextureResource> EmptyTextureResourcePtr;

class EmptyTextureResource : public ITextureResource {
private:

    boost::weak_ptr<EmptyTextureResource> weak_this;
    
    EmptyTextureResource(unsigned int w, unsigned int h, unsigned int d) 
        : ITextureResource() {

        this->width = w;
        this->height = h;
        this->channels = d/8;
                
        switch (d){
        case 8: this->format = LUMINANCE; break;
        case 24: this->format = RGB; break;
        case 32: this->format = RGBA; break;                    
        }
        this->data = new unsigned char[width*height*d/8];
    }
public:
    static EmptyTextureResourcePtr Create(unsigned int w,
                                          unsigned int h,
                                          unsigned int d) {
        EmptyTextureResourcePtr ptr(new EmptyTextureResource(w,h,d));
        ptr->weak_this = ptr;
        return ptr;
    }

    static EmptyTextureResourcePtr Clone(ITextureResourcePtr tex) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int c = tex->GetChannels();
        EmptyTextureResourcePtr ptr = Create(w,h,c*8);

        ptr->CopyData(tex);
        return ptr;
    }
    
    static EmptyTextureResourcePtr CloneChannel(ITextureResourcePtr tex,
                                                unsigned int channel) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int c = tex->GetChannels();
        EmptyTextureResourcePtr ptr = Create(w,h,8);

        unsigned char* from = tex->GetData();
        unsigned char* to = ptr->GetData();

        for (unsigned int x=0; x<w; x++)
            for (unsigned int y=0; y<h; y++)
                to[x+y*w] = from[x*c+y*w*c];
        return ptr;
    }
            
    ~EmptyTextureResource() { exit(-1);  delete[] data; }
    void Load() {}
    void Unload() {} //delete data; }

    void RebindTexture() {
        changedEvent.Notify(TextureChangedEventArg(EmptyTextureResourcePtr(weak_this)));
    }

    void CopyData(ITextureResourcePtr tex) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int c = tex->GetChannels();
        unsigned char* from = tex->GetData();
        std::memcpy(data, from, sizeof(unsigned char)*w*h*c);
    }

    //void CopyData(Tex<float>* tex);

    unsigned char& operator()(const unsigned int x,
                              const unsigned int y,
                              const unsigned int component = 0) {
        return data[y*width*channels+x*channels+component];
    }
};
//    }
//}

#endif
