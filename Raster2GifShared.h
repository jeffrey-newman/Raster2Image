//
// Created by a1091793 on 20/2/17.
//

#ifndef RASTER2PNG_RASTER2GIFSHARED_H
#define RASTER2PNG_RASTER2GIFSHARED_H

#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

struct Raster2GifData
{
    std::vector<std::string> raster_files;
    std::string config_file;
    std::string gif_file;
    boost::filesystem::path gif_path;
    int colours;
    bool do_timestamp;
    int year_start;
    boost::shared_ptr<boost::program_options::options_description> desc;
    int delay;
    int font_size;
    unsigned int origin_x;
    unsigned int origin_y;
    unsigned int dx;
    unsigned int dy;

    //Linear Gradient Parameters
    double min;
    double max;
};

/**
 * Additional command line parser which interprets '@something' as a
 * option "config-file" with the value "something"
 * @param s A string token taken from the command line flags
 * @return Pair containing the converted flag ('config-file') and the value ([path to config file])
 */
std::pair<std::string, std::string> at_option_parser(std::string const&s)
{
    if ('@' == s[0])
        return std::make_pair(std::string("config-file"), s.substr(1));
    else
        return std::pair<std::string, std::string>();
}

boost::optional<boost::shared_ptr<Raster2GifData> > ProcessCmdLineOptions(int argc, char * argv[], std::string colour_options, bool is_linear_gradient = false)
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;
    boost::shared_ptr<Raster2GifData> cmd_line_data(new Raster2GifData);
    cmd_line_data->desc.reset(new boost::program_options::options_description("Available options"));
    cmd_line_data->desc->add_options()
            ("help", "display help message")
            ("raster,r", po::value<std::vector<std::string> >(&(cmd_line_data->raster_files))->composing(), "gdal compatible rasters to plot")
            ("gif,g", po::value<std::string>(&(cmd_line_data->gif_file)), "file path to save the png")
            ("style,s", po::value<int>(&(cmd_line_data->colours))->default_value(0), colour_options.c_str())
            ("config-file,c", po::value<std::string>(&(cmd_line_data->config_file)), "can be specified with '@name', too")
            ("time-stamp,t", po::value<bool>(&cmd_line_data->do_timestamp)->default_value(false), "print timestamp on each frame. Only incrememntal years supported")
            ("year-start,y", po::value<int>(&cmd_line_data->year_start)->default_value(0), "Year to begin counting frames from")
            ("delay,d", po::value<int>(&cmd_line_data->delay)->default_value(100), "Delay between frames in hundredths of seconds. Default is 1 second")
            ("font-size,f", po::value<int>(&cmd_line_data->font_size)->default_value(192), "Size of font to write year")
            ("origin-x,x", po::value<unsigned int>(&cmd_line_data->origin_x)->default_value(0), "Subset print: origin - column number")
            ("origin-y,y", po::value<unsigned int>(&cmd_line_data->origin_y)->default_value(0), "Subset print: origin - row number")
            ("dx,d", po::value<unsigned int>(&cmd_line_data->dx)->default_value(99999), "Subset print: range - number of columns subset spans")
            ("dy,e", po::value<unsigned int>(&cmd_line_data->dy)->default_value(99999), "Subset print: range - number of row subset spans")
            ;
    cmd_line_data->desc->add_options()
            ("min,n", po::value<double>(&cmd_line_data->min)->default_value(0.0), "Value of raster to begin linear gradient colouring from")
            ("max,m", po::value<double>(&cmd_line_data->max)->default_value(10.0), "Value of raster to end linear gradient colouring from")
            ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc,argv).options(cmd_line_data->desc.operator*()).extra_parser(at_option_parser).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << cmd_line_data->desc.operator*() << "\n";
        return boost::none;
    }

    if (vm.count("config-file"))
    {
        // Load the file and tokenize it
        std::ifstream ifs(cmd_line_data->config_file.c_str());
        if (!ifs) {
            std::cout << "Could not open the config file\n";
            return boost::none;
        }
        po::store(po::parse_config_file(ifs, cmd_line_data->desc.operator*()), vm);
        po::notify(vm);
    }

    if (!(vm.count("raster")))
    {
        std::cout << "Need to specify raster to print\n" << cmd_line_data->desc.operator*() << "\n";
        return boost::none;
    }

    if (!(vm.count("gif")))
    {
        std::cout << "Need to specify file name to write git to\n" << cmd_line_data->desc.operator*() << "\n";
        return boost::none;
    }
    cmd_line_data->gif_path = fs::path(cmd_line_data->gif_file);
    if (!(fs::exists(cmd_line_data->gif_path.parent_path())))
    {
        std::cout << "Parent path of " << cmd_line_data->gif_path << " does not exist. Aborting\n";
        return boost::none;
    }

    return cmd_line_data;

};


#endif //RASTER2PNG_RASTER2GIFSHARED_H
