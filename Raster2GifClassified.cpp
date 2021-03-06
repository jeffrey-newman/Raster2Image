//
// Created by a1091793 on 18/2/17.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <blink/raster/utility.h>

#include "GifWriterClassified.h"
#include "Raster2GifShared.h"



/**
 * The main method
 * @param argc
 * @param argv
 * @return
 */
int
main (int argc, char * argv[])
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    std::string colour_options("0 for LANDUSE0 and 1 for LANDUSE1");
    boost::optional<boost::shared_ptr<Raster2GifData> > cmd_line_data_opt = ProcessCmdLineOptions(argc, argv, colour_options, false);
    if(!cmd_line_data_opt) return EXIT_SUCCESS;
    boost::shared_ptr<Raster2GifData> cmd_line_data = *cmd_line_data_opt;


    boost::shared_ptr<GifWriterClassified> writer(nullptr);
    if (cmd_line_data->colours == LANDUSE0) writer.reset(new GifWriterClassified(LANDUSE0, cmd_line_data->delay, cmd_line_data->do_timestamp, cmd_line_data->year_start, cmd_line_data->font_size, cmd_line_data->origin_x, cmd_line_data->origin_y, cmd_line_data->dx, cmd_line_data->dy));
    if (cmd_line_data->colours == LANDUSE1) writer.reset(new GifWriterClassified(LANDUSE1, cmd_line_data->delay, cmd_line_data->do_timestamp, cmd_line_data->year_start, cmd_line_data->font_size, cmd_line_data->origin_x, cmd_line_data->origin_y, cmd_line_data->dx, cmd_line_data->dy));

    BOOST_FOREACH(std::string & raster_str, cmd_line_data->raster_files)
                {
                    fs::path raster_path(raster_str);
                    //    fs::path raster_path(raster_file);
                    if (!(fs::exists(raster_path)))
                    {
                        std::cout << "For raster: " << raster_path << "\n";
                        std::cout << "Specified raster does not exist on filesystem\n" << cmd_line_data->desc.operator*() << "\n";
                        return EXIT_SUCCESS;
                    }

                    blink::raster::gdal_raster<int> raster = blink::raster::open_gdal_raster<int>(raster_path, GA_ReadOnly);
                    writer->addFrame(raster);
                }

    writer->save(cmd_line_data->gif_path);



    return EXIT_SUCCESS;
}