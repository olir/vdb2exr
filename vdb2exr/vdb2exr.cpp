// vdb2exr.cpp : read openvd volume texture, convert to to slices, and write as openexr.
//
// === TESTCASE ===
// Convert vdb from disney to exr and import it as exr texture into Unreal Engine.
//   Testdata: https://www.disneyanimation.com/data-sets/?drawer=/resources/clouds/
//   UE4 Import and Setup Example: https://www.youtube.com/watch?v=eOrTudg7P1E
// 
// 
// === BUILDING ===
// Primary Dependencies:
// - https://github.com/AcademySoftwareFoundation/openvdb
// - https://github.com/AcademySoftwareFoundation/openexr
// - https://github.com/syoyo/tinyexr

// Secondary Dependencies:
// - https://github.com/microsoft/vcpkg
// - https://developer.microsoft.com/de-de/windows/downloads/windows-10-sdk
// 
// === IMPLEMENTATION ===
// Tutorials this implementation is based on:
// - https://www.openvdb.org/documentation/doxygen/codeExamples.html#sModifyingGrids
// - https://www.openexr.com/documentation/ReadingAndWritingImageFiles.pdf
//
// === CONFIGURATION ===
// Setup: Windows 10, VS2019, Release/x64 Configuration:
//   C++: Multithreaded (/MT)
//   Addional Include Paths:
//       ...\openvdb\openvdb
//       ...\vcpkg\installed\x64 - windows\include
//   Addional Library Paths:
//       ...\openvdb\build\openvdb\openvdb\Release
//       ...\vcpkg\installed\x64 - windows\lib
//   Linker Input:
//       ...\openvdb\build\openvdb\openvdb\Release\openvdb.lib
//       ...\vcpkg\installed\x64 - windows\lib\Half - 2_5.lib
//       ...\vcpkg\installed\x64-windows\lib\tbb.lib
//


#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <string>


#include <cstdio>
#include <cstdlib>
#include <vector>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif

//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include "stb_image_resize.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include "tinyexr.h"


#include <openvdb/openvdb.h>
#include <openvdb/tools/ChangeBackground.h>
#include "vdb2exr.h"


int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: vdb2exr input.vdb output.exr\n");

        exit(-1);
    }

    openvdb::initialize();
    // Create a shared pointer to a newly-allocated grid of a built-in type:
    // in this case, a FloatGrid, which stores one single-precision floating point
    // value per voxel.  Other built-in grid types include BoolGrid, DoubleGrid,
    // Int32Grid and Vec3SGrid (see openvdb.h for the complete list).
    // The grid comprises a sparse tree representation of voxel data,
    // user-supplied metadata and a voxel space to world space transform,
    // which defaults to the identity transform.


    // Create a VDB file object.
    printf("Reading vdb file [ %s ] \n", argv[1]);
    openvdb::io::File file(argv[1]);
    // Open the file.  This reads the file header, but not any grids.
    ;
    if (!file.open()) {
        fprintf(stderr, "Error: Open VDB failed!\n");
        return -5;
    }

    // Loop over all grids in the file and retrieve a shared pointer
    // to the one named "LevelSetSphere".  (This can also be done
    // more simply by calling file.readGrid("LevelSetSphere").)
    openvdb::GridBase::Ptr baseGrid;
    for (openvdb::io::File::NameIterator nameIter = file.beginName();
        nameIter != file.endName(); ++nameIter)
    {
        std::cout << " reading grid " << nameIter.gridName() << "\n";

        // Read in only the grid we are interested in.
        //if (nameIter.gridName() == "density") {
            baseGrid = file.readGrid(nameIter.gridName());
        //}
        //else {
        //    std::cout << "skipping grid " << nameIter.gridName() << std::endl;
        //}
    }
    file.close();

    std::cout << "Reading completed. Converting..." << "\n";

    // From the example above, "LevelSetSphere" is known to be a FloatGrid,
    // so cast the generic grid pointer to a FloatGrid pointer.
    openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
    
    /*
    // Convert the level set sphere to a narrow-band fog volume, in which
    // interior voxels have value 1, exterior voxels have value 0, and
    // narrow-band voxels have values varying linearly from 0 to 1.
    const float outside = grid->background();
    const float gwidth = 2.0 * outside;


    // Visit and update all of the grid's active values, which correspond to
    // voxels on the narrow band.
    for (openvdb::FloatGrid::ValueOnIter iter = grid->beginValueOn(); iter; ++iter) {
        openvdb::OPENVDB_VERSION_NAME::math::Coord c = iter.getCoord();
        float dist = iter.getValue();
        iter.setValue((outside - dist) / gwidth);
        printf("d-on [ %d %d %d ] = %f -> %f\n", c.x(), c.y(), c.z(), dist, (outside - dist) / gwidth);
    }

    // Visit all of the grid's inactive tile and voxel values and update the values
    // that correspond to the interior region.
    for (openvdb::FloatGrid::ValueOffIter iter = grid->beginValueOff(); iter; ++iter) {
        openvdb::OPENVDB_VERSION_NAME::math::Coord c = iter.getCoord();
        float dist = iter.getValue();
        //printf("d-off [ %d %d %d ] = %f\n", c.x(), c.y(), c.z(), dist);
        if (dist < 0.0) {
            iter.setValue(1.0);
            iter.setValueOff();
        }
    }
    // Set exterior voxels to 0.
    openvdb::tools::changeBackground(grid->tree(), 0.0);
    */
    

    // https://artifacts.aswf.io/io/aswf/openvdb/openvdb_introduction_2015/1.0.0/openvdb_introduction_2015-1.0.0.pdf

    openvdb::OPENVDB_VERSION_NAME::math::Coord dim = grid->evalActiveVoxelDim();
    openvdb::OPENVDB_VERSION_NAME::math::Coord min = grid->evalActiveVoxelBoundingBox().min();
    int width, height;
    int dx, dy, dz;
    dx = dim.x();
    dz = dim.z();
    dy = dim.y();
    int uw = dy;
    while (uw*uw/4>dy ) {
        uw = uw / 2;
    }
    int uv = (dy + uw - 1) / uw;
    width = dim.x() * uv;
    height = dim.z() * uw;

    printf("  prepare output %d x %d x %d   [%d,%d,%d] as   %d x %d   tiling %d x %d\n", dim.x(), dim.y(), dim.z(),min.x(), min.y(), min.z(), width, height, uv, uw);

    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = 3;

    std::vector<float> images[3];
    images[0].resize(width * height);
    images[1].resize(width * height);
    images[2].resize(width * height);
    // useless? Clear image...
    for (int i = 0; i < width * height; i++) {
        images[0][i] = 0.0f;
        images[1][i] = 0.0f;
        images[2][i] = 0.0f;
    }

    openvdb::OPENVDB_VERSION_NAME::FloatTree tree = grid->tree();
    openvdb::OPENVDB_VERSION_NAME::math::Coord cursor;

    for (int y = 0; y < dy; y++) {
        cursor.setY(min.y() + y);
        int v = y % uv;
        int w = (y - v) / uv;
        for (int z = 0; z < dz; z++) {
            cursor.setZ(min.z() + z);
            for (int x = 0; x < dx; x++) {
                cursor.setX(min.x() + x);
                float value = tree.getValue(cursor);
                int i = x;
                i += z * dx*uv;
                i += v * dx;
                i += w * dx*uv*dz;
                images[0][i] = value;
                images[1][i] = value;
                images[2][i] = value;
                if (value > 0.0f) {
                    // printf("img [ %d :: %d %d %d ] = -> %f\n", i, x, y, z, value);
                }
            }
        }
    }
    // printf("assert [ %d == %d ]\n", (dim.z()-1) + (dim.x()-1) * dim.z() + (dim.y()-1) * dim.x() * dim.z() +1, width * height);

    /*
    grid->tree();
    for (int i = 0; i < width * height; i++) {
    }
    */


    std::cout << "  writing image ..." << "\n";

    float* image_ptr[3];
    image_ptr[0] = &(images[2].at(0)); // B
    image_ptr[1] = &(images[1].at(0)); // G
    image_ptr[2] = &(images[0].at(0)); // R

    image.images = (unsigned char**)image_ptr;
    image.width = width;
    image.height = height;

    header.num_channels = 3;
    header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    // Must be BGR(A) order, since most of EXR viewers expect this channel order.
    strncpy_s(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
    strncpy_s(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
    strncpy_s(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

    header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
    for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of output image to be stored in .EXR
        header.compression_type = TINYEXR_COMPRESSIONTYPE_PIZ;
    }

    const char* err;
    std::string fn = argv[2];
    
    int idx = fn.rfind('.');
    if (idx == std::string::npos) {
        // No extension found
        fn = fn + "_" + std::to_string(uv) + "x" + std::to_string(uw) + ".exr";
    }
    else {
        fn = std::string(&fn[0], &fn[fn.find_last_of(".")]) + "_" + std::to_string(uv) + "x" + std::to_string(uw) + ".exr";
    }
    
    int ret = SaveEXRImageToFile(&image, &header, fn.c_str(), &err);
    if (ret != TINYEXR_SUCCESS) {
        fprintf(stderr, "Save EXR err: %s\n", err);
        return ret;
    }
    printf("Saved exr file. [ %s ] \n", fn.c_str());

    //free(rgb);

    free(header.channels);
    free(header.pixel_types);
    free(header.requested_pixel_types);

    std::cout << "  done." << "\n";

    return 0;
}
