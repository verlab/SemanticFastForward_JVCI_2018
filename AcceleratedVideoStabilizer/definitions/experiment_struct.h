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
 * @file experiment_struct.h
 *
 * Fields declaration of the experiment settings.
 *
 */

#ifndef EXPERIMENT_STRUCT_H
#define EXPERIMENT_STRUCT_H

#include <stdio.h>
#include <string>

struct EXPERIMENT {
    std::string     id;
    std::string     video_filename;                 /** Complete path and filename of the video with extension. */
    std::string     output_path;                    /** Complete path to the folder where the results will be saved. It is important that the user have permission to write there. */
    std::string     original_video_filename;        /** Complete path and filename of the original video. */
    std::string     read_master_frames_filename;    /** Complete path and filename of the txt file with the selected master frames. */
    std::string     save_master_frames_filename;    /** Complete path and filename to save the txt file with the selected master frames. */
    std::string     save_video_filename;            /** Complete path and filename of the save the stabilized video. */
    std::string     selected_frames_filename;       /** Complete path and filename of the csv file with the selected master frames that was selected to create the reduced video. */
    std::string     semantic_costs_filename;        /** Complete path and filename of the csv file with the semantic costs of the frame transitions. */
    std::string     instability_costs_filename;     /** Complete path and filename of the csv file with the jitter costs of the transitions. */
    std::string     optical_flow_filename;          /** Complete path and filename of the csv file with the optical flow calculated by the FlowNet. */
    std::string     log_file_name;                  /** Complete path and filename to save the txt file log execution. */
    int             segment_size;                   /** Size of the segments where the master frames will be calculated. It needs to be a power of two (2, 4, 8, 16, ...). */
    bool            save_master_frames_in_disk;     /** After calculte the master frames, do you want to save it in a file? After you can load it directly withou calculate again */
    bool            save_video_in_disk;             /** Save stabilized video in Disk. */
    bool            running_parallel;               /** Running the master frames selection in parallel processors. */
    bool            exist;                          /** True indicates the informations is updated. If you do not want run this configuration anymore just use value false */
};

#endif // EXPERIMENT_STRUCT_H
