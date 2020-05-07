#include "envi_parser.h"

inline bool fileExists(const std::string &name) {
    ifstream f(name.c_str());
    return f.good();
}

string getBaseName(const string &s) {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return (s.substr(i + 1, s.length() - i));
    }
    return ("");
}

template<typename TIdx>
vector<TIdx>
EnviParser::makeFloatCube(string cubepath, string darkrefpath, string whiterefpath, UInt16 width, UInt16 bands,
                          UInt16 *cubeHeight, TIdx pixelGain) {

    if (!fileExists(cubepath)) {
        printf("Can't find the cube file. Make sure you point at the correct location.\n");
        return vector<TIdx>();
    }
    if (!fileExists(darkrefpath)) {
        printf("Can't find the dark reference file. Make sure you point at the correct location.\n");
        return vector<TIdx>();
    }
    if (!fileExists(whiterefpath)) {
        printf("Can't find the white reference file. Make sure you point at the correct location.\n");
        return vector<TIdx>();
    }

    // Setup matrices
    vector<UInt16> image;
    vector<UInt16> darkref;
    vector<UInt16> whiteref;

    // Read files into 3d arrays
    size_t imageSize = readRawENVI(image, cubepath, width, bands);
    size_t darkSize = readRawENVI(darkref, darkrefpath, width, bands);
    size_t whiteSize = readRawENVI(whiteref, whiterefpath, width, bands);

    auto _cubeHeight = static_cast<UInt16>(imageSize / (bands * width));
    auto blackHeight = static_cast<UInt16>(darkSize / (bands * width));
    auto whiteHeight = static_cast<UInt16>(whiteSize / (bands * width));

    *cubeHeight = _cubeHeight;
    vector<TIdx> imaged(image.begin(), image.end());
    vector<TIdx> darkrefd(darkref.begin(), darkref.end());
    vector<TIdx> whiterefd(whiteref.begin(), whiteref.end());

    // Normalize image cube with dark ref and white ref
    normalizeENVI(imaged, darkrefd, whiterefd, (TIdx) bands, EnviParser::elBandInterleaveByLine,
                  (TIdx) width, (TIdx) _cubeHeight, (TIdx) whiteHeight, (TIdx) blackHeight);


    if (pixelGain != 1.0f)
        std::transform(imaged.begin(), imaged.end(), imaged.begin(), std::bind1st(std::multiplies<TIdx>(), pixelGain));

    // Convert matrix to a tiled version, 1 wide and 224 long
    // This way, the first width*cubeHeight pixels are band 1, etc.
    // When we convert it to numpy, this will be right automatically
    return BILToTiled(imaged, (short) width, _cubeHeight, (short) bands);
}

template<typename TIdx>
vector<TIdx> EnviParser::BILToTiled(vector<TIdx> bil, UInt16 tileWidth, UInt16 tileHeight, UInt16 bandCount) {

    if ((size_t) bil.size() % (size_t) (bandCount * tileWidth * tileHeight) != 0)
        throw std::runtime_error("Source image height is not dividable by the number of bands.");

    vector<TIdx> tiled(tileWidth * tileHeight * bandCount);

    int srcy = 0;
    for (int lineCnt = 0; lineCnt < tileHeight; lineCnt++) {
        for (int bandCnt = 0; bandCnt < bandCount; bandCnt++) {
            const int desty = (bandCnt * tileHeight) + lineCnt;

            TIdx *dstPtr = &tiled[desty * tileWidth];
            const TIdx *srcPtr = &bil[srcy * tileWidth];

            memcpy(dstPtr, srcPtr, tileWidth * sizeof(TIdx));
            srcy++;
        }
    }
    return tiled;
}

template<typename T>
size_t inline EnviParser::readToVector(const string &filename, vector<T> &buf) {
    std::ifstream ifs(filename, ios::binary);
    if (!ifs) throw std::runtime_error("Cannot open file: " + filename);
    ifs.seekg(0, std::ios::end);
    auto size = (size_t) ifs.tellg();
    ifs.seekg(0);
    buf.resize(size / sizeof(T));
    ifs.read(static_cast<char *>(static_cast<void *>(buf.data())), size);

    if (!ifs) throw std::runtime_error("Not all bytes read");
    return buf.size();
}

template<typename TIdx>
size_t EnviParser::readRawENVI(vector<TIdx> &dst, string &filename, TIdx width, TIdx bands) {
    vector<TIdx> buf;
    auto size = (size_t) readToVector(filename, buf);

    if (size % (width * bands) != 0) {
        char buff[255];
        snprintf(buff, sizeof(buff), "Cannot deduce height from file size. "
                                     "Size = %lu is not a multiple of (Width = %d x Bands = %d)\n", size, width, bands);
        printf("%s", buff);
        throw std::runtime_error(buff);
    }

    dst = buf;
    return size;
}

template<typename TIdx>
void EnviParser::normalizeENVI(vector<TIdx> &cube, const vector<TIdx> &black, const vector<TIdx> &white, TIdx bands,
                               EnviLayout cubeLayout, TIdx cubeCols, TIdx cubeRows, TIdx whiteHeight,
                               TIdx blackHeight) {
    if (cubeLayout != elBandInterleaveByLine)
        throw std::runtime_error("Specified cubeLayout not supported");

    vector<TIdx> whiteRef(bands * cubeCols * whiteHeight);
    vector<TIdx> blackRef(bands * cubeCols * blackHeight);
    //Take average for white
    for (UInt16 y = 0; y < whiteHeight; y++) {
        for (UInt16 b = 0; b < bands; b++) {
            TIdx *dstPtr = &whiteRef[b * cubeCols];
            const TIdx *srcPtr = &white.at((y * bands + b) * cubeCols);
            for (TIdx px = 0; px < cubeCols; px++, dstPtr++, srcPtr++)
                *dstPtr += *srcPtr;
        }
    }
    //Take average for black
    for (UInt16 y = 0; y < blackHeight; y++) {
        for (UInt16 b = 0; b < bands; b++) {
            TIdx *dstPtr = &blackRef[b * cubeCols];
            const TIdx *srcPtr = &black[(y * bands + b) * cubeCols];
            for (TIdx px = 0; px < cubeCols; px++, dstPtr++, srcPtr++)
                *dstPtr += *srcPtr;
        }
    }
    //Average
    for (UInt16 b = 0; b < bands; b++) {
        TIdx *ptrWhite = &whiteRef[b * cubeCols];
        TIdx *ptrBlack = &blackRef[b * cubeCols];
        for (UInt16 px = 0; px < cubeCols; px++, ptrWhite++, ptrBlack++) {
            *ptrWhite /= whiteHeight;
            *ptrBlack /= blackHeight;
        }
    }
    //Normalise
    for (UInt16 y = 0; y < cubeRows; y++) {
        for (UInt16 b = 0; b < bands; b++) {
            const TIdx *ptrWhite = &whiteRef[b * cubeCols];
            const TIdx *ptrBlack = &blackRef[b * cubeCols];
            TIdx *ptrDst = &cube[(y * bands + b) * cubeCols];
            for (TIdx x = 0; x < cubeCols; x++, ptrWhite++, ptrBlack++, ptrDst++)
                *ptrDst = (*ptrDst - *ptrBlack) / (*ptrWhite - *ptrBlack);
        }
    }
}

vector<float>
EnviParser::convertCaptureVectorFloat(const string &cubepath, const string &darkrefpath, const string &whiterefpath,
                                      UInt16 width, UInt16 *numBands, UInt16 *cubeHeight, float pixelGain,
                                      bool normalize, bool log_derive) {
    // Everything is templated. Change this to easily switch output type
    typedef float outputType;

    if (normalize || log_derive) {
        pixelGain = 1.0;
    }

    auto cube = makeFloatCube<outputType>(cubepath, darkrefpath, whiterefpath, width, *numBands, cubeHeight, pixelGain);

    if (normalize) {
        float min = *min_element(cube.begin(), cube.end());
        float max = *max_element(cube.begin(), cube.end());
        std::transform(cube.begin(), cube.end(), cube.begin(), bind2nd(std::minus<float>(), min));
        std::transform(cube.begin(), cube.end(), cube.begin(), bind2nd(std::divides<float>(), max));
    }

    if (log_derive) {
        // Check for at least two bands
        if (cube.size() >= (size_t) (width * (*cubeHeight) * 2lu)) {
            vector<outputType> normalized(static_cast<unsigned long>(width * (*cubeHeight) * (*numBands - 1)));

            for (UInt16 b = 0; b < (*numBands - 1); b++) {
                unsigned int size = width * (*cubeHeight);
                unsigned int offSet = b * size;
                vector<outputType> bandA(cube.begin() + offSet, cube.begin() + offSet + size);
                vector<outputType> bandB(cube.begin() + offSet + size, cube.begin() + offSet + 2 * size);
                std::transform(bandB.begin(), bandB.end(), bandA.begin(), normalized.begin() + offSet,
                               std::divides<outputType>());
            }

            std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                           bind2nd(std::minus<outputType>(), 1.0));
            *numBands = *numBands - 1;
            //std::transform(normalized.begin(), normalized.end(), normalized.begin(),[](outputType x){return x-1.0; });
            return normalized;
        }
    }


    return cube;
}



