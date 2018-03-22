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
 * @file sequence_processing.cpp
 *
 * Functions related with sequence processing, such as find intermediate homography, get transition weight, select new frame.
 *
 * Calculate intermediate homography matrix. Calculate matrix operations root and pow. Calculate frame weight. Get transition weight. Select new frame from original video.
 *
 */

#include "definitions/experiment_struct.h"

#include "headers/sequence_processing.h"
#include "headers/homography.h"

/**
 * @brief Function that calculates the n-ith root of a matrix. Uses the Armadillo lib.
 *
 * @see http://arma.sourceforge.net/
 *
 * @param matrix - matrix to calculate the \c root-th root.
 * @param root - root index.
 * @param matrix_result - object to save the result of the matrix root.
 *
 * @return
 *      \c bool \b true - if the root can be found. \n
 *      \c bool \b false - if the root can not be found.
 *
 * @author Michel Melo da Silva
 * @date 05/05/2016
 */
bool matrixRoot ( cv::Mat &matrix, int root, cv::Mat &matrix_result ) {
    arma::mat arma_mat(reinterpret_cast<double*>(matrix.data), matrix.rows, matrix.cols );
    arma_mat = arma_mat.t();

    int iterations = std::log(root)/std::log(2);
    arma::cx_mat aux (arma_mat, arma::zeros(arma_mat.n_rows, arma_mat.n_cols)) ;

    arma::cx_mat arma_mat_check_shur ;
    arma::vec arma_vec_check_singluar;

    for (int i = 0; i < iterations; ++i) {
        // Check if the matrix is singular and the schur decomposition.
        if ( !arma::schur( arma_mat_check_shur , aux ) || !arma::svd( arma_vec_check_singluar , aux ) ){
            return false;
        }
        aux = arma::sqrtmat(aux);
    }

    arma_mat = real(aux).t();

    matrix_result = cv::Mat( arma_mat.n_rows, arma_mat.n_cols, CV_64F, arma_mat.memptr() ).clone();

    return true;
}

/**
 * @brief Function that calculates the n-ith pow of a matrix. Uses the Armadillo lib.
 *
 * @see http://arma.sourceforge.net/
 *
 * @param matrix - matrix to calculate the \c pow-th pow.
 * @param pow - pow index.
 * @param matrix_result - object to save the result of the matrix pow.
 *
 * @return \c void
 *
 * @author Washington Luis de Souza Ramos
 * @date 06/04/2016
 */
void matrixPow ( cv::Mat &matrix, int pow, cv::Mat &matrix_result ) {

    matrix_result = cv::Mat::eye(matrix.size(), matrix.type());// matrix.clone();
    cv::Mat base = matrix.clone();

    while(pow)
    {
        if(pow & 1)
            matrix_result *= base;
        pow >>= 1;
        base *= base;
    }

}

/**
 * @brief Function that calculates the homography matrix that leave the frame_i to the intermediate plan between the plans of frame_master_pre and frame_master_pre
 *          with relation of the distance between them.
 *
 * @param d - distance between the current frame and the previous master frame.
 * @param D - distance between previous master frame and the posterior master frame.
 * @param N - size of the segments.
 * @param frame_i - current frame.
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
bool findIntermediateHomographyMatrix ( const int d, const int D, const int N ,
                                        const cv::Mat &frame_i , const cv::Mat &frame_master_pre , const cv::Mat &frame_master_pos ,
                                        cv::Mat& homography_matrix_result ){

    cv::Mat homography_matrix_to_master_pre,
            homography_matrix_to_master_pos,
            H_pre_exp(3,3,CV_64F),
            H_pre(3,3,CV_64F),
            H_pos_exp(3,3,CV_64F),
            H_pos(3,3,CV_64F);

    bool bool_pre = false,
            bool_pos = false;

    int exp_pos  = (int) round ( double(d)*(double(2*N)/double(D)) ) ,
            root = 2 * N,
            exp_pre = root - exp_pos;

    if ( findHomographyMatrix ( frame_i, frame_master_pre, homography_matrix_to_master_pre ) ) {
        matrixPow(homography_matrix_to_master_pre, exp_pre, H_pre_exp);
        bool_pre = matrixRoot(H_pre_exp, root, H_pre);
    }

    if ( findHomographyMatrix ( frame_i, frame_master_pos, homography_matrix_to_master_pos ) ) {
        matrixPow(homography_matrix_to_master_pos, exp_pos, H_pos_exp);
        bool_pos = matrixRoot(H_pos_exp, root, H_pos);
    }

    // raiz "root" de homography_matrix_to_master_pre elevado a "exp" ) vezes ( raiz "root" de homography_matrix_to_master_pre elevado a "exp_pos" );
    if ( bool_pre && bool_pos )
        homography_matrix_result = H_pre * H_pos;
    else {
        if ( bool_pre )
            homography_matrix_result = H_pre;
        else {
            if ( bool_pos )
                homography_matrix_result = H_pos;
        }
    }

    return bool_pos || bool_pre ;
}

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
bool findIntermediateHomographyMatrix ( const int d, const int D, const int N , const cv::Mat &frame_i ,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pre,
                                        const std::vector<cv::KeyPoint> &keypoints_master_pos,
                                        const cv::Mat &descriptors_master_pre,
                                        const cv::Mat &descriptors_master_pos,
                                        cv::Mat& homography_matrix_result ){

    std::vector<cv::KeyPoint> keypoints_frame_i;
    cv::Mat descriptors_frame_i, ransac_mask;//TODO: Remove useless ransac_mask (create a new findHomography function)

    //Load the descriptors of the frame i
    getKeypointsAndDescriptors(frame_i, keypoints_frame_i, descriptors_frame_i);

    if(keypoints_master_pre.empty() && descriptors_master_pre.empty()){
        return findHomographyMatrix(keypoints_frame_i, keypoints_master_pos,
                                                     descriptors_frame_i, descriptors_master_pos,
                                                     homography_matrix_result);
    } else if(keypoints_master_pos.empty() && descriptors_master_pos.empty()){
        return findHomographyMatrix(keypoints_frame_i, keypoints_master_pre,
                                                     descriptors_frame_i, descriptors_master_pre,
                                                     homography_matrix_result);
    }

    cv::Mat homography_matrix_to_master_pre,
            homography_matrix_to_master_pos,
            H_pre_exp(3,3,CV_64F),
            H_pre(3,3,CV_64F),
            H_pos_exp(3,3,CV_64F),
            H_pos(3,3,CV_64F);

    bool bool_pre = false,
            bool_pos = false;

    int exp_pos  = (int) round ( double(d)*(double(2*N)/double(D)) ) ,
            root = 2 * N,
            exp_pre = root - exp_pos;

    if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pre, descriptors_frame_i,
                              descriptors_master_pre, homography_matrix_to_master_pre, ransac_mask)) {
        matrixPow(homography_matrix_to_master_pre, exp_pre, H_pre_exp);
        bool_pre = matrixRoot(H_pre_exp, root, H_pre);
    }

    if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pos, descriptors_frame_i,
                              descriptors_master_pos, homography_matrix_to_master_pos, ransac_mask)) {
        matrixPow(homography_matrix_to_master_pos, exp_pos, H_pos_exp);
        bool_pos = matrixRoot(H_pos_exp, root, H_pos);
    }

    // raiz "root" de homography_matrix_to_master_pre elevado a "exp" ) vezes ( raiz "root" de homography_matrix_to_master_pre elevado a "exp_pos" );
    if ( bool_pre && bool_pos )
        homography_matrix_result = H_pre * H_pos;
    else if ( bool_pre )
        homography_matrix_result = H_pre;
    else if ( bool_pos )
        homography_matrix_result = H_pos;
    else return false;

    return true;
}

/**
 * @brief Function that calculates the homography matrix that leave the frame_i to the intermediate plan between
 *          the frames related to the previous and posterior master, with respect to the distance between them.
 *
 * @param s - frame shift value between the previous master frame and the current frame.
 * @param S - frame shift between previous master frame and the posterior master frame.
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
                                        cv::Mat& homography_matrix_result){

    std::vector<cv::KeyPoint> keypoints_frame_i;
    cv::Mat descriptors_frame_i, ransac_mask;//TODO: Remove useless ransac_mask (create a new findHomography function)

    //Load the descriptors of the frame i
    getKeypointsAndDescriptors(frame_i, keypoints_frame_i, descriptors_frame_i);

    if((keypoints_master_pre.empty() && descriptors_master_pre.empty()) || s == S){
        return findHomographyMatrix(keypoints_frame_i, keypoints_master_pos,
                                                     descriptors_frame_i, descriptors_master_pos,
                                                     homography_matrix_result);
    } else if((keypoints_master_pos.empty() && descriptors_master_pos.empty()) || s == 0.f){
        return findHomographyMatrix(keypoints_frame_i, keypoints_master_pre,
                                                     descriptors_frame_i, descriptors_master_pre,
                                                     homography_matrix_result);
    }

    cv::Mat homography_matrix_to_master_pre,
            homography_matrix_to_master_pos,
            H_pre_exp(3,3,CV_64F),
            H_pre(3,3,CV_64F),
            H_pos_exp(3,3,CV_64F),
            H_pos(3,3,CV_64F);

    bool bool_pre = false,
            bool_pos = false;

    float min_s = std::min(s, S-s);//Ensure the maximum root
    int root = int(pow(2, ceil(log2(S/min_s)))),
            exp_pos  = (int) round (s/S*root),
            exp_pre = root - exp_pos;

    if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pre, descriptors_frame_i,
                              descriptors_master_pre, homography_matrix_to_master_pre, ransac_mask)) {
        matrixPow(homography_matrix_to_master_pre, exp_pre, H_pre_exp);
        bool_pre = matrixRoot(H_pre_exp, root, H_pre);
    }

    if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pos, descriptors_frame_i,
                              descriptors_master_pos, homography_matrix_to_master_pos, ransac_mask)) {
        matrixPow(homography_matrix_to_master_pos, exp_pos, H_pos_exp);
        bool_pos = matrixRoot(H_pos_exp, root, H_pos);
    }

    // raiz "root" de homography_matrix_to_master_pre elevado a "exp" ) vezes ( raiz "root" de homography_matrix_to_master_pre elevado a "exp_pos" );
    if ( bool_pre && bool_pos )
        homography_matrix_result = H_pre * H_pos;
    else if ( bool_pre )
        homography_matrix_result = H_pre;
    else if ( bool_pos )
        homography_matrix_result = H_pos;
    else return false;

    return true;
}

/**
 * @brief Function that calculates Gaussian value in the position index.
 *
 * @param index - index in the Gaussian function.
 *
 * @return \c double - value of the Gaussian function int the position index.
 *
 * @author Michel Melo da Silva
 * @date 20/04/2016
 */
double gaussianValue(double index)
{
    double mean = 100;
    double dp = 2*mean*0.05;
    index = index * mean;

    return (double) exp( (-1.0 * pow((int)index - (int)mean, 2)) / (2 * pow(dp, 2)) );
}

/**
 * @brief Function that calculates the frame weigth used to chose a new frame to compose the video.
 *
 * @param ransac_inliers_previous - RANSAC inliers between the frame and the previous frame used.
 * @param ransac_inliers_posterior - RANSAC inliers between the frame and the posterior frame used.
 * @param area_ratio - percentage of area into the crop area that the frame occupy after apply the intermediate homography matrix.
 * @param semantic_weight - semantic information contained in the transition from previous index, passing by index, up to posterior index.
 *
 * @return \c double - value of the frame weight.
 *
 * @author Michel Melo da Silva
 * @date 20/04/2016
 */
float frameWeight ( const int ransac_inliers_previous , const int ransac_inliers_posterior , const double area_ratio ,  const double semantic_weight ) {
    return (ransac_inliers_posterior + ransac_inliers_previous) * ( gaussianValue(area_ratio) ) * ( 0.5f + semantic_weight );
}

/**
 * @brief Function that get the semantic information of the transition from the frame_src to the frame_dst calculted by
 *          MATLAB and saved in a CSV file.
 *
 * @param index_previous - index of the first frame of the first transition.
 * @param index_current - index of the last frame of the first transition and first frame of the second transition.
 * @param index_posterior - index of the second frame of the second transition.
 * @param experiment_settings - struct with the experiment settings used to open the CSV file.
 *
 * @return \c double - value of the semantic information about the transition.
 *
 * @author Michel Melo da Silva
 * @date 30/04/2016
 */
double semantic_weight ( const int index_previous , const int index_current , const int index_posterior , const EXPERIMENT& experiment_settings ) {

    return getSemanticCost(experiment_settings, index_previous , index_current ) + getSemanticCost(experiment_settings, index_current, index_posterior) ;
}

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
int selectNewFrame ( const int d, const int D, const int N , const int index , const int index_previous , const int index_posterior ,
                     const std::vector<cv::KeyPoint> &keypoints_master_pre, const std::vector<cv::KeyPoint> &keypoints_master_pos,
                     const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos, const cv::Rect& crop_area ,
                     const EXPERIMENT& experiment_settings , cv::Mat& new_frame ) {

    cv::VideoCapture video ( experiment_settings.original_video_filename );

    if ( !video.isOpened() ) {
        std::cout << " --(!) ERROR: Can not open the original video \"" << experiment_settings.original_video_filename << "\"." << std::endl;
        exit(-5);
    }

    cv::Mat image_index_previous ,
            image_index_posterior ,
            current_frame ,
            homography_matrix ,
            ransac_mask ;

    int index_previous_process = index_previous ,
            index_posterior_process = index_posterior ,
            new_index = index_previous_process ;

    if ( index_posterior-index_previous > 100 ) {
        index_previous_process = std::max(index_previous, index_posterior - 100);
        index_posterior_process = std::min(index_posterior, index_previous_process + 100);
    }

    video.set(CV_CAP_PROP_POS_FRAMES, index_posterior_process);
    video.read(image_index_posterior);
    video.set(CV_CAP_PROP_POS_FRAMES, index_previous_process);
    video.read(image_index_previous);

    double weight_max = 0.0f;

    // In this moment the read head is at the position index_previous plus one.
    for ( int i = index_previous_process+1 ; i < index_posterior_process ; i++ ) {

        if ( i == index)
            continue;

        int inliers_previous = 0 ,
                inliers_posterior = 0 ;

        double area_ratio = 0.0f,
                current_weight = 0.0f,
                semantic_cost = 0.0f;

        video >> current_frame ;

        if ( findHomographyMatrix(current_frame, image_index_previous, homography_matrix, ransac_mask) )
            inliers_previous = cv::sum(ransac_mask)[0];

        if ( findHomographyMatrix(current_frame, image_index_posterior, homography_matrix, ransac_mask) )
            inliers_posterior = cv::sum(ransac_mask)[0];

        if ( findIntermediateHomographyMatrix( d, D, N, current_frame, keypoints_master_pre, keypoints_master_pos,
                                               descriptors_master_pre, descriptors_master_pos, homography_matrix ) ) {

            area_ratio = 1 - getAreaRatio( current_frame , homography_matrix , crop_area ) ;

            if ( area_ratio < MAXIMUM_AREA_ALLOWED )
                area_ratio = 0.0f;

        }

        semantic_cost = semantic_weight( index_previous_process , i , index_posterior_process , experiment_settings ) ;

        current_weight = frameWeight( inliers_previous, inliers_posterior , area_ratio ,  semantic_cost );

        if ( current_weight > weight_max ) {
            new_frame = current_frame.clone();
            weight_max = current_weight;
            new_index = i;
        }

        //        std::cout << "Previous: " << index_previous << std::endl;
        //        std::cout << "Posterior: " << index_posterior << std::endl;
        //        std::cout << "Index: " << index << std::endl;
        //        std::cout << "Inliers previous: " << inliers_previous << std::endl;
        //        std::cout << "Inliers posterior: " << inliers_posterior << std::endl;
        //        std::cout << "Area ratio: " << area_ratio << std::endl;
        //        std::cout << "Area ratio Gaussian value: " << gaussianValue(area_ratio) << std::endl;
        //        std::cout << "Semantic cost: " << semantic_cost << std::endl;
        //        std::cout << "current cost: " << current_weight << std::endl;

        //        cv::rectangle( result , crop_area , cv::Scalar(0,255,0) , 2 ) ;
        //        cv::imshow("nova_video", result);
        //        cv::waitKey(0);
    }

    return new_index ;

}

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
int selectNewFrame ( const float s, const float S, const int index , const int index_previous , const int index_posterior ,
                     const std::vector<cv::KeyPoint> &keypoints_master_pre, const std::vector<cv::KeyPoint> &keypoints_master_pos,
                     const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos, const cv::Rect& crop_area ,
                     const EXPERIMENT& experiment_settings , cv::Mat& new_frame ) {

    cv::VideoCapture video ( experiment_settings.original_video_filename );

    if ( !video.isOpened() ) {
        std::cout << " --(!) ERROR: Can not open the original video \"" << experiment_settings.original_video_filename << "\"." << std::endl;
        exit(-5);
    }

    cv::Mat image_index_previous ,
            image_index_posterior ,
            current_frame ,
            homography_matrix ,
            result ;

    video.set(CV_CAP_PROP_POS_FRAMES, index_posterior);
    video.read(image_index_posterior);
    video.set(CV_CAP_PROP_POS_FRAMES, index_previous);
    video.read(image_index_previous);

    double weight_max = 0.0d;

    int index_previous_process = index_previous ,
            index_posterior_process = index_posterior ,
            new_index = index_previous_process ;

    if ( index_posterior-index_previous > 100 ) {
        index_previous_process = std::max(index_previous, index_posterior - 100);
        index_posterior_process = std::min(index_posterior, index_previous + 100);
        video.set(CV_CAP_PROP_POS_FRAMES, index_previous_process);
    }

    // In this moment the read head is at the position index_previous plus one.
    for ( int i = index_previous_process+1 ; i < index_posterior_process ; i++ ) {

        if ( i == index)
            continue;

        int inliers_previous = 0 ,
                inliers_posterior = 0 ;

        double area_ratio = 0.0d ,
                current_weight = 0.0d ,
                semantic_cost = 0.0d ;

        video >> current_frame ;

        std::vector<cv::KeyPoint> keypoints_frame_i;
        cv::Mat descriptors_frame_i, ransac_mask;//TODO: Remove useless ransac_mask (create a new findHomography function)

        //Load the descriptors of the frame i
        getKeypointsAndDescriptors(current_frame, keypoints_frame_i, descriptors_frame_i);

        if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pre, descriptors_frame_i,
                                  descriptors_master_pre, homography_matrix, ransac_mask))
            inliers_previous = cv::sum(ransac_mask)[0];

        if (findHomographyMatrix (keypoints_frame_i, keypoints_master_pos, descriptors_frame_i,
                                  descriptors_master_pos, homography_matrix, ransac_mask))
            inliers_posterior = cv::sum(ransac_mask)[0];

        if (findIntermediateHomographyMatrix( s , S , current_frame, keypoints_master_pre, keypoints_master_pos,
                                               descriptors_master_pre, descriptors_master_pos, homography_matrix )) {

            applyHomographyMatrix( current_frame , homography_matrix , result );

            area_ratio = 1 - getAreaRatio( current_frame , homography_matrix , crop_area ) ;

            if ( area_ratio < MAXIMUM_AREA_ALLOWED )
                area_ratio = 0.0d;

        }

        semantic_cost = semantic_weight( index_previous , i , index_posterior , experiment_settings ) ;

        current_weight = frameWeight( inliers_previous, inliers_posterior , area_ratio ,  semantic_cost );

        if ( current_weight > weight_max ) {
            new_frame = current_frame.clone();
            weight_max = current_weight;
            new_index = i;
        }

    }

    return new_index;
}
