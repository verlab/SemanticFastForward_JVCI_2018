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
 * @file main.cpp
 *
 * Main file that contains the main.cpp
 *
 */

#include <stdio.h>
#include <iomanip>

#include <time.h>

#include <armadillo>
#include <boost/filesystem.hpp>

#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/stitching/stitcher.hpp>

#include "definitions/experiments.h"
#include "definitions/define.h"

#include "executables/execute_commands.h"

#include "headers/homography.h"
#include "headers/sequence_processing.h"
#include "headers/master_frames.h"
#include "headers/line_and_point_operations.h"
#include "headers/image_reconstruction.h"
#include "headers/message_handler.h"

int log_number_length,
num_of_reconstructed_frames = 0 ,
num_of_dropped_frames = 0 ,
num_of_good_frames = 0 ,
num_of_fails_in_homography = 0,
saved_frames = 0;

EXPERIMENT experiment_settings;

/**
 * @brief getItFormatted - Formats the number with padding.
 * @param frame_number
 * @return
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
std::string getItFormatted(int frame_number);

/**
 * @brief writeToOutput - Writes the given image to the output (save_video and/or screen)
 * @param save_video
 * @param image
 * @param frame_number
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void writeToOutput(cv::VideoWriter& save_video, cv::Mat& image, uint frame_number);

/**
 * @brief getStableFrame - Returns a frame stabilized given the parameters.
 * @param input_frame
 * @param homography_matrix
 * @param drop_area
 * @param crop_area
 * @param frame_number
 * @param d
 * @param D
 * @param selected_frames
 * @param image_master_pre
 * @param image_master_pos
 * @param trial - The number of the current attempt
 * @param stable_frame
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void getStableFrameTemporally(cv::Mat& input_frame, cv::Mat& homography_matrix,
                    const cv::Rect& drop_area, const cv::Rect& crop_area,
                    int frame_number, int d, int D, cv::vector<int> &selected_frames,
                    const std::vector<cv::KeyPoint> &keypoints_master_pre,
                    const std::vector<cv::KeyPoint> &keypoints_master_pos, const cv::Mat &descriptors_master_pre,
                    const cv::Mat &descriptors_master_pos, int attempt, MessageHandler& msg_handler, cv::Mat& stable_frame);

/**
 * @brief getStableFrameTemporally - Returns a frame stabilized given the parameters.
 * @param input_frame
 * @param homography_matrix
 * @param drop_area
 * @param crop_area
 * @param frame_number
 * @param s - Spatial distance to the previous master (using the instability costs)
 * @param S - Spatial distance between the masters (using the instability costs)
 * @param selected_frames
 * @param image_master_pre
 * @param image_master_pos
 * @param trial - The number of the current attempt
 * @param msg_handler - The message handler
 * @param stable_frame
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void getStableFrameSpatially(cv::Mat& input_frame, cv::Mat& homography_matrix,
                    const cv::Rect &drop_area, const cv::Rect &crop_area,
                    int frame_number, float s, float S, cv::vector<int> &selected_frames,
                    const std::vector<cv::KeyPoint> &keypoints_master_pre, const std::vector<cv::KeyPoint> &keypoints_master_pos,
                    const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos,
                    int attempt, MessageHandler &msg_handler, cv::Mat& stable_frame);

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

/**
 * @brief main.cpp
 *
 * \b Usage: \n
 * < Program_name > < Settings_file > [ Range_min = 0 ] [ Range_max = num_frames ] \n\n
 * Example 1: Run VideoStabilization in the Experiment_1 processing the whole video. \n
 * -> VideoStabilization Experiment_1.xml \n
 * Example 2: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the last one. \n
 * -> VideoStabilization Experiment_1.xml 150 \n
 * Example 2: Run VideoStabilization in the Experiment_1 processing from the 150 frame until the frame 490. \n
 * -> VideoStabilization Experiment_1.xml 150 490 \n\n
 *
 * The program can \b exit with following codes: \n
 * \b -1 - Wrong number of input parameters. \n
 * \b -2 - Experiment id does not exist. \n
 * \b -3 - Can not open the accelerated video. \n
 * \b -4 - Can not create file to save the output video. \n
 * \b -5 - Can not open the video original video. \n
 * \b -6 - Can not open the CSV file with the selected frame numbers of the accelerated video. \n
 * \b -7 - Can not create directory to save the output data. \n
 * \b -8 - Can not create log text file. \n
 * \b -9 - Range limits is not well defined. \n
 * \b -10 - Can not open the CSV file with the semantic costs of the original video. \n
 * \b -11 - transition from frame_src to frame_dst larger than number of transitions describle int the file the semantic costs
 *
 * @param argc - number of parameters in argv.
 * @param argv - < Settings_file > [ Range_min = 0 ] [ Range_max = num_frames ]
 *
 */
int main( int argc , char* argv[] )
{

    if ( argc < 2 ) {
        std::cerr << " --(!) ERROR: incorrect call to program. \n Usage: " << argv[0] << " < Settings_file > [ Range_min = 0 ] [ Range_max = num_frames ]" << std::endl;
        exit(-1);
    }

    EXECUTE_INFO;

    if ( argv[1] == std::string("-h") ) {
        std::cerr << " Usage: " << argv[0] << " < Settings_file > [ Range_min = 0 ] [ Range_max = num_frames ]" << std::endl
                  << std::endl;
        exit(-2);
    }

    experiment_settings = load_experiments_settings( argv[1] );

    EXECUTE_EXPERIMENT_ID;

    cv::VideoCapture video (experiment_settings.video_filename);

    if ( !video.isOpened() ) {
        std::cerr << " --(!) ERROR: Can not open the video \"" << experiment_settings.video_filename << "\"." << std::endl;
        exit(-3);
    }

    int num_frames = (int)video.get(CV_CAP_PROP_FRAME_COUNT),
            i_master = 0 ,
            d = 0 ,
            D = 0 ,
            N = experiment_settings.segment_size ,
            range_min,
            range_max,
            video_width,
            video_height;
    if ( ! boost::filesystem::create_directory(boost::filesystem::path(experiment_settings.output_path)) ) {
        std::cerr << " --(!) ERROR: Can not create directory \"" << experiment_settings.output_path << "\" to save the output data." << std::endl;
        exit(-7);
    }

    //std::vector< std::vector<double> > instability_costs = loadInstabilityCostsFromFile(experiment_settings);
    std::vector<int> master_frames = getMasterFrames( experiment_settings , num_frames ), selected_frames;
    readSelectedFramesCSV(experiment_settings.selected_frames_filename, selected_frames);

    switch ( argc ) {
    case 2:
        range_min = 0;
        range_max = num_frames;
        break;
    case 3:
        range_min = std::atoi(argv[2]);
        range_max = num_frames;
        break;
    case 4:
        range_min = std::atoi(argv[2]);
        range_max = std::atoi(argv[3]);
        break;
    default:
        std::cerr << " --(!) ERROR: incorrect call to program. \n Usage: VideoStabilization < Settings_file > [ Range_min = 0 ] [ Range_max = num_frames ]" << std::endl;
        exit(-1);
        break;
    }

    // ----------------------------------------------------------------------
    // DEBUG
    // Print the frame masters vector in C++ vector initialized form.
    //if ( DEBUG_INTERMEDIATE_FRAME ) {
    //    std::cout << "{ " ;
    //    for ( unsigned int i = 0 ; i < master_frames.size()-1 ; i++)
    //        std::cout << master_frames[i] << " , " ;
    //    std::cout << master_frames[master_frames.size()-1] << " } ; " << std::endl;
    //}
    // ----------------------------------------------------------------------

    video_width = video.get(CV_CAP_PROP_FRAME_WIDTH);
    video_height = video.get(CV_CAP_PROP_FRAME_HEIGHT);

    // ! ATTENTION: do not exclude those rects, they are used in the execute_commands.h.
    //const cv::Rect view_area( 0, 0, video_width, video_height );
    const cv::Rect crop_area( video_width * CROP_PORTION,
                              video_height * CROP_PORTION,
                              video_width  - (2 * video_width  * CROP_PORTION),
                              video_height - (2 * video_height * CROP_PORTION));
    const cv::Rect drop_area( video_width  * DROP_PORTION,
                              video_height * DROP_PORTION,
                              video_width  - (2 * video_width * DROP_PORTION),
                              video_height - (2 * video_height * DROP_PORTION));

    cv::VideoWriter save_video;

    if ( experiment_settings.save_video_in_disk ) {
        save_video = cv::VideoWriter( experiment_settings.save_video_filename , CV_FOURCC('m','p','4','v') ,
                                      video.get(CV_CAP_PROP_FPS) , cv::Size(crop_area.width , crop_area.height) );
        if ( !save_video.isOpened() ){
            std::cerr << " --(!) ERROR: Can not create file \"" << experiment_settings.save_video_filename << "\" to save the video." << std::endl;
            exit(-4);
        }
    }

    MessageHandler msg_handler(experiment_settings.log_file_name);

    std::vector<cv::KeyPoint> keypoints_frame_pre, keypoints_frame_pos, keypoints_current_frame;

    cv::Mat image_master_pre ,
            image_master_pos ,
            current_frame ,
            descriptors_frame_pre,
            descriptors_frame_pos,
            descriptors_current_frame,
            result,
            result_cropped,
            homography_matrix;

    // ----------------------------------------------------------------------
    // VIEW
    //if ( VIEW )
    //    cv::namedWindow("Homography");
    // ----------------------------------------------------------------------

    msg_handler.reportStatus(SSTR("\n Starting the process: " << currentDateTime() << std::endl << std::endl), BOTH);
    msg_handler.reportStatus(SSTR(" Stabilizing video: \"" << experiment_settings.video_filename << "\" with " << num_frames << " frames." << std::endl), SCREEN);
    msg_handler.reportStatus(SSTR(" Range: from frame " << range_min << " up to frame " << range_max << std::endl << std::endl << "Progress: " << "0%"), SCREEN);

    std::flush(std::cout);

    log_number_length = length(num_frames);

    //Increments the i_master until it reaches the range_min
    while ( master_frames[i_master+1] < range_min ) i_master++;

    video.set( CV_CAP_PROP_POS_FRAMES , master_frames[i_master] );
    video.read( image_master_pre );
    video.set( CV_CAP_PROP_POS_FRAMES , master_frames[i_master+1] );
    video.read( image_master_pos );
    video.set( CV_CAP_PROP_POS_FRAMES , range_min );

    //Loading the descriptors of the master frames
    getKeypointsAndDescriptors(image_master_pre, keypoints_frame_pre, descriptors_frame_pre);
    getKeypointsAndDescriptors(image_master_pos, keypoints_frame_pos, descriptors_frame_pos);

    // -----------------------------------------------------------------------------------------------------------------------------------
    // LOG FILE
    msg_handler.reportStatus(SSTR(" --> Running exepriment: " << argv[1] << " | Experiment ID: " << experiment_settings.id << std::endl
                             << std::endl
                             << " --> Stabilizing video: \"" << experiment_settings.video_filename << "\" with " << num_frames << " frames." << std::endl
                             << " Range: from frame " << range_min << " up to frame " << range_max << std::endl
                             << std::endl), LOG_FILE);

    if ( experiment_settings.read_master_frames_filename.compare("") == 0 )
        msg_handler.reportStatus(SSTR(" --> Master frames calculated in the progress." << std::endl << std::endl), LOG_FILE);
    else
        msg_handler.reportStatus(SSTR(" --> Master frames loaded from: " << experiment_settings.read_master_frames_filename << std::endl << std::endl), LOG_FILE);
    // -----------------------------------------------------------------------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Process frames before the first master frame. /////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    D = master_frames[i_master];
    //S = -1;

    // Process frames before the first master frame.
    for (int i = range_min; i < master_frames[i_master]; i++) {

        video >> current_frame;

        d = D - i;
        //s = instability_costs[i][d];

        getKeypointsAndDescriptors(current_frame, keypoints_current_frame, descriptors_current_frame);

        if ( findHomographyMatrix( keypoints_current_frame, keypoints_frame_pre, descriptors_current_frame, descriptors_frame_pre, homography_matrix ) ) {

            //getStableFrameSpatially(current_frame, homography_matrix, drop_area, crop_area, i, s, S, selected_frames,
            //               keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);
            getStableFrameTemporally(current_frame, homography_matrix, drop_area, crop_area, i, d, D, selected_frames,
                           keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);

        } else {
            result = current_frame.clone();
            num_of_fails_in_homography++;
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(i) << " | [F] Failed on finding a homography matrix to the first master." << std::endl), BOTH);
        }

        ////////////////////////////////////////
        /// WRITING THE RESULT TO THE OUTPUT ///
        ////////////////////////////////////////
        if ( experiment_settings.save_video_in_disk ){
            result_cropped = result(crop_area);
            writeToOutput(save_video, result_cropped, i);
        }
        //EXECUTE_VIEW
    }

    D = master_frames[i_master+1] - master_frames[i_master];

    //Processing the first master (No homography is required)
    if ( range_min < master_frames[0] ) {
        // First master frame without homography.
        video >> current_frame;
        result = current_frame.clone();
        msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(master_frames[0]) << " | [M] Kept. [Master]" << std::endl), LOG_FILE);
        num_of_good_frames++;

        ////////////////////////////////////////
        /// WRITING THE RESULT TO THE OUTPUT ///
        ////////////////////////////////////////
        if ( experiment_settings.save_video_in_disk ){
            result_cropped = result(crop_area);
            writeToOutput(save_video, result_cropped, master_frames[0]);
        }
        //EXECUTE_VIEW
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Process frames betweeb the first and the last master frames. //////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Setting some values
    int i_min = std::max(master_frames[0]+1, range_min) ,
            i_max = std::min(master_frames[master_frames.size()-1], range_max),
            percentage = std::min(10, i_max - i_min) ,
            number = i_min + round((double)(i_max-i_min)/(double)percentage) ,
            cnt = 1;

    for (int i = i_min; i < i_max; i++){

        if(i == number) {
            std::cout << " -> " << cnt*(100/percentage) << "% ";
            cnt++;
            number = i_min + round(cnt*((i_max-i_min)/percentage));
            std::flush(std::cout);
        }

        if ( i != master_frames[i_master+1] ) {
            //////////////////////////////////
            /// THIS IS NOT A MASTER FRAME ///
            //////////////////////////////////

            d = i - master_frames[i_master];
            //s = instability_costs[i][d];
            //S = s + instability_costs[i][D - d];

            video >> current_frame;

            // Test if it is possible obtain an intermediate homography matrix.
            if ( findIntermediateHomographyMatrix( d, D, N, current_frame,
                                                   keypoints_frame_pre, keypoints_frame_pos,
                                                   descriptors_frame_pre, descriptors_frame_pos, homography_matrix ) ) {

                //getStableFrameSpatially(current_frame, homography_matrix, drop_area, crop_area, i, s, S, selected_frames,
                //               keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);
                getStableFrameTemporally(current_frame, homography_matrix, drop_area, crop_area, i, d, D, selected_frames,
                               keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);

            } else {
                result = current_frame.clone();
                num_of_fails_in_homography++;
                msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(i) << " | [F] Failed on finding an intermediate homography." << std::endl), BOTH);
            }


        } else {
            // If current frame is a master frame update the master frame posterior, previous and the D value.
            // Only show the master without homography.

            //////////////////////////////
            /// THIS IS A MASTER FRAME ///
            //////////////////////////////

            i_master++;

            // TODO: verify memory leak in the clone() method.
            image_master_pre = image_master_pos.clone();
            video.set(CV_CAP_PROP_POS_FRAMES, master_frames[i_master+1]);
            video.read(image_master_pos);
            video.set(CV_CAP_PROP_POS_FRAMES, i);
            video.read(current_frame);

            //Loading the descriptors of the new master frames
            keypoints_frame_pre.swap(keypoints_frame_pos);
            descriptors_frame_pre = descriptors_frame_pos.clone();
            getKeypointsAndDescriptors(image_master_pos, keypoints_frame_pos, descriptors_frame_pos);

            result = current_frame.clone();
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(i) << " | [M] Kept. [Master]" << std::endl), BOTH);
            num_of_good_frames++;

            D = master_frames[i_master+1] - master_frames[i_master];

            // ----------------------------------------------------------------------
            // DEBUG
            //if ( DEBUG_INTERMEDIATE_FRAME )
            //    msg_handler.reportStatus(SSTR("i: " << i << " | D: " << D << " | masterFrames[" << i_master << "]: " << master_frames[i_master]
            //                 <<  " | masterFrames[" << i_master+1 <<"]: " << master_frames[i_master+1] << std::endl), SCREEN);
            // ----------------------------------------------------------------------

        }

        ////////////////////////////////////////
        /// WRITING THE RESULT TO THE OUTPUT ///
        ////////////////////////////////////////
        if ( experiment_settings.save_video_in_disk ){
            result_cropped = result(crop_area);
            writeToOutput(save_video, result_cropped, i);
        }
        //EXECUTE_VIEW
    }

    //Processing the last master (No homography is required)
    if ( range_max > master_frames[master_frames.size()-1] ) {
        // Last master frame without homography.
        video >> current_frame;
        result = current_frame.clone();
        msg_handler.reportStatus(SSTR(" Frame : " << master_frames[master_frames.size()-1] << " | [M] Kept. [Master]" << std::endl), BOTH);
        num_of_good_frames++;

        ////////////////////////////////////////
        /// WRITING THE RESULT TO THE OUTPUT ///
        ////////////////////////////////////////
        if ( experiment_settings.save_video_in_disk ){
            result_cropped = result(crop_area);
            writeToOutput(save_video, result_cropped, master_frames[master_frames.size()-1]);
        }
        //EXECUTE_VIEW
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Process frames after the last master frame. ///////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int last_index = std::min(num_frames, range_max);
    D = last_index - master_frames[master_frames.size()-1];
    //S = -1;

    // Process frames after the last master frame.
    for ( int i = master_frames[master_frames.size()-1]+1 ; i < last_index; i++ ) {

        video >> current_frame;

        d = last_index - i;
        //s = instability_costs[i][d];

        getKeypointsAndDescriptors(current_frame, keypoints_current_frame, descriptors_current_frame);

        if ( findHomographyMatrix( keypoints_current_frame, keypoints_frame_pre, descriptors_current_frame, descriptors_frame_pre, homography_matrix ) ) {

            //getStableFrameSpatially(current_frame, homography_matrix, drop_area, crop_area, i, s, S, selected_frames,
            //               keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);
            getStableFrameTemporally(current_frame, homography_matrix, drop_area, crop_area, i, d, D, selected_frames,
                           keypoints_frame_pre, keypoints_frame_pos, descriptors_frame_pre, descriptors_frame_pos, 1, msg_handler, result);

        } else {
            result = current_frame.clone();
            num_of_fails_in_homography++;
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(i) << " | [F] Failed on finding a homography matrix to the last master." << std::endl), BOTH);
        }

        ////////////////////////////////////////
        /// WRITING THE RESULT TO THE OUTPUT ///
        ////////////////////////////////////////
        if ( experiment_settings.save_video_in_disk ){
            result_cropped = result(crop_area);
            writeToOutput(save_video, result_cropped, i);
        }
        //EXECUTE_VIEW
    }

    std::cout << " -> 100%" << std::endl
              << std::endl;

    msg_handler.reportStatus(SSTR(std::endl), BOTH);

    if ( experiment_settings.save_video_in_disk ) {
        msg_handler.reportStatus(SSTR(" --> Stabilized video saved in: " << std::endl
                                      << experiment_settings.save_video_filename << std::endl
                                      << "Number of frames saved: " << saved_frames << std::endl
                                      << std::endl), SCREEN);

        msg_handler.reportStatus(SSTR(" --> Stabilized video saved in: " << std::endl
                                      << experiment_settings.save_video_filename << std::endl
                                      << "Number of frames saved: " << saved_frames << std::endl
                                      << std::endl), LOG_FILE);
    }

    msg_handler.reportStatus(SSTR(" --> General info: " << std::endl
                                  << ".Number of reconstructed frames: " << num_of_reconstructed_frames << std::endl
                                  << ".Number of dropped frames: " << num_of_dropped_frames << std::endl
                                  << ".Number of good frames: " << num_of_good_frames << std::endl
                                  << ".Number of frames where homography has failed: " << num_of_fails_in_homography << std::endl
                                  << std::endl), SCREEN);

    // -----------------------------------------------------------------------------------------------------------------------------------
    // LOG FILE
    msg_handler.reportStatus(SSTR(" --> General info: " << std::endl
                                  << ".Number of reconstructed frames: " << num_of_reconstructed_frames << std::endl
                                  << ".Number of dropped frames: " << num_of_dropped_frames << std::endl
                                  << ".Number of good frames: " << num_of_good_frames << std::endl
                                  << ".Number of frames where homography has failed: " << num_of_fails_in_homography << std::endl
                                  << std::endl), LOG_FILE);
    // -----------------------------------------------------------------------------------------------------------------------------------

    save_video.release();
    video.release();

    msg_handler.reportStatus(SSTR("\n Process finished: " << currentDateTime() << std::endl << std::endl), BOTH);

    return 0;
}

/**
 * @brief getItFormatted - Formats the number with padding.
 * @param frame_number
 * @return
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
std::string getItFormatted(int frame_number){
    return SSTR(std::setfill('0') << std::setw(log_number_length) << frame_number);
}

/**
 * @brief writeToOutput - Writes the given image to the output (save_video and/or screen)
 * @param save_video
 * @param image
 * @param frame_number
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void writeToOutput(cv::VideoWriter& save_video, cv::Mat& image, uint frame_number){
    if ( FRAME_NUMBER_RESULT ){
        cv::putText(image, SSTR(getItFormatted(frame_number)), cv::Point(150,150), cv::FONT_HERSHEY_TRIPLEX, 2.5, cv::Scalar(0,0,255));
    }
    save_video << image;
    saved_frames++;
    //cv::imshow("Frame", image);
    //cv::waitKey(0);
}

/**
 * @brief getStableFrameTemporally - Returns a frame stabilized given the parameters.
 * @param input_frame
 * @param homography_matrix
 * @param drop_area
 * @param crop_area
 * @param frame_number
 * @param d - Temporal distance to the previous master
 * @param D - Temporal distance between the masters
 * @param selected_frames
 * @param image_master_pre
 * @param image_master_pos
 * @param trial - The number of the current attempt
 * @param msg_handler - The message handler
 * @param stable_frame
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void getStableFrameTemporally(cv::Mat& input_frame, cv::Mat& homography_matrix,
                    const cv::Rect &drop_area, const cv::Rect &crop_area,
                    int frame_number, int d, int D, cv::vector<int> &selected_frames,
                    const std::vector<cv::KeyPoint> &keypoints_master_pre, const std::vector<cv::KeyPoint> &keypoints_master_pos,
                    const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos,
                    int attempt, MessageHandler &msg_handler, cv::Mat& stable_frame){

    cv::Mat reconstructed_frame;
    int N = experiment_settings.segment_size;

    //Get the coverage when applying the given homography to the frame
    HomogCoverage coverage = getHomogCoverage(input_frame, homography_matrix, drop_area, crop_area);

    if(coverage == CROP_AREA){

        /// CASE 1: Homography makes it good, frame is kept.
        msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [K] Kept." << std::endl), BOTH);
        num_of_good_frames++;

        applyHomographyMatrix( input_frame , homography_matrix , stable_frame );
    }else if (coverage == DROP_AREA){

        /// CASE 2: Homography makes it regular, frame needs to be reconstructed
        if ( reconstructImage(input_frame , homography_matrix, selected_frames[frame_number], experiment_settings, drop_area, crop_area, reconstructed_frame) ){
            stable_frame = reconstructed_frame.clone();
            num_of_reconstructed_frames++;
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [R] Reconstructed using the original video." << std::endl), BOTH);
        } else {
            cv::Mat new_frame;

            // Failed on reconstructing the image. A new frame will be selected.
            msg_handler.reportStatus(SSTR(" Frame : " << SSTR ( getItFormatted(frame_number) ) << " | [E] Reconstruction failed using " << NUM_MAX_IMAGES_TO_RECONSTRUCT
                                          << " previous and posterior frames. A new frame will be selected in the original video." << std::endl), BOTH);

            /// CASE 2.1: Homography makes it awful, a new frame needs to be selected
            int new_frame_index = selectNewFrame ( d , D , N , selected_frames[frame_number] ,
                                                   selected_frames[frame_number-1] , selected_frames[frame_number+1] ,
                    keypoints_master_pre, keypoints_master_pos,
                    descriptors_master_pre, descriptors_master_pos,
                    crop_area , experiment_settings , new_frame );
            selected_frames[frame_number] = new_frame_index ;

            // Frame will be dropped because it does not cover the threshold area.
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped, a new one was selected in the original video. Trying it again [" << attempt << "]..." << std::endl), BOTH);

            if(attempt <= MAX_DROP_ATTEMPTS){

                if(attempt == 1)//Counts only one drop
                    num_of_dropped_frames++;

                if ( findIntermediateHomographyMatrix( d , D , N , new_frame,
                                                       keypoints_master_pre, keypoints_master_pos,
                                                       descriptors_master_pre, descriptors_master_pos, homography_matrix ) ) {
                    getStableFrameTemporally(new_frame, homography_matrix, drop_area, crop_area, frame_number, d, D, selected_frames,
                                   keypoints_master_pre, keypoints_master_pos, descriptors_master_pre, descriptors_master_pos, ++attempt, msg_handler, stable_frame);
                }
            } else {
                stable_frame = new_frame.clone();
                msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped! Gave up on trying to reconstruct this frame. Using the original frame instead." << std::endl), BOTH);
            }
            num_of_dropped_frames++;
        }

    }else{
        cv::Mat new_frame;

        /// CASE 3: Homography makes it awful, a new frame needs to be selected
        int new_frame_index = selectNewFrame ( d , D , N , selected_frames[frame_number] ,
                                               selected_frames[frame_number-1], selected_frames[frame_number+1],
                keypoints_master_pre, keypoints_master_pos,
                descriptors_master_pre, descriptors_master_pos,
                crop_area , experiment_settings , new_frame );
        selected_frames[frame_number] = new_frame_index ;

        // Frame will be dropped because it does not cover the threshold area.
        msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped, a new one was selected in the original video. Trying it again [" << attempt << "]..." << std::endl), BOTH);

        if(attempt <= MAX_DROP_ATTEMPTS){

            if(attempt == 1)//Counts only one drop
                num_of_dropped_frames++;

            if ( findIntermediateHomographyMatrix( d , D , N , new_frame,
                                                   keypoints_master_pre, keypoints_master_pos,
                                                   descriptors_master_pre, descriptors_master_pos, homography_matrix ) ) {
                getStableFrameTemporally(new_frame, homography_matrix, drop_area, crop_area, frame_number, d, D, selected_frames,
                               keypoints_master_pre, keypoints_master_pos, descriptors_master_pre, descriptors_master_pos, ++attempt, msg_handler, stable_frame);
            }
        }else{
            stable_frame = new_frame.clone();
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped! Gave up on trying to reconstruct this frame. Using the original frame instead." << std::endl), BOTH);
        }
    }
}

/**
 * @brief getStableFrameSpatially - Returns a frame stabilized given the parameters.
 * @param input_frame
 * @param homography_matrix
 * @param drop_area
 * @param crop_area
 * @param frame_number
 * @param s - Spatial distance to the previous master (using the instability costs)
 * @param S - Spatial distance between the masters (using the instability costs)
 * @param selected_frames
 * @param image_master_pre
 * @param image_master_pos
 * @param trial - The number of the current attempt
 * @param msg_handler - The message handler
 * @param stable_frame
 *
 * @author Washington Luis de Souza Ramos
 * @date 10/09/2016
 */
void getStableFrameSpatially(cv::Mat& input_frame, cv::Mat& homography_matrix,
                    const cv::Rect &drop_area, const cv::Rect &crop_area,
                    int frame_number, float s, float S, cv::vector<int> &selected_frames,
                    const std::vector<cv::KeyPoint> &keypoints_master_pre, const std::vector<cv::KeyPoint> &keypoints_master_pos,
                    const cv::Mat &descriptors_master_pre, const cv::Mat &descriptors_master_pos,
                    int attempt, MessageHandler &msg_handler, cv::Mat& stable_frame){

    cv::Mat reconstructed_frame;

    //Get the coverage when applying the given homography to the frame
    HomogCoverage coverage = getHomogCoverage(input_frame, homography_matrix, drop_area, crop_area);

    if(coverage == CROP_AREA){

        /// CASE 1: Homography makes it good, frame is kept.
        msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [K] Kept." << std::endl), BOTH);
        num_of_good_frames++;

        applyHomographyMatrix( input_frame , homography_matrix , stable_frame );
    }else if (coverage == DROP_AREA){

        /// CASE 2: Homography makes it regular, frame needs to be reconstructed
        if ( reconstructImage(input_frame , homography_matrix, selected_frames[frame_number], experiment_settings, drop_area, crop_area, reconstructed_frame) ){
            stable_frame = reconstructed_frame.clone();
            num_of_reconstructed_frames++;
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [R] Reconstructed using the original video." << std::endl), BOTH);
        } else {
            cv::Mat new_frame;

            // Failed on reconstructing the image. A new frame will be selected.
            msg_handler.reportStatus(SSTR(" Frame : " << SSTR ( getItFormatted(frame_number) ) << " | [E] Reconstruction failed using " << NUM_MAX_IMAGES_TO_RECONSTRUCT
                                          << " previous and posterior frames. A new frame will be selected in the original video." << std::endl), BOTH);

            /// CASE 2.1: Homography makes it awful, a new frame needs to be selected
            int new_frame_index = selectNewFrame ( s, S, selected_frames[frame_number] ,
                                                   selected_frames[frame_number-1] , selected_frames[frame_number+1] ,
                    keypoints_master_pre, keypoints_master_pos,
                    descriptors_master_pre, descriptors_master_pos,
                    crop_area , experiment_settings , new_frame );
            selected_frames[frame_number] = new_frame_index ;

            // Frame will be dropped because it does not cover the threshold area.
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped, a new one was selected in the original video. Trying it again [" << attempt << "]..." << std::endl), BOTH);

            if(attempt <= MAX_DROP_ATTEMPTS){

                if ( findIntermediateHomographyMatrix( s, S, new_frame,
                                                       keypoints_master_pre, keypoints_master_pos,
                                                       descriptors_master_pre, descriptors_master_pos, homography_matrix ) ) {
                    getStableFrameSpatially(new_frame, homography_matrix, drop_area, crop_area, frame_number, s, S, selected_frames,
                                   keypoints_master_pre, keypoints_master_pos, descriptors_master_pre, descriptors_master_pos, ++attempt, msg_handler, stable_frame);
                }
            } else {
                stable_frame = new_frame.clone();
                msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped! Gave up on trying to reconstruct this frame. Using the original frame instead." << std::endl), BOTH);
            }
            if(attempt == 1)//Counts only one drop
                num_of_dropped_frames++;
        }

    }else{
        cv::Mat new_frame;

        /// CASE 3: Homography makes it awful, a new frame needs to be selected
        int new_frame_index = selectNewFrame ( s, S, selected_frames[frame_number] ,
                                               selected_frames[frame_number-1], selected_frames[frame_number+1],
                keypoints_master_pre, keypoints_master_pos,
                descriptors_master_pre, descriptors_master_pos,
                crop_area , experiment_settings , new_frame );
        selected_frames[frame_number] = new_frame_index ;

        // Frame will be dropped because it does not cover the threshold area.
        msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped, a new one was selected in the original video. Trying it again [" << attempt << "]..." << std::endl), BOTH);

        if(attempt <= MAX_DROP_ATTEMPTS){

            if ( findIntermediateHomographyMatrix( s, S, new_frame,
                                                   keypoints_master_pre, keypoints_master_pos,
                                                   descriptors_master_pre, descriptors_master_pos, homography_matrix ) ) {
                getStableFrameSpatially(new_frame, homography_matrix, drop_area, crop_area, frame_number, s, S, selected_frames,
                               keypoints_master_pre, keypoints_master_pos, descriptors_master_pre, descriptors_master_pos, ++attempt, msg_handler, stable_frame);
            }
        } else {
            stable_frame = new_frame.clone();
            msg_handler.reportStatus(SSTR(" Frame : " << getItFormatted(frame_number) << " | [D] Dropped! Gave up on trying to reconstruct this frame. Using the original frame instead." << std::endl), BOTH);
        }

        if(attempt == 1)//Counts only one drop
            num_of_dropped_frames++;
    }
}

