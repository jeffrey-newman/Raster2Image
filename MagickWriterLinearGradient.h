//
// Created by a1091793 on 18/2/17.
//

#ifndef RASTER2PNG_IMAGEMAGICKWRITERLINEARGRADIENT_H
#define RASTER2PNG_IMAGEMAGICKWRITERLINEARGRADIENT_H

#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include <Magick++.h>

enum BuiltInGradients {BLUES, GREENS};

struct ColourMapperLinearGradient
{
    ColourMapperLinearGradient(double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : red0(_red0), green0(_green0), blue0(_blue0), red1(_red1), green1(_green1), blue1(_blue1), min(_min), max(_max)
    {

    }

    ColourMapperLinearGradient()
            : red0(0), green0(0), blue0(0), red1(0), green1(0), blue1(0), min(0), max(0)
    {

    }

    double red0;
    double green0;
    double blue0;

    double red1;
    double green1;
    double blue1;

    double min;
    double max;
};

class MagickWriterLinearGradient {
private:

    ColourMapperLinearGradient colours;

public:
    MagickWriterLinearGradient(double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : colours(_red0, _green0, _blue0, _red1, _green1, _blue1, _min, _max)
    {

    }

    MagickWriterLinearGradient(ColourMapperLinearGradient & _colours)
            : colours(_colours)
    {

    }


    MagickWriterLinearGradient(BuiltInGradients colour)
    {
        if (colour == BLUES)
        {
            colours.red0 = 159.89 /255.0;
            colours.green0 = 175.7 / 255.0;
            colours.blue0 = 255 / 255.0;

            colours.red1 = 0.0;
            colours.green1 = 43.27 / 255.0;
            colours.blue1 = 186.12 / 255.0;

            colours.min = 0;
            colours.max= 10;
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

        boost::optional<double> no_data_val = raster.noDataVal();
        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                double value = raster.get(blink::raster::coordinate_2d(j,i));
                double red = 0, blue = 0, green = 0;
                //Check for no data

                if (value == no_data_val)
                {
                    red = 1;
                    green = 1;
                    blue = 1;
                }
                else
                {
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
                *(view.get(i,j,1,1)) = mgc_color;

            }

        }
        view.sync();
        image.magick("png");
        image.write(file_path.c_str());

    }

};


#endif //RASTER2PNG_IMAGEMAGICKWRITERLINEARGRADIENT_H
