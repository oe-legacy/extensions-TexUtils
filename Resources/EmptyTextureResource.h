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
            
    ~EmptyTextureResource() { exit(-1);  delete[] data; }
    void Load() {}
    void Unload() {} //delete data; }

    void RebindTexture() {
        changedEvent.Notify(TextureChangedEventArg(EmptyTextureResourcePtr(weak_this)));
    }

    unsigned char& operator()(const unsigned int x,
                              const unsigned int y,
                              const unsigned int component) {
        return data[y*width*channels+x*channels+component];
    }
};
//    }
//}

#endif
