//
// Created by a1091793 on 30/10/17.
//

#ifndef GEON_OPT_OPENCVWRITERCLASSIFIED_HPP
#define GEON_OPT_OPENCVWRITERCLASSIFIED_HPP

#include <boost/filesystem.hpp>
#include <blink/raster/gdal_raster.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "ColourMaps.hpp"


class OpenCVWriterClassified
{
private:
    ColourMapperClassified colour_map;

public:
    OpenCVWriterClassified(ColourMapperClassified & _colour_map)
    : colour_map(_colour_map)
            {

            }

    OpenCVWriterClassified(BuiltinColourMaps _colour_map_id)
    {

        typedef std::pair<const int, ColourRGB> ColourMapPair;
        if (_colour_map_id == LANDUSE1)
        {
            colour_map.insert(ColourMapPair(0,ColourRGB(120,	120,	120)));
            colour_map.insert(ColourMapPair(1,ColourRGB(20, 	128,	  0)));
            colour_map.insert(ColourMapPair(2,ColourRGB(192,	  0,	  0)));
            colour_map.insert(ColourMapPair(3,ColourRGB(255,	 66,	  0)));
            colour_map.insert(ColourMapPair(4,ColourRGB(255,	  0,	192)));
            colour_map.insert(ColourMapPair(5,ColourRGB(255,	160,	255)));
            colour_map.insert(ColourMapPair(6,ColourRGB(146,	255,	 87)));
            colour_map.insert(ColourMapPair(7,ColourRGB(192,	  0,	192)));
            colour_map.insert(ColourMapPair(8,ColourRGB(255,	192,	  0)));
            colour_map.insert(ColourMapPair(9,ColourRGB(192,	 72,	  0)));
            colour_map.insert(ColourMapPair(10,ColourRGB(  0,	192,	 32)));
            colour_map.insert(ColourMapPair(11,ColourRGB(135,	135,	135)));
            colour_map.insert(ColourMapPair(12,ColourRGB( 90,	 90,	 90)));
            colour_map.insert(ColourMapPair(13,ColourRGB(  0,	192,	255)));
            colour_map.insert(ColourMapPair(14,ColourRGB( 60,	 60,	 60)));
            colour_map.insert(ColourMapPair(15,ColourRGB(  0,	128,	255)));
            colour_map.insert(ColourMapPair(16,ColourRGB(233,	233,	255)));
        }
    }

    template <typename T> void
    render(blink::raster::gdal_raster<T> & raster, const boost::filesystem::path & file_path)
    {
        boost::shared_ptr<cv::Mat> output_img = getCVMat(raster);
        cv::imwrite(file_path.string(), *output_img);
    }


    template <typename T> boost::shared_ptr<cv::Mat>
            getCVMat(blink::raster::gdal_raster<T> & raster)
    {
        boost::shared_ptr<cv::Mat> output_img(new cv::Mat(raster.nRows(), raster.nCols(), CV_8UC3));


        boost::optional<T> no_data_val = raster.noDataVal();
        cv::Vec3b no_data_color(1,1,1);

//        std::cerr << "Specifying pixmap for image\n";
        for (int i = 0; i < raster.nCols(); ++i)
        {
            for (int j = 0; j < raster.nRows(); ++j)
            {
                T class_val = raster.get(blink::raster::coordinate_2d(j, i));
                if (class_val == no_data_val)
                {
                    output_img->at<cv::Vec3b>(j,i) = no_data_color;
                }
                else
                {
                    ColourRGB &colour = colour_map[int(class_val)];
                    cv::Vec3b colour_cv(colour.blue, colour.green, colour.red);
                    output_img->at<cv::Vec3b>(j, i) = colour_cv;
                }

            }

        }
        return output_img;
    }
};


#endif //GEON_OPT_OPENCVWRITERCLASSIFIED_HPP



