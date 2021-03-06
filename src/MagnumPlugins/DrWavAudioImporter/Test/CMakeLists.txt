#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

if(CORRADE_TARGET_EMSCRIPTEN OR CORRADE_TARGET_ANDROID)
    set(DRWAVAUDIOIMPORTER_TEST_DIR ".")
else()
    set(DRWAVAUDIOIMPORTER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

corrade_add_test(DrWavAudioImporterTest DrWavImporterTest.cpp
    LIBRARIES MagnumDrWavAudioImporterTestLib
    FILES
        unsupportedChannelCount.wav
        unsupportedFormat.wav
        wrongSignature.wav

        invalidPadding.wav
        invalidLength.wav
        invalidDataChunk.wav
        invalidFactChunk.wav

        mono4.wav
        mono8junk.wav
        mono8ALaw.wav
        mono8MuLaw.wav
        mono8.wav
        mono16.wav
        mono24.wav

        stereo4.wav
        stereo8.wav
        stereo8ALaw.wav
        stereo8MuLaw.wav
        stereo12.wav
        stereo16.wav
        stereo24.wav
        stereo32.wav

        mono32f.wav
        stereo32f.wav
        stereo64f.wav

        surround51Channel16.wav
        surround71Channel24.wav

        extension8.wav
        extensionALaw.wav
        extensionMuLaw.wav
        extension12.wav
        extension16.wav
        extension24.wav
        extension32.wav

        extension32f.wav
        extension64f.wav)
target_include_directories(DrWavAudioImporterTest PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# On Win32 we need to avoid dllimporting DrWavAudioImporter symbols, because it
# would search for the symbols in some DLL even though they were linked
# statically. However it apparently doesn't matter that they were dllexported
# when building the static library. EH.
if(WIN32)
    target_compile_definitions(DrWavAudioImporterTest PRIVATE "MAGNUM_DRWAVAUDIOIMPORTER_BUILD_STATIC")
endif()
set_target_properties(DrWavAudioImporterTest PROPERTIES FOLDER "MagnumPlugins/DrWavAudioImporter/Test")
