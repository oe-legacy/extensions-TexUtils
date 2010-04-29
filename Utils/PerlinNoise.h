#ifndef _PERLIN_NOISE_GENERATOR_
#define _PERLIN_NOISE_GENERATOR_

#include <Math/RandomGenerator.h>
#include <Resources/Texture3D.h>

namespace OpenEngine {
namespace Utils {

typedef float REAL;

class PerlinNoise {
 private:
    static FloatTexture2DPtr Combine(FloatTexture2DPtr l,
                                     FloatTexture2DPtr r,
                                     int multiplier = 0) {

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
                                     int multiplier = 0) {

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
            Smooth(noise, smooth);

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
            Smooth3D(noise, smooth);

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
                noise = Combine3D(noise, small, multiplier);
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

    static void Normalize3D(FloatTexture3DPtr tex, REAL bLimit, REAL uLimit) {
        unsigned int w = tex->GetWidth();
        unsigned int h = tex->GetHeight();
        unsigned int d = tex->GetDepth();

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

    static void Smooth(FloatTexture2DPtr tex, unsigned int itr) {
        unsigned int w = tex->GetWidth();
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
                    //*(tex->GetPixel(x,y)) = ((l+r+d+u)+4*c)/8;
                    *(tex->GetPixel(x,y)) = ((l+r+d+u)+(ul+ur+dl+dr)+8*c)/16;
                }
            }
        }
    }

    static void Smooth3D(FloatTexture3DPtr tex, unsigned int itr) {
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

                        // @todo: do this
                        //*(tex->GetPixel(x,y)) = ((l+r+d+u)+4*c)/8;
                        *(tex->GetVoxel(x,y,z)) = 
                            ((l+r+d+u)+(ul+ur+dl+dr)+8*c)/16;
                    }
                }
            }
        }
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

};

} // NS Utils
} // NS OpenEngine

#endif //_PERLIN_NOISE_GENERATOR_
