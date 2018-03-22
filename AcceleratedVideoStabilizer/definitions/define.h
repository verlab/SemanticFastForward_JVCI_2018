//////////////////////////////////////////////////////////////////////////////////////
////   This file is part of SemanticFastForward_EPIC@ECCVW.
//
//    SemanticFastForward_EPIC@ECCVW is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with SemanticFastForward_EPIC@ECCVW.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////////

/**
 * @file define.h
 *
 * Macros used in the code relate with debug/view flags and usual values.
 *
 */

#ifndef DEFINE_H
#define DEFINE_H

/** Show images for each step of processing in a window called 'Homography'. */
#define VIEW                            1 /*true*/    /*false*/

/** Show images for each step of processing in a window called 'Homography'. */
#define VIEW_DELAY                      0

/** Print the frame number in the images before save it.*/
#define FRAME_NUMBER_RESULT             0 /*true*/    /*false*/

/** Print variable values for each step of processing in the main cpp file.*/
#define DEBUG_INTERMEDIATE_FRAME        0 /*true*/    /*false*/

/** Print variable values for each step of processing in the main cpp file.*/
#define DEBUG_FRAME_STATUS              1 /*true*/    /*false*/

/** Print variable values for each step of processing in the homography cpp file.*/
#define DEBUG_HOMOGRAPHY                0 /*true*/    /*false*/

/** Print variable values for each step of processing in the image_reconstruciton cpp file.*/
#define DEBUG_RECONSTRUCTION            0 /*true*/    /*false*/

/** Minimum number of good matches to calcule the homography matrix.*/
#define MIN_NUMBER_OF_GOOD_MATCHES      50

/** Maximum area allowed to check if an image need to continue on being reconstructed.*/
#define MAXIMUM_AREA_ALLOWED          0.001d

/** Number maximum of images that could be used to reconstruct a image in the image_reconstruction.cpp */
#define NUM_MAX_IMAGES_TO_RECONSTRUCT   30

/** The SURF Hessian minimum threshold */
#define MIN_HESSIAN 400

/** Multiplier to the minimum distance from descriptors to set a threshold in matches search*/
#define MATCHES_THRESHOLD_FACTOR 3

/** The percentage of the original image that can be lost **/
#define CROP_PORTION 0.05f

/** The percentage of the original image that can be lost **/
#define DROP_PORTION 0.2f

/** Max number of drops if smaller than the drop area **/
#define MAX_DROP_ATTEMPTS 3

/** Max number of frames skip from any frame. Defined by the fast-forward algorithm. **/
#define MAX_SKIP 100

#endif // DEFINE_H
