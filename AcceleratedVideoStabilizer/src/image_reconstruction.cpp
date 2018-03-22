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
 * @file image_reconstruction.cpp
 *
 * Functions related with reconstruction of the image after application of the homography matrix to fill the whole frame boundaries.
 *
 * Functions to check the frame boundaries. Warp images to increase the image area. Reconstruction the image using frame from the original video.
 *
 */

#include "headers/image_reconstruction.h"

/**
 * @brief Function that verify if the image fill all the frame limite given by the rect boundaries.
 *
 * @param image - image that will be checked.
 * @param frame_boundaries - area where the image should fill.
 *
 * @return
 *      \c bool \b true  - if the image fill all the frame limit. \n
 *      \c bool \b false - if the image does not fill all the frame limit.
 *
 * @author Michel Melo da Silva
 * @date 14/04/2016
 */
bool checkImageBoundaries ( const cv::Mat &image , const cv::Rect &frame_boundaries ){

    cv::Mat img_ad_threshold = cv::Mat(image.rows, image.cols, CV_8UC1),
            image_gray ,
            intersection_mask ;

    cv::cvtColor( image , image_gray , CV_BGR2GRAY );

    cv::GaussianBlur( image_gray , image_gray , cv::Size( 1, 1 ) , 0 , 0 );
    cv::threshold( image_gray , img_ad_threshold , 1 , 255 , CV_THRESH_BINARY);

    cv::morphologyEx( img_ad_threshold , img_ad_threshold , cv::MORPH_CLOSE ,
                      cv::getStructuringElement( cv::MORPH_RECT , cv::Size(10, 10) ) );

    cv::Mat frame_mask = cv::Mat::zeros(img_ad_threshold.rows, img_ad_threshold.cols, img_ad_threshold.type());
    frame_mask(frame_boundaries) = cv::Scalar::all(255);
    intersection_mask = frame_mask & img_ad_threshold;

    double percentage = 1.0d - double(cv::countNonZero(intersection_mask))/double(frame_boundaries.height*frame_boundaries.width);

    return ( percentage < MAXIMUM_AREA_ALLOWED );
}

/**
 * @brief Function that verify if the image_mask fill all the frame limite given by the rect boundaries.
 *
 * @param image_mask - mask of the image to be checked.
 * @param frame_boundaries - area where the image should fill.
 *
 * @return
 *      \c bool \b true  - if the image fill all the frame limit. \n
 *      \c bool \b false - if the image does not fill all the frame limit.
 *
 * @author Michel Melo da Silva
 * @date 03/05/2016
 */
bool checkImageBoundariesMask ( const cv::Mat &image_mask , const cv::Rect &frame_boundaries ){

    cv::Mat frame_mask = cv::Mat::zeros(image_mask.rows, image_mask.cols, image_mask.type());
    frame_mask(frame_boundaries) = cv::Scalar(255);
    cv::Mat intersection_mask = frame_mask & image_mask;
    cv::cvtColor( intersection_mask , intersection_mask , CV_BGR2GRAY );

    double percentage = 1.0d - double(cv::countNonZero(intersection_mask))/double(frame_boundaries.height*frame_boundaries.width);

    return ( percentage < MAXIMUM_AREA_ALLOWED );
}

/**
 * @brief Function that warps the image_to_warp with the image image_fixed and save the result into de image_result and then the resut is cropped into the original image area.
 *
 * @param image_to_warp - image to warp.
 * @param image_fixed - image fixed that will be in the first plane.
 * @param image_fixed_mask - mask of the fixed mask.
 * @param image_result - object to save the result image.
 * @param image_result_mask - objecto to save the mask of the result image.
 *
 * @return
 *      \c bool \b true  - if the corner consistency is maintained after application of the homography matrix found to warp the image. It also returns
 *              false if is not found a homography matrix between the images. \n
 *      \c bool \b false - if the consistency is not maintained after application of the homography matrix found to warp the image. In this case the
 *              imageResult is a simple copy of the imageFixed.
 *
 * @author Michel Melo da Silva
 * @date 03/05/2016
 */
bool warpMaskCrop( const cv::Mat &image_to_warp, const cv::Mat &image_fixed , const cv::Mat &image_fixed_mask ,
                   cv::Mat &image_result , cv::Mat &image_result_mask )
{
    cv::Mat gray_image_src ,
            gray_image_dst ;

    // Convert to Grayscale

    cv::cvtColor( image_fixed, gray_image_dst, CV_BGR2GRAY );
    cv::cvtColor( image_to_warp, gray_image_src, CV_BGR2GRAY );
    if( !gray_image_src.data || !gray_image_dst.data ) {
        std::cout<< " --(!) ERROR: Could not convert images to Grayscale!" << std::endl;
        return false;
    } else {
        cv::Mat homography_matrix;
        if(!findHomographyMatrix(gray_image_src, gray_image_dst, homography_matrix)){
            image_result = image_fixed.clone();
            return false;
        }

        //-- Get the corners from the imageSrc
        std::vector<cv::Point2f> img_corners(4);
        img_corners[0] = cv::Point( 0                    , 0                    );
        img_corners[1] = cv::Point( gray_image_src.cols  , 0                    );
        img_corners[2] = cv::Point( 0                    , gray_image_src.rows  );
        img_corners[3] = cv::Point( gray_image_src.cols  , gray_image_src.rows  );
        std::vector<cv::Point2f> new_img_corners(4);

        cv::perspectiveTransform( img_corners, new_img_corners, homography_matrix);

        int height = gray_image_dst.rows,
                width = gray_image_dst.cols;

        for (int i = 0 ; i < 4 ; i++){
            if ( int(ceil(new_img_corners[i].x)) > width )
                width = int(ceil(new_img_corners[i].x));
            if ( int(ceil(new_img_corners[i].y)) > height )
                height = int(ceil(new_img_corners[i].y));
        }

        if ( ( ! checkHomographyConsistency( new_img_corners ) ) ||
             ( width > 4 * image_to_warp.cols ) ||
             ( height > 4 * image_to_warp.rows )  ) {
            // ----------------------------------------------------------------------
            // DEBUG
            if ( DEBUG_HOMOGRAPHY )
                std::cout << "Projecao errada**************************" << std::endl;
            // ----------------------------------------------------------------------
            image_result = image_fixed.clone();
            return false;
        }

        // ----------------------------------------------------------------------
        // DEBUG
        if ( DEBUG_HOMOGRAPHY )
            std::cout<< "height: " << height << std::endl
                     << "width: " << width << std::endl;
        // ----------------------------------------------------------------------

        cv::Mat image_to_warp_mask = cv::Mat(image_to_warp.rows, image_to_warp.cols, image_to_warp.type(), cv::Scalar::all(255)),
                image_to_warp_mask_homography ;


        // Use the Homography Matrix to warp the images
        cv::warpPerspective( image_to_warp , image_result , homography_matrix , cv::Size(width, height) );
        cv::warpPerspective( image_to_warp_mask , image_to_warp_mask_homography , homography_matrix , cv::Size(width, height) );
        image_result_mask = cv::Mat(image_to_warp_mask_homography.rows, image_to_warp_mask_homography.cols, image_to_warp_mask_homography.type(), cv::Scalar::all(0));
        image_fixed_mask.copyTo(image_result_mask(cv::Rect(0,0,image_fixed_mask.cols,image_fixed_mask.rows)));

        //        imshow("image_fixed", image_fixed_mask);
        //        imshow("image_to_warp", image_to_warp_mask_homography);
        //        cv::waitKey(0);
        //        cv::destroyAllWindows();

        cv::Mat A = image_fixed_mask.clone(),
                B = image_to_warp_mask_homography.clone() ;

        B = B ( cv::Rect(0,0,image_fixed_mask.cols,image_fixed_mask.rows));

        cv::Mat AB = A & B ,
                B_A = B - A;

        cv::morphologyEx( AB , AB , cv::MORPH_DILATE ,
                          cv::getStructuringElement( cv::MORPH_RECT , cv::Size(3, 3) ) );
        cv::morphologyEx( B_A , B_A , cv::MORPH_DILATE ,
                          cv::getStructuringElement( cv::MORPH_RECT , cv::Size(3, 3) ) );

        //        imshow("AB", AB);
        //        imshow("B_A", B_A);
        //        cv::waitKey(0);
        //        cv::destroyAllWindows();

        AB = AB & B_A ;

        image_result_mask = image_result_mask | image_to_warp_mask_homography;

        image_fixed.copyTo( image_result(cv::Rect(0,0,image_fixed_mask.cols,image_fixed_mask.rows)) , image_fixed_mask );

        image_result = image_result ( cv::Rect(0,0,image_to_warp.cols,image_to_warp.rows) );
        image_result_mask = image_result_mask ( cv::Rect(0,0,image_to_warp.cols,image_to_warp.rows) );
        AB = AB ( cv::Rect(0,0,image_to_warp.cols,image_to_warp.rows) );


        //        imshow("AB", AB);
        //        cv::waitKey(0);
        //        cv::destroyAllWindows();

        cv::cvtColor( AB , AB , CV_BGR2GRAY );

        cv::inpaint(image_result, AB, image_result, 1, cv::INPAINT_TELEA);

        //        imshow("Inpaint", image_result);
        //        cv::waitKey(0);
        //        cv::destroyAllWindows();

        return true;
    }
}


/**
 * @brief Function that warps the image_to_warp with the image image_fixed and save the result into de image_result.
 *
 * @param image_to_warp - image to warp.
 * @param image_fixed - image fixed that will be in the first plane.
 * @param image_fixed_mask - mask of the fixed mask.
 * @param image_result - object to save the result image.
 * @param image_result_mask - objecto to save the mask of the result image.
 *
 * @return
 *      \c bool \b true  - if the corner consistency is maintained after application of the homography matrix found to warp the image. It also returns
 *              false if is not found a homography matrix between the images. \n
 *      \c bool \b false - if the consistency is not maintained after application of the homography matrix found to warp the image. In this case the
 *              imageResult is a simple copy of the imageFixed.
 * @author Michel Melo da Silva
 * @date 20/04/2016
 */
bool warpMask( const cv::Mat &image_to_warp, const cv::Mat &image_fixed , const cv::Mat &image_fixed_mask , cv::Mat &image_result , cv::Mat &image_result_mask )
{
    cv::Mat gray_image_src ,
            gray_image_dst ;

    // Convert to Grayscale
    cv::cvtColor( image_fixed, gray_image_dst, CV_BGR2GRAY );
    cv::cvtColor( image_to_warp, gray_image_src, CV_BGR2GRAY );
    if( !gray_image_src.data || !gray_image_dst.data ) {
        std::cout<< " --(!) ERROR: Could not convert images to Grayscale!" << std::endl;
        return false;
    } else {
        cv::Mat homography_matrix;
        if(!findHomographyMatrix(gray_image_src, gray_image_dst, homography_matrix)){
            image_result = image_fixed.clone();
            return false;
        }

        //-- Get the corners from the imageSrc
        std::vector<cv::Point2f> img_corners(4);
        img_corners[0] = cv::Point( 0                    , 0                    );
        img_corners[1] = cv::Point( gray_image_src.cols  , 0                    );
        img_corners[2] = cv::Point( 0                    , gray_image_src.rows  );
        img_corners[3] = cv::Point( gray_image_src.cols  , gray_image_src.rows  );
        std::vector<cv::Point2f> new_img_corners(4);

        cv::perspectiveTransform( img_corners, new_img_corners, homography_matrix);

        int height = gray_image_dst.rows,
                width = gray_image_dst.cols;

        for (int i = 0 ; i < 4 ; i++){
            if ( int(ceil(new_img_corners[i].x)) > width )
                width = int(ceil(new_img_corners[i].x));
            if ( int(ceil(new_img_corners[i].y)) > height )
                height = int(ceil(new_img_corners[i].y));
        }

        if ( ( ! checkHomographyConsistency( new_img_corners ) ) ||
             ( width > 4 * image_to_warp.cols ) ||
             ( height > 4 * image_to_warp.rows )  ) {
            // ----------------------------------------------------------------------
            // DEBUG
            if ( DEBUG_HOMOGRAPHY )
                std::cout << "Projecao errada**************************" << std::endl;
            // ----------------------------------------------------------------------
            image_result = image_fixed.clone();
            return false;
        }

        // ----------------------------------------------------------------------
        // DEBUG
        if ( DEBUG_HOMOGRAPHY )
            std::cout<< "height: " << height << std::endl
                     << "width: " << width << std::endl;
        // ----------------------------------------------------------------------

        cv::Mat image_to_warp_mask = cv::Mat(image_to_warp.rows, image_to_warp.cols, image_to_warp.type(), cv::Scalar::all(255)),
                image_to_warp_mask_homography ;


        // Use the Homography Matrix to warp the images
        cv::warpPerspective( image_to_warp , image_result , homography_matrix , cv::Size(width, height) );
        cv::warpPerspective( image_to_warp_mask , image_to_warp_mask_homography , homography_matrix , cv::Size(width, height) );
        image_result_mask = cv::Mat(image_to_warp_mask_homography.rows, image_to_warp_mask_homography.cols, image_to_warp_mask_homography.type(), cv::Scalar::all(0));
        image_fixed_mask.copyTo(image_result_mask(cv::Rect(0,0,image_fixed_mask.cols,image_fixed_mask.rows)));
        image_result_mask = image_result_mask | image_to_warp_mask_homography;

        image_fixed.copyTo( image_result(cv::Rect(0,0,image_fixed_mask.cols,image_fixed_mask.rows)) , image_fixed_mask );

        cv::imshow("warp_image", image_result);
        cv::waitKey(0);

        return true;
    }
}

/**
 * @brief Function that warps the image_to_warp with the image image_fixed and save the result into de image_result.
 *
 * @param image_to_warp - image to warp.
 * @param image_fixed - image fixed that will be in the first plane.
 * @param image_result - object to save the result image.
 *
 * @return
 *      \c bool \b true  - if the corner consistency is maintained after application of the homography matrix found to warp the image. It also returns
 *              false if is not found a homography matrix between the images. \n
 *      \c bool \b false - if the consistency is not maintained after application of the homography matrix found to warp the image. In this case the
 *              imageResult is a simple copy of the imageFixed.
 *
 * @author Michel Melo da Silva
 * @date 14/11/2015
 */
bool warp( const cv::Mat &image_to_warp, const cv::Mat &image_fixed , cv::Mat &image_result )
{
    cv::Mat gray_image_src ,
            gray_image_dst ;

    // Convert to Grayscale
    cv::cvtColor( image_fixed, gray_image_dst, CV_BGR2GRAY );
    cv::cvtColor( image_to_warp, gray_image_src, CV_BGR2GRAY );
    if( !gray_image_src.data || !gray_image_dst.data ) {
        std::cout<< " --(!) ERROR: Could not convert images to Grayscale!" << std::endl;
        return false;
    } else {
        cv::Mat homography_matrix;
        if(!findHomographyMatrix(gray_image_src, gray_image_dst, homography_matrix)){
            image_result = image_fixed.clone();
            return false;
        }

        //-- Get the corners from the imageSrc
        std::vector<cv::Point2f> img_corners(4);
        img_corners[0] = cv::Point( 0                    , 0                     );
        img_corners[1] = cv::Point( gray_image_src.cols  , 0                     );
        img_corners[2] = cv::Point( 0                    , gray_image_src.rows    );
        img_corners[3] = cv::Point( gray_image_src.cols  , gray_image_src.rows    );
        std::vector<cv::Point2f> new_img_corners(4);

        cv::perspectiveTransform( img_corners, new_img_corners, homography_matrix);

        int height = gray_image_dst.rows,
                width = gray_image_dst.cols;

        for (int i = 0 ; i < 4 ; i++){
            if ( int(ceil(new_img_corners[i].x)) > width )
                width = int(ceil(new_img_corners[i].x));
            if ( int(ceil(new_img_corners[i].y)) > height )
                height = int(ceil(new_img_corners[i].y));
        }

        if ( ( ! checkHomographyConsistency( new_img_corners ) ) ||
             ( width > 4 * gray_image_dst.cols ) ||
             ( height > 4 * gray_image_dst.rows )  ) {
            // ----------------------------------------------------------------------
            // DEBUG
            if ( DEBUG_HOMOGRAPHY )
                std::cout << "Projecao errada**************************" << std::endl;
            // ----------------------------------------------------------------------
            image_result = image_fixed.clone();
            return false;
        }

        // ----------------------------------------------------------------------
        // DEBUG
        if ( DEBUG_HOMOGRAPHY )
            std::cout<< "height: " << height << std::endl
                     << "width: " << width << std::endl;
        // ----------------------------------------------------------------------

        // Use the Homography Matrix to warp the images
        cv::warpPerspective( image_to_warp , image_result , homography_matrix , cv::Size(width, height) );

        cv::Mat half = cv::Mat(image_result,cv::Rect(0,0,image_fixed.cols,image_fixed.rows)) ,
                img_ad_threshold = cv::Mat(image_fixed.rows, image_fixed.cols, CV_8UC1) ,
                image_fixed_gray;

        cv::cvtColor( image_fixed, image_fixed_gray, CV_BGR2GRAY );

        cv::GaussianBlur( image_fixed_gray, image_fixed_gray, cv::Size( 1, 1 ), 0, 0 );
        cv::threshold(image_fixed_gray, img_ad_threshold, 1, 255, CV_THRESH_BINARY);

        image_fixed.copyTo(half, img_ad_threshold);
        return true;
    }
}

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
bool imageReconstruction ( const cv::Mat &image , const int index , const EXPERIMENT &experiment_settings , const cv::Rect &frame_boundaries , cv::Mat &reconstructed_image ){

    cv::VideoCapture video ( experiment_settings.original_video_filename );

    if ( !video.isOpened() ) {
        std::cout << " --(!) ERROR: Can not open the original video \"" << experiment_settings.original_video_filename << "\"." << std::endl;
        exit(-5);
    }

    //cv::namedWindow("Reconstruction");

    cv::Mat result = image.clone();

    int num_frames = int(video.get(CV_CAP_PROP_FRAME_COUNT)) ,
            min_index = std::max(index - NUM_MAX_IMAGES_TO_RECONSTRUCT, 0) ,
            max_index = std::min(index + NUM_MAX_IMAGES_TO_RECONSTRUCT, num_frames) ;

    std::vector<cv::Mat> frame_buffer(max_index-min_index+1);

    video.set( CV_CAP_PROP_POS_FRAMES, min_index);

    for ( unsigned int i = 0 ; i < frame_buffer.size() ; i++ )
        video >> frame_buffer[i];

    for ( int i = 1 ; i <= NUM_MAX_IMAGES_TO_RECONSTRUCT ; i++ ) {

        reconstructed_image = result.clone();

        warp( frame_buffer[NUM_MAX_IMAGES_TO_RECONSTRUCT-i] , reconstructed_image , result );
        reconstructed_image = result.clone();
        warp( frame_buffer[NUM_MAX_IMAGES_TO_RECONSTRUCT+i] , reconstructed_image , result );
        reconstructed_image = result.clone();
        if ( DEBUG_RECONSTRUCTION ){
            cv::rectangle( result , frame_boundaries , cv::Scalar(0,255,0) , 2 ) ;
        }
        EXECUTE_VIEW;

        if ( checkImageBoundaries( reconstructed_image , frame_boundaries ) ) {
            //EXECUTE_VIEW;
            // ---------------------------------------------------------------------------
            // DEBUG
            if ( DEBUG_RECONSTRUCTION )
                std::cout << "Done with reconstruction using " << i << " frames." << std::endl;
            // ---------------------------------------------------------------------------
            video.release();
            return true;
        }
    }
    //EXECUTE_VIEW;
    video.release();

    return false;
}

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
bool reconstructImage ( const cv::Mat &image , const cv::Mat &homography_matrix , const int index , const EXPERIMENT &experiment_settings ,
                        const cv::Rect &drop_boundaries, const cv::Rect &frame_boundaries , cv::Mat &reconstructed_image ){

    cv::VideoCapture video ( experiment_settings.original_video_filename );

    if ( !video.isOpened() ) {
        std::cout << " --(!) ERROR: Can not open the original video \"" << experiment_settings.original_video_filename << "\"." << std::endl;
        exit(-5);
    }

    //cv::namedWindow("Reconstruction");

    cv::Mat image_homography;
    applyHomographyMatrix( image , homography_matrix , image_homography );

    cv::Mat result = image_homography.clone() ,
            image_fixed_mask = cv::Mat(image.rows, image.cols, image_homography.type(), cv::Scalar::all(255)),
            reconstructed_image_mask ,
            result_mask ;

    applyHomographyMatrix( image_fixed_mask , homography_matrix , result_mask );

    int num_frames = int(video.get(CV_CAP_PROP_FRAME_COUNT)) ,
            min_index = index - NUM_MAX_IMAGES_TO_RECONSTRUCT ,
            max_index = index + NUM_MAX_IMAGES_TO_RECONSTRUCT ;

    //Setting the reconstruction type
    ReconstructionType reconstruction_type = PRE_AND_POS;
    if (min_index < 0){
        min_index = 0;
        reconstruction_type = ONLY_POS;
    } else if (max_index > num_frames){
        max_index = num_frames;
        reconstruction_type = ONLY_PRE;
    }

    std::vector<cv::Mat> frame_buffer(max_index-min_index+1);

    video.set( CV_CAP_PROP_POS_FRAMES, min_index);

    for ( unsigned int i = 0 ; i < frame_buffer.size() ; i++ )
        video >> frame_buffer[i];

    if (reconstruction_type == PRE_AND_POS){
        for ( int i = 1 ; i <= NUM_MAX_IMAGES_TO_RECONSTRUCT ; i++ ) {
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

            warpMaskCrop( frame_buffer[NUM_MAX_IMAGES_TO_RECONSTRUCT-i] , reconstructed_image , reconstructed_image_mask , result , result_mask );
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

//            if ( DEBUG_RECONSTRUCTION ){
//                cv::rectangle( result , frame_boundaries , cv::Scalar(0,255,0) , 2 ) ;
//                cv::rectangle( result , drop_boundaries , cv::Scalar(0,0,255) , 2 ) ;
//                //std::string str = SSTR("/home/washingtonramos/frame_224_" << (i*2) << ".png");
//                //cv::imwrite(str, result);
//            }
//            EXECUTE_VIEW;

            warpMaskCrop( frame_buffer[NUM_MAX_IMAGES_TO_RECONSTRUCT+i] , reconstructed_image , reconstructed_image_mask , result , result_mask );
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

//            if ( DEBUG_RECONSTRUCTION ){
//                cv::rectangle( result , frame_boundaries , cv::Scalar(0,255,0) , 2 ) ;
//                cv::rectangle( result , drop_boundaries , cv::Scalar(0,0,255) , 2 ) ;
//                //std::string str = SSTR("/home/washingtonramos/frame_224_" << (i*2+1) << ".png");
//                //cv::imwrite(str, result);
//            }
//            EXECUTE_VIEW;

            if ( checkImageBoundariesMask( reconstructed_image_mask , frame_boundaries ) ) {
                //EXECUTE_VIEW;
                // ---------------------------------------------------------------------------
                // DEBUG
                //if ( DEBUG_RECONSTRUCTION )
                //   std::cout << "Done with reconstruction using " << i << " frames." << std::endl;
                // ---------------------------------------------------------------------------
                video.release();
                return true;
            }
        }
    } else if(reconstruction_type == ONLY_PRE) {
        for ( int i = 0 ; i < NUM_MAX_IMAGES_TO_RECONSTRUCT ; i++ ) {
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

            warpMaskCrop( frame_buffer[i] , reconstructed_image , reconstructed_image_mask , result , result_mask );
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

//            if ( DEBUG_RECONSTRUCTION ){
//                cv::rectangle( result , frame_boundaries , cv::Scalar(0,255,0) , 2 ) ;
//                cv::rectangle( result , drop_boundaries , cv::Scalar(255,0,0) , 2 ) ;
//            }
//            EXECUTE_VIEW;

            if ( checkImageBoundariesMask( reconstructed_image_mask , frame_boundaries ) ) {
                //EXECUTE_VIEW;
                // ---------------------------------------------------------------------------
                // DEBUG
                if ( DEBUG_RECONSTRUCTION )
                   std::cout << "Done with reconstruction using " << i << " frames." << std::endl;
                // ---------------------------------------------------------------------------
                video.release();
                return true;
            }
        }
    } else {
        for ( int i = frame_buffer.size() - NUM_MAX_IMAGES_TO_RECONSTRUCT ; i < frame_buffer.size() ; i++ ) {
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

            warpMaskCrop( frame_buffer[i] , reconstructed_image , reconstructed_image_mask , result , result_mask );
            reconstructed_image = result.clone();
            reconstructed_image_mask = result_mask.clone();

//            if ( DEBUG_RECONSTRUCTION ){
//                cv::rectangle( result , frame_boundaries , cv::Scalar(0,255,0) , 2 ) ;
//                cv::rectangle( result , drop_boundaries , cv::Scalar(255,0,0) , 2 ) ;
//            }
//            EXECUTE_VIEW;

            if ( checkImageBoundariesMask( reconstructed_image_mask , frame_boundaries ) ) {
                //EXECUTE_VIEW;
                // ---------------------------------------------------------------------------
                // DEBUG
                if ( DEBUG_RECONSTRUCTION )
                   std::cout << "Done with reconstruction using " << i << " frames." << std::endl;
                // ---------------------------------------------------------------------------
                video.release();
                return true;
            }
        }
    }

    //EXECUTE_VIEW;
    video.release();

    return false;
}
