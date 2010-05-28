#ifndef _PERLIN_NOISE_GENERATOR_
#define _PERLIN_NOISE_GENERATOR_

#include <Math/RandomGenerator.h>
#include <Resources/Texture3D.h>
#include <Utils/TextureTool.h>

namespace OpenEngine {
namespace Utils {

typedef float REAL;

class PerlinNoise {
 private:
    static FloatTexture2DPtr Combine(FloatTexture2DPtr l,
                                     FloatTexture2DPtr r,
                                     int multiplier = 1) {

        unsigned int w = max(l->GetWidth(),r->GetWidth());
        unsigned int h = max(l->GetHeight(),r->GetHeight());
        FloatTexture2DPtr output(new FloatTexture2D(w,h,1));

        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                REAL xCoord = (REAL)x / (REAL)w;
                REAL yCoord = (REAL)y / (REAL)h;

                REAL lValue =
                    l->InterpolatedPixel(xCoord,yCoord)[0];

                REAL rValue = multiplier *
                    r->InterpolatedPixel(xCoord,yCoord)[0];

                REAL value = (lValue + rValue);
                *(output->GetPixel(x,y)) = value;
            }
        }
        return output;
    }

    static FloatTexture3DPtr Combine3D(FloatTexture3DPtr l,
                                     FloatTexture3DPtr r,
                                     int multiplier = 1) {

        unsigned int w = max(l->GetWidth(),r->GetWidth());
        unsigned int h = max(l->GetHeight(),r->GetHeight());
        unsigned int d = max(l->GetDepth(),r->GetDepth());
        FloatTexture3DPtr output(new FloatTexture3D(w,h,d,1));

        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                for (unsigned int z=0; z<d; z++) {
                    REAL xCoord = (REAL)x / (REAL)w;
                    REAL yCoord = (REAL)y / (REAL)h;
                    REAL zCoord = (REAL)z / (REAL)d;

                    REAL lValue =
                        l->InterpolatedVoxel(xCoord,yCoord,zCoord)[0];

                    REAL rValue = multiplier *
                        r->InterpolatedVoxel(xCoord,yCoord,zCoord)[0];

                    REAL value = (lValue + rValue);
                    *(output->GetVoxel(x,y,z)) = value;
                }
            }
        }
        return output;
    }

    static FloatTexture2DPtr CreateNoise(unsigned int periodX,
                                         unsigned int periodY,
                                         unsigned int amplitude,
                                         unsigned int seed = 0) {
        unsigned int w = periodX;
        unsigned int h = periodY;
        FloatTexture2DPtr output(new FloatTexture2D(w,h,1));

        RandomGenerator r;
        r.Seed(seed);
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                REAL value = r.UniformFloat(0,amplitude*2);
                *(output->GetPixel(x,y)) = value;
            }
        }
        return output;
    }

    static FloatTexture3DPtr CreateNoise3D(unsigned int periodX,
                                           unsigned int periodY,
                                           unsigned int periodZ,
                                           unsigned int amplitude,
                                           unsigned int seed = 0) {
        unsigned int w = periodX;
        unsigned int h = periodY;
        unsigned int d = periodZ;
        FloatTexture3DPtr output(new FloatTexture3D(w,h,d,1));

        //logger.info << "amplitude: " << amplitude << logger.end;
        RandomGenerator r;
        r.Seed(seed);
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                for (unsigned int z=0; z<d; z++) {
                    REAL value = r.UniformFloat(0,amplitude*2);
                    *(output->GetVoxel(x,y,z)) = value;
                }
            }
        }
        return output;
    }

    static FloatTexture2DPtr Generate(unsigned int xResolution,
                                      unsigned int yResolution,
                                      unsigned int bandwidth,
                                      float mResolution,
                                      float mBandwidth,
                                      unsigned int smooth,
                                      unsigned int layers,
                                      RandomGenerator& r) {

            FloatTexture2DPtr noise =
                CreateNoise(xResolution, yResolution, bandwidth, 
                            r.UniformInt(0,256));

            if (layers != 0) {
                FloatTexture2DPtr small = 
                    Generate(xResolution * mResolution, 
                             yResolution * mResolution, 
                             bandwidth * mBandwidth,
                             mResolution, mBandwidth,
                             smooth, layers-1, r);
                int multiplier = 1;
                if (layers % 2 == 0)
                    multiplier *= -1;
                noise = Combine(noise, small, multiplier);
                Smooth(noise, smooth);
            }

#ifdef DEBUG_PRINT
            logger.info << "resolution: " << resolution;
            logger.info << " bandwidth: " << bandwidth << logger.end;
            Save(small, "small-r"+
                 Convert::ToString(resolution) + "-b" +
                 Convert::ToString(bandwidth));
            logger.info << "multiplier:" << multiplier << logger.end;
            Save(noise, "noise-r"+
                 Convert::ToString(resolution) + "-b" +
                 Convert::ToString(bandwidth));
#endif
            return noise;
    }

    static FloatTexture3DPtr Generate3D(unsigned int xResolution,
                                        unsigned int yResolution,
                                        unsigned int zResolution,
                                        unsigned int bandwidth,
                                        float mResolution,
                                        float mBandwidth,
                                        unsigned int smooth,
                                        unsigned int layers,
                                        RandomGenerator& r) {

            FloatTexture3DPtr noise =
                CreateNoise3D(xResolution, yResolution, zResolution,
                              bandwidth, r.UniformInt(0,256));
            /*
            {
            // dump layers
            string layername = "layer";
            layername += Convert::ToString(layers);
            TextureTool
                ::DumpTexture(ToRGBAinAlphaChannel3D(GetNormalize3D(noise,0,1)),
                              "output/" + layername);
            }
            */
            if (layers != 0) {
                FloatTexture3DPtr small = 
                    Generate3D(xResolution * mResolution, 
                               yResolution * mResolution, 
                               zResolution * mResolution, 
                               bandwidth * mBandwidth,
                               mResolution, mBandwidth,
                               smooth, layers-1, r);
                int multiplier = 1;
                if (layers % 2 == 0)
                    multiplier *= -1;
                noise = Combine3D(small, noise, multiplier);
                Smooth3D(noise, smooth);
                /*
                {
                    string layername = "combinedlayers";
                    layername += Convert::ToString(layers);
                    TextureTool
                ::DumpTexture(ToRGBAinAlphaChannel3D(GetNormalize3D(noise,0,1)),
                              "output/" + layername);
                }
                */
            }

#ifdef DEBUG_PRINT
            logger.info << "resolution: " << resolution;
            logger.info << " bandwidth: " << bandwidth << logger.end;
/*             Save(small, "small-r"+ */
/*                  Convert::ToString(resolution) + "-b" + */
/*                  Convert::ToString(bandwidth)); */
            logger.info << "multiplier:" << multiplier << logger.end;
/*             Save(noise, "noise-r"+ */
/*                  Convert::ToString(resolution) + "-b" + */
/*                  Convert::ToString(bandwidth)); */
#endif
            return noise;
    }

 public:

    static void Normalize(FloatTexture2DPtr tex, REAL bLimit, REAL uLimit) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();

        // find min and max value in tex
        REAL min = numeric_limits<REAL>::max();
        REAL max = numeric_limits<REAL>::min();
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                REAL v = *(tex->GetPixel(x,y));
                if (v<min) min = v;
                if (v>max) max = v;
            }
        }

        // normalize each pixel
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                // calculate value between 0 and 1
                REAL value = (*(tex->GetPixel(x,y))-min)/max;
                // scale pixels between bLimit and uLimit
                *(tex->GetPixel(x,y)) = (value * (uLimit-bLimit)) + bLimit;
            }
        }
    }

    static FloatTexture3DPtr 
    GetNormalize3D(FloatTexture3DPtr tex, REAL bLimit, REAL uLimit) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int d = tex->GetDepth();
        unsigned int c = tex->GetChannels();
        FloatTexture3DPtr output(new FloatTexture3D(w,h,d,c));
        const float* din = tex->GetData();
        float* dout = output->GetData();
        for (unsigned int z=0; z<d; z++) {
            for (unsigned int y=0; y<h; y++) {
                for (unsigned int x=0; x<w; x++) {
                    for (unsigned int ch=0; ch<c; ch++) {
                        dout[(x+y*w+z*w*h)*c+ch] = din[(x+y*w+z*w*h)*c+ch]; 
                    }
                }
            }
        }
        Normalize3D(output,bLimit,uLimit);
        return output;
    }
        
    static void Normalize3D(FloatTexture3DPtr tex, REAL bLimit, REAL uLimit) {
        const unsigned int w = tex->GetWidth();
        const unsigned int h = tex->GetHeight();
        const unsigned int d = tex->GetDepth();

        // find min and max value in tex
        REAL min = numeric_limits<REAL>::max();
        REAL max = numeric_limits<REAL>::min();
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                for (unsigned int z=0; z<d; z++) {
                    REAL v = *(tex->GetVoxel(x,y,z));
                    if (v<min) min = v;
                    if (v>max) max = v;
                }
            }
        }

        /*
        logger.info << "min: " << min << logger.end;
        logger.info << "max: " << max << logger.end;
        */

        // normalize each pixel
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                for (unsigned int z=0; z<d; z++) {
                    // calculate value between 0 and 1
                    REAL value = (*(tex->GetVoxel(x,y,z))-min)/max;
                    // scale pixels between bLimit and uLimit
                    *(tex->GetVoxel(x,y,z)) = 
                        (value * (uLimit-bLimit)) + bLimit;
                }
            }
        }
    }

    static void Smooth(FloatTexture2DPtr tex, unsigned int itr, int halfsize = 1) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int channels = tex->GetChannels();
        FloatTexture2DPtr tempXdir(new FloatTexture2D(w,h,channels));

        for (unsigned int x = 0; x < w; ++x) {
            for (unsigned int y = 0; y < h; ++y) {
                float* pixel = tempXdir->GetPixel(x, y);
                for (unsigned char c = 0; c < channels; ++c) {
                    pixel[c] = 0;
                    for (int X = -halfsize; X <= halfsize; ++X) {
                        pixel[c] += tex->GetPixel(x + X, y)[c];
                    }
                    pixel[c] /= (halfsize * 2 + 1);
                }
            }
        }

        for (unsigned int x = 0; x < w; ++x) {
            for (unsigned int y = 0; y < h; ++y) {
                float* pixel = tex->GetPixel(x, y);
                for (unsigned char c = 0; c < channels; ++c){
                    pixel[c] = 0;
                    for (int Y = -halfsize; Y <= halfsize; ++Y){
                        pixel[c] += tempXdir->GetPixel(x, y + Y)[c];
                    }
                    pixel[c] /= (halfsize * 2 + 1);
                }
            }
        }

    /*        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        for (unsigned int i=0; i<itr; i++) {
            for (unsigned int x=0; x<w; x++) {
                for (unsigned int y=0; y<h; y++) {
                    REAL l = *(tex->GetPixel(x-1,y));
                    REAL r = *(tex->GetPixel(x+1,y));
                    REAL d = *(tex->GetPixel(x,y-1));
                    REAL u = *(tex->GetPixel(x,y+1));

                    REAL ul = *(tex->GetPixel(x-1,y+1));
                    REAL ur = *(tex->GetPixel(x+1,y+1));
                    REAL dl = *(tex->GetPixel(x-1,y-1));
                    REAL dr = *(tex->GetPixel(x+1,y-1));

                    REAL c = *(tex->GetPixel(x,y));
                    // *(tex->GetPixel(x,y)) = ((l+r+d+u)+4*c)/8;
                    *(tex->GetPixel(x,y)) = ((l+r+d+u)+(ul+ur+dl+dr)+8*c)/16;
                }
            }
        }
    */
    }

    static void Smooth3D(FloatTexture3DPtr tex,
                         unsigned int itr, int halfsize = 1) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int d = tex->GetDepth();
        unsigned int channels = tex->GetChannels();
        FloatTexture3DPtr tempXdir(new FloatTexture3D(w,h,d,channels));
        FloatTexture3DPtr tempYdir(new FloatTexture3D(w,h,d,channels));

        for (unsigned int i = 0; i < itr; ++i) {

            for (unsigned int x = 0; x < w; ++x) {
                for (unsigned int y = 0; y < h; ++y) {
                    for (unsigned int z = 0; z < d; ++z) {
                        
                        float* pixel = tempXdir->GetVoxel(x, y, z);
                        for (unsigned char c = 0; c < channels; ++c) {
                            pixel[c] = 0;
                            for (int X = -halfsize; X <= halfsize; ++X) {
                                pixel[c] += tex->GetVoxel(x + X, y, z)[c];
                            }
                            pixel[c] /= (halfsize * 2 + 1);
                        }
                    }
                }
            }
            
            for (unsigned int x = 0; x < w; ++x) {
                for (unsigned int y = 0; y < h; ++y) {
                    for (unsigned int z = 0; z < d; ++z) {
                        float* pixel = tempYdir->GetVoxel(x, y, z);
                        for (unsigned char c = 0; c < channels; ++c){
                            pixel[c] = 0;
                            for (int Y = -halfsize; Y <= halfsize; ++Y){
                                pixel[c] += tempXdir->GetVoxel(x, y + Y, z)[c];
                            }
                            pixel[c] /= (halfsize * 2 + 1);
                        }
                    }
                }
            }
            
            for (unsigned int x = 0; x < w; ++x) {
                for (unsigned int y = 0; y < h; ++y) {
                    for (unsigned int z = 0; z < d; ++z) {
                        float* pixel = tex->GetVoxel(x, y, z);
                        for (unsigned char c = 0; c < channels; ++c){
                            pixel[c] = 0;
                            for (int Z = -halfsize; Z <= halfsize; ++Z){
                                pixel[c] += tempYdir->GetVoxel(x, y, z+Z)[c];
                            }
                            pixel[c] /= (halfsize * 2 + 1);
                        }
                    }
                }
            }
        }
        /*
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int d = tex->GetDepth();
        for (unsigned int i=0; i<itr; i++) {
            for (unsigned int x=0; x<w; x++) {
                for (unsigned int y=0; y<h; y++) {
                    for (unsigned int z=0; z<d; z++) {

                        REAL l = *(tex->GetVoxel(x-1,y,z));
                        REAL r = *(tex->GetVoxel(x+1,y,z));
                        REAL d = *(tex->GetVoxel(x,y-1,z));
                        REAL u = *(tex->GetVoxel(x,y+1,z));
                        REAL ul = *(tex->GetVoxel(x-1,y+1,z));
                        REAL ur = *(tex->GetVoxel(x+1,y+1,z));
                        REAL dl = *(tex->GetVoxel(x-1,y-1,z));
                        REAL dr = *(tex->GetVoxel(x+1,y-1,z));
                        REAL c = *(tex->GetVoxel(x,y,z));

                        REAL fl = *(tex->GetVoxel(x-1,y,z+1));
                        REAL fr = *(tex->GetVoxel(x+1,y,z+1));
                        REAL fd = *(tex->GetVoxel(x,y-1,z+1));
                        REAL fu = *(tex->GetVoxel(x,y+1,z+1));
                        REAL ful = *(tex->GetVoxel(x-1,y+1,z+1));
                        REAL fur = *(tex->GetVoxel(x+1,y+1,z+1));
                        REAL fdl = *(tex->GetVoxel(x-1,y-1,z+1));
                        REAL fdr = *(tex->GetVoxel(x+1,y-1,z+1));
                        REAL fc = *(tex->GetVoxel(x,y,z+1));

                        REAL bl = *(tex->GetVoxel(x-1,y,z-1));
                        REAL br = *(tex->GetVoxel(x+1,y,z-1));
                        REAL bd = *(tex->GetVoxel(x,y-1,z-1));
                        REAL bu = *(tex->GetVoxel(x,y+1,z-1));
                        REAL bul = *(tex->GetVoxel(x-1,y+1,z-1));
                        REAL bur = *(tex->GetVoxel(x+1,y+1,z-1));
                        REAL bdl = *(tex->GetVoxel(x-1,y-1,z-1));
                        REAL bdr = *(tex->GetVoxel(x+1,y-1,z-1));
                        REAL bc = *(tex->GetVoxel(x,y,z-1));

                        REAL m = (l+r+d+u) + (ul+ur+dl+dr) + 26.0*c;
                        REAL f = (fl+fr+fd+fu) + (ful+fur+fdl+fdr) + fc;
                        REAL b = (bl+br+bd+bu) + (bul+bur+bdl+bdr) + bc;

                        *(tex->GetVoxel(x,y,z)) = (m+f+b)/27.0;
                    }
                }
            }
        }
        */
    }

    static FloatTexture2DPtr Generate(unsigned int xResolution,
                                      unsigned int yResolution,
                                      unsigned int bandwidth,
                                      float mResolution,
                                      float mBandwidth,
                                      unsigned int smooth,
                                      unsigned int layers,
                                      unsigned int seed) {

#ifdef DEBUG_PRINT
        logger.info << "resolution: " << resolution;
        logger.info << " bandwidth: " << bandwidth << logger.end;
        Save(noise, "noise-r"+
             Convert::ToString(resolution) + "-b" +
             Convert::ToString(bandwidth));
#endif

        RandomGenerator r;
        r.Seed(seed);
        return Generate(xResolution, yResolution, bandwidth, mResolution,
                        mBandwidth, smooth, layers, r);
    }

    static FloatTexture3DPtr Generate3D(unsigned int xResolution,
                                        unsigned int yResolution,
                                        unsigned int zResolution,
                                        unsigned int bandwidth,
                                        float mResolution,
                                        float mBandwidth,
                                        unsigned int smooth,
                                        unsigned int layers,
                                        unsigned int seed) {

#ifdef DEBUG_PRINT
        logger.info << "resolution: " << resolution;
        logger.info << " bandwidth: " << bandwidth << logger.end;
/*         Save(noise, "noise-r"+ */
/*              Convert::ToString(resolution) + "-b" + */
/*              Convert::ToString(bandwidth)); */
#endif

        RandomGenerator r;
        r.Seed(seed);
        return Generate3D(xResolution, yResolution, zResolution,
                          bandwidth, mResolution,
                          mBandwidth, smooth, layers, r);
    }


static UCharTexture2DPtr ToUCharTexture(FloatTexture2DPtr tex) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    unsigned int c = tex->GetChannels();
    UCharTexture2DPtr output(new UCharTexture2D(w,h,c));
    
    for (unsigned int ch=0; ch<c; ch++) {
        /*
        REAL min = numeric_limits<REAL>::max();
        REAL max = numeric_limits<REAL>::min();
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                REAL v = *(tex->GetPixel(x,y) + ch);
                if (v<min) min = v;
                if (v>max) max = v;
            }
        }
        logger.info << "max: " << max << logger.end;
        logger.info << "min: " << min << logger.end;
        */
        for (unsigned int x=0; x<w; x++) {
            for (unsigned int y=0; y<h; y++) {
                /*
                *(output->GetPixel(x,y) + ch) =
                    ((*(tex->GetPixel(x,y) + ch)-min)/max) * 255;
                */
                *(output->GetPixel(x,y) + ch) =
                    *(tex->GetPixel(x,y) + ch) * 255;
            }
        }
    }
    return output;
}

static void Threshold(FloatTexture2DPtr tex, REAL threshold) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            if(*(tex->GetPixel(x,y)) < threshold)
                *(tex->GetPixel(x,y)) = 0;
        }
    }
}

static void CloudExpCurve(FloatTexture2DPtr tex) {
    //unsigned int CloudCover = 85; // 0-255 =density
    //REAL CloudSharpness = 0.5; //0-1 =sharpness
    REAL CloudCover = 0.215; // 0-255 =density
    REAL CloudSharpness = 10; //0-1 =sharpness

    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            /*
            *(tex->GetPixel(x,y)) = *(tex->GetPixel(x,y)) - CloudCover;
            if(*(tex->GetPixel(x,y)) < 0)
                *(tex->GetPixel(x,y)) = 0;
            *(tex->GetPixel(x,y)) = 255 - (pow(CloudSharpness , *(tex->GetPixel(x,y)) ) * 255);
*/
            *(tex->GetPixel(x,y)) = *(tex->GetPixel(x,y)) - CloudCover;
            *(tex->GetPixel(x,y)) = 
                1.0 - exp( -CloudSharpness * *(tex->GetPixel(x,y)) );
            if(*(tex->GetPixel(x,y)) < 0)
                *(tex->GetPixel(x,y)) = 0;
        }
    }
}

static void CloudExpCurve3D(FloatTexture3DPtr tex) {
    //unsigned int CloudCover = 85; // 0-255 =density
    //REAL CloudSharpness = 0.5; //0-1 =sharpness
    REAL CloudCover = 0.215; // 0-255 =density
    REAL CloudSharpness = 10; //0-1 =sharpness

    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    unsigned int d = tex->GetDepth();
    for (unsigned int x=0; x<w; x++) {
        for (unsigned int y=0; y<h; y++) {
            for (unsigned int z=0; z<d; z++) {
            /*
            *(tex->GetPixel(x,y)) = *(tex->GetPixel(x,y)) - CloudCover;
            if(*(tex->GetPixel(x,y)) < 0)
                *(tex->GetPixel(x,y)) = 0;
            *(tex->GetPixel(x,y)) = 255 - (pow(CloudSharpness , *(tex->GetPixel(x,y)) ) * 255);
*/
                *(tex->GetVoxel(x,y,z)) = *(tex->GetVoxel(x,y,z)) - CloudCover;
                *(tex->GetVoxel(x,y,z)) = 
                    1.0 - exp( -CloudSharpness * *(tex->GetVoxel(x,y,z)) );
                if(*(tex->GetVoxel(x,y,z)) < 0)
                    *(tex->GetVoxel(x,y,z)) = 0;
            }
        }
    }
}

static FloatTexture2DPtr ToRGBAinAlphaChannel(FloatTexture2DPtr tex) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    FloatTexture2DPtr output(new FloatTexture2D(w,h,4));
    float* din = tex->GetData();
    float* dout = output->GetData();
    for (unsigned int y=0; y<h; y++) {
        for (unsigned int x=0; x<w; x++) {
            dout[(x+y*w)*4+0] = 1.0f;
            dout[(x+y*w)*4+1] = 1.0f;
            dout[(x+y*w)*4+2] = 1.0f;
            dout[(x+y*w)*4+3] = din[x+y*w]; 
        }
    }
    return output;
}

static FloatTexture3DPtr ToRGBAinAlphaChannel3D(FloatTexture3DPtr tex) {
    unsigned int w = tex->GetWidth();
    unsigned int h = tex->GetHeight();
    unsigned int d = tex->GetDepth();
    FloatTexture3DPtr output(new FloatTexture3D(w,h,d,4));
    float* din = tex->GetData();
    float* dout = output->GetData();
    for (unsigned int z=0; z<d; z++) {
        for (unsigned int y=0; y<h; y++) {
            for (unsigned int x=0; x<w; x++) {
                dout[(x+y*w+z*w*h)*4+0] = 1.0f;
                dout[(x+y*w+z*w*h)*4+1] = 1.0f;
                dout[(x+y*w+z*w*h)*4+2] = 1.0f;
                dout[(x+y*w+z*w*h)*4+3] = din[x+y*w+z*w*h];
            }
        }
    }
    return output;
}


};

} // NS Utils
} // NS OpenEngine

#endif //_PERLIN_NOISE_GENERATOR_
