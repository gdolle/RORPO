#ifndef RORPO_INCLUDED
#define RORPO_INCLUDED

/* Copyright (C) 2014 Odyssee Merveille
 
This file is part of libRORPO

    libRORPO is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libRORPO is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libRORPO.  If not, see <http://www.gnu.org/licenses/>.
   
*/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <omp.h>
#include <cstdlib>

#include "RORPO/sorting.hpp"
#include "RORPO/Algo.hpp"
#include "RORPO/Geodilation.hpp"
#include "RORPO/RPO.hpp"


template<typename T, typename MaskType>
Image3D<T> RORPO(const Image3D<T> &image, int L, int dilatSize, int nbCores,
                 Image3D<MaskType> &mask, int limitOri)
{

    // ############################# RPO  ######################################
	
	// the 7 RPO images with a 2-pixel border 
    Image3D<T> RPO1(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO2(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO3(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO4(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO5(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO6(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
    Image3D<T> RPO7(image.Dimx() + 4, image.Dimy() + 4, image.Dimz() + 4, 2);
	
    std::cout<<"DILAT SIZE before RPO "<<dilatSize<<std::endl;
    RPO(image, L, RPO1, RPO2, RPO3, RPO4, RPO5,RPO6, RPO7, dilatSize, nbCores, mask);
	
	// ############### Sorting RPO orientations ################################
	
    Image3D<T> RPOt1 = RPO1.copy_image();
    Image3D<T> RPOt2 = RPO2.copy_image();
    Image3D<T> RPOt3 = RPO3.copy_image();
    Image3D<T> RPOt4 = RPO4.copy_image();
    Image3D<T> RPOt5 = RPO5.copy_image();
    Image3D<T> RPOt6 = RPO6.copy_image();
    Image3D<T> RPOt7 = RPO7.copy_image();
	
    sorting(RPOt1, RPOt2, RPOt3, RPOt4, RPOt5, RPOt6, RPOt7, RPOt1.image_size());
	
	 // Clear Images which are non useful anymore
    RPOt1.clear_image();
    RPOt5.clear_image();
    RPOt6.clear_image();

	// Compute RORPO without limit orientations
    Image3D<T> RORPO_res = diff(RPOt7, RPOt4);
    RPOt7.clear_image();
    
    
     // ################### Limit Orientations Treatment #######################

     // ------------------------- Computation of Imin --------------------------
	 
	// ---- Imin limit case 4 orientations ----
	 
	if (limitOri!= 0)
	{
		Image3D<T> Imin4(image.Dimx(), image.Dimy(), image.Dimz());
		  
		//horizontal + vertical + diag1 + diag4 
		Image3D<T> Imin4_1 = RPO1.copy_image();
		min_crush(Imin4_1, RPO2);
		min_crush(Imin4_1, RPO4);
		min_crush(Imin4_1, RPO7);
		 
		max_crush(Imin4, Imin4_1);
		Imin4_1.clear_image();

		//horizontal + vertical + diag2 + diag3
		Image3D<T> Imin4_2 = RPO1.copy_image();
		min_crush(Imin4_2, RPO2);
		min_crush(Imin4_2, RPO5);
		min_crush(Imin4_2, RPO6);
		 
		max_crush(Imin4, Imin4_2);
		Imin4_2.clear_image();


		//horizontal + profondeur + diag2+ diag4
		Image3D<T> Imin4_3 = RPO1.copy_image();
		min_crush(Imin4_3, RPO3);
		min_crush(Imin4_3, RPO5);
		min_crush(Imin4_3, RPO7);

		max_crush(Imin4, Imin4_3);
		Imin4_3.clear_image();
		 
		
		//horizontal + profondeur + diag1+ diag3
		Image3D<T> Imin4_4 = RPO1.copy_image();
		min_crush(Imin4_4, RPO3);
		min_crush(Imin4_4, RPO4);
		min_crush(Imin4_4, RPO6);

		max_crush(Imin4, Imin4_4);
		Imin4_4.clear_image();
		 
		//vertical + profondeur + diag1+ diag2
		Image3D<T> Imin4_5 = RPO2.copy_image();
		min_crush(Imin4_5, RPO3);
		min_crush(Imin4_5, RPO4);
		min_crush(Imin4_5, RPO5);

		max_crush(Imin4, Imin4_5);
		Imin4_5.clear_image();	 

		//vertical + profondeur + diag3+ diag4
		Image3D<T> Imin4_6 = RPO2.copy_image();
		min_crush(Imin4_6, RPO3);
		min_crush(Imin4_6, RPO6);
		min_crush(Imin4_6, RPO7);
	 
		max_crush(Imin4, Imin4_6);
		Imin4_6.clear_image();    
		
		//horizontal + vertical + depth + diag1
		Image3D<T> Imin4_7 = RPO1.copy_image();
		min_crush(Imin4_7, RPO2);
		min_crush(Imin4_7, RPO3);
		min_crush(Imin4_7, RPO4);
	 
		max_crush(Imin4, Imin4_7);
		Imin4_7.clear_image();    
		
		//horizontal + vertical + depth + diag2
		Image3D<T> Imin4_8 = RPO1.copy_image();
		min_crush(Imin4_8, RPO2);
		min_crush(Imin4_8, RPO3);
		min_crush(Imin4_8, RPO5);
	 
		max_crush(Imin4, Imin4_8);
		Imin4_8.clear_image();    
		
		//horizontal + vertical + depth + diag3
		Image3D<T> Imin4_9 = RPO1.copy_image();
		min_crush(Imin4_9, RPO2);
		min_crush(Imin4_9, RPO3);
		min_crush(Imin4_9, RPO6);
	 
		max_crush(Imin4, Imin4_9);
		Imin4_9.clear_image();    
		
		//horizontal + vertical + depth + diag4
		Image3D<T> Imin4_10 = RPO1.copy_image();
		min_crush(Imin4_10, RPO2);
		min_crush(Imin4_10, RPO3);
		min_crush(Imin4_10, RPO7);
	 
		max_crush(Imin4, Imin4_10);
		Imin4_10.clear_image();    


		// ---- Imin limit case 5 orientations ----
		Image3D<T> Imin5 = RPO4.copy_image();
		min_crush(Imin5, RPO5);
		min_crush(Imin5, RPO6);
		min_crush(Imin5, RPO7);
	
  
		// ----------------------- Computation of Imin2 ----------------------------
		//geodesic reconstruction of RPO6 in RPO4
		Image3D<T> RPO6_geo = geodilation(RPOt2, RPOt4, 18, -1);
		RPOt2.clear_image();

		//geodesic reconstruction of RPO5 in RPO4
		Image3D<T> RPO5_geo = geodilation(RPOt3, RPOt4, 18, -1);;
		RPOt3.clear_image();
		RPOt4.clear_image();

		// ---- Imin2 limit case 4 orientations ----
		Image3D<T> Imin2_4 = Imin4.copy_image();
		min_crush(Imin2_4, RPO5_geo);
		RPO5_geo.clear_image();


		// ---- Imin2 limit case 5 orientations ----
		Image3D<T> Imin2_5 = Imin5.copy_image();
		min_crush(Imin2_5, RPO6_geo);

		RPO6_geo.clear_image();

		// --------------------------- Final Result --------------------------------

		Image3D<T> Diff_Imin4 = diff(Imin4, Imin2_4);
		Image3D<T> Diff_Imin5 = diff(Imin5, Imin2_5);
		
		Imin4.clear_image();
		Imin2_4.clear_image();
		Imin5.clear_image();
		Imin2_5.clear_image();
		
		if (limitOri == 1) 
		{
			max_crush(RORPO_res, Diff_Imin4);
			max_crush(RORPO_res, Diff_Imin5);
		}
		
		else if (limitOri == 2)
		{
			max_crush(RORPO_res, Diff_Imin5);
		}
		
	}
	
	// Clear Images which are non useful anymore
	 RPO1.clear_image();
	 RPO2.clear_image();
	 RPO3.clear_image();
	 RPO4.clear_image();
	 RPO5.clear_image();
	 RPO6.clear_image();
	 RPO7.clear_image();
	 
   return RORPO_res;
   
}



#endif // RORPO_INCLUDED
