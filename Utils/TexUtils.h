// Texture Utils.
// -------------------------------------------------------------------
// Copyright (C) 2010 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Logging/Logger.h>

namespace OpenEngine {
    namespace Utils {

        class TexUtils {
        public:
            template <class T> static Texture2DPtr(T) Scale(Texture2DPtr(T) src, 
                                                            unsigned int width, 
                                                            unsigned int height) {
                src->Load();

                Texture2D<T>* dst = new Texture2D<T>(width, height, src->GetColorFormat());
                dst->SetMipmapping(src->UseMipmapping());
                dst->SetWrapping(src->GetWrapping());
                dst->SetFiltering(src->GetFiltering());
                dst->SetCompression(src->UseCompression());
                dst->Load();
                
                for (unsigned int u = 0; u < width; ++u){
                    for (unsigned int v = 0; v < height; ++v){
                        float x = float(u) / float(width);
                        float y = float(v) / float(height);
                        T* pixel = dst->GetPixel(u,v);
                        Vector<4, T> color = src->InterpolatedPixel(x,y);
                        for (unsigned int c = 0; c < dst->GetChannels(); ++c){
                            pixel[c] = color.Get(c);
                        }
                    }
                }

                return Texture2DPtr(T)(dst);
            }
        };

    }
}
