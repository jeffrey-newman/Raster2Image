//
// Created by a1091793 on 14/2/17.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <blink/raster/utility.h>
#include "MagickWriterGradient.h"
#include "ColourMapperParsers.h"

// Additional command line parser which interprets '@something' as a
// option "config-file" with the value "something"
std::pair<std::string, std::string> at_option_parser(std::string const&s)
{
    if ('@' == s[0])
        return std::make_pair(std::string("config-file"), s.substr(1));
    else
        return std::pair<std::string, std::string>();
}

typedef std::pair<std::string, boost::filesystem::path> CmdLinePaths;

bool
pathify(CmdLinePaths & path)
{
    if (path.first.empty())
    {
        std::cout << "Warning: path given to pathify in Raster2ImageGradient.cpp is empty\n";
        return false;
    }
    path.second = boost::filesystem::path(path.first);
    if (!(boost::filesystem::exists(path.second)))
    {
        std::cout << "Warning: path " << path.first << " does not exist\n";
        return false;
    }
    return true;
}

bool
pathify_mk(CmdLinePaths & path)
{
    path.second = boost::filesystem::path(path.first);
    if (!(boost::filesystem::exists(path.second)))
    {
        try
        {
            boost::filesystem::create_directories(path.second);
            std::cout << "path " << path.first << " did not exist, so created\n";
        }
        catch(boost::filesystem::filesystem_error& e)
        {
            std::cout << "Attempted to create " << path.first << " but was unable\n";
            std::cout << e.what() << "\n";
            return false;
        }

        return true;
    }
    return true;
}

int
main (int argc, char * argv[])
{

    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    std::vector<std::string> raster_files;
    std::string config_file;
    int colours = -1;
    CmdLinePaths legend_file_pth;


    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "display help message")
            ("raster,r", po::value<std::vector<std::string> >(&raster_files)->composing(), "gdal compatible rasters to plot")
//            ("png,p", po::value<std::string>(&png_file), "file path to save the png")
            ("style,s", po::value<int>(&colours), "Specify 0 for blues, 1 for Greens")
            ("legend,l", po::value<std::string>(&legend_file_pth.first)->default_value("none"), "Legend file for colouring, only used if styles not set")
            ("config-file,c", po::value<std::string>(&config_file), "can be specified with '@name', too")
            ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(desc).extra_parser(at_option_parser).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    if (vm.count("config-file"))
    {
        // Load the file and tokenize it
        std::ifstream ifs(config_file.c_str());
        if (!ifs) {
            std::cout << "Could not open the config file\n";
            return EXIT_FAILURE;
        }
        po::store(po::parse_config_file(ifs, desc), vm);
        po::notify(vm);
    }


    if (!(vm.count("raster")))
    {
        std::cout << "Need to specify raster to print\n" << desc << "\n";
        return EXIT_SUCCESS;
    }

    boost::shared_ptr<ColourMapperGradient> colourmap(nullptr);
    boost::shared_ptr<MagickWriterGradient> writer(nullptr);
    if (colours == BLUES) writer.reset(new MagickWriterGradient(BLUES));
    if (colours == GREENS) writer.reset(new MagickWriterGradient(GREENS));
    if (colours == -1) 
    {
        if (legend_file_pth.first == "none")
        {
            std::cout << "Legend file or preset colour gradient not specified\n";
            return EXIT_SUCCESS;
        }
        else
        {
            if (pathify(legend_file_pth))
            {
                colourmap = parseColourMapGradient(legend_file_pth.second);
                writer.reset(new MagickWriterGradient(*colourmap));
            }
            else
            {
                std::cout << "Legend file path does not exist\n";
                return EXIT_SUCCESS;
            }
        }

    }



    BOOST_FOREACH(std::string & raster_str, raster_files)
                {
                    fs::path raster_path(raster_str);
                    //    fs::path raster_path(raster_file);
                    if (!(fs::exists(raster_path)))
                    {
                        std::cout << "For raster: " << raster_path << "\n";
                        std::cout << "Specified raster does not exist on filesystem\n" << desc << "\n";
                        return EXIT_SUCCESS;
                    }
                    fs::path png_filename = raster_path.parent_path() /  (raster_path.stem().string() + ".png");
                    blink::raster::gdal_raster<double> raster = blink::raster::open_gdal_raster<double>(raster_path, GA_ReadOnly);
                    writer->render(raster, png_filename);
                    std::cout << "I wrote " << raster_path << " to " << png_filename << "\n";
                }

    return EXIT_SUCCESS;
}