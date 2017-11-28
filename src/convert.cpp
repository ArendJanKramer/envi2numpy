////////////////////////////////////////////////////////////////////////////////
// convert.cpp
// Arend Jan Kramer
//
// Read RAW file and export to png
//
// Compile with:
// g++ convert.cpp -o convert
////////////////////////////////////////////////////////////////////////////////

#include "envi_parser.cpp"
#include "cnpy.h"

using namespace std;

int main(int argc, char **argv) {
    // Input data
    string base = "/home/arendjan/Desktop/DDSL_kopie/envi_numpy/NHL_A,_B,_C,_D,_E,_F,_G_witte_achtergrond_2017-05-31_07-33-28";
    string cubepath = base + "/capture/NHL_A,_B,_C,_D,_E,_F,_G_witte_achtergrond_2017-05-31_07-33-28.raw";
    string whiterefpath = base + "/capture/WHITEREF_NHL_A,_B,_C,_D,_E,_F,_G_witte_achtergrond_2017-05-31_07-33-28.raw";
    string darkrefpath = base + "/capture/DARKREF_NHL_A,_B,_C,_D,_E,_F,_G_witte_achtergrond_2017-05-31_07-33-28.raw";

    envi_parser enviParser;

    UInt16 numbands = 224;
    UInt16 width = 640;

    float pixelGain = 80.0;

    // Setup matrices
    vector<UInt16> image;
    vector<UInt16> darkref;
    vector<UInt16> whiteref;

    // Read files into 3d arrays
    printf("Read image:\n");
    size_t imageSize = enviParser.readRawENVI(image, cubepath, width, numbands);
    printf("Read darkref:\n");
    size_t darkSize = enviParser.readRawENVI(darkref, darkrefpath, width, numbands);
    printf("Read whiteref:\n");
    size_t whiteSize = enviParser.readRawENVI(whiteref, whiterefpath, width, numbands);

    auto cubeHeight = static_cast<UInt16>(imageSize / (numbands * width));
    auto blackHeight = static_cast<UInt16>(darkSize / (numbands * width));
    auto whiteHeight = static_cast<UInt16>(whiteSize / (numbands * width));

    vector<float> imaged(image.begin(), image.end());
    vector<float> darkrefd(darkref.begin(), darkref.end());
    vector<float> whiterefd(whiteref.begin(), whiteref.end());

    // Normalize image cube with dark ref and white ref
    enviParser.normalizeENVI(imaged, darkrefd, whiterefd, (float) numbands, envi_parser::elBandInterleaveByLine,
                            (float) width, (float) cubeHeight, (float) whiteHeight, (float) blackHeight);


    std::transform(imaged.begin(), imaged.end(), imaged.begin(), std::bind1st(std::multiplies<float>(), pixelGain));

    // Convert matrix to a tiled version, 1 wide and 224 long
    // This way, the first width*cubeHeight pixels are band 1, etc.
    // When we convert it to numpy, this will be right automatically
    vector<float> tiled = enviParser.BILToTiled(imaged, (short) width, (short) cubeHeight, (short) numbands);

    printf(" -> Writing numpy array...\n\n");

    cnpy::npy_save(base + "/cube-new.npy", &tiled[0], {numbands, cubeHeight, width}, "w");

}


