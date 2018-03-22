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

#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H

/**
 * @brief The ErrorMessage enum
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
* \b -11 - Transition from frame_src to frame_dst larger than number of transitions described in the file of the semantic costs                                                 *
*/
enum ErrorMessage{WRONG_INPUT, CANT_OPEN_ACC_VIDEO,
                  CANT_CREATE_OUT_VIDEO, CANT_OPEN_ORI_VIDEO,
                 CANT_OPEN_CSV, CANT_CREATE_DIR, CANT_CREATE_LOG,
                 RANGE_WRONGLY_DEFINED, CANT_OPEN_SEMANTIC_CSV, LARGE_TRANSITION};

#endif // ERROR_MESSAGES_H
