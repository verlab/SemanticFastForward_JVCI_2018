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
 * @file homography.cpp
 *
 * Functions related with homography transformation.
 *
 * Functions find homography matrix between two images. Apply homography matrix. Check if the image corner consistency is held after application of the homography matrix.
 * Check the area ratio between the image after the homography matrix application and the frame boundaries.
 *
 */

#include "definitions/define.h"

#include "headers/homography.h"

/**
 * @brief Function that find the homography matrix that leaves the imageSrc to the plan of the imageDst. Returns the Ransak mask after finding the homography matrix.
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
bool findHomographyMatrix( const cv::Mat &image_src, const cv::Mat &image_dst, cv::Mat &homography_matrix, cv::Mat &ransac_mask )
{
    //-- Step 1: Detect the keypoints using SURF Detector.
    int min_hessian = 400;
    std::vector< cv::KeyPoint > keypoints_image_src,
            keypoints_image_dst;

    cv::SurfFeatureDetector detector( min_hessian );
    detector.detect( image_src, keypoints_image_src );
    detector.detect( image_dst, keypoints_image_dst );

    //-- Step 2: Calculate descriptors (feature vectors).
    cv::SurfDescriptorExtractor extractor;

    cv::Mat descriptors_image_src,
            descriptors_image_dst;

    extractor.compute( image_src, keypoints_image_src, descriptors_image_src );
    extractor.compute( image_dst, keypoints_image_dst, descriptors_image_dst );

    //-- Step 3: Matching descriptor vectors using BF matcher.
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_image_src, descriptors_image_dst, matches );

    double max_dist = 0,
           min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints.
    for( int i = 0; i < descriptors_image_src.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        else if( dist > max_dist ) max_dist = dist;
    }

    // Catch identical images.
    if( max_dist == 0 ){
        ransac_mask = cv::Mat::zeros(1,1,CV_32F);
        return false;
    }

    //    std::cout << "-- Number of matches: " << matches.size() << std::endl
    //              << "-- Max dist : " <<  max_dist << std::endl
    //              << "-- Min dist : " << min_dist << std::endl
    //              << "-- Threshold match: " << thresholdMatch*min_dist << std::endl << std::endl;

    //-- Step 4: Select only "good" matches (i.e. whose distance is less than 3*min_dist ).
    std::vector< cv::DMatch > good_matches;
    float matches_threshold = MATCHES_THRESHOLD_FACTOR*min_dist;

    for( unsigned int i = 0; i < matches.size(); i++ )
        if( matches[i].distance < matches_threshold)
            good_matches.push_back(matches[i]);

    // Catch not enough number of matches ( good_matches.size() < 4 ).
    if( good_matches.size() < MIN_NUMBER_OF_GOOD_MATCHES ){
        ransac_mask = cv::Mat::zeros(1,1,CV_32F);
        return false;
    }

    std::vector< cv::Point2f > selected_keypoints_image_src(good_matches.size()),
            selected_keypoints_image_dst(good_matches.size());

    for( unsigned int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches.
        selected_keypoints_image_src[i] = keypoints_image_src[ good_matches[i].queryIdx ].pt;
        selected_keypoints_image_dst[i] = keypoints_image_dst[ good_matches[i].trainIdx ].pt;
    }


    //-- Step 5: Find the Homography Matrix.
    homography_matrix = cv::findHomography( selected_keypoints_image_src, selected_keypoints_image_dst, CV_RANSAC, 3, ransac_mask);

    //    std::cout << "-- Number of good matches: " << ransacMask.size().height << std::endl
    //              << "-- Number of RANSAC inliers: " << cv::sum( ransacMask )[0] << std::endl;

    return true;
}

/**
 * @brief Function that find the homography matrix that leaves the imageSrc to the plan of the imageDst.
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
bool findHomographyMatrix( const cv::Mat &image_src, const cv::Mat &image_dst, cv::Mat &homography_matrix )
{
    //-- Step 1: Detect the keypoints using SURF Detector.
    int min_hessian = 400;
    std::vector< cv::KeyPoint > keypoints_image_src,
            keypoints_image_dst;

    //    cv::SiftFeatureDetector detector;
    cv::SurfFeatureDetector detector( min_hessian );
    detector.detect( image_src, keypoints_image_src );
    detector.detect( image_dst, keypoints_image_dst );

    //-- Step 2: Calculate descriptors (feature vectors).
    cv::SurfDescriptorExtractor extractor;
    //    cv::SiftDescriptorExtractor extractor;

    cv::Mat descriptors_image_src,
            descriptors_image_dst;

    extractor.compute( image_src, keypoints_image_src, descriptors_image_src );
    extractor.compute( image_dst, keypoints_image_dst, descriptors_image_dst );

    //-- Step 3: Matching descriptor vectors using BF matcher.
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_image_src, descriptors_image_dst, matches );

    double max_dist = 0,
            min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints.
    for( int i = 0; i < descriptors_image_src.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        else if( dist > max_dist ) max_dist = dist;
    }

    // Catch identical or no keypoints images.
    if( max_dist == 0 || keypoints_image_src.empty() || keypoints_image_dst.empty())
        return false;

    //-- Step 4: Select only "good" matches (i.e. whose distance is less than 3*min_dist ).
    std::vector< cv::DMatch > good_matches;
    float matches_threshold = MATCHES_THRESHOLD_FACTOR*min_dist;

    for( int i = 0; i < descriptors_image_src.rows; i++ )
        if( matches[i].distance < matches_threshold )
            good_matches.push_back(matches[i]);

    // Catch not enoughy number of matches ( good_matches.size() < 4 ).
    if( good_matches.size() < MIN_NUMBER_OF_GOOD_MATCHES )
        return false;

    // ----------------------------------------------------------------------
    // DEBUG
    if ( DEBUG_HOMOGRAPHY )
        std::cout << "-- Max dist : " <<  max_dist << std::endl
                  << "-- Min dist : " << min_dist << std::endl
                  << "-- Good matches : " <<  good_matches.size() << std::endl;
    // ----------------------------------------------------------------------

    std::vector< cv::Point2f > selected_keypoints_image_src(good_matches.size()),
            selected_keypoints_image_dst(good_matches.size());

    for( unsigned int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches.
        selected_keypoints_image_src[i] = keypoints_image_src[ good_matches[i].queryIdx ].pt;
        selected_keypoints_image_dst[i] = keypoints_image_dst[ good_matches[i].trainIdx ].pt;
    }

    //-- Step 5: Find the Homography Matrix.
    homography_matrix = cv::findHomography( selected_keypoints_image_src , selected_keypoints_image_dst , CV_RANSAC );

    return true;
}

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
                           cv::Mat &homography_matrix, cv::Mat &ransac_mask )
{
    //Following steps after detecting keypoints and describing the image
    //-- Step 3: Matching descriptor vectors using BF matcher.
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_image_src, descriptors_image_dst, matches );

    double max_dist = 0.,
           min_dist = 100.,
           sum = 0.,
            matches_threshold;

    //-- Quick calculation of max and min distances between keypoints.
    for( unsigned int i = 0; i < matches.size(); i++ )
    {
        double dist = matches[i].distance;
        sum = sum + dist;
        if( dist < min_dist ) min_dist = dist;
        else if( dist > max_dist ) max_dist = dist;
    }

    matches_threshold = sum/matches.size();

    // Catch identical or no keypoints images.
    if( max_dist == 0 || keypoints_image_src.empty() || keypoints_image_dst.empty()){
        ransac_mask = cv::Mat::zeros(1,1,CV_32F);
        return false;
    }

    //float matches_threshold = MATCHES_THRESHOLD_FACTOR*average;

    //    std::cout << "-- Number of matches: " << matches.size() << std::endl
    //              << "-- Max dist : " <<  max_dist << std::endl
    //              << "-- Min dist : " << min_dist << std::endl
    //              << "-- Threshold match: " << thresholdMatch*min_dist << std::endl << std::endl;

    //-- Step 4: Select only "good" matches (i.e. whose distance is less than 3*min_dist ).
    std::vector< cv::DMatch > good_matches;


    for( unsigned int i = 0; i < matches.size(); i++ )
        if( matches[i].distance < matches_threshold)
            good_matches.push_back(matches[i]);

    // Catch not enough number of matches ( good_matches.size() < 4 ).
    if( good_matches.size() < MIN_NUMBER_OF_GOOD_MATCHES ){
        ransac_mask = cv::Mat::zeros(1,1,CV_32F);
        return false;
    }

    std::vector< cv::Point2f > selected_keypoints_image_src(good_matches.size()),
            selected_keypoints_image_dst(good_matches.size());

    for( unsigned int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches.
        selected_keypoints_image_src[i] = keypoints_image_src[ good_matches[i].queryIdx ].pt;
        selected_keypoints_image_dst[i] = keypoints_image_dst[ good_matches[i].trainIdx ].pt;
    }

    //-- Step 5: Find the Homography Matrix.
    homography_matrix = cv::findHomography( selected_keypoints_image_src, selected_keypoints_image_dst, CV_RANSAC, 3, ransac_mask);

    //    std::cout << "-- Number of good matches: " << ransacMask.size().height << std::endl
    //              << "-- Number of RANSAC inliers: " << cv::sum( ransacMask )[0] << std::endl;

    return true;
}

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
                          cv::Mat &homography_matrix)
{
    //-- Step 3: Matching descriptor vectors using BF matcher.
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors_image_src, descriptors_image_dst, matches );

    double max_dist = 0,
            min_dist = 100;

    //-- Quick calculation of max and min distances between keypoints.
    for( int i = 0; i < descriptors_image_src.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        else if( dist > max_dist ) max_dist = dist;
    }

    // Catch identical images.
    if( max_dist == 0 )
        return false;

    //-- Step 4: Select only "good" matches (i.e. whose distance is less than 3*min_dist ).
    std::vector< cv::DMatch > good_matches;
    float matches_threshold = MATCHES_THRESHOLD_FACTOR*min_dist;

    for( int i = 0; i < descriptors_image_src.rows; i++ )
        if( matches[i].distance < matches_threshold )
            good_matches.push_back(matches[i]);

    // Catch not enoughy number of matches ( good_matches.size() < 4 ).
    if( good_matches.size() < MIN_NUMBER_OF_GOOD_MATCHES )
        return false;

    // ----------------------------------------------------------------------
    // DEBUG
//    if ( DEBUG_HOMOGRAPHY )
//        std::cout << "-- Max dist : " <<  max_dist << std::endl
//                  << "-- Min dist : " << min_dist << std::endl
//                  << "-- Good matches : " <<  good_matches.size() << std::endl;
    // ----------------------------------------------------------------------

    std::vector< cv::Point2f > selected_keypoints_image_src(good_matches.size()),
            selected_keypoints_image_dst(good_matches.size());

    for( unsigned int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches.
        selected_keypoints_image_src[i] = keypoints_image_src[ good_matches[i].queryIdx ].pt;
        selected_keypoints_image_dst[i] = keypoints_image_dst[ good_matches[i].trainIdx ].pt;
    }

    //-- Step 5: Find the Homography Matrix.
    homography_matrix = cv::findHomography( selected_keypoints_image_src, selected_keypoints_image_dst, CV_RANSAC);

    return true;
}

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
bool applyHomographyMatrix( const cv::Mat &image_src, const cv::Mat &homography_matrix, cv::Mat &image_result )
{
    //-- Get the corners from the imageSrc
    std::vector<cv::Point2f> img_corners(4);
    img_corners[0] = cv::Point2f ( 0             , 0);
    img_corners[1] = cv::Point2f ( image_src.cols , 0 );
    img_corners[2] = cv::Point2f ( 0             , image_src.rows );
    img_corners[3] = cv::Point2f ( image_src.cols , image_src.rows );
    std::vector<cv::Point2f> new_img_corners(4);

    perspectiveTransform( img_corners, new_img_corners, homography_matrix);

    int height = image_src.rows,
        width = image_src.cols;

    for (int i = 0 ; i < 4 ; i++){
        if ( int(ceil(new_img_corners.at(i).x)) > width )
            width = int(ceil(new_img_corners.at(i).x));
        if ( int(ceil(new_img_corners.at(i).y)) > height )
            height = int(ceil(new_img_corners.at(i).y));
    }

    if ( ! checkHomographyConsistency( new_img_corners )
         || width > 4 * image_src.cols
         || height > 4 * image_src.rows ){
        // ----------------------------------------------------------------------
        // DEBUG
        if ( DEBUG_HOMOGRAPHY )
            std::cout << "Projecao errada**************************" << std::endl;
        // ----------------------------------------------------------------------
        image_result = image_src.clone();
        return false;
    }

    // ----------------------------------------------------------------------
    // DEBUG
    if ( DEBUG_HOMOGRAPHY )
        std::cout<< "height: " << height << std::endl
                 << "width: " << width << std::endl;
    // ----------------------------------------------------------------------

    // Use the Homography Matrix to warp the images
    cv::warpPerspective( image_src, image_result, homography_matrix , cv::Size(width, height) );
    return true;
}

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
bool checkHomographyConsistency (const std::vector<cv::Point2f> img_corners, const cv::Mat &homography_matrix ) {

    std::vector<cv::Point2f> new_img_corners(4);
    perspectiveTransform( img_corners, new_img_corners, homography_matrix);

    return !( new_img_corners[3].x < new_img_corners[2].x ||
         new_img_corners[3].x < new_img_corners[0].x ||
         new_img_corners[3].y < new_img_corners[1].y ||
         new_img_corners[3].y < new_img_corners[0].y ||
         new_img_corners[1].x < new_img_corners[0].x ||
         new_img_corners[1].x < new_img_corners[2].x ||
         new_img_corners[1].y > new_img_corners[2].y ||
         new_img_corners[2].y < new_img_corners[0].y   );
}

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
bool checkHomographyConsistency ( const std::vector<cv::Point2f> new_img_corners ) {

    bool c1 = (int)new_img_corners[3].x <= (int)new_img_corners[2].x,
            c2 = (int)new_img_corners[3].x <= (int)new_img_corners[0].x ,
            c3 = (int)new_img_corners[3].y <= (int)new_img_corners[1].y ,
            c4 = (int)new_img_corners[3].y <= (int)new_img_corners[0].y ,
            c5 = (int)new_img_corners[1].x <= (int)new_img_corners[0].x ,
            c6 = (int)new_img_corners[1].x <= (int)new_img_corners[2].x ,
            c7 = (int)new_img_corners[1].y >= (int)new_img_corners[2].y ,
            c8 = (int)new_img_corners[2].y <= (int)new_img_corners[0].y ,
            c = c1 || c2 || c3 || c4 || c5 || c6 || c7 || c8 ,
            cr = !c;

    return cr ;
}

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
double getAreaRatio ( const cv::Mat& image_src, const cv::Mat& homography_matrix, const cv::Rect& frame_limits ){

    cv::Mat homography_mask = cv::Mat(image_src.rows, image_src.cols, image_src.type(), cv::Scalar::all(255)),
            frame_mask,
            intersection_mask,
            homography_mask_result;

    applyHomographyMatrix(homography_mask, homography_matrix, homography_mask_result);
    frame_mask = cv::Mat::zeros(homography_mask_result.rows, homography_mask_result.cols, image_src.type());
    frame_mask(frame_limits) = cv::Scalar::all(255);
    intersection_mask = frame_mask & homography_mask_result;

    cv::cvtColor(intersection_mask, intersection_mask, CV_BGR2GRAY);

    // Transform it to binary and invert it. White on black is needed.
    cv::Mat intersection_mask_bw;
    cv::threshold(intersection_mask(frame_limits), intersection_mask_bw, 1, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

   return cv::countNonZero(intersection_mask_bw)/double(frame_limits.height*frame_limits.width);
}

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
HomogCoverage getHomogCoverage ( const cv::Mat& image_src, const cv::Mat& homography_matrix, const cv::Rect& drop_area, const cv::Rect& crop_area){

    cv::Mat homography_mask = cv::Mat(image_src.rows, image_src.cols, image_src.type(), cv::Scalar::all(255)),
            frame_mask_da,//Mask for the drop area (da)
            frame_mask_ca,//Mask for the crop area (ca)
            intersection_mask_da,
            intersection_mask_ca,
            homography_mask_result;

    applyHomographyMatrix(homography_mask, homography_matrix, homography_mask_result);

    frame_mask_da = cv::Mat::zeros(homography_mask_result.rows, homography_mask_result.cols, image_src.type());
    frame_mask_ca = cv::Mat::zeros(homography_mask_result.rows, homography_mask_result.cols, image_src.type());

    frame_mask_da(drop_area) = cv::Scalar::all(255);
    frame_mask_ca(crop_area) = cv::Scalar::all(255);

    intersection_mask_da = frame_mask_da & homography_mask_result;
    intersection_mask_ca = frame_mask_ca & homography_mask_result;

    cv::cvtColor(intersection_mask_da, intersection_mask_da, CV_BGR2GRAY);
    cv::cvtColor(intersection_mask_ca, intersection_mask_ca, CV_BGR2GRAY);

    // Transform it to binary and invert it. White on black is needed.
    cv::Mat intersection_mask_bw;

    cv::threshold(intersection_mask_ca(crop_area), intersection_mask_bw, 1, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    if(cv::countNonZero(intersection_mask_bw)/double(crop_area.height*crop_area.width) <= MAXIMUM_AREA_ALLOWED)
        return CROP_AREA;

    cv::threshold(intersection_mask_da(drop_area), intersection_mask_bw, 1, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    if(cv::countNonZero(intersection_mask_bw)/double(drop_area.height*drop_area.width) <= MAXIMUM_AREA_ALLOWED)
        return DROP_AREA;

    else
        return NO_AREA;
}

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
void getKeypointsAndDescriptors( const cv::Mat &image, std::vector< cv::KeyPoint > &keypoints, cv::Mat &descriptors )
{
    //First steps for finding the homography matrix
    //-- Step 1: Detect the keypoints using SURF Detector.
    cv::SurfFeatureDetector detector( MIN_HESSIAN );
    detector.detect( image, keypoints );

    //-- Step 2: Calculate descriptors (feature vectors).
    cv::SurfDescriptorExtractor extractor;
    extractor.compute( image, keypoints, descriptors );
}
