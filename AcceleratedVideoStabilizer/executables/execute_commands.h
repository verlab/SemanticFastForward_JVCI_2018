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
 * @file execute_commands.h
 *
 * Macro with specific codes.
 *
 */

#ifndef PROGRAM_H
#define PROGRAM_H

#include <sstream>
#include <stdio.h>

#define EXECUTE_INFO    \
    std::cout << std::endl \
            << " ------------------------------------------------------------------ " << std::endl \
            << "              Video Stabilization." << std::endl \
            << " --> Running Armadillo version: " << arma::arma_version::as_string() << std::endl \
            << " --> Running OpenCV version: " << CV_VERSION << std::endl \
            << " --> Project webpage (database, source code and results): " << std::endl \
            << " \" www.verlab.dcc.ufmg.br/semantic_hyperlapse_project \" " << std::endl \
            << " --> If you are using it to academic purposes, please cite: " << std::endl \
            << " \" Autores. Titulo. Conferencia. Ano \" " << std::endl \
            << " --> If you need help feel free to contact us: " << std::endl \
            << " \" michel.silva66@gmail.com \" and \" washington.ramos@outlook.com \" " << std::endl \
            << " --> Institution: " << std::endl \
            << " VerLab @ Universidade Federal de Minas Gerais (UFMG), Brazil. " << std::endl \
            << " ------------------------------------------------------------------ " << std::endl \
            << std::endl;

#define EXECUTE_EXPERIMENT_ID    \
    std::cout << std::endl \
            << " Running exepriment: " << argv[1] << " | Experiment ID: " << experiment_settings.id << std::endl \
            << std::endl; \

#define EXECUTE_VIEW  \
    if ( VIEW ) { \
        imshow( "Homography", result ); \
        cv::waitKey(VIEW_DELAY); \
    }

#define EXECUTE_IMAGE_STUFFS( string_length , frame ) \
    if ( FRAME_NUMBER_RESULT ) \
        cv::putText(result, SSTR(std::setfill('0') << std::setw(string_length) << frame), cv::Point(150,150), cv::FONT_HERSHEY_TRIPLEX, 2.5, cv::Scalar(0,0,255)); \
    if ( experiment_settings.save_video_in_disk ){ \
        result = result( crop_area ); \
        save_video << result; \
        saved_frames++; \
    } else { \
        cv::rectangle( result , view_area , cv::Scalar(255,0,0) , 2 ) ; \
        cv::rectangle( result , crop_area , cv::Scalar(0,255,0) , 2 ) ; \
        cv::rectangle( result , drop_area , cv::Scalar(0,0,255) , 2 ) ; \
    }

#define EXECUTE_IMAGE_RECONSTRUCTION    \
    /*if ( ! imageReconstruction(result, selected_frames[i], experiment_settings, crop_area, reconstructed_frame) )*/ \
    if ( ! imageReconstructionMask(current_frame , homography_matrix, selected_frames[i], experiment_settings, crop_area, reconstructed_frame) ) \
        if ( DEBUG_FRAME_STATUS ) \
            std::cout << " [!] Failled in reconstruct the image. " << std::endl; \
    result = reconstructed_frame.clone(); \
    num_of_reconstructed_frames++;

#define EXECUTE_IMAGE_CONDITIONAL_ACTIONS    \
if ( getAreaRatio( current_frame , homography_matrix , crop_area ) > MAXIMUM_AREA_ALLOWED ) { \
    if ( getAreaRatio( current_frame , homography_matrix , drop_area ) > MAXIMUM_AREA_ALLOWED ) { \
        log_file << " Frame : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [D] Dropped, a new one will be selected in the original video." << std::endl; \
        if ( DEBUG_FRAME_STATUS ) \
            std::cout << " i : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [D] Dropped, a new one will be selected in the original video." << std::endl; \
        num_of_dropped_frames++; \
    } else { \
        log_file << " Frame : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [R] Reconstructed using the original video." << std::endl; \
        if ( DEBUG_FRAME_STATUS ) \
            std::cout << " i : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [R] Reconstructed using the original video." << std::endl; \
        EXECUTE_IMAGE_RECONSTRUCTION \
    } \
} else { \
    log_file << " Frame : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [K] Kept." << std::endl; \
    if ( DEBUG_FRAME_STATUS ) \
        std::cout << " i : " << SSTR ( std::setfill('0') << std::setw(log_number_length) << i ) << " | [K] Kept." << std::endl; \
    num_of_good_frames++; \
}

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#endif // PROGRAM_H

