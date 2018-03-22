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
 * @file experiments.h
 *
 * Definitions of experiment settings.
 *
 */

#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <unistd.h>

#include <opencv2/core/core.hpp>

#include "definitions/experiment_struct.h"

#include "executables/execute_commands.h"

#include "headers/file_operations.h"

EXPERIMENT load_experiments_settings ( std::string settingsFilename ){
    EXPERIMENT experiment_settings;


    std::string     video_path;                     /** <i>std::string</i> <b>video_path:</b>Complete path to the video. */
    std::string     output_path;                    /** <i>std::string</i> <b>output_path:</b> Complete path to the folder where the results will be saved. It is important that the user have permission to write there. */
    std::string     video_name;                     /** <i>std::string</i> <b>video_name:</b> Filename of the video with extension. */
    std::string     original_video_filename;        /** <i>std::string</i> <b>original_video_filename:</b> Complete path and filename of the original video. */
    std::string     read_masterframes_filename;     /** <i>std::string</i> <b>read_masterframes_filename:</b> Complete path and filename of the txt file with the selected master frames. */
    std::string     selected_frames_filename;       /** <i>std::string</i> <b>selected_frames_filename:</b> Complete path and filename of the csv file with the selected master frames that was selected to create the reduced video. */
    std::string     semantic_costs_filename;        /** <i>std::string</i> <b>semantic_costs_filename:</b> Complete path and filename of the csv file with the semantic cost of the frame transitions. */
    std::string     instability_costs_filename;     /** <i>std::string</i> <b>instability_costs_filename:</b> Complete path and filename of the csv file with the jitter costs of the transitions. */
    std::string     optical_flow_filename;          /** <i>std::string</i> <b>optical_flow_filename:</b> Complete path and filename of the csv file with the optical flow calculated by FlowNet. */
    int             segmentSize = 0;                /** <i>int</i> <b>segmentSize:</b> Size of the segments where the master frames will be calculated. It needs to be a power of two (2, 4, 8, 16, ...). */
    bool            saveMasterFramesInDisk = false; /** <i>bool</i> <b>saveMasterFramesInDisk:</b> After calculte the master frames, do you want to save it in a file? After you can load it directly withou calculate again */
    bool            saveVideoInDisk = false;        /** <i>bool</i> <b>saveVideoInDisk:</b> Save stabilized video in Disk. */
    bool            runningParallel = true;         /** <i>bool</i> <b>runningParallel:</b> Running the master frames selection in parallel processors. */
 
    cv::FileStorage fs(settingsFilename, cv::FileStorage::READ);
    
    if (!fs.isOpened()){
      return experiment_settings;
    }

    video_path = filter_string(fs["video_path"]);
    video_name = filter_string(fs["video_name"]);
    output_path = filter_string(fs["output_path"]);
    original_video_filename = filter_string(fs["original_video_filename"]);
    selected_frames_filename = filter_string(fs["selected_frames_filename"]);
    read_masterframes_filename = filter_string(fs["read_masterframes_filename"]);
    semantic_costs_filename = filter_string(fs["semantic_costs_filename"]);
    
    segmentSize = fs["segmentSize"];
    
    runningParallel = str2bool(fs["runningParallel"]);
    saveMasterFramesInDisk = str2bool(fs["saveMasterFramesInDisk"]);
    saveVideoInDisk = str2bool(fs["saveVideoInDisk"]);


    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);

    experiment_settings.id = getExperimentId();
    experiment_settings.video_filename = video_path + "/" + video_name;
    experiment_settings.output_path = SSTR ( output_path << "/" << video_name.substr(0,video_name.find_last_of('.')) << "_N" << segmentSize << "_"
                                             << hostname << "_ExpID_" << experiment_settings.id);
    experiment_settings.save_video_filename = SSTR ( experiment_settings.output_path << "/StabilizedVideo_" << video_name.substr(0,video_name.find_last_of('.')) << "_N"
                                                     << segmentSize << "_" << hostname << "_ExpID_" << experiment_settings.id << ".avi" );
    experiment_settings.save_master_frames_filename = SSTR ( experiment_settings.output_path << "/MasterFrames_N" << segmentSize << "_"
                                                             << video_name.substr(0,video_name.find_last_of('.')) << ".csv");
    experiment_settings.log_file_name = SSTR ( experiment_settings.output_path << "/Log_" << video_name.substr(0,video_name.find_last_of('.')) << "_N"
                                               << segmentSize << "_" << hostname << "_ExpID_" << experiment_settings.id << ".txt" ) ;
    experiment_settings.read_master_frames_filename = read_masterframes_filename;
    experiment_settings.save_master_frames_in_disk = saveMasterFramesInDisk;
    experiment_settings.save_video_in_disk = saveVideoInDisk;
    experiment_settings.semantic_costs_filename = semantic_costs_filename;
    experiment_settings.instability_costs_filename = instability_costs_filename;
    experiment_settings.selected_frames_filename = selected_frames_filename;
    experiment_settings.original_video_filename = original_video_filename;
    experiment_settings.segment_size = segmentSize;
    experiment_settings.running_parallel = runningParallel;
    experiment_settings.optical_flow_filename = optical_flow_filename;

    return experiment_settings;
}

#endif // EXPERIMENTS_H
