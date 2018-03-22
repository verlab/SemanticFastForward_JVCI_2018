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
 * @file line_and_point_operations.h
 *
 * Header functions for the line_and_point_operations.cpp.
 *
 */

#ifndef LINE_SEGMENT_INTERSECTION_H
#define LINE_SEGMENT_INTERSECTION_H

#include <stdio.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/** Maximum difference to consider two values equals.*/
#define EPSILON 1E-5

/**
 * @brief Function that verify if two line segments have intersection point.
 *
 * @see https://github.com/Itseez/opencv/blob/master/modules/imgproc/src/min_enclosing_triangle.cpp
 *
 * @param a1 - first point of the first line segment.
 * @param b1 - second point of the first line segment.
 * @param a2 - first point of the second line segment.
 * @param b2 - second point of the second line segment.
 *
 * @return
 *      \c bool \b true  - if there is a intersection point. \n
 *      \c bool \b false - if there is not a intersection point.
 *
 * @author Michel Melo da Silva
 * @date 04/04/2016
 */
bool lineSegmentIntersection    ( const cv::Point2f &a1 , const cv::Point2f &b1 , const cv::Point2f &a2 , const cv::Point2f &b2 );

/**
 * @brief Function that calculate the homography matrix that leave the frame_i to the intermediate plan between the plans of frame_master_pre and frame_master_pre
 *          with relation of the distance between them.
 *
 * @see https://github.com/Itseez/opencv/blob/master/modules/imgproc/src/min_enclosing_triangle.cpp
 *
 * @param a1 - first point of the first line segment.
 * @param b1 - second point of the first line segment.
 * @param a2 - first point of the second line segment.
 * @param b2 - second point of the second line segment.
 * @param intersection - intersection point between the line segments.
 *
 * @return
 *      \c bool \b true - if there is a intersection point. \n
 *      \c bool \b false - if there is not a intersection point.
 *
 * @author Michel Melo da Silva
 * @date 04/04/2016
 */
bool lineSegmentIntersection    ( const cv::Point2f &a1 , const cv::Point2f &b1 , const cv::Point2f &a2 , const cv::Point2f &b2 , cv::Point2f &intersection );

/**
 * @brief Function that compute the angle and magnitude of the optical flow from the points_src to points_dst.
 *
 * @param points_src - vector of source points.
 * @param points_dst - vector of destination points.
 * @param angles - object to save the angle of the vectors.
 * @param mags - object to save the magnitude of the vectors.
 *
 * @return \c void
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/04/2016
 */
void computeAnglesAndMags       ( const std::vector<cv::Point2f>& points_src , const std::vector<cv::Point2f>& points_dst , std::vector<float>& angles , std::vector<float>& mags );

#endif // LINE_SEGMENT_INTERSECTION_H
