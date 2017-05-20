//
// Created by a1091793 on 17/2/17.
//


#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <blink/raster/utility.h>
#include "PngWriterLinearGradient.h"

void
get_all(const boost::filesystem::path& root, const std::string& filter, std::vector<boost::filesystem::path>& ret)
{
    namespace fs = boost::filesystem;
    if(!fs::exists(root) || !fs::is_directory(root)) return;

    fs::recursive_directory_iterator it(root);
    fs::recursive_directory_iterator endit;

    while(it != endit)
    {
        if(fs::is_regular_file(*it) && it->path().filename().string().find(filter) != std::string::npos)
        {
            ret.push_back(it->path());
        }
        ++it;

    }
}

int
main (int argc, char * argv[])
{

    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    std::string directory;
    std::string filter("None");
    int colours = BLUES;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "display help message")
            ("directory,d", po::value<std::string>(&directory), "directory to find gdal compatible raster to plot")
            ("filter,f", po::value<std::string>(&filter), "filter which selects what files to convert")
            ("style,s", po::value<int>(&colours), "Specify 1 for blues, 2 for Greens")
//            ("png,p", po::value<std::string>(&png_template), "file template to save the pngs")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return EXIT_SUCCESS;
    }

    if (!(vm.count("directory")))
    {
        std::cout << "Need to specify directory\n" << desc << "\n";
        return EXIT_SUCCESS;
    }

    fs::path directory_path(directory);
    if (!(fs::exists(directory)))
    {
        std::cout << "Specified raster directory does not exist on filesystem\n" << desc << "\n";
        return EXIT_SUCCESS;
    }


    // Iterate through directory to find appropriate rasters
    std::vector<fs::path> rasters;
    get_all(directory_path, filter, rasters);
    boost::shared_ptr<PngWriterLinearGradient> writer(nullptr);
    if (colours == BLUES) writer.reset(new PngWriterLinearGradient(BLUES));
    if (colours == GREENS) writer.reset(new PngWriterLinearGradient(GREENS));
//    PngWriterLinearGradient writer(BLUES);
    BOOST_FOREACH(fs::path raster_path, rasters)
                {
                    try
                    {
                        fs::path png_filename = raster_path.parent_path() /  (raster_path.stem().string() + ".png");
                        blink::raster::gdal_raster<double> raster = blink::raster::open_gdal_raster<double>(raster_path, GA_ReadOnly);
                        writer->render(raster, png_filename);
                    }
                    catch (boost::exception & e)
                    {
                        std::cout << "\tFor file " << raster_path << ":\n";
                        std::cout << boost::diagnostic_information(e) << "\n";
                    }
                    catch (...)
                    {
                        std::cout << "\tFor file " << raster_path << ", some exception occured during opening and printing to png\n";
                    }


                }




    return EXIT_SUCCESS;
}