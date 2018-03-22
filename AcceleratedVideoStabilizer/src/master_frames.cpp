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
 * @file master_frames.cpp
 *
 * Functions to get the master frames from file or calculating it.
 *
 * Calculate master frames. Calculate master frames in a parallel way. Call file_operations.cpp methods to get read master frames from file.
 *
 */

#include "executables/execute_commands.h"

#include "headers/master_frames.h"

/**
 * @brief Function that counts the digits of the number.
 *
 * @param number - number to calculate the number of digits.
 *
 * @return \c int - number of digits of the input number.
 *
 * @author Michel Melo da Silva
 * @date 20/04/2016
 */
int length ( int number )
{
    int counter=0;
    while(number)
    {
        number=number/10;
        counter++;
    }
    return (counter);
}

/**
 * @brief Function that calculates the master frames in a sequence given a N in the field segmentSize in the experiment_settings in a non-parallel (sequential) way.
 *
 * @param experiment_settings - object with the experiment settings.
 *
 * @return \c std::vector<int> - vector with the master frames found in the sequence.
 *
 * @author Michel Melo da Silva
 * @date 02/04/2016
 */
std::vector<int> findMasterFrames ( const EXPERIMENT& experiment_settings ){

    int size_segment = experiment_settings.segment_size;

    // Create the images
    cv::Mat homographyMatrix,
            ransacMask,
            frame;

    cv::VideoCapture video ( experiment_settings.video_filename );

    int num_segments = (int)(video.get(CV_CAP_PROP_FRAME_COUNT)/size_segment),
            inliers = 0,
            max_inliers = 0,
            master_index = 0;

    // -------------------------------------------------------------
    // DEBUG
    std::cout << "Number of frames: " << (int)video.get(CV_CAP_PROP_FRAME_COUNT) << std::endl
              << "Number of segments: " << num_segments << std::endl
              << std::endl;
    // -------------------------------------------------------------

    std::vector<int> masters ( num_segments );
    std::vector< std::vector< cv::KeyPoint > > segment_keypoints ( size_segment );
    std::vector<cv::Mat> segment_descriptors ( size_segment );


    int log_number_length = length((int)video.get(CV_CAP_PROP_FRAME_COUNT));

    // Iterate over all segments from 1 to n in the video.
    for ( int i_seg = 0 ; i_seg < num_segments*size_segment ; i_seg += size_segment ) {

        max_inliers = 0;
        master_index = i_seg;

        //Getting keypoints and descriptors to avoid unnecessary computation
        for ( int i = 0 ; i < size_segment ; i ++ ){
            //Load frame
            video >> frame;

            //Preload kpts and descriptors
            getKeypointsAndDescriptors(frame, segment_keypoints[i], segment_descriptors[i]);
        }

        for ( int i_master = i_seg ; i_master < i_seg + size_segment ; i_master++ ){

            inliers = 0;

            for ( int i_frame = i_seg ; i_frame < i_seg + size_segment ; i_frame++ ){

                if (i_frame == i_master)
                    continue;

                if ( findHomographyMatrix(segment_keypoints[i_frame%size_segment], segment_keypoints[i_master%size_segment],
                                          segment_descriptors[i_frame%size_segment], segment_descriptors[i_master%size_segment],
                                          homographyMatrix, ransacMask) )
                    inliers += cv::sum(ransacMask)[0];
            }

            //Update master frame and inliers
            if ( inliers > max_inliers ){
                master_index = i_master;
                max_inliers = inliers;
            }

        }

        // -------------------------------------------------------------
        // DEBUG
        std::cout << "Segment " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i_seg / size_segment << "/" << num_segments)
                  << " | Master: " << SSTR ( std::setfill('0') << std::setw(log_number_length) << master_index ) << std::endl;
        // -------------------------------------------------------------

        masters[i_seg/size_segment] = master_index;
    }

    video.release();

    return masters;
}

/**
 * @brief Function that calculates the master frames in a sequence given a N in the field segmentSize in the experiment_settings in a parallel way.
 *
 * @param experiment_settings - object with the experiment settings.
 *
 * @return \c std::vector<int> - vector with the master frames found in the sequence.
 *
 * @author Michel Melo da Silva
 * @date 03/04/2016
 */
std::vector<int> findMasterFramesParallel ( const EXPERIMENT& experiment_settings ){

    int size_segment = experiment_settings.segment_size;

    int num_procs = omp_get_num_procs();

    // -------------------------------------------------------------
    // DEBUG
    std::cout << "Number of cores: " << num_procs << std::endl
              << std::endl;
    // -------------------------------------------------------------

    std::vector<cv::VideoCapture> video(num_procs);
    for (int i = 0; i < num_procs; i++)
        video[i] = cv::VideoCapture(experiment_settings.video_filename);

    // Create the images
    std::vector<cv::Mat> homographyMatrix(num_procs),
            ransacMask(num_procs),
            frame(num_procs);

    int num_segments = (int)(video[0].get(CV_CAP_PROP_FRAME_COUNT)/size_segment),
            max_inliers [num_procs],
            master_index [num_procs];

    int log_number_length = length((int)video[0].get(CV_CAP_PROP_FRAME_COUNT));

    // -------------------------------------------------------------
    // DEBUG
    std::cout << "Number of frames: " << (int)video[0].get(CV_CAP_PROP_FRAME_COUNT) << std::endl
              << "Number of segments: " << num_segments << std::endl
              << std::endl;
    // -------------------------------------------------------------

    std::vector<int> masters ( num_segments );

    // Iterate over all segments from 1 to n in the video in a parallel form.
#pragma omp parallel for
    for ( int i_seg = 0 ; i_seg < num_segments*size_segment ; i_seg = i_seg + size_segment ) {

        int threadId = omp_get_thread_num();
        max_inliers[threadId] = 0;
        master_index[threadId] = i_seg;

        std::vector< std::vector< cv::KeyPoint > > segment_keypoints ( size_segment );
        std::vector<cv::Mat> segment_descriptors ( size_segment );

        //Getting keypoints and descriptors to avoid unnecessary computation
        for ( int i = 0 ; i < size_segment ; i ++ ){
            //Load frame
            video[threadId].set(CV_CAP_PROP_POS_FRAMES, (i_seg+i));
            video[threadId].read(frame.at(threadId));
            cv::cvtColor(frame.at(threadId), frame.at(threadId), CV_BGR2GRAY);

            //Preload kpts and descriptors
            getKeypointsAndDescriptors(frame.at(threadId), segment_keypoints[i], segment_descriptors[i]);
        }

        for ( int i_master = i_seg ; i_master < i_seg + size_segment ; i_master++ ){

            int inliers = 0;

            for ( int i_frame = i_seg ; i_frame < i_seg + size_segment ; i_frame++ ) {

                if (i_frame == i_master)
                    continue;

                if(i_frame == 128)
                    std::cout << "Chegou!" << std::endl;

                if ( findHomographyMatrix(segment_keypoints[i_frame%size_segment], segment_keypoints[i_master%size_segment],
                                          segment_descriptors[i_frame%size_segment], segment_descriptors[i_master%size_segment],
                                          homographyMatrix.at(threadId), ransacMask.at(threadId)) )
                    inliers += cv::sum(ransacMask.at(threadId))[0];
            }

            //Update master frame and inliers
            if ( inliers > max_inliers[threadId] ){
                master_index[threadId] = i_master;
                max_inliers[threadId] = inliers;
            }

        }

        // -------------------------------------------------------------
        // DEBUG
        std::cout << "Segment " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i_seg / size_segment )
                  << " | Master: " << SSTR ( std::setfill('0') << std::setw(log_number_length) << master_index[threadId] ) << std::endl;
        // -------------------------------------------------------------

        masters[i_seg/size_segment] = master_index[threadId];

    }

    for ( int i = 0 ; i < num_procs ; i++ )
        video[i].release();

    return masters;
}

/**
 * @brief Function that return the master frames in a sequence according with experiment settings, by load from a file or calculating them.
 *
 * @param experiment_settings - object with the experiment settings.
 * @param num_frames - number of frames of the reduced video.
 *
 * @return \c std::vector<int> - vector with the master frames found in the sequence.
 *
 * @author Michel Melo da Silva
 * @date 04/04/2016
 */
std::vector<int> getMasterFrames ( const EXPERIMENT experiment_settings , const int num_frames ) {

    std::vector<int> masterFrames;

    // --------------------------------------------------------------------------------------------------
    // Load or calculate master frames.
    if ( experiment_settings.read_master_frames_filename.compare("") == 0 ){

        std::cout << "--> Finding Masters to each segment" << std::endl
                  << std::endl;

        if ( experiment_settings.running_parallel )
            masterFrames = findMasterFramesParallel ( experiment_settings );
        else
            masterFrames = findMasterFrames( experiment_settings );

        if ( experiment_settings.save_master_frames_in_disk )
            saveMasterFramesToFile ( experiment_settings , masterFrames );

    } else {

        masterFrames = loadMasterFramesFromFile ( experiment_settings );

        std::cout << "--> Master Frames loaded from file: " << std::endl
                  << "  \"" << experiment_settings.read_master_frames_filename << "\"" << std::endl
                  << std::endl ;

        if ( (int)masterFrames.size() != (num_frames/experiment_settings.segment_size) ) {
            std::cout << "--(!) ERROR: The master frames loaded from: \"" << experiment_settings.read_master_frames_filename
                      << "\" does not match with the number of master frames necessary for a segment size equals "
                      << experiment_settings.segment_size << std::endl;
            exit(-4);
        }

    }
    // --------------------------------------------------------------------------------------------------

    return masterFrames;
}
