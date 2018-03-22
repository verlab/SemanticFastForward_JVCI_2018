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
 * @file file_operations.h
 *
 * Header functions for the file_operations.cpp.
 *
 */

#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <sstream>
#include <iomanip>
#include <limits>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include "definitions/experiment_struct.h"

/**
 * @brief Function to load the master frames from a file defined in the field read_masterframes_filename in the experiment_settings.
 *
 * @param experiment_settings - variable with the experiment setting.
 *
 * @return \c std::vector - vector of master frames loaded from file describle in the experiment settings.
 *
 * @author Michel Melo da Silva
 * @date 08/04/2016
 */
std::vector<int>    loadMasterFramesFromFile ( const EXPERIMENT& experiment_settings ) ;

/**
 * @brief Function to save the calculated master frames into a file defined in the field save_masterframes_filename in the experiment_settings.
 *
 * @param experiment_settings - variable with the experiment setting.
 * @param masters_frames - vector of master frames to be saved in file describle in the experiment settings.
 *
 * @return
 *      \c bool \b true  - if the writer return sucess while writing the file. \n
 *      \c bool \b false - if the writer return fail while writing the file.
 *
 * @author Michel Melo da Silva
 * @date 08/04/2016
 */
bool                saveMasterFramesToFile ( const EXPERIMENT& experiment_settings , const std::vector<int> masters_frames ) ;

/**
 * @brief Function to read a CSV composed by frames selected by the selection algorithm.
 *
 * @param filename - std::string with the filename of the CSV file that contains the selected frames of the reduced video.
 * @param selected_frames - vector to save the selected frames.
 *
 * @return \c void
 *
 * @author Washington Luis de Souza Ramos
 * @date 14/04/2016
 */
void                readSelectedFramesCSV (std::string filename, std::vector<int>& selected_frames) ;

/**
 * @brief Function that manages the experiments ID automatically.
 *
 * @return \c std::string - The experiment id formatted by 4 digits
 *
 * @author Washington Luis de Souza Ramos
 * @date 19/04/2016
 */
std::string         getExperimentId () ;

/**
 * @brief Function that returns the semantic cost of the transiction from the frame_src to
 *          the frame_dst. The file where the semantic cost will be find is defined in the experiment_settings
 *
 * @param experiment_settings - variable with the experiment setting.
 * @param frame_src - first frame of the transition.
 * @param frame_dst - last frame of the transition.
 *
 * @return \c double - The semantic cost of the transiction of the frame_src to the frame_dst. Returns the complementary value ( 1 - x ).
 *
 * @author Michel Melo da Silva
 * @date 30/04/2016
 */
double              getSemanticCost ( const EXPERIMENT &experiment_settings , const int frame_src , const int frame_dst ) ;

/**
 * @brief loadInstabilityCostsFromFile Loads the instability costs from a CSV file (matrix format).
 * @param experiment_settings
 * @return The matrix of instability costs
 *
 * @author Washington Luis de Souza Ramos
 * @date 12/09/2016
 */
std::vector< std::vector<double> > loadInstabilityCostsFromFile(const EXPERIMENT &experiment_settings);


/**
 * @brief loadOpticalFlow Loads the Optical Flow calculated by the FlowNet from a CSV file.
 * @param experiment_settings
 * @return Vector[j][i] with the optical flow from the image j to j+1 (i=0 is dx and i=1 is dy).
 *
 * @author Michel Melo da Silva
 * @date 09/01/2017
 */
std::vector< std::vector<double> > loadOpticalFlow(const EXPERIMENT &experiment_settings);

/**
 * @brief str2bool convert a string to boolean.
 * @param str - string to convert
 * @return boolean
 *
 * @author Felipe Cadar Chamone
 * @date 14/07/2017
 */
bool str2bool(std::string str);

/**
 * @brief filter_string filter a string to remove white spaces and line break.
 * @param str - string to be filtered
 * @return str - filtered string
 *
 * @author Felipe Cadar Chamone
 * @date 14/07/2017
 */
std::string filter_string(std::string str);


#endif // FILE_OPERATIONS_H
