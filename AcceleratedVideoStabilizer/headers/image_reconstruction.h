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
 * @file image_reconstruction.h
 *
 * Header functions for the image_reconstruction.cpp.
 *
 */

#ifndef IMAGE_RECONSTRUCTION_H
#define IMAGE_RECONSTRUCTION_H

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>

#include "definitions/define.h"
#include "definitions/experiment_struct.h"

#include "executables/execute_commands.h"

#include "homography.h"

/**
 * @brief Function that reconstructs an image using panorama based on homography in a image sequence.
 *
 * @param image - image initial with homography transformation.
 * @param index - index of the initial image in the original video.
 * @param experiment_settings - object with the experiment settings.
 * @param frame_boundaries - limits of the frame that needs to be filled by the reconstructed image.
 * @param reconstructed_image - object to save the final reconstructed image.
 *
 * @return
 *      \c bool \b true  - if the frame was successful reconstructed filling all the frame limit. \n
 *      \c bool \b false - if the frame was not successful reconstructed filling all the frame limit.
 *
 * @author Michel Melo da Silva
 * @date 14/04/2016
 */
bool    imageReconstruction     ( const cv::Mat &image , const int index , const EXPERIMENT &experiment_settings ,
                                  const cv::Rect &frame_boundaries, cv::Mat &reconstructedImage ) ;

/**
 * @brief Function that reconstructs an image using panorama based on homography in a image sequence.
 *
 * @param image - image initial without homography transformation.
 * @param homography_matrix - homography matrix that will be applied to the original image where the reconstruct will start.
 * @param index - index of the initial image in the original video.
 * @param experiment_settings - object with the experiment settings.
 * @param frame_boundaries - limits of the frame that needs to be filled by the reconstructed image.
 * @param reconstructed_image - object to save the final reconstructed image.
 *
 * @return
 *      \c bool \b true  - if the frame was successful reconstructed filling all the frame limit. \n
 *      \c bool \b false - if the frame was not successful reconstructed filling all the frame limit.
 *
 * @author Michel Melo da Silva
 * @date 14/04/2016
 */
bool    reconstructImage (const cv::Mat &image , const cv::Mat &homography_matrix , const int index , const EXPERIMENT &experiment_settings ,
                          const cv::Rect &drop_boundaries, const cv::Rect &frame_boundaries , cv::Mat &reconstructed_image );

enum ReconstructionType{
    PRE_AND_POS,//Reconstructs using both sides
    ONLY_PRE,//Reconstructs using only the previous frames
    ONLY_POS//Reconstructs using only the posterior frames
};

#endif // IMAGE_RECONSTRUCTION_H
