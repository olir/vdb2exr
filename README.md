# Volume Texture Converter. Converts OpenVDB to OpenEXR as sliced texture tiles ready to be imported in Unreal Engine.

![Preview](https://github.com/olir/vdb2exr/blob/master/Testdata/Preview-sixteenth-eighth-quarter.JPG?raw=true)

Images of rendered clouds in Unreal Engine 4.26 at different grade of detail.

The tool reads a vdb file and maps voxel data to image slice:

<img src="https://github.com/olir/vdb2exr/blob/master/Testdata/Preview-Slices.JPG?raw=true" alt="Preview" style="zoom:25%;" />

Each slice is a image where density is mapped to grayscale values:

<img src="https://github.com/olir/vdb2exr/blob/master/Testdata/Preview-One-Slice.JPG?raw=true" alt="Preview" style="zoom: 25%;" />

## === TESTCASE ===

Convert vdb from disney to exr and import it as exr texture into Unreal Engine.
  Testdata: Walt Disney Animation Studios Samples from Cloud Data Set
Source: https://www.disneyanimation.com/data-sets/?drawer=/resources/clouds/
  UE4 Import and Setup Example: https://www.youtube.com/watch?v=eOrTudg7P1E

## === BUILDING / Content ===

Primary Dependencies:

- https://github.com/AcademySoftwareFoundation/openvdb
- https://github.com/syoyo/tinyexr
- https://github.com/AcademySoftwareFoundation/openexr

Secondary Dependencies:
- https://github.com/microsoft/vcpkg
- https://developer.microsoft.com/de-de/windows/downloads/windows-10-sdk

## === IMPLEMENTATION ===

Tutorials this implementation is based on:

- https://www.openvdb.org/documentation/doxygen/codeExamples.html#sModifyingGrids

- https://www.openexr.com/documentation/ReadingAndWritingImageFiles.pdf

  ## === CONFIGURATION ===

  Setup: Windows 10, VS2019, Release/x64 Configuration:
  C++: Multithreaded (/MT)
  Addional Include Paths:
      ...\openvdb\openvdb
      ...\vcpkg\installed\x64 - windows\include
  Addional Library Paths:
      ...\openvdb\build\openvdb\openvdb\Release
      ...\vcpkg\installed\x64 - windows\lib
  Linker Input:
      ...\openvdb\build\openvdb\openvdb\Release\openvdb.lib
      ...\vcpkg\installed\x64 - windows\lib\Half - 2_5.lib
      ...\vcpkg\installed\x64-windows\lib\tbb.lib
  

## LICENSING:

The contents of this package are Copyright 2021 Oliver Rode and are licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. A copy of this license is available at http://creativecommons.org/licenses/by-sa/3.0/. 

Binary Content in `Testdata` and `3RDPARTY` folder are redistributed under it's own conditions as described in license and readme files contained in these folders.

`tinyexr` is under 3-clause BSD. `tinyexr` uses miniz, which is developed by Rich Geldreich [richgel99@gmail.com](mailto:richgel99@gmail.com), and licensed under public domain. `tinyexr` tools uses stb, which is licensed under public domain: https://github.com/nothings/stb `tinyexr` uses some code from OpenEXR, which is licensed under 3-clause BSD license.