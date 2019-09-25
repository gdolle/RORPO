#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>
#include <sstream>

#include "docopt.h"
#include "Image/Image.hpp"
#include "Image/Image_IO_nifti.hpp"
#include "RORPO/RPO.hpp"

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
int RPO_usage(Image3D<PixelType> image,
                std::string outputPath,
                int L,
                int dilatSize,
                std::vector<int> window,
                int nbCores,
                int verbose)
{
    int dimz = image.Dimz();
    int dimy = image.Dimy();
    int dimx= image.Dimx();
    
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

    if (verbose){
		std::cout<<"NIFTI Image"<<std::endl;
        std::cout<<"dimx= "<<dimx<<"; dimy= "<<dimy<<"; dimz= "<<dimz<<std::endl;
	}

    // ------------------ Compute input image intensity range ------------------

    std::vector<PixelType> minmax = image.min_max_value();

    if (verbose){
        std::cout<< "Image intensity range: "<< (float)minmax[0]<<", "
                 << (float)minmax[1] << std::endl;
        std::cout<<std::endl;
	}


    // #################### Convert input image to char #######################

    if (window[2] == 1 || typeid(PixelType) == typeid(float) ||
            typeid(PixelType) == typeid(double)){

        if (window[2] == 1) { // window the intensity range to [0,255]
            if (minmax[0] > (PixelType) window[0])
                window[0] = minmax[0];

            if (minmax[1] < (PixelType) window[1])
                window[1] = minmax[1];

            image.window_dynamic(window[0], window[1]);

            if(verbose){
                std::cout<<"Convert image intensity range from: [";
                std::cout<<window[0]<<", "<<window[1]<<"] to [";
                std::cout<<"0"<<", "<<"255"<<"]"<<std::endl;
            }
        }

        else{ // convert the full intensity range to [0,255]
            image.window_dynamic(minmax[0], minmax[1]);
            if(verbose){
                std::cout<<"Convert image intensity range from: [";
                std::cout<<minmax[0]<<", "<<minmax[1]<<"] to [";
                std::cout<<"0"<<", "<<"255"<<"]"<<std::endl;
            }
        }

        minmax = image.min_max_value();
        Image3D<uint8_t>imageChar= image.copy_image_2_uchar();

        if(verbose)
            std::cout<<"Convert image to uint8"<<std::endl;
            
        Image3D<uint8_t> RPO1(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO2(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO3(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO4(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO5(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO6(dimx + 4, dimy + 4, dimz + 4);
		Image3D<uint8_t> RPO7(dimx + 4, dimy + 4, dimz + 4);

        // Run RORPO multiscale
        std::cout<<" DILAT SIZE before RPO: "<<std::endl;
        RPO<uint8_t, unsigned char>(imageChar, L, RPO1, RPO2, RPO3, RPO4, RPO5, RPO6, RPO7, dilatSize, nbCores, mask);

        // Write the result to nifti image
        write_3D_nifti_image<uint8_t>(RPO1, RPO1name);
        write_3D_nifti_image<uint8_t>(RPO2, RPO2name);
        write_3D_nifti_image<uint8_t>(RPO3, RPO3name);
        write_3D_nifti_image<uint8_t>(RPO4, RPO4name);
        write_3D_nifti_image<uint8_t>(RPO5, RPO5name);
        write_3D_nifti_image<uint8_t>(RPO6, RPO6name);
        write_3D_nifti_image<uint8_t>(RPO7, RPO7name);

    }

    // ################## Keep input image in PixelType ########################

    else {

        // ------------------------ Negative intensities -----------------------

        if (minmax[0] < 0){
            image - minmax[0];

            if(verbose){
                std::cout<<"Convert image intensity range from [";
                std::cout<<minmax[0]<<", "<<minmax[1]<<"] to [";
                std::cout<<"0"<<", "<<minmax[1] - minmax[0]<<"]"<<std::endl;
            }
            minmax = image.min_max_value();
        }
        
		// the 7 RPO images with a 2-pixel border 
		Image3D<PixelType> RPO1(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO2(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO3(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO4(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO5(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO6(dimx + 4, dimy + 4, dimz + 4, 2);
		Image3D<PixelType> RPO7(dimx + 4, dimy + 4, dimz + 4, 2);

        
        // Run RORPO multiscale
        std::cout<<" DILAT SIZE before RPO: "<<dilatSize<<std::endl;
        RPO<PixelType, unsigned char>(image, L, RPO1, RPO2, RPO3, RPO4, RPO5, RPO6, RPO7, dilatSize, nbCores, mask);
        
	
        // Write the result to nifti image
        write_3D_nifti_image<PixelType>(RPO1, RPO1name);
        write_3D_nifti_image<PixelType>(RPO2, RPO2name);
        write_3D_nifti_image<PixelType>(RPO3, RPO3name);
        write_3D_nifti_image<PixelType>(RPO4, RPO4name);
        write_3D_nifti_image<PixelType>(RPO5, RPO5name);
        write_3D_nifti_image<PixelType>(RPO6, RPO6name);
        write_3D_nifti_image<PixelType>(RPO7, RPO7name);

    }

    return 0;
} // RORPO_multiscale_usage


// Parse command line with docopt
static const char USAGE[] =
R"(RORPO_multiscale_usage.

    USAGE:
    RPO <imagePath> <outputPath> <pathLenght> <dilatSize> [--window=min,max] [--core=nbCores] [--verbose]

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
                                                  "PO");

    std::cout<<" "<<std::endl;
    std::cout<<"Parameters: "<<std::endl;
    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    std::string imagePath = args["<imagePath>"].asString();
    std::string outputPath = args["<outputPath>"].asString();
    int L = std::stoi(args["<pathLenght>"].asString());
    int dilatSize = std::stoi(args["<dilatSize>"].asString());
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
    nifti_image *nim = NULL;
    nim = nifti_image_read(imagePath.c_str(), 1);

    // ---------------- Find image type and run RORPO multiscale ---------------
    int error;
    if (verbose){
        std::cout<<" "<<std::endl;
        std::cout << "------ INPUT IMAGE -------" << std::endl;
    }

    switch(nim->datatype){
        case 2: { // uint8
            if (verbose)
                std::cout<<"Input image type: uint8"<<std::endl;

            Image3D<uint8_t> image = read_3D_nifti_image<uint8_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<uint8_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
                                                   
            break;
    }

        case 4: { // uint16
            if (verbose)
                std::cout<<"Input image type: uint16 "<<std::endl;

            Image3D<u_int16_t> image = read_3D_nifti_image<u_int16_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<u_int16_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
            break;
        }

        case 8: { // uint32
            if (verbose)
                std::cout<<"Input image type: int32 "<<std::endl;

            Image3D<int32_t> image = read_3D_nifti_image<int32_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<int32_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
            break;
        }

        case 16: { // float
            if (verbose)
                std::cout<<"Input image type: float "<<std::endl;

            Image3D<float_t> image = read_3D_nifti_image<float_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<float_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
        }
        case 256: { // int8
            if (verbose)
                std::cout<<"Input image type: int8 "<<std::endl;

            Image3D<int8_t> image = read_3D_nifti_image<int8_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<int8_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
            
            break;
        }
        case 512: { // int16
            if (verbose)
                std::cout<<"Input image type: int16 "<<std::endl;

            Image3D<int16_t> image = read_3D_nifti_image<int16_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<int16_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
            
            break;
        }
        case 768: { // uint32
            if (verbose)
                std::cout<<"Input image type: uint32 "<<std::endl;

            Image3D<uint32_t> image = read_3D_nifti_image<uint32_t>(nim);
            nifti_image_free(nim);

            error = RPO_usage<uint32_t>(image, outputPath, L, dilatSize, window, nbCores, verbose);
            
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

