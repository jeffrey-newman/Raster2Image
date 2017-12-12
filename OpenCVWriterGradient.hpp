//
// Created by a1091793 on 30/10/17.
//

#ifndef GEON_OPT_OPENCVWRITERGRADIENT_HPP
#define GEON_OPT_OPENCVWRITERGRADIENT_HPP

#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColourMaps.hpp"




class OpenCVWriterGradient {
private:

    ColourMapperGradient colours;

public:
    OpenCVWriterGradient(double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : colours(ColourMapperGradient::LINEAR, _red0, _green0, _blue0, _red1, _green1, _blue1, _min, _max)
    {

        if (colours.transfrm == ColourMapperGradient::LOG)
        {
            colours.min = log10(colours.min);
            colours.max = log10(colours.max);
        }
    }

    OpenCVWriterGradient(ColourMapperGradient & _colours)
            : colours(_colours)
    {

    }


    OpenCVWriterGradient(BuiltInGradients colour)
    {
        if (colour == BLUES)
        {
            colours.transfrm = ColourMapperGradient::LINEAR;
            colours.red0 = 159.89 ;
            colours.green0 = 175.7;
            colours.blue0 = 255 ;

            colours.red1 = 0.0;
            colours.green1 = 43.27;
            colours.blue1 = 186.12;

            colours.min = 0;
            colours.max= 10;
        }

    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster, const boost::filesystem::path & file_path)
    {
        cv::Mat output_img(raster.nRows(), raster.nCols(), CV_8UC3);
        boost::optional<T> no_data_val = raster.noDataVal();

        double red = 0, blue = 0, green = 0;

        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                T value = (raster.get(blink::raster::coordinate_2d(j,i)));
                double value_d = double(value);
                if (colours.transfrm == ColourMapperGradient::LOG)
                {
                    value_d = log10(value_d);
                }
                //Check for no data

                if (value == no_data_val)
                {
                    red = 255;
                    green = 255;
                    blue = 255;
                }
                else
                {
                    if (value_d < colours.min) value_d = colours.min;
                    if (value_d > colours.max) value_d = colours.max;
                    double relative_val = (value_d- colours.min) / (colours.max - colours.min);
                    red = colours.red0 - (colours.red0 - colours.red1) * relative_val;
                    green = colours.green0 - (colours.green0 - colours.green1) * relative_val;
                    blue = colours.blue0 - (colours.blue0 - colours.blue1) * relative_val;
                }

                if (red < 0) red  = 0;
                if (red > 255) red = 1;
                if (green < 0) green = 0;
                if (green > 255) green = 1;
                if (blue < 0) blue = 0;
                if (blue > 255) blue = 1;
                cv::Vec3b mgc_color(blue, green, red);
                output_img.at<cv::Vec3b>(j,i) = mgc_color;
            }
        }
        cv::imwrite(file_path.string(), output_img);
    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster)
    {
        boost::shared_ptr<cv::Mat> output_img(new cv::Mat(raster.nCols(), raster.nRows(), CV_8UC3));
        boost::optional<double> no_data_val = double(raster.noDataVal());

        double red = 0, blue = 0, green = 0;

        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                double value = double(raster.get(blink::raster::coordinate_2d(j,i)));
                if (colours.transfrm == ColourMapperGradient::LOG)
                {
                    value = log10(value);
                }
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
                    T relative_val = (value - colours.min) / (colours.max - colours.min);
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
                cv::Vec3b mgc_color(red,green,blue);
                output_img->at<cv::Vec3b>(j,i) = mgc_color;

            }

        }
    }

};


#endif //GEON_OPT_OPENCVWRITERGRADIENT_HPP
