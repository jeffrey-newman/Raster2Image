//
// Created by a1091793 on 13/2/17.
//

#ifndef RASTER2PNG_GIFWRITERCLASSIFIED_H
#define RASTER2PNG_GIFWRITERCLASSIFIED_H

//#include <boost/scoped_ptr.hpp>
#include <cmath>
#include <Magick++.h>
#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>



class GifWriterClassified {

public:
    typedef std::map<int, Magick::ColorRGB > ColourMapper;
private:
    ColourMapper colour_map;

    std::vector<Magick::Image> frames;

    //time between frames in seconds
    float delay;
    bool do_timestamp;
    int year;

    std::string font_family;
    Magick::StyleType font_style;
    unsigned int font_weight;
    int font_size;

    unsigned int origin_x;
    unsigned int origin_y;
    unsigned int dx;
    unsigned int dy;


public:
    GifWriterClassified(ColourMapper & _colour_map, int _delay = 100, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                        unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
//            : saver((AnimatedGifSaver *) nullptr), is_initialised(false),
           : colour_map(_colour_map), delay(_delay), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy)
    {

    }


    GifWriterClassified(BuiltinColourMaps _colour_map_id, int _delay = 100, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                        unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
//            : saver((AnimatedGifSaver *) nullptr), is_initialised(false),
            : delay(_delay), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy)
    {
        typedef std::pair<const int, Magick::ColorRGB> ColourMapPair;
        if (_colour_map_id == LANDUSE0)
        {
            colour_map.insert(ColourMapPair(0,  Magick::ColorRGB(120 / 255.0,	120 / 255.0,	120 / 255.0)));
            colour_map.insert(ColourMapPair(1,  Magick::ColorRGB(20 / 255.0,	128 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(2,  Magick::ColorRGB(192 / 255.0,	  0 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(3,  Magick::ColorRGB(255 / 255.0,	 66 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(4,  Magick::ColorRGB(255 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(5,  Magick::ColorRGB(255 / 255.0,	160 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(6,  Magick::ColorRGB(146 / 255.0,	255 / 255.0,	 87 / 255.0)));
            colour_map.insert(ColourMapPair(7,  Magick::ColorRGB(192 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(8,  Magick::ColorRGB(255 / 255.0,	192 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(9,  Magick::ColorRGB(192 / 255.0,	 72 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(10, Magick::ColorRGB(  0 / 255.0,	192 / 255.0,	 32 / 255.0)));
            colour_map.insert(ColourMapPair(11, Magick::ColorRGB(135 / 255.0,	135 / 255.0,	135 / 255.0)));
            colour_map.insert(ColourMapPair(12, Magick::ColorRGB( 90 / 255.0,	 90 / 255.0,	 90 / 255.0)));
            colour_map.insert(ColourMapPair(13, Magick::ColorRGB(  0 / 255.0,	192 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(14, Magick::ColorRGB( 60 / 255.0,	 60 / 255.0,	 60 / 255.0)));
            colour_map.insert(ColourMapPair(15, Magick::ColorRGB(  0 / 255.0,	128 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(16, Magick::ColorRGB(233 / 255.0,	233 / 255.0,	255 / 255.0)));
        }
    }



    template <typename T> void
    addFrame(blink::raster::gdal_raster<T> & raster)
    {
        unsigned int nCols = std::min((unsigned int) raster.nCols(), dx);
        unsigned int nRows = std::min((unsigned int) raster.nRows(), dy);
        Magick::Image image(Magick::Geometry(nCols, nRows), "white");
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
                int class_val = raster.get(blink::raster::coordinate_2d(j, i));
                Magick::ColorRGB & colour = colour_map[class_val];
                *(view.get((i- origin_x),(j-origin_y),1,1)) = colour;
            }
        }
        view.sync();

        //Annotate with year if required.
        if (do_timestamp)
        {
//            Magick::DrawableFont();
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


#endif //RASTER2PNG_GIFWRITERCLASSIFIED_H
