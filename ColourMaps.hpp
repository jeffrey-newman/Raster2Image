//
// Created by a1091793 on 7/11/17.
//

#ifndef GEON_OPT_COLOURMAPS_HPP
#define GEON_OPT_COLOURMAPS_HPP

enum BuiltinColourMaps{LANDUSE1, LANDUSE2};

struct ColourRGB
{
    double red; double green; double blue;
    ColourRGB(double _red = 0, double _green = 0, double _blue = 0):
            red(_red), green(_green), blue(_blue)
    {

    }
};

typedef std::map<int,  ColourRGB > ColourMapperClassified;


enum BuiltInGradients {BLUES, GREENS};

struct ColourMapperGradient
{
    enum Transform {LINEAR, LOG};
    ColourMapperGradient(Transform transfrm, double _red0, double _green0, double _blue0, double _red1, double _green1, double _blue1, double _min, double _max)
            : red0(_red0), green0(_green0), blue0(_blue0), red1(_red1), green1(_green1), blue1(_blue1), min(_min), max(_max)
    {

    }

    ColourMapperGradient()
            : transfrm(LINEAR), red0(0), green0(0), blue0(0), red1(0), green1(0), blue1(0), min(0), max(0)
    {

    }

    Transform transfrm;
    double red0;
    double green0;
    double blue0;

    double red1;
    double green1;
    double blue1;

    double min;
    double max;
};

#endif //GEON_OPT_COLOURMAPS_HPP
