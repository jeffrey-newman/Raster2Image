//
// Created by a1091793 on 13/2/17.
//

#ifndef RASTER2PNG_GIFWRITERLINEARGRADIENT_H
#define RASTER2PNG_GIFWRITERLINEARGRADIENT_H

#include <cmath>
#include <boost/scoped_ptr.hpp>
//#include "AnimatedGifSaver.h"
#include <Magick++.h>
#include <blink/raster/gdal_raster.h>

enum BuiltInGradients {BLUES, GREENS};

struct ColourMapperGradient
{
    enum Transform {LINEAR, LOG};
    ColourMapperGradient(Transform transfrm, double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : red0(_red0), green0(_green0), blue0(_blue0), red1(_red1), green1(_green1), blue1(_blue1), min(_min), max(_max)
    {

    }

    ColourMapperGradient()
            : transfrm(LINEAR), red0(0), green0(0), blue0(0), red1(0), green1(0), blue1(0), min(0), max(0)
    {

    }

    Transform transfrm;
    double red0;
    double green0;
    double blue0;

    double red1;
    double green1;
    double blue1;

    double min;
    double max;
};

class GifWriterGradient {

private:
    ColourMapperGradient colours;

    unsigned int origin_x;
    unsigned int origin_y;
    unsigned int dx;
    unsigned int dy;

//    boost::scoped_ptr<AnimatedGifSaver> saver;
    std::vector<Magick::Image> frames;
//    bool is_initialised;

    //time between frames in seconds
    float delay;
    bool do_timestamp;
    int year;

    std::string font_family;
    Magick::StyleType font_style;
    unsigned int font_weight;
    int font_size;

public:
    GifWriterGradient(int _red0, int _green0, int _blue0, int _red1, int _green1, int _blue1,  double _min, double _max, int _delay = 100, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                            unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
            : colours(ColourMapperGradient::LINEAR, _red0, _green0, _blue0, _red1, _green1, _blue1, _min, _max)
//            , saver((AnimatedGifSaver *) nullptr), is_initialised(false),
              , delay(_delay), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy)
    {

    }

    GifWriterGradient(BuiltInGradients colour,  double _min, double _max, int _delay = 100, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                            unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
//    : saver((AnimatedGifSaver *) nullptr), is_initialised(false)
    :  delay(_delay), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy)
    {
        if (colour == BLUES)
        {
            colours.transfrm = ColourMapperGradient::LINEAR;
            colours.red0 = 159.89 /255.0;
            colours.green0 = 175.7 / 255.0;
            colours.blue0 = 255 / 255.0;

            colours.red1 = 0.0;
            colours.green1 = 43.27 / 255.0;
            colours.blue1 = 186.12 / 255.0;

            colours.min = _min;
            colours.max= _max;
        }

    }

    template <typename T> void
    addFrame(blink::raster::gdal_raster<T> & raster)
    {
//        if (!is_initialised)
//        {
//            saver.reset(new AnimatedGifSaver(raster.nCols(), raster.nRows()));
//        }
        unsigned int nCols = std::min((unsigned int) raster.nCols(), dx);
        unsigned int nRows = std::min((unsigned int) raster.nRows(), dy);
        Magick::Image image(Magick::Geometry(raster.nCols(), raster.nRows()), "white");
        // Set the image type to TrueColor DirectClass representation.
        image.type(Magick::TrueColorType);
        // Ensure that there is only one reference to underlying image
        // If this is not done, then image pixels will not be modified.
        image.modifyImage();

        // Allocate pixel view
        Magick::Pixels view(image);

        unsigned int end_x = origin_x + nCols;
        unsigned int end_y = origin_y + nRows;
        for (int i = origin_x; i < end_x; ++i)
        {
            for (int j = origin_y; j < end_y; ++j)
            {
                double value = raster.get(blink::raster::coordinate_2d(j,i));
                double red = 0, blue = 0, green = 0;
                //Check for no data
                boost::optional<double> no_data_val = raster.noDataVal();                
                if (value == no_data_val)
                {
                    red = 1;
                    green = 1;
                    blue = 1;
                }
                else
                {
                    if (colours.transfrm == ColourMapperGradient::LOG)
                    {
                        value = log10(value);
                    }
                    if (value < colours.min) value = colours.min;
                    if (value > colours.max) value = colours.max;
                    double relative_val = (value - colours.min) / (colours.max - colours.min);
                    red = colours.red0 - (colours.red0 - colours.red1) * relative_val;
                    green = colours.green0 - (colours.green0 - colours.green1) * relative_val;
                    blue = colours.blue0 - (colours.blue0 - colours.blue1) * relative_val;
                }

                if (red < 0) red  = 0;
                if (red > 1) red = 1;
                if (green < 0) green = 0;
                if (green > 1) green = 1;
                if (blue < 0) blue = 0;
                if (blue > 1) blue = 1;
                Magick::ColorRGB mgc_color(red,green,blue);
                *(view.get((i- origin_x),(j-origin_y),1,1)) = mgc_color;
            }

        }
        view.sync();

        //Annotate with year if required.
        if (do_timestamp)
        {
            // set the text rendering font (the color is determined by the "current" image setting)
//            image.font("-*-bitstream charter-medium-r-normal-*-*-180-*-*-*-*-iso8859-1");
            image.font("helvetica");
            image.fontPointsize(double(font_size));
            // draw text with different gravity position
            image.annotate(std::to_string(year++), Magick::NorthWestGravity);
        }

        image.animationDelay(delay);
        frames.push_back(image);

    }

    void
    save(boost::filesystem::path & file_path)
    {
        Magick::writeImages(frames.begin(), frames.end(), file_path.c_str());
//        saver->Save(file_path.c_str());
    }

};


#endif //RASTER2PNG_GIFWRITERLINEARGRADIENT_H
