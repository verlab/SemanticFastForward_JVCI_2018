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
 * @file sequence_processing.h
 *
 * Header functions for the sequence_processing.cpp.
 *
 */

#ifndef SEQUENCE_PROCESSING_H
#define SEQUENCE_PROCESSING_H

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <omp.h>
#include <armadillo>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "definitions/experiment_struct.h"

#include "file_operations.h"

/**
 * @brief Function that calculates the homography matrix that leave the frame_i to the intermediate plan between the plans of frame_master_pre and frame_master_pre
 *          with relation of the distance between them.
 *
 * @param d - distance between the current frame and the previous master frame.
 * @param D - distance between previous master frame and the posterior master frame.
 * @param N - size of the segments.
 * @param frame_i - current image.
 * @param frame_master_pre - image of the previous master frame.
 * @param frame_master_pos - image of the posterior master frame.
 * @param homography_matrix_result - object to save the result homography matrix.
 *
 * @return
 *      \c bool \b true - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \c false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Michel Melo da Silva
 * @date 04/04/2016
 */
bool                findIntermediateHomographyMatrix        ( const int d , const int D , const int N ,
                                                              const cv::Mat& frame_i , const cv::Mat& frame_master_pre , const cv::Mat& frame_master_pos ,
                                                              cv::Mat& homography_matrix_result ) ;

/**
 * @brief Function that calculates the homography matrix that leave the frame_i to the intermediate plan between
 *          the frames related to the previous and posterior master, with respect to the distance between them.
 *
 * @param d - distance between the current frame and the previous master frame.
 * @param D - distance between previous master frame and the posterior master frame.
 * @param N - size of the segments.
 * @param frame_i - current frame.
 * @param keypoints_master_pre - keypoints of the previous master.
 * @param keypoints_master_pos - keypoints of the posterior master.
 * @param descriptors_master_pre - descriptors of the previous master.
 * @param descriptors_master_pos - descriptors of the posterior master.
 * @param homography_matrix_result - object to save the result homography matrix.
 *
 * @return
 *      \c bool \b true - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \c false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Washington Luis de Souza Ramos
 * @date 30/08/2016
 */
bool findIntermediateHomographyMatrix (const int d, const int D, const int N , const cv::Mat &frame_i ,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pre,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pos,
                                        const cv::Mat &descriptors_master_pre,
                                        const cv::Mat &descriptors_master_pos,
                                        cv::Mat& homography_matrix_result );

/**
 * @brief Function that calculates the homography matrix that leave the frame_i to the intermediate plan between
 *          the frames related to the previous and posterior master, with respect to the distance between them.
 *
 * @param d - frame shift value between the previous master frame and the current frame.
 * @param D - frame shift between previous master frame and the posterior master frame.
 * @param frame_i - current frame.
 * @param keypoints_master_pre - keypoints of the previous master.
 * @param keypoints_master_pos - keypoints of the posterior master.
 * @param descriptors_master_pre - descriptors of the previous master.
 * @param descriptors_master_pos - descriptors of the posterior master.
 * @param homography_matrix_result - object to save the result homography matrix.
 *
 * @return
 *      \c bool \b true - if there is enough points in both images and good matches between them to find a homography matrix. \n
 *      \c bool \c false - if there is not enough points in both images or good matches between them to find a homography matrix.
 *
 * @author Washington Luis de Souza Ramos
 * @date 08/09/2016
 */
bool findIntermediateHomographyMatrix (const float s, const float S, const cv::Mat &frame_i ,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pre,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pos,
                                        const cv::Mat &descriptors_master_pre,
                                        const cv::Mat &descriptors_master_pos,
                                        cv::Mat& homography_matrix_result);

/**
 * @brief Function that selects a new frame in the original video using the values of semantic information of the transition from the frame_src to the frame_dst calculted by
 *          MATLAB and saved in a CSV file, the area ratio of the iamge after apply the homography transformation and the RANSCAC inliers from the previous and posterior frames
 *          that compose the reduced video.
 *
 * @param d - distance between the current frame and the previous master frame.
 * @param D - distance between previous master frame and the posterior master frame.
 * @param N - size of the segments.
 * @param index - index of the frame that will be replaced.
 * @param index_previous - index of the last frame in the reduced video.
 * @param index_posterior - index of the next frame in the reduced video.
 * @param image_master_previous - image with the master previous
 * @param image_master_posterior - image with the master posterior
 * @param crop_area - crop area of the video.
 * @param experiment_settings - experiment settings struct.
 * @param new_frame - image that will receive the new selected frame.
 *
 * @return \c int - returns the index of the new selected frame.
 *
 * @author Michel Melo da Silva
 * @date 30/04/2016
 */
int                 selectNewFrame                        (const int d , const int D , const int N , const int index , const int index_previous , const int index_posterior ,
                                                              const std::vector<cv::KeyPoint> &keypoints_master_pre , const std::vector<cv::KeyPoint> &keypoints_master_pos , const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos, const cv::Rect &crop_area ,
                                                              const EXPERIMENT &experiment_settings , cv::Mat& new_frame );

/**
 * @brief Function that selects a new frame in the original video using the values of semantic information of the transition from the frame_src to the frame_dst calculted by
 *          MATLAB and saved in a CSV file, the area ratio of the iamge after apply the homography transformation and the RANSCAC inliers from the previous and posterior frames
 *          that compose the reduced video.
 *
 * @param s - frame shift value between the previous master frame and the current frame.
 * @param S - frame shift between previous master frame and the posterior master frame.
 * @param index - index of the frame that will be replaced.
 * @param index_previous - index of the last frame in the reduced video.
 * @param index_posterior - index of the next frame in the reduced video.
 * @param image_master_previous - image with the master previous
 * @param image_master_posterior - image with the master posterior
 * @param crop_area - crop area of the video.
 * @param experiment_settings - experiment settings struct.
 * @param new_frame - image that will receive the new selected frame.
 *
 * @return \c int - returns the index of the new selected frame.
 *
 * @author Washington Luis de Souza Ramos
 * @date 08/09/2016
 */
int selectNewFrame (const float s, const float S, const int index , const int index_previous , const int index_posterior ,
                       const std::vector<cv::KeyPoint> &keypoints_master_pre , const std::vector<cv::KeyPoint> &keypoints_master_pos , const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos, const cv::Rect& crop_area ,
                       const EXPERIMENT& experiment_settings , cv::Mat& new_frame );

#endif // SEQUENCE_posCESSING_H
