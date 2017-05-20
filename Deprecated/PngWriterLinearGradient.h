//
// Created by a1091793 on 9/2/17.
//

#ifndef RASTER2PNG_PNGWRITERLINEARGRADIENT_H
#define RASTER2PNG_PNGWRITERLINEARGRADIENT_H

#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include "pngwriter.h"

enum BuiltInGradients {BLUES, GREENS};

class PngWriterLinearGradient {
private:
    double red0;
    double green0;
    double blue0;

    double red1;
    double green1;
    double blue1;

    double min;
    double max;

public:
    PngWriterLinearGradient(double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : red0(_red0), green0(_green0), blue0(_blue0), red1(_red1), green1(_green1), blue1(_blue1), min(_min), max(_max)
    {

    }

    PngWriterLinearGradient(BuiltInGradients colour)
    {
        if (colour == BLUES)
        {
            red0 = 159.89 /255.0;
            green0 = 175.7 / 255.0;
            blue0 = 255 / 255.0;

            red1 = 0.0;
            green1 = 43.27 / 255.0;
            blue1 = 186.12 / 255.0;

            min = 0;
            max= 10;
        }

    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster, boost::filesystem::path & file_path)
    {
        pngwriter image(raster.nCols()+1, raster.nRows()+1, 1.0, file_path.c_str());
        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                double value = raster.get(blink::raster::coordinate_2d(j,i));
                double red = 0, blue = 0, green = 0;
                //Check for no data
                boost::optional<double> no_data_val = raster.noDataVal();
                if (value == no_data_val)
                {
                    red = 255.0;
                    green = 255.0;
                    blue = 255.0;
                }
                else
                {
                    if (value < min) value = min;
                    if (value > max) value = max;
                    double relative_val = (value - min) / (max - min);
                    red = red0 - (red0 - red1) * relative_val;
                    green = green0 - (green0 - green1) * relative_val;
                    blue = blue0 - (blue0 - blue1) * relative_val;
                }
                int x = i +1;
                if (x > raster.nCols()) x = raster.nCols();
                if (x < 1) x = 1;
                int y = raster.nRows() - j;
                if (y < 1 ) y = 1;
                if (y > raster.nRows()) y = raster.nRows();
                if (red < 0) red  = 0;
                if (red > 1) red = 1;
                if (green < 0) green = 0;
                if (green > 1) green = 1;
                if (blue < 0) blue = 0;
                if (blue > 1) blue = 1;

                image.plot(x,y,red, green, blue);
            }

        }
//        image.write_png();
        image.close();
    }

};


#endif //RASTER2PNG_PNGWRITERLINEARGRADIENT_H
