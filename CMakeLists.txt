cmake_minimum_required(VERSION 3.5)
project(Raster2Png)

set(CMAKE_CXX_STANDARD 11)

find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(GDAL)

set(AHHZ_RASTER_UTIL "Not Specified" CACHE PATH "Path to Alex Hagen-Zanker's Raster Utility Include direction (select above blink)")
set(AHHZ_RASTER_ITERATOR "Not Specified" CACHE PATH "Path to Alex Hagen-Zanker's Raster Iterator Include direction (select folder above blink)")

#find_package(PNG)
#find_package(GIF)
find_package(ImageMagick COMPONENTS Magick++ MagickWand MagickCore)
#if(ImageMagick_Magick++_FOUND)
#    # Find Imagemagick Library directory
#    get_filename_component(MAGICK_LIB_DIR ${ImageMagick_Magick++_LIBRARY} DIRECTORY)
#    # Find where Magick++-config lives
#    file(GLOB_RECURSE MAGICK_CONFIG FOLLOW_SYMLINKS ${MAGICK_LIB_DIR}/Magick++-config)
#    # Ask about CXX and lib flags/locations
#    set(MAGICK_CONFIG ${MAGICK_CONFIG} CACHE string "Path to Magick++-config utility")
#    execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
#    execute_process(COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
#    # Add these to cache
#    set(MAGICK_CXX_FLAGS "${MAGICK_CXX_FLAGS}" CACHE string "ImageMagick configuration specific compilation flags." )
#    set(MAGICK_LD_FLAGS  "${MAGICK_LD_FLAGS}" CACHE string "ImageMagick configuration specific linking flags.")
#    # Split into list:
#    string(REGEX MATCHALL "([^\ ]+)" MAGICK_CXX_FLAGS "${MAGICK_CXX_FLAGS}")
#    string(REGEX MATCHALL "([^\ ]+)" MAGICK_LD_FLAGS "${MAGICK_LD_FLAGS}")
#    # Remove trailing whitespace (CMAKE warns about this)
#    string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
#    string(STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)
#endif(ImageMagick_Magick++_FOUND)

# Find where Magick++-config lives
find_program(MAGICK_CONFIG "Magick++-config")
# Ask about CXX and lib flags/locations
execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
execute_process(COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
message(STATUS "${MAGICK_CXX_FLAGS}")
message(STATUS "${MAGICK_LD_FLAGS}")
# Remove trailing whitespace (CMAKE warns about this)
string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
string(STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)


#include_directories(${PNG_INCLUDE_DIRS} )
include_directories(${GDAL_INCLUDE_DIR} )
include_directories(${Boost_INCLUDE_DIRS} )
include_directories(${AHHZ_RASTER_UTIL} )
include_directories(${AHHZ_RASTER_ITERATOR} )
#include_directories(${GIF_INCLUDE_DIR})
include_directories(${ImageMagick_Magick++_INCLUDE_DIR})

#add_definitions(${PNG_DEFINITIONS})

## add path to FindPNGwriter.cmake, e.g. in the directory in cmake/
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
#find_package(PNGwriter)
#if(PNGwriter_FOUND)
#    # where to find the pngwriter.h header file (-I include for your compiler)
#    include_directories(${PNGwriter_INCLUDE_DIRS})
#    # additional compiler flags (e.g. -DNO_FREETYPE)
#    add_definitions(${PNGwriter_DEFINITIONS})
#    # libraries to link against (including dependencies)
#    #  set(LIBS ${LIBS} ${PNGwriter_LIBRARIES})
#endif(PNGwriter_FOUND)

# find PNGwriter installation
#   optional: prefer static libraries over shared ones (but do not force them)
#set(PNGwriter_USE_STATIC_LIBS ON)

#   optional: specifiy (minimal) version / require to find it
#           (PNGwriter 0.5.6 REQUIRED)

file(GLOB_RECURSE AHHZ_Utils
        "${AHHZ_RASTER_UTIL}/*.h"
        "${AHHZ_RASTER_UTIL}/*.hpp"
        "${AHHZ_RASTER_UTIL}/*.cpp"
        "${AHHZ_RASTER_ITERATOR}/*.h"
        "${AHHZ_RASTER_ITERATOR}/*.hpp"
        "${AHHZ_RASTER_ITERATOR}/*.cpp"
        )

#        set(SOURCE_FILES main.cpp)
#add_executable(Raster2Png ${SOURCE_FILES} ${AHHZ_Utils})
#target_link_libraries(Raster2Png ${PNGwriter_LIBRARIES} ${GDAL_LIBRARY} ${Boost_LIBRARIES})

#set(GIFWriter_SRC
#        AnimatedGifSaver.cpp
#        )
#
#set(GIFWriter_HDRS
#        AnimatedGifSaver.h
#        )

set(PLOTTER_CLASSES_SRC
        MagickWriterClassified.cpp
        MagickWriterGradient.cpp
        GifWriterLinearGradient.cpp
        GifWriterClassified.cpp
        )

set(PLOTTER_CLASSES_HDRS
        MagickWriterClassified.h
        MagickWriterGradient.h
        GifWriterLinearGradient.h
        GifWriterClassified.h
        )

SET(MagickLibs
        ${ImageMagick_Magick++_LIBRARY}
        ${ImageMagick_MagickWand_LIBRARY}
        ${ImageMagick_MagickCore_LIBRARY}
        )

add_executable(Raster2PngLinearGradient Raster2ImageLinearGradient.cpp ${PLOTTER_CLASSES_SRC} ${AHHZ_Utils})
target_link_libraries(Raster2PngLinearGradient  ${Boost_LIBRARIES} ${GDAL_LIBRARY} ${MagickLibs})
#target_compile_options(Raster2PngLinearGradient PUBLIC ${MAGICK_CXX_FLAGS})
#target_link_libraries(Raster2PngLinearGradient ${MAGICK_LD_FLAGS})
target_compile_definitions(Raster2PngLinearGradient PUBLIC "-DMAGICKCORE_QUANTUM_DEPTH=16" PUBLIC "-DMAGICKCORE_HDRI_ENABLE=0")


add_executable(Raster2PngClassified Raster2ImageClassified.cpp ${PLOTTER_CLASSES_SRC} ${AHHZ_Utils})
target_link_libraries(Raster2PngClassified ${Boost_LIBRARIES} ${GDAL_LIBRARY} ${MagickLibs} )
target_compile_definitions(Raster2PngClassified PUBLIC "-DMAGICKCORE_QUANTUM_DEPTH=16" PUBLIC "-DMAGICKCORE_HDRI_ENABLE=0")
#target_compile_options(Raster2PngClassified PUBLIC ${MAGICK_CXX_FLAGS})
#target_link_libraries(Raster2PngClassified ${MAGICK_LD_FLAGS})

add_executable(Raster2GifLinearGradient Raster2GifLinearGradient.cpp ${PLOTTER_CLASSES_SRC} ${AHHZ_Utils} )
target_link_libraries(Raster2GifLinearGradient ${GIF_LIBRARIES} ${Boost_LIBRARIES} ${GDAL_LIBRARY}  ${MagickLibs})
target_compile_definitions(Raster2GifLinearGradient PUBLIC "-DMAGICKCORE_QUANTUM_DEPTH=16" PUBLIC "-DMAGICKCORE_HDRI_ENABLE=0")
#target_compile_options(Raster2GifLinearGradient PUBLIC ${MAGICK_CXX_FLAGS})
#target_link_libraries(Raster2GifLinearGradient ${MAGICK_LD_FLAGS})

add_executable(Raster2GifClassified Raster2GifClassified.cpp ${PLOTTER_CLASSES_SRC} ${AHHZ_Utils} )
target_link_libraries(Raster2GifClassified ${GIF_LIBRARIES} ${Boost_LIBRARIES} ${GDAL_LIBRARY}  ${MagickLibs})
target_compile_definitions(Raster2GifClassified PUBLIC "-DMAGICKCORE_QUANTUM_DEPTH=16" PUBLIC "-DMAGICKCORE_HDRI_ENABLE=0")
#target_compile_options(Raster2GifClassified PUBLIC ${MAGICK_CXX_FLAGS})
#target_link_libraries(Raster2GifClassified ${MAGICK_LD_FLAGS})

#add_executable(DirectoryRaster2PngLinearGradient DirectoryRaster2PngLinearGradient.cpp ${PLOTTER_CLASSES_SRC} ${AHHZ_Utils})
#target_link_libraries(DirectoryRaster2PngLinearGradient ${PNG_LIBRARIES} ${PNGwriter_LIBRARIES} ${Boost_LIBRARIES} ${GDAL_LIBRARY})


#add_executable(Raster2Png ${SOURCE_FILES})
#target_link_libraries(Raster2Png ${PNG_LIBRARIES} ${PNGwriter_LIBRARIES} ${Boost_LIBRARIES} )