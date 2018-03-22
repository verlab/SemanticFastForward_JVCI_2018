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
 * @file master_frames.h
 *
 * Header functions for the master_frames.cpp.
 *
 */

#ifndef MASTERFRAMES_H
#define MASTERFRAMES_H

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <omp.h>
#include <armadillo>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "definitions/experiment_struct.h"

#include "homography.h"
#include "file_operations.h"

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
std::vector<int>    getMasterFrames     ( const EXPERIMENT experiment_settings , const int num_frames );

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
int                 length      ( int number );

#endif // MASTERFRAMES_H
