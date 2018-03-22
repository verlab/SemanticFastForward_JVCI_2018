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
 * @file homography.h
 *
 * Header functions for the homography.cpp.
 *
 */

#ifndef HOMOGRAPHY_H
#define HOMOGRAPHY_H

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "definitions/define.h"

/**
 * @brief The HomogCoverage enum
 */
enum HomogCoverage {
                    NO_AREA,/** It can't cover neither the drop area nor the crop area **/
                    DROP_AREA,/** It covers the drop area, but doesn't cover the crop area **/
                    CROP_AREA/** It covers both the drop and the crop areas **/
                   };

/**
 * @brief Function that find the homography matrix that leave the imageSrc to the plan of the imageDst. Returns the Ransak mask after finding the homography matrix.
 *
 * @param image_src - image to where the homography will be calculated.
 * @param image_dst - image of the destination of the homography.
 * @param homography_matrix - object to homography matrix calculated.
 * @param ransac_mask - RANSAC mask of the homography matrix. 1 means inliers and 0 means outliers.
 *
 * @return
 *      \c bool \b true  - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \b false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Michel Melo da Silva
 * @date 28/03/2016
 */
bool    findHomographyMatrix        ( const cv::Mat &image_src , const cv::Mat &image_dst , cv::Mat &homography_matrix, cv::Mat &ransac_mask ) ;

/**
 * @brief Function that find the homography matrix that leave the imageSrc to the plan of the imageDst.
 *
 * @param image_src - image to where the homography will be calculated.
 * @param image_dst - image of the destination of the homography.
 * @param homography_matrix - object to save the homography matrix calculated.
 *
 * @return
 *      \c bool \b true  - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \b false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Michel Melo da Silva
 * @date 28/03/2016
 */
bool    findHomographyMatrix        ( const cv::Mat &image_src , const cv::Mat &image_dst , cv::Mat &homography_matrix ) ;

/**
 * @brief Function that finds the homography matrix given the keypoints and descriptors. Returns the Ransak mask after finding the homography matrix.
 *
 * @param keypoints_image_src - keypoints of the source image.
 * @param keypoints_image_dst - keypoints of the target image.
 * @param descriptors_image_src - descriptors of the source image.
 * @param descriptors_image_dst - descriptors of the target image.
 * @param homography_matrix - object to homography matrix calculated.
 * @param ransac_mask - RANSAC mask of the homography matrix. 1 means inliers and 0 means outliers.
 *
 * @return
 *      \c bool \b true  - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \b false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Washington Luis de Souza Ramos
 * @date 30/08/2016
 */
bool findHomographyMatrix( const std::vector<cv::KeyPoint> &keypoints_image_src, const std::vector<cv::KeyPoint> &keypoints_image_dst,
                           const cv::Mat &descriptors_image_src, const cv::Mat &descriptors_image_dst,
                           cv::Mat &homography_matrix, cv::Mat &ransac_mask );

/**
 * @brief Function that find the homography matrix that leaves the imageSrc to the plan of the imageDst.
 *
 * @param keypoints_image_src - keypoints of the source image.
 * @param keypoints_image_dst - keypoints of the target image.
 * @param descriptors_image_src - descriptors of the source image.
 * @param descriptors_image_dst - descriptors of the target image.
 * @param homography_matrix - object to save the homography matrix calculated.
 *
 * @return
 *      \c bool \b true  - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \b false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Washington Luis de Souza Ramos
 * @date 09/09/2016
 */
bool findHomographyMatrix(const std::vector<cv::KeyPoint> &keypoints_image_src, const std::vector<cv::KeyPoint> &keypoints_image_dst,
                          const cv::Mat &descriptors_image_src, const cv::Mat &descriptors_image_dst,
                          cv::Mat &homography_matrix);

/**
 * @brief Function that apply homography matrix in a given image.
 *
 * @param image_src - image where the homography matrix will be applied.
 * @param homography_matrix - homography matrix.
 * @param image_result - object to save the image after the application of the homography matrix.
 *
 * @return
 *      \c bool \b true  - if the corner consistency is maintained after application of the homography matrix. \n
 *      \c bool \b false - if the consistency is not maintained after application of the homography matrix. In this case the imageResult is a simple copy of the imageSrc.
 *
 * @author Michel Melo da Silva
 * @date 25/03/2016
 */
bool    applyHomographyMatrix       ( const cv::Mat &image_src , const cv::Mat &homography_matrix , cv::Mat &image_result ) ;

/**
 * @brief Function that checks if after made the homography transformation the corner consistency is maintained.
 *
 * @param img_corners - image corners in the following order: \n
 *              \c img_corners[0] = 0    , 0 \n
 *              \c img_corners[1] = cols , 0 \n
 *              \c img_corners[2] = 0    , rows \n
 *              \c img_corners[3] = cols , rows
 * @param homography_matrix - homography matrix that will be checked the consistency.
 *
 * @return
 *      \c bool \b true  - if the consistency is maintained. \n
 *      \c bool \b false - if the consistency is not maintained.
 *
 * @author Michel Melo da Silva
 * @date 08/04/2016
 */
bool    checkHomographyConsistency  ( const std::vector<cv::Point2f> img_corners , const cv::Mat &homography_matrix ) ;

/**
 * @brief Function that checks if after made the homography transformation the corner consistency is maintained.
 *
 * @param new_img_corners - image corners in the following order: \n
 *              \c img_corners[0] = 0    , 0 \n
 *              \c img_corners[1] = cols , 0 \n
 *              \c img_corners[2] = 0    , rows \n
 *              \c img_corners[3] = cols , rows
 *
 * @return
 *      \c bool \b true  - if the consistency is maintained. \n
 *      \c bool \b false - if the consistency is not maintained.
 *
 * @author Michel Melo da Silva
 * @date 12/04/2016
 */
bool    checkHomographyConsistency  ( const std::vector<cv::Point2f> new_img_corners );

/**
 * @brief Function that calculates the loss(%) of the homography transformation in a ROI. It returns the
 *          ratio between the non-image area and the frame_limits area.
 *
 * @param image_src - The source image.
 * @param homography_matrix - The homography matrix.
 * @param frame_limits - The region of interest (ROI).
 *
 * @return \c double - The ratio between non-image area and the whole frame area.
 *
 * @author Washington Luis de Souza Ramos
 * @date 14/04/2016
 */
double  getAreaRatio                ( const cv::Mat& image_src, const cv::Mat& homography_matrix, const cv::Rect& frame_limits );

/**
 * @brief Function that calculates the loss(%) of the homography transformation in a ROI. It returns the
 *          coverage of a transformation which is NONE, DROP_AREA or CROP_AREA.
 * @param image_src - The source image.
 * @param homography_matrix - The homography matrix.
 * @param drop_area - The region of interest (ROI) for the drop area.
 * @param crop_area - The region of interest (ROI) for the crop area.
 * @return The HomogCoverage enum value
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
HomogCoverage getHomogCoverage     ( const cv::Mat& image_src, const cv::Mat& homography_matrix, const cv::Rect& drop_area, const cv::Rect& crop_area);

/**
 * @brief Function that gets the keypoints and describe an image in order to avoid unnecessary computations for tasks like finding the homography matrix.
 *
 * @param image - image to where the keypoints and descriptors are to be extracted from.
 * @param keypoints - the keypoints found in the image.
 * @param descriptors - the descriptors of the image.
 *
 * @author Washington Luis de Souza Ramos
 * @date 30/08/2016
 */
void getKeypointsAndDescriptors( const cv::Mat &image, std::vector< cv::KeyPoint > &keypoints, cv::Mat &descriptors );

#endif // HOMOGRAPHY_H
