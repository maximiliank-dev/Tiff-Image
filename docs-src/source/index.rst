.. TiffWriterDoc documentation master file, created by
   sphinx-quickstart on Sun Mar 29 15:02:48 2026.

Tifflib Documentation
=============================

Tifflib is a C++ library for reading and writing TIFF image files.
It supports grayscale, binary (bit-level), and RGB image formats, with
PackBits compression and configurable endianness.

Overview
--------

The library consists of two main subsystems:

- **TiffWriter** — encodes an ``ImageContainer`` to a TIFF file on disk,
  writing the TIFF header, IFD metadata tags, and raw image strip data.
- **TiffReader** — parses a TIFF file from disk, reads the IFD tags and
  strip data, and reconstructs an ``ImageContainer``.

Both subsystems share a common ``ImageContainer<PT>`` data structure that
stores pixel data in row-major order, and a ``VirtualEndianHandler``
abstraction for portable byte-order handling.

Architecture
------------

The diagram below shows the class hierarchy of the writer subsystem.

See :doc:`TiffWriter-diagram` for the full class diagram.

Dependencies
------------

The project uses `vcpkg <https://vcpkg.io>`_ to manage C++ dependencies:

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Package
     - Purpose
   * - ``opencv4``
     - Used in the test suite to read back written TIFF files for validation
   * - ``gtest``
     - Google Test framework for unit and integration tests
   * - ``tiff``
     - Reference TIFF library (vcpkg dependency)

Build tools required:

- **CMake** ≥ 3.20
- **Visual Studio 2026** (generator: *Visual Studio 18 2026*)
- **vcpkg** with the ``VCPKG_ROOT`` environment variable set

Building
--------

Configure and build using the bundled CMake preset:

.. code-block:: bat

   cmake --preset default
   cmake --build --preset default

The preset configures the ``x64-windows`` vcpkg triplet and places build
output in the ``build/`` directory. vcpkg will automatically install all
declared dependencies on first configure.

Running the Tests
-----------------

The test suite lives in the ``test/`` subdirectory and is a separate CMake
project. It uses **Google Test** and **OpenCV** for validating written and
read-back TIFF files.

Configure and build the tests:

.. code-block:: bat

   cd test
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
   cmake --build build --config Release

Run all tests:

.. code-block:: bat

   cd test\build
   ctest -C Release --output-on-failure

Or run the test binary directly:

.. code-block:: bat

   test\build\Release\hello_test.exe

Test files covered:

- ``test_read.cc`` — TIFF file reading
- ``test_rgb.cpp`` — RGB image round-trip (write + read back)
- ``test_metadata.cpp`` — IFD tag parsing
- ``test_ramp_image.cpp`` — gradient/ramp image generation

.. toctree::
   :hidden:

   FeatureSupport
   TiffReader-diagram
   TiffWriter-diagram
   code-classes

.. include:: TiffWriter-diagram.rst
