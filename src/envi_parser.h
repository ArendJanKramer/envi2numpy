//
// Created by Arend Jan Kramer on 21-11-17.
// Functions readRawENVI, normalizeENVI and BILToTiled kindly borrowed from K. Dijkstra's DDSL
//

#ifndef ENVI2NUMPY_ENVI_PARSER_H
#define ENVI2NUMPY_ENVI_PARSER_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdarg>
#include <algorithm>
#include <iterator>
#include "cnpy.h"
#include <utility>

using namespace std;
typedef unsigned short UInt16;

class EnviParser {

public:
    enum EnviLayout {
        elBandSequential, elBandInterleaveByLine, elBandInterleaveByPixel
    };

    template<typename TIdx>
    vector<TIdx> makeFloatCube(string cubepath, string darkrefpath, string whiterefpath, UInt16 width, UInt16 bands,
                               UInt16 *cubeHeight, TIdx pixelGain);

    template<typename TIdx>
    size_t readRawENVI(vector<TIdx> &dst, string &filename, TIdx width, TIdx bands);

    template<typename T>
    size_t inline readToVector(const string &filename, vector<T> &buf);

    template<typename TIdx>
    void normalizeENVI(vector<TIdx> &cube, const vector<TIdx> &black, const vector<TIdx> &white, TIdx bands,
                       EnviLayout cubeLayout, TIdx cubeCols, TIdx cubeRows, TIdx whiteHeight, TIdx blackHeight);

    template<typename TIdx>
    vector<TIdx> BILToTiled(vector<TIdx> bil, UInt16 tileWidth, UInt16 tileHeight, UInt16 bandCount);

    vector<float>
    convertCaptureVectorFloat(const string &cubepath, const string &darkrefpath, const string &whiterefpath,
                              UInt16 width, UInt16 *numBands, UInt16 *cubeHeight, float pixelGain, bool normalize,
                              bool log_derive);

};


#endif //ENVI2NUMPY_ENVI_PARSER_H_H
