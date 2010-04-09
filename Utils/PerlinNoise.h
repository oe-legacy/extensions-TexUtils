#ifndef _PERLIN_NOISE_GENERATOR_
#define _PERLIN_NOISE_GENERATOR_

#include <Math/RandomGenerator.h>


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
};

} // NS Utils
} // NS OpenEngine

#endif //_PERLIN_NOISE_GENERATOR_
