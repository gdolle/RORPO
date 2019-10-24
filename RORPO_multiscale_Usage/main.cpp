#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <sstream>

#include "docopt.h"
#include "Image/Image.hpp"
#include "Image/Image_IO_ITK.hpp"
#include "RORPO/RORPO_multiscale.hpp"

typedef uint16_t u_int16_t;


// Split a string
std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> internal;
  std::stringstream ss(str); // Turn the string into a stream.
  std::string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }
  return internal;
}


template<typename PixelType>
int RORPO_multiscale_usage(Image3D<PixelType> image,
                std::string outputPath,
                std::vector<int>& scaleList,
                int dilatSize,
                std::vector<int>& window,
                int nbCores,
                int verbose,
                std::string maskPath,
                int limitOri)
{
    unsigned int dimz = image.dimZ();
    unsigned int dimy = image.dimY();
    unsigned int dimx= image.dimX();
	
    if (verbose){
        std::cout << "dimensions: [" << dimx << ", " << dimy << ", " << dimz << std::endl;
    }

    // ------------------ Compute input image intensity range ------------------

    std::pair<PixelType,PixelType> minmax = image.min_max_value();

    if (verbose){
        std::cout<< "Image intensity range: "
                 << minmax.first << ", "
                 << minmax.second << std::endl
                 << std::endl;
	}

    // -------------------------- mask Image -----------------------------------
		
    Image3D<uint8_t> mask;

    if (!maskPath.empty()) // A mask image is given
	{
        mask = Read_Itk_Image<uint8_t>(maskPath);

        if (mask.dimX() != dimx || mask.dimY() != dimy || mask.dimZ() != dimz){
            std::cerr<<"Size of the mask image (dimx= "<<mask.dimX()
                    <<" dimy= "<<mask.dimY()<<" dimz="<<mask.dimZ()
                   << ") is different from size of the input image"<<std::endl;
            return 1;
        }
    }

    // #################### Convert input image to char #######################

    if (window[2] == 1 || typeid(PixelType) == typeid(float) ||
            typeid(PixelType) == typeid(double))
    {
        if (minmax.first > (PixelType) window[0])
            window[0] = minmax.first;

        if (minmax.second < (PixelType) window[1])
            window[1] = minmax.second;

        if(verbose){
            std::cout<<"Convert image intensity range from: [";
            std::cout<<minmax.first<<", "<<minmax.second<<"] to [";
            std::cout<<window[0]<<", "<<window[1]<<"]"<<std::endl;
        }

        image.window_dynamic(window[0], window[1]);

        if(verbose)
            std::cout << "Convert image to uint8" << std::endl;

        minmax.first = 0;
        minmax.second = 255;
        Image3D<uint8_t> imageChar = image.copy_image_2_uchar();

        // Run RORPO multiscale
        Image3D<uint8_t> multiscale=
                RORPO_multiscale<uint8_t, uint8_t>(imageChar,
                                                   scaleList,
                                                   dilatSize,
                                                   nbCores,
                                                   verbose,
                                                   mask,
                                                   limitOri);

        // Write the result to nifti image
        Write_Itk_Image<uint8_t>( multiscale, outputPath );
    }

    // ################## Keep input image in PixelType ########################

    else {

        // ------------------------ Negative intensities -----------------------

        if (minmax.first < 0)
        {
            image -= minmax.first;

            if(verbose){
                std::cout << "Convert image intensity range from [";
                std::cout << minmax.first << ", " << minmax.second << "] to [";
                std::cout << "0" << ", " << minmax.second - minmax.first << "]"
                            << std::endl;
            }
        }

        // Run RORPO multiscale
        Image3D<PixelType> multiscale =
                RORPO_multiscale<PixelType, uint8_t>(image,
                                                              scaleList,
                                                              dilatSize,
                                                              nbCores,
                                                              verbose,
                                                              mask,
                                                              limitOri);

        // Write the result to nifti image
        Write_Itk_Image<PixelType>(multiscale, outputPath);
    }

    return 0;
} // RORPO_multiscale_usage


// Parse command line with docopt
static const char USAGE[] =
R"(RORPO_multiscale_usage.

    USAGE:
    RORPO_multiscale_usage <imagePath> <outputPath> <scaleMin> <factor> <nbScales>  <dilatSize> [--window=min,max] [--core=nbCores] [--mask=maskPath] [--verbose] [--limit=limitOri]

    Options:
	 --limit=<limitOri>   Limit case treatment 0 for none, 2 for 5-ori only and 1 for both 4 and 5-ori.
         --core=<nbCores>     Number of CPUs used for RPO computation
         --window=min,max     Convert intensity range [min, max] of the intput \
                              image to [0,255] and convert to uint8 image\
                              (strongly decrease computation time).
         --mask=maskPath      Path to a mask for the input image \
                              (0 for the background; not 0 for the foreground).\
                              mask image type must be uint8.
         --verbose            Activation of a verbose mode.
         --dicom            Specify that <imagePath> is a DICOM image.
        )";


int main(int argc, char **argv)
{

    // -------------- Parse arguments and initialize parameters ----------------
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                  {argv + 1, argv + argc},
                                                  true,
                                                  "RORPO_multiscale_usage 2.0");

    std::cout<<" "<<std::endl;
    std::cout<<"Parameters: "<<std::endl;
    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    std::string imagePath = args["<imagePath>"].asString();
    std::string outputPath = args["<outputPath>"].asString();
    float scaleMin = std::stoi(args["<scaleMin>"].asString());
    float factor = std::stof(args["<factor>"].asString());
    int nbScales = std::stoi(args["<nbScales>"].asString());
    int dilatSize = std::stoi(args["<dilatSize>"].asString());
    std::vector<int> window(3);
    int nbCores = 1;
    int limitOri = 0;
    std::string maskPath;
    bool verbose = args["--verbose"].asBool();
    bool dicom = args.count("--dicom");

    if (args["--limit"])
        limitOri = std::stoi(args["--limit"].asString());
        
    if (args["--mask"])
        maskPath = args["--mask"].asString();

    if (args["--core"])
        nbCores = std::stoi(args["--core"].asString());

    if (args["--window"]){
        std::vector<std::string> windowVector =
                split(args["--window"].asString(),',');

        window[0] = std::stoi(windowVector[0]);
        window[1] = std::stoi(windowVector[1]);
        window[2] = 1; // --window used
    }
    else
        window[2] = 0; // --window not used


    // -------------------------- Scales computation ---------------------------

    std::vector<int> scaleList(nbScales);
    scaleList[0] = scaleMin;

    for (int i = 1; i < nbScales; ++i)
        scaleList[i] = int(scaleMin * pow(factor, i));

    if (verbose){
        std::cout<<"Scales : ";
        std::cout<<scaleList[0];
        for (int i = 1; i < nbScales; ++i)
            std::cout<<','<<scaleList[i];
    }

    // -------------------------- Read ITK Image -----------------------------
    Image3DMetadata imageMetadata = Read_Itk_Metadata(imagePath);

    // ---------------- Find image type and run RORPO multiscale ---------------
    int error;
    if (verbose){
        std::cout<<" "<<std::endl;
        std::cout << "------ INPUT IMAGE -------" << std::endl;
        std::cout << "Input image type: " << imageMetadata.pixelTypeString << std::endl;
    }

    if ( imageMetadata.nbDimensions != 3 ) {
        std::cout << "Error: input image dimension is " << imageMetadata.nbDimensions << " but should be 3 " << std::endl;
        return 1;
    }

    switch (imageMetadata.pixelType){
        case itk::ImageIOBase::UCHAR:
        {
            Image3D<unsigned char> image = dicom?Read_Itk_Image_Series<unsigned char>(imagePath):Read_Itk_Image<unsigned char>(imagePath);
            error = RORPO_multiscale_usage<unsigned char>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::CHAR:
        {
            Image3D<char> image = dicom?Read_Itk_Image_Series<char>(imagePath):Read_Itk_Image<char>(imagePath);
            error = RORPO_multiscale_usage<char>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::USHORT:
        {
            Image3D<unsigned short> image = dicom?Read_Itk_Image_Series<unsigned short>(imagePath):Read_Itk_Image<unsigned short>(imagePath);
            error = RORPO_multiscale_usage<unsigned short>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::SHORT:
        {
            Image3D<short> image = dicom?Read_Itk_Image_Series<short>(imagePath):Read_Itk_Image<short>(imagePath);
            error = RORPO_multiscale_usage<short>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::UINT:
        {
            Image3D<unsigned int> image = dicom?Read_Itk_Image_Series<unsigned int>(imagePath):Read_Itk_Image<unsigned int>(imagePath);
            error = RORPO_multiscale_usage<unsigned int>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::INT:
        {
            Image3D<int> image = dicom?Read_Itk_Image_Series<int>(imagePath):Read_Itk_Image<int>(imagePath);
            error = RORPO_multiscale_usage<int>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
#if ITK4
        case itk::ImageIOBase::ULONG:
        {
            Image3D<unsigned long> image = dicom?Read_Itk_Image_Series<unsigned long>(imagePath):Read_Itk_Image<unsigned long>(imagePath);
            error = RORPO_multiscale_usage<unsigned long>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
#endif
        case itk::ImageIOBase::LONG:
        {
            Image3D<long> image = dicom?Read_Itk_Image_Series<long>(imagePath):Read_Itk_Image<long>(imagePath);
            error = RORPO_multiscale_usage<long>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
#if ITK4
        case itk::ImageIOBase::ULONGLONG:
        {
            Image3D<unsigned long long> image = dicom?Read_Itk_Image_Series<unsigned long long>(imagePath):Read_Itk_Image<unsigned long long>(imagePath);
            error = RORPO_multiscale_usage<unsigned long long>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
        case itk::ImageIOBase::LONGLONG:
        {
            Image3D<long long> image = dicom?Read_Itk_Image_Series<long long>(imagePath):Read_Itk_Image<long long>(imagePath);
            error = RORPO_multiscale_usage<long long>(image,
                                                    outputPath,
                                                    scaleList,
                                                    dilatSize,
                                                    window,
                                                    nbCores,
                                                    verbose,
                                                    maskPath,
                                                    limitOri);
            break;
        }
#endif
        case itk::ImageIOBase::FLOAT:
        {
            Image3D<float> image = dicom?Read_Itk_Image_Series<float>(imagePath):Read_Itk_Image<float>(imagePath);
            error = RORPO_multiscale_usage<float>(image,
                                                  outputPath,
                                                  scaleList,
                                                  dilatSize,
                                                  window,
                                                  nbCores,
                                                  verbose,
                                                  maskPath,
                                                  limitOri);
            break;
        }
        case itk::ImageIOBase::DOUBLE:
        {
            Image3D<double> image = dicom?Read_Itk_Image_Series<double>(imagePath):Read_Itk_Image<double>(imagePath);
            error = RORPO_multiscale_usage<double>(image,
                                                   outputPath,
                                                   scaleList,
                                                   dilatSize,
                                                   window,
                                                   nbCores,
                                                   verbose,
                                                   maskPath,
                                                   limitOri);
            break;
        }
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
        default:
            error = 1;
            std::cout << "Error: pixel type unknown." << std::endl;
            break;
    }
    return error;

}//end main
