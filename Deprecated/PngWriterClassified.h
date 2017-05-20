//
// Created by a1091793 on 9/2/17.
//

#ifndef RASTER2PNG_PNGWRITERCLASSIFIED_H
#define RASTER2PNG_PNGWRITERCLASSIFIED_H

#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include "pngwriter.h"

enum BuiltinColourMaps{LANDUSE1, LANDUSE2};

class PngWriterClassified {

public:
    typedef std::map<int, std::tuple<double, double, double > > ColourMapper;
private:
    ColourMapper colour_map;

public:
    PngWriterClassified(ColourMapper & _colour_map)
            : colour_map(_colour_map)
    {

    }

    PngWriterClassified(BuiltinColourMaps _colour_map_id)
    {
        typedef std::tuple<double, double, double> RGB;
        typedef std::pair<const int, RGB> ColourMapPair;
        if (_colour_map_id == LANDUSE1)
        {
            colour_map.insert(ColourMapPair(0,RGB(120 / 255.0,	120 / 255.0,	120 / 255.0)));
            colour_map.insert(ColourMapPair(1,RGB(20 / 255.0,	128 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(2,RGB(192 / 255.0,	  0 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(3,RGB(255 / 255.0,	 66 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(4,RGB(255 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(5,RGB(255 / 255.0,	160 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(6,RGB(146 / 255.0,	255 / 255.0,	 87 / 255.0)));
            colour_map.insert(ColourMapPair(7,RGB(192 / 255.0,	  0 / 255.0,	192 / 255.0)));
            colour_map.insert(ColourMapPair(8,RGB(255 / 255.0,	192 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(9,RGB(192 / 255.0,	 72 / 255.0,	  0 / 255.0)));
            colour_map.insert(ColourMapPair(10,RGB(  0 / 255.0,	192 / 255.0,	 32 / 255.0)));
            colour_map.insert(ColourMapPair(11,RGB(135 / 255.0,	135 / 255.0,	135 / 255.0)));
            colour_map.insert(ColourMapPair(12,RGB( 90 / 255.0,	 90 / 255.0,	 90 / 255.0)));
            colour_map.insert(ColourMapPair(13,RGB(  0 / 255.0,	192 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(14,RGB( 60 / 255.0,	 60 / 255.0,	 60 / 255.0)));
            colour_map.insert(ColourMapPair(15,RGB(  0 / 255.0,	128 / 255.0,	255 / 255.0)));
            colour_map.insert(ColourMapPair(16,RGB(233 / 255.0,	233 / 255.0,	255 / 255.0)));
        }
    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster, boost::filesystem::path & file_path) {
        pngwriter image(raster.nCols(), raster.nRows(), 1.0, file_path.c_str());
        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                int class_val = raster.get(blink::raster::coordinate_2d(j, i));
                std::tuple<double, double, double> &colour = colour_map[class_val];
                image.plot(i + 1, raster.nRows() - j, std::get<0>(colour), std::get<1>(colour),
                           std::get<2>(colour));
            }

        }
        image.close();
    }

};


#endif //RASTER2PNG_PNGWRITERCLASSIFIED_H
