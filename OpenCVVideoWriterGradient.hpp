//
// Created by a1091793 on 31/10/17.
//

#ifndef GEON_OPT_OPENCVVIDEOWRITERGRADIENT_HPP
#define GEON_OPT_OPENCVVIDEOWRITERGRADIENT_HPP



#include <cmath>
#include <boost/filesystem.hpp>
#include "OpenCVWriterGradient.hpp"



class OpenCVVideoWriterGradient {


private:

    //time between frames in seconds
    double fps;
    bool do_timestamp;
    int year;

    std::string font_family;
    unsigned int font_weight;
    int font_size;

    unsigned int origin_x;
    unsigned int origin_y;
    unsigned int dx;
    unsigned int dy;

    OpenCVWriterGradient raster_img_writer;
    std::vector<cv::Mat> frames;


public:
    OpenCVVideoWriterGradient(ColourMapperGradient & _colour_map, int _fps = 1, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                                unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
//            : saver((AnimatedGifSaver *) nullptr), is_initialised(false),
            : fps(_delay), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy), raster_img_writer(_colour_mapcolo)
    {

    }


    OpenCVVideoWriterGradient(BuiltInGradients _colour_map_id, int _fps = 1, bool _do_timestamp = false, int _year_start = 0, int _font_size =192, unsigned int _origin_x = 0,
                                unsigned int _origin_y = 0, unsigned int _dx = 999999, unsigned int _dy = 999999)
//            : saver((AnimatedGifSaver *) nullptr), is_initialised(false),
            : fps(_fps), do_timestamp(_do_timestamp), year(_year_start), font_size(_font_size), origin_x(_origin_x), origin_y(_origin_y), dx(_dx), dy(_dy), raster_img_writer(_colour_map_id)
    {

    }



    template <typename T> void
    addFrame(blink::raster::gdal_raster<T> & raster)
    {
        frames.push_back(raster_img_writer.getCVMat(raster));

        //Annotate with year if required.
        if (do_timestamp)
        {
            cv::putText(frames.back(), std::to_string(year++), cv::Point(0,0), FONT_HERSHEY_SIMPLEX, 192, cv::Scalar(1,1,1));
        }

    }

    void
    save(boost::filesystem::path & file_path)
    {
        cv::VideoWriter annimation(file_path.string(), cv::VideoWriter::fourcc('P','I','M','1'), fps,  cv::Size(frames.front().rows, frames.front().cols));
        foreach(cv::Mat & mat, frames)
        {
            annimation.write(mat);
        }


//        saver->Save(file_path.c_str());
    }
};


#endif //GEON_OPT_OPENCVVIDEOWRITERGRADIENT_HPP
