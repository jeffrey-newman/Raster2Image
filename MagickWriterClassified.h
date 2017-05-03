//
// Created by a1091793 on 19/2/17.
//

#ifndef RASTER2PNG_IMAGEMAGICKWRITERCLASSIFIED_H
#define RASTER2PNG_IMAGEMAGICKWRITERCLASSIFIED_H

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include <Magick++.h>
#include <boost/spirit/include/qi.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

enum BuiltinColourMaps{LANDUSE1, LANDUSE2};

typedef std::map<int, Magick::ColorRGB > ColourMapperClassified;

class MagickWriterClassified {

private:
    ColourMapperClassified colour_map;

public:
    MagickWriterClassified(ColourMapperClassified & _colour_map)
            : colour_map(_colour_map)
    {

    }

    MagickWriterClassified(BuiltinColourMaps _colour_map_id)
    {

        typedef std::pair<const int, Magick::ColorRGB> ColourMapPair;
        if (_colour_map_id == LANDUSE1)
        {
            colour_map.insert(ColourMapPair(0,Magick::ColorRGB(120 / 255.0,	120 / 255.0,	120 / 255.0)));
            colour_map.insert(ColourMapPair(1,Magick::ColorRGB(20 / 255.0,	128 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(2,Magick::ColorRGB(192 / 255.0,	  0 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(3,Magick::ColorRGB(255 / 255.0,	 66 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(4,Magick::ColorRGB(255 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(5,Magick::ColorRGB(255 / 255.0,	160 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(6,Magick::ColorRGB(146 / 255.0,	255 / 255.0,	 87 / 255.0)));
            colour_map.insert(ColourMapPair(7,Magick::ColorRGB(192 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(8,Magick::ColorRGB(255 / 255.0,	192 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(9,Magick::ColorRGB(192 / 255.0,	 72 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(10,Magick::ColorRGB(  0 / 255.0,	192 / 255.0,	 32 / 255.0)));
            colour_map.insert(ColourMapPair(11,Magick::ColorRGB(135 / 255.0,	135 / 255.0,	135 / 255.0)));
            colour_map.insert(ColourMapPair(12,Magick::ColorRGB( 90 / 255.0,	 90 / 255.0,	 90 / 255.0)));
            colour_map.insert(ColourMapPair(13,Magick::ColorRGB(  0 / 255.0,	192 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(14,Magick::ColorRGB( 60 / 255.0,	 60 / 255.0,	 60 / 255.0)));
            colour_map.insert(ColourMapPair(15,Magick::ColorRGB(  0 / 255.0,	128 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(16,Magick::ColorRGB(233 / 255.0,	233 / 255.0,	255 / 255.0)));
        }
    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster, boost::filesystem::path & file_path)
    {
        Magick::Image image(Magick::Geometry(raster.nCols(), raster.nRows()), "white");
        // Set the image type to TrueColor DirectClass representation.
        image.type(Magick::TrueColorType);
        // Ensure that there is only one reference to underlying image
        // If this is not done, then image pixels will not be modified.
        image.modifyImage();

        // Allocate pixel view
        Magick::Pixels view(image);

        boost::optional<int> no_data_val = raster.noDataVal();
        Magick::ColorRGB no_data_color(1,1,1);

        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                int class_val = raster.get(blink::raster::coordinate_2d(j, i));
                if (class_val == no_data_val)
                {
                    *(view.get(i,j,1,1)) = no_data_color;
                }
                else
                {
                    Magick::ColorRGB &colour = colour_map[class_val];
                    *(view.get(i, j, 1, 1)) = colour;
                }

            }

        }
        view.sync();
        image.magick("png");
        image.write(file_path.c_str());
    }

};



#endif //RASTER2PNG_IMAGEMAGICKWRITERCLASSIFIED_H
