//
// Created by a1091793 on 27/4/17.
//

#ifndef GEON_ZONAL_OPT_COLOURMAPPERPARSERS_H
#define GEON_ZONAL_OPT_COLOURMAPPERPARSERS_H

#include "MagickWriterClassified.h"
#include "MagickWriterGradient.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>


/**
 * @brief this defines what the parser should skip in the optFile
 * @details think of these are grammers - they define the rules of
 * how the input file should be formatted. This one defines how
 * comments should be formatted - because these are skipped over, as well as white space
 * @see Boost spirit documentation
 */
struct Skipper : boost::spirit::qi::grammar< std::string::iterator >
{
    Skipper() : Skipper::base_type(skip_it)
    {
        namespace qi = boost::spirit::qi;
        namespace ph = boost::phoenix;

        comment_char = '#';
        skip_it = (qi::space | comment);
        comment = qi::lexeme[qi::lit(comment_char) >> *(qi::char_ - qi::eol) >> qi::eol ];
    }

    boost::spirit::qi::rule< std::string::iterator > skip_it; /**< Rule that does the work. passes over commetns and whitespace in the file */
    boost::spirit::qi::rule<std::string::iterator> comment; /**< Rule that detects comments in the file.*/
    char comment_char; /**< Define what the comment character is. I.e. we follow the Epanet inp file tradition of ';' */
};

boost::shared_ptr<ColourMapperClassified> parseColourMapClassified(boost::filesystem::path  classified_legend_file)
{
    std::vector<boost::fusion::vector<int, std::vector<double> > > colours;
    boost::shared_ptr<ColourMapperClassified> colour_mapper(new ColourMapperClassified);

    namespace qi = boost::spirit::qi;
    namespace ph = boost::phoenix;

    qi::rule<std::string::iterator, std::vector<double>(), Skipper > rgb_parser = qi::double_ >>  qi::double_ >> qi::double_;
    qi::rule<std::string::iterator, boost::fusion::vector<int, std::vector<double> > (), Skipper > category_parser = qi::int_ >> rgb_parser;
    qi::rule<std::string::iterator, std::vector<boost::fusion::vector<int, std::vector<double> > >(), Skipper > legend_parser = +category_parser;


    //Now we read the file.
    std::ifstream fin(classified_legend_file.string().c_str(), std::ios_base::in);
    if (fin.is_open())
    {
        //Start parsing the file - String to store the contents of the file
        std::string fileContents;
        //Copy contents of file to the string, make sure to include the white space
        fin.unsetf(std::ios::skipws);
        std::copy(std::istream_iterator<char>(fin),
                  std::istream_iterator<char>(),
                  std::back_inserter(fileContents));
        // The parser grammer requires a line ending after some things. Can cause problems
        // if one of these things is on the last line of the file. So we append another
        // new line just in case.
        fileContents.append("     \n");
        //Print the read-in contents to clog.

        //Now we parse the file. WE need (rewindable) iterators at the start and end of the string.
        std::string::iterator it = fileContents.begin();
        std::string::iterator end = fileContents.end();
        Skipper skipper;
        bool r = boost::spirit::qi::phrase_parse(it, end, legend_parser, skipper, colours);

        if (r && it == end)
        {
//			std::cout << "-------------------------\n";
//			std::cout << "Parsing succeeded\n";
//			std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(it, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \"" << rest << "\"\n";
            std::cout << "-------------------------\n";
            throw std::runtime_error("Error while parsing file. Check format of classified colour mapper file");
        }
        fin.close();


        typedef boost::fusion::vector<int, std::vector<double> >  CategoryColourMap;
        typedef std::pair<const int, Magick::ColorRGB> ColourMapPair;
        BOOST_FOREACH(CategoryColourMap & clr_map, colours)
                    {
                        colour_mapper->insert(ColourMapPair(boost::fusion::at_c<0>(clr_map),Magick::ColorRGB(boost::fusion::at_c<1>(clr_map)[0] / 255.0,
                                                                                                             boost::fusion::at_c<1>(clr_map)[1]  / 255.0,
                                                                                                             boost::fusion::at_c<1>(clr_map)[2]  / 255.0
                                             )             )                                                );
                    }

    }
    else
    {
        std::string errmsg = "Error while opening file. File with name " + classified_legend_file.string() + " does not exist?";
        throw std::runtime_error(errmsg.c_str());
    }

    return colour_mapper;
}


BOOST_FUSION_ADAPT_STRUCT(ColourMapperGradient,
                                  (ColourMapperGradient::Transform, transfrm)
                                  (double, red0)
                                  (double, green0)
                                  (double, blue0)
                                  (double, red1)
                                  (double, green1)
                                  (double, blue1)
                                  (double, min)
                                  (double, max)
)


boost::shared_ptr<ColourMapperGradient> parseColourMapLinearGradient(boost::filesystem::path  classified_legend_file)
{

    boost::shared_ptr<ColourMapperGradient> colours(new ColourMapperGradient);

    namespace qi = boost::spirit::qi;
    namespace ph = boost::phoenix;

    qi::rule<std::string::iterator, Skipper > values_parser =
                            ( qi::lit("transform") >> qi::lit("=") >> (qi::lit("Linear")[qi::_val = ColourMapperGradient::LINEAR] | qi::lit("Log")[qi::_val = ColourMapperGradient::LOG])
                            | qi::lit("red0") >> qi::lit("=") >> qi::double_[ph::ref(colours->red0) = qi::_1 / ph::val(255.0)]
                            | qi::lit("green0") >> qi::lit("=") >> qi::double_[ph::ref(colours->green0) = qi::_1 / ph::val(255.0)]
                            | qi::lit("blue0") >> qi::lit("=") >> qi::double_[ph::ref(colours->blue0) = qi::_1 / ph::val(255.0)]
                            | qi::lit("red1") >> qi::lit("=") >> qi::double_[ph::ref(colours->red1) = qi::_1 / ph::val(255.0)]
                            | qi::lit("green1") >> qi::lit("=") >> qi::double_[ph::ref(colours->green1) = qi::_1 / ph::val(255.0)]
                            | qi::lit("blue1") >> qi::lit("=") >> qi::double_[ph::ref(colours->blue1) = qi::_1 / ph::val(255.0)]
                            | qi::lit("min") >> qi::lit("=") >> qi::double_[ph::ref(colours->min) = qi::_1]
                            | qi::lit("max") >> qi::lit("=") >> qi::double_[ph::ref(colours->max) = qi::_1] );
    qi::rule<std::string::iterator, Skipper > parser = *(values_parser);




    //Now we read the file.
    std::ifstream fin(classified_legend_file.string().c_str(), std::ios_base::in);
    if (fin.is_open())
    {
        //Start parsing the file - String to store the contents of the file
        std::string fileContents;
        //Copy contents of file to the string, make sure to include the white space
        fin.unsetf(std::ios::skipws);
        std::copy(std::istream_iterator<char>(fin),
                  std::istream_iterator<char>(),
                  std::back_inserter(fileContents));
        // The parser grammer requires a line ending after some things. Can cause problems
        // if one of these things is on the last line of the file. So we append another
        // new line just in case.
        fileContents.append("     \n");
        //Print the read-in contents to clog.

        //Now we parse the file. WE need (rewindable) iterators at the start and end of the string.
        std::string::iterator it = fileContents.begin();
        std::string::iterator end = fileContents.end();
        Skipper skipper;
        bool r = boost::spirit::qi::phrase_parse(it, end, parser, skipper);

        if (r && it == end)
        {
//			std::cout << "-------------------------\n";
//			std::cout << "Parsing succeeded\n";
//			std::cout << "-------------------------\n";
        }
        else
        {
            std::string rest(it, end);
            std::cout << "-------------------------\n";
            std::cout << "Parsing failed\n";
            std::cout << "stopped at: \"" << rest << "\"\n";
            std::cout << "-------------------------\n";
            throw std::runtime_error("Error while parsing file. Check format of linear gradient colour mapper file");
        }
        fin.close();


    }
    else
    {
        std::string errmsg = "Error while opening file. File with name " + classified_legend_file.string() + " does not exist?";
        throw std::runtime_error(errmsg.c_str());
    }

    return colours;
}

#endif //GEON_ZONAL_OPT_COLOURMAPPERPARSERS_H