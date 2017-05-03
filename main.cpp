//#include "/Users/a1091793/anaconda/include/libpng16/png.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <blink/raster/utility.h> // To open rasters
#include "pngwriter.h"

const double RED0 = 159.89 /255.0;
const double GREEN0 = 175.7 / 255.0;
const double BLUE0 = 255 / 255.0;

const double RED1 = 0.0;
const double GREEN1 = 43.27 / 255.0;
const double BLUE1 = 186.12 / 255.0;

const double MIN_INUNDATION = 0;
const double MAX_INUNDATION = 10;

void printRaster2PNG(blink::raster::gdal_raster<double> & raster, boost::filesystem::path & file_path)
{

    pngwriter image(raster.nCols()+1, raster.nRows()+1, 1.0, file_path.c_str());
    for (int i = 0; i < raster.nCols(); ++i)
    {
        for (int j = 0; j < raster.nRows(); ++j)
        {
            double inundation = raster.get(blink::raster::coordinate_2d(j,i));
            double red = 0, blue = 0, green = 0;
            //Check for no data
            boost::optional<double> no_data_val = raster.noDataVal();
            if (inundation == no_data_val)
            {
                red = 255.0;
                green = 255.0;
                blue = 255.0;
            }
            else
            {
                if (inundation < MIN_INUNDATION) inundation = MIN_INUNDATION;
                if (inundation > MAX_INUNDATION) inundation = MAX_INUNDATION;
                double relative_depth = (inundation - MIN_INUNDATION) / (MAX_INUNDATION - MIN_INUNDATION);
                red = RED0 - (RED0 - RED1) * relative_depth;
                green = GREEN0 - (GREEN0 - GREEN1) * relative_depth;
                blue = BLUE0 - (BLUE0 - BLUE1) * relative_depth;
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
    image.write_png();
    image.close();

}

int main()
{
    boost::filesystem::path map("/Volumes/home/Jeffs Resilio Sync Folder/Projects/Patawalonga Integrated Flood Risk Project/Results/Inundation/Normal/inundation_2016_rcp45_ari20.tif");
    blink::raster::gdal_raster<double> inundation = blink::raster::open_gdal_raster<double>(map, GA_ReadOnly);
    boost::filesystem::path out("sample.png");
    printRaster2PNG(inundation, out);
    return 0;
}