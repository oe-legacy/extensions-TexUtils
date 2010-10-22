#ifndef _VALUE_NOISE_GENERATOR_
#define _VALUE_NOISE_GENERATOR_

#include <Math/RandomGenerator.h>
#include <Resources/Texture3D.h>
#include <Utils/TextureTool.h>
#include <Utils/TexUtils.h>

namespace OpenEngine {
namespace Utils {

typedef float REAL;

class ValueNoise {
 private:
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
                                      unsigned int blur,
                                      unsigned int layers,
                                      RandomGenerator& r) {

            FloatTexture2DPtr noise =
                CreateNoise(xResolution, yResolution, bandwidth, 
                            r.UniformInt(0,256));

#ifdef DEBUG_PRINT
            FloatTexture2DPtr noiseClone(noise->Clone());
            Normalize(noiseClone,0,1); 
            FloatTexture2DPtr noiseTexture = 
                ToRGBAinAlphaChannel(noiseClone);
            TextureTool<unsigned char>::
                DumpTexture(ToUCharTexture(noiseTexture),
                            std::string("generated") +
                            "-rx" + Convert::ToString(xResolution) +
                            "-b"  + Convert::ToString(bandwidth) +
                            ".png");
#endif
            if (layers != 0) {
                FloatTexture2DPtr smallTex = 
                    Generate(xResolution * mResolution, 
                             yResolution * mResolution, 
                             bandwidth * mBandwidth,
                             mResolution, mBandwidth,
                             blur, layers-1, r);
                //int multiplier = 1;
                //if (layers % 2 == 0)
                    //multiplier *= -1;
                noise = TexUtils::Combine(noise, smallTex/*, multiplier*/);
                TexUtils::Blur(noise, blur);

#ifdef DEBUG_PRINT
                FloatTexture2DPtr smallClone(noise->Clone());
                Normalize(smallClone,0,1);
                FloatTexture2DPtr smallTexture = 
                    ToRGBAinAlphaChannel(smallClone);
                TextureTool<unsigned char>::
                    DumpTexture(ToUCharTexture(smallTexture),
                                std::string("combined") +
                                "-l" + Convert::ToString(layers) +
                                "-b" + Convert::ToString(bandwidth) +
                                ".png");
#endif
            }

#ifdef DEBUG_PRINT
            logger.info << "resolution: " << resolution;
            logger.info << " bandwidth: " << bandwidth << logger.end;
            logger.info << "multiplier:" << multiplier << logger.end;
#endif
            return noise;
    }

    static FloatTexture3DPtr Generate3D(unsigned int xResolution,
                                        unsigned int yResolution,
                                        unsigned int zResolution,
                                        unsigned int bandwidth,
                                        float mResolution,
                                        float mBandwidth,
                                        unsigned int blur,
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
                               blur, layers-1, r);
                int multiplier = 1;
                if (layers % 2 == 0)
                    multiplier *= -1;
                noise = TexUtils::Combine3D(small, noise, multiplier);
                TexUtils::Blur3D(noise, blur);
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
    static FloatTexture2DPtr Generate(unsigned int xResolution,
                                      unsigned int yResolution,
                                      unsigned int bandwidth,
                                      float mResolution,
                                      float mBandwidth,
                                      unsigned int blur,
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
                        mBandwidth, blur, layers, r);
    }

    static FloatTexture3DPtr Generate3D(unsigned int xResolution,
                                        unsigned int yResolution,
                                        unsigned int zResolution,
                                        unsigned int bandwidth,
                                        float mResolution,
                                        float mBandwidth,
                                        unsigned int blur,
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
                          mBandwidth, blur, layers, r);
    }

};

} // NS Utils
} // NS OpenEngine

#endif //_VALUE_NOISE_GENERATOR_
