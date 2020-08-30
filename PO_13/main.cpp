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
int RPO_13_usage(Image3D<PixelType> image,
                std::string outputPath,
                int L,
		int dilationSize,
                std::vector<int> window,
                int nbCores,
                int verbose)
{
    using minmax_type = std::pair<PixelType,PixelType>;

    int dimx= image.dimX();
    int dimy = image.dimY();
    int dimz = image.dimZ();
    
    Image3D<unsigned char> mask;
    
	std::stringstream ss1;
	ss1 <<outputPath<< "_PO1.nii";
	std::string RPO1name = ss1.str();
	
	std::stringstream ss2;
	ss2 <<outputPath<< "_PO2.nii";
	std::string RPO2name = ss2.str();
	
	std::stringstream ss3;
	ss3 <<outputPath<< "_PO3.nii";
	std::string RPO3name = ss3.str();
	
	std::stringstream ss4;
	ss4 <<outputPath<< "_PO4.nii";
	std::string RPO4name = ss4.str();
	
	std::stringstream ss5;
	ss5 <<outputPath<< "_PO5.nii";
	std::string RPO5name = ss5.str();
	
	std::stringstream ss6;
	ss6 <<outputPath<< "_PO6.nii";
	std::string RPO6name = ss6.str();
	
	std::stringstream ss7;
	ss7 <<outputPath<< "_PO7.nii";
	std::string RPO7name = ss7.str();
	
	std::stringstream ss8;
	ss8 <<outputPath<< "_PO8.nii";
	std::string RPO8name = ss8.str();
	
	std::stringstream ss9;
	ss9 <<outputPath<< "_PO9.nii";
	std::string RPO9name = ss9.str();

	std::stringstream ss10;
	ss10 <<outputPath<< "_PO10.nii";
	std::string RPO10name = ss10.str();
	
	std::stringstream ss11;
	ss11 <<outputPath<< "_PO11.nii";
	std::string RPO11name = ss11.str();
	
	std::stringstream ss12;
	ss12 <<outputPath<< "_PO12.nii";
	std::string RPO12name = ss12.str();
	
	std::stringstream ss13;
	ss13 <<outputPath<< "_PO13.nii";
	std::string RPO13name = ss13.str();

    if (verbose){
		std::cout<<"NIFTI Image"<<std::endl;
        std::cout<<"dimx= "<<dimx<<"; dimy= "<<dimy<<"; dimz= "<<dimz<<std::endl;
	}

    // ------------------ Compute input image intensity range ------------------

    minmax_type minmax = image.min_max_value();

    if (verbose){
        std::cout<< "Image intensity range: "<< (float)minmax.first<<", "
                 << (float)minmax.second << std::endl;
        std::cout<<std::endl;
	}


    // #################### Convert input image to char #######################

    if (window[2] == 1 || typeid(PixelType) == typeid(float) ||
            typeid(PixelType) == typeid(double)){

        if (window[2] == 1) { // window the intensity range to [0,255]
            if (minmax.first > (PixelType) window[0])
                window[0] = minmax.first;

            if (minmax.second < (PixelType) window[1])
                window[1] = minmax.second;

            image.window_dynamic(window[0], window[1]);

            if(verbose){
                std::cout<<"Convert image intensity range from: [";
                std::cout<<window[0]<<", "<<window[1]<<"] to [";
                std::cout<<"0"<<", "<<"255"<<"]"<<std::endl;
            }
        }

        else{ // convert the full intensity range to [0,255]
            image.window_dynamic(minmax.first, minmax.second);
            if(verbose){
                std::cout<<"Convert image intensity range from: [";
                std::cout<<minmax.first<<", "<<minmax.second<<"] to [";
                std::cout<<"0"<<", "<<"255"<<"]"<<std::endl;
            }
        }

        minmax = image.min_max_value();
        Image3D<uint8_t>imageChar= image.copy_image_2_uchar();

        if(verbose)
            std::cout<<"Convert image to uint8"<<std::endl;
            
        Image3D<uint8_t> RPO1(dimx, dimy, dimz);
		Image3D<uint8_t> RPO2(dimx, dimy, dimz);
		Image3D<uint8_t> RPO3(dimx, dimy, dimz);
		Image3D<uint8_t> RPO4(dimx, dimy, dimz);
		Image3D<uint8_t> RPO5(dimx, dimy, dimz);
		Image3D<uint8_t> RPO6(dimx, dimy, dimz);
		Image3D<uint8_t> RPO7(dimx, dimy, dimz);
		Image3D<uint8_t> RPO8(dimx, dimy, dimz);
		Image3D<uint8_t> RPO9(dimx, dimy, dimz);
		Image3D<uint8_t> RPO10(dimx, dimy, dimz);
		Image3D<uint8_t> RPO11(dimx, dimy, dimz);
		Image3D<uint8_t> RPO12(dimx, dimy, dimz);
		Image3D<uint8_t> RPO13(dimx, dimy, dimz);

        // Run RORPO multiscale
        RPO_13<uint8_t, unsigned char>(imageChar, L, RPO1, RPO2, RPO3, RPO4,
                RPO5, RPO6, RPO7, RPO8, RPO9, RPO10, RPO11, RPO12, RPO13,
                dilationSize, nbCores, mask);

        // Write the result to nifti image
        Write_Itk_Image<uint8_t>(RPO1, RPO1name);
        Write_Itk_Image<uint8_t>(RPO2, RPO2name);
        Write_Itk_Image<uint8_t>(RPO3, RPO3name);
        Write_Itk_Image<uint8_t>(RPO4, RPO4name);
        Write_Itk_Image<uint8_t>(RPO5, RPO5name);
        Write_Itk_Image<uint8_t>(RPO6, RPO6name);
        Write_Itk_Image<uint8_t>(RPO7, RPO7name);
        Write_Itk_Image<uint8_t>(RPO8, RPO8name);
        Write_Itk_Image<uint8_t>(RPO9, RPO9name);
        Write_Itk_Image<uint8_t>(RPO10, RPO10name);
        Write_Itk_Image<uint8_t>(RPO11, RPO11name);
        Write_Itk_Image<uint8_t>(RPO12, RPO12name);
        Write_Itk_Image<uint8_t>(RPO13, RPO13name);
    }

    // ################## Keep input image in PixelType ########################

    else {

        // ------------------------ Negative intensities -----------------------

        if (minmax.first < 0){
            image - minmax.first;

            if(verbose){
                std::cout<<"Convert image intensity range from [";
                std::cout<<minmax.first<<", "<<minmax.second<<"] to [";
                std::cout<<"0"<<", "<<minmax.second - minmax.first<<"]"<<std::endl;
            }
            minmax = image.min_max_value();
        }
        
        Image3D<PixelType> RPO1(dimx, dimy, dimz);
		Image3D<PixelType> RPO2(dimx, dimy, dimz);
		Image3D<PixelType> RPO3(dimx, dimy, dimz);
		Image3D<PixelType> RPO4(dimx, dimy, dimz);
		Image3D<PixelType> RPO5(dimx, dimy, dimz);
		Image3D<PixelType> RPO6(dimx, dimy, dimz);
		Image3D<PixelType> RPO7(dimx, dimy, dimz);
		Image3D<PixelType> RPO8(dimx, dimy, dimz);
		Image3D<PixelType> RPO9(dimx, dimy, dimz);
		Image3D<PixelType> RPO10(dimx, dimy, dimz);
		Image3D<PixelType> RPO11(dimx, dimy, dimz);
		Image3D<PixelType> RPO12(dimx, dimy, dimz);
		Image3D<PixelType> RPO13(dimx, dimy, dimz);
        
        // Run RORPO multiscale
        RPO_13<PixelType, unsigned char>(image, L, RPO1, RPO2, RPO3, RPO4, RPO5, RPO6, RPO7, RPO8, RPO9, RPO10, RPO11, RPO12, RPO13, dilationSize, nbCores, mask);
        
	
        // Write the result to nifti image
        Write_Itk_Image<PixelType>(RPO1, RPO1name);
        Write_Itk_Image<PixelType>(RPO2, RPO2name);
        Write_Itk_Image<PixelType>(RPO3, RPO3name);
        Write_Itk_Image<PixelType>(RPO4, RPO4name);
        Write_Itk_Image<PixelType>(RPO5, RPO5name);
        Write_Itk_Image<PixelType>(RPO6, RPO6name);
        Write_Itk_Image<PixelType>(RPO7, RPO7name);
        Write_Itk_Image<PixelType>(RPO8, RPO8name);
        Write_Itk_Image<PixelType>(RPO9, RPO9name);
        Write_Itk_Image<PixelType>(RPO10, RPO10name);
        Write_Itk_Image<PixelType>(RPO11, RPO11name);
        Write_Itk_Image<PixelType>(RPO12, RPO12name);
        Write_Itk_Image<PixelType>(RPO13, RPO13name);
        

    }

    return 0;
} // RORPO_multiscale_usage


// Parse command line with docopt
static const char USAGE[] =
R"(RORPO_multiscale_usage.

    USAGE:
    RORPO_multiscale_usage <imagePath> <outputPath> <pathLenght> <dilationSize> [--window=min,max] [--core=nbCores] [--verbose]

    Options:
         --core=<nbCores>     Number of CPUs used for RPO computation
         --window=min,max     Convert intensity range [min, max] of the intput \
                              image to [0,255] and convert to uint8 image\
                              (strongly decrease computation time).
         --verbose            Activation of a verbose mode.
        )";


int main(int argc, char **argv)
{

    // -------------- Parse arguments and initialize parameters ----------------
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
                                                  {argv + 1, argv + argc},
                                                  true,
                                                  "PO 13 orientations");

    std::cout<<" "<<std::endl;
    std::cout<<"Parameters: "<<std::endl;
    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    std::string imagePath = args["<imagePath>"].asString();
    std::string outputPath = args["<outputPath>"].asString();
    float L = std::stoi(args["<pathLenght>"].asString());
    float dilationSize = std::stoi(args["<dilationSize>"].asString());
    std::vector<int> window(3);
    int nbCores = 1;
    bool verbose = args["--verbose"].asBool();

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

    // -------------------------- Read Nifti Image -----------------------------
    const std::string img_path = imagePath.c_str();
    Image3DMetadata img_md = Read_Itk_Metadata( img_path );

    // ---------------- Find image type and run RORPO multiscale ---------------
    int error;
    if (verbose){
        std::cout<<" "<<std::endl;
        std::cout << "------ INPUT IMAGE -------" << std::endl;
    }

    switch(img_md.pixelType){
        case 2: { // uint8
            if (verbose)
                std::cout<<"Input image type: uint8"<<std::endl;

            Image3D<uint8_t> image = Read_Itk_Image<uint8_t>(img_path);

            error = RPO_13_usage<uint8_t>(image, outputPath, L, dilationSize, window, nbCores, verbose);
                                                   
            break;
    }

        case 4: { // uint16
            if (verbose)
                std::cout<<"Input image type: uint16 "<<std::endl;

            Image3D<u_int16_t> image = Read_Itk_Image<u_int16_t>(img_path);

            error = RPO_13_usage<u_int16_t>(image, outputPath, L, dilationSize, window, nbCores, verbose);
            break;
        }

        case 8: { // uint32
            if (verbose)
                std::cout<<"Input image type: int32 "<<std::endl;

            Image3D<int32_t> image = Read_Itk_Image<int32_t>(img_path);

            error = RPO_13_usage<int32_t>(image, outputPath, L, dilationSize, window, nbCores, verbose);
            break;
        }

        case 16: { // float
            if (verbose)
                std::cout<<"Input image type: float "<<std::endl;

            Image3D<float_t> image = Read_Itk_Image<float_t>(img_path);

            error = RPO_13_usage<float_t>(image, outputPath, L, dilationSize,  window, nbCores, verbose);
        }
        case 256: { // int8
            if (verbose)
                std::cout<<"Input image type: int8 "<<std::endl;

            Image3D<int8_t> image = Read_Itk_Image<int8_t>(img_path);

            error = RPO_13_usage<int8_t>(image, outputPath, L, dilationSize,  window, nbCores, verbose);
            
            break;
        }
        case 512: { // int16
            if (verbose)
                std::cout<<"Input image type: int16 "<<std::endl;

            Image3D<int16_t> image = Read_Itk_Image<int16_t>(img_path);

            error = RPO_13_usage<int16_t>(image, outputPath, L, dilationSize, window, nbCores, verbose);
            
            break;
        }
        case 768: { // uint32
            if (verbose)
                std::cout<<"Input image type: uint32 "<<std::endl;

            Image3D<uint32_t> image = Read_Itk_Image<uint32_t>(img_path);

            error = RPO_13_usage<uint32_t>(image, outputPath, L, dilationSize,  window, nbCores, verbose);
            
            break;
        }
        default : {
            std::cerr<<"Input image type not supported "<<std::endl;
            error = 1;
            break;
        }
    } //end switch

    return error;

}//end main

