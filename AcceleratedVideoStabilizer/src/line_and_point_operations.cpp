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
 * @file line_and_point_operations.cpp
 *
 * Functions to handle line and point functions.
 *
 * Functions to find lines and segment line intersection. Compute angle and magnitude of vectors.
 *
 */

#include "headers/line_and_point_operations.h"

/**
 * @brief Function that returns the maximum of the three provided numbers.
 *
 * @ref https://github.com/Itseez/opencv/blob/master/modules/imgproc/src/min_enclosing_triangle.cpp
 *
 * @param number1 - fisrt number.
 * @param number2 - second number.
 * @param number3 - third number.
 *
 * @return \c double - maximum of the three numbers.
 *
 * @author Itseez
 * @date 08/04/2016
 */
static double maximum(double number1, double number2, double number3) {
    return std::max(std::max(number1, number2), number3);
}

//! Check if the two numbers are equal (almost)
/*!
* The expression for determining if two real numbers are equal is:
* if (Abs(x - y) <= EPSILON * Max(1.0f, Abs(x), Abs(y))).
*
* @see https://github.com/Itseez/opencv/blob/master/modules/imgproc/src/min_enclosing_triangle.cpp
*
* @param number1 - First number.
* @param number2 - Second number.
*
* @return
*       \c bool \b true - if they are almost equals. \n
*       \c bool \b false - otherwise.
*/
static bool almostEqual(double number1, double number2) {
    return (std::abs(number1 - number2) <= (EPSILON * maximum(1.0, std::abs(number1), std::abs(number2))));
}

//! Determine the intersection point of two lines, if this point exists
/*! Two lines intersect if they are not parallel (Parallel lines intersect at
* +/- infinity, but we do not consider this case here).
*
* The lines are specified by a pair of points each. If they intersect, then
* the function returns true, else it returns false.
*
* Lines can be specified in the following form:
*      A1x + B1x = C1
*      A2x + B2x = C2
*
* If det (= A1*B2 - A2*B1) == 0, then lines are parallel
*                                else they intersect
*
* If they intersect, then let us denote the intersection point with P(x, y) where:
*      x = (C1*B2 - C2*B1) / (det)
*      y = (C2*A1 - C1*A2) / (det)
*
* @see https://github.com/Itseez/opencv/blob/master/modules/imgproc/src/min_enclosing_triangle.cpp
*
* @param a1 - First point for determining the first line.
* @param b1 - Second point for determining the first line.
* @param a2 - First point for determining the second line.
* @param b2 - Second point for determining the second line.
* @param intersection - The intersection point, if this point exists.
*/
static bool lineIntersection(const cv::Point2f &a1, const cv::Point2f &b1, const cv::Point2f &a2,
                             const cv::Point2f &b2, cv::Point2f &intersection) {
    double A1 = b1.y - a1.y;
    double B1 = a1.x - b1.x;
    double C1 = (a1.x * A1) + (a1.y * B1);

    double A2 = b2.y - a2.y;
    double B2 = a2.x - b2.x;
    double C2 = (a2.x * A2) + (a2.y * B2);

    double det = (A1 * B2) - (A2 * B1);

    if (!almostEqual(det, 0)) {
        intersection.x = static_cast<float>(((C1 * B2) - (C2 * B1)) / (det));
        intersection.y = static_cast<float>(((C2 * A1) - (C1 * A2)) / (det));

        return true;
    }

    return false;
}

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
bool lineSegmentIntersection(const cv::Point2f &a1, const cv::Point2f &b1, const cv::Point2f &a2,
                             const cv::Point2f &b2) {
    cv::Point2f intersection;
    lineIntersection(a1,b1,a2,b2,intersection);

    return ( ( intersection.x > a1.x && intersection.x < b1.x ) && ( intersection.x > a2.x && intersection.x < b2.x ) );
}

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
bool lineSegmentIntersection(const cv::Point2f &a1, const cv::Point2f &b1, const cv::Point2f &a2,
                             const cv::Point2f &b2, cv::Point2f &intersection) {
    lineIntersection(a1,b1,a2,b2,intersection);

    return (
                ( ( intersection.x > a1.x && intersection.x < b1.x ) && ( intersection.x > a2.x && intersection.x < b2.x ) ) ||
                ( ( intersection.x > a1.x && intersection.x < b1.x ) && ( intersection.x < a2.x && intersection.x > b2.x ) ) ||
                ( ( intersection.x < a1.x && intersection.x > b1.x ) && ( intersection.x > a2.x && intersection.x < b2.x ) ) ||
                ( ( intersection.x < a1.x && intersection.x > b1.x ) && ( intersection.x < a2.x && intersection.x > b2.x ) )
                );
}

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
void computeAnglesAndMags( const std::vector<cv::Point2f>& points_src , const std::vector<cv::Point2f>& points_dst , std::vector<float>& angles , std::vector<float>& mags )
{
    const float radConst = 180/M_PI;
    float angle;
    for (unsigned int i = 0; i < points_dst.size(); ++i) {
        float dy = points_dst[i].y - points_src[i].y;
        float dx = points_dst[i].x - points_src[i].x ;
        angle = -atan2(dy, dx); // angle in radians
        if(angle < 0) angle += M_PI * 2;
        angles[i] = angle*radConst;

        mags[i] = std::sqrt(std::pow(dx,2) + std::pow(dy,2));
    }
}

