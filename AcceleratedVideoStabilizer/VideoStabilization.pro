######################################################################################
####   This file is part of SemanticFastForward_EPIC@ECCVW.
##
##    SemanticFastForward_EPIC@ECCVW is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    SemanticFastForward_JVCI is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with SemanticFastForward_EPIC@ECCVW.  If not, see <http://www.gnu.org/licenses/>.
##
######################################################################################

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += \
    src/main.cpp \
    src/homography.cpp \
    src/sequence_processing.cpp \
    src/file_operations.cpp \
    src/master_frames.cpp \
    src/image_reconstruction.cpp \
    src/line_and_point_operations.cpp \
    src/message_handler.cpp

HEADERS += \
    definitions/experiments.h \
    definitions/experiment_struct.h \
    definitions/define.h \
    executables/execute_commands.h \
    headers/homography.h \
    headers/sequence_processing.h \
    headers/file_operations.h \
    headers/master_frames.h \
    headers/image_reconstruction.h \
    headers/line_and_point_operations.h \
    headers/message_handler.h \
    headers/error_messages.h

OTHER_FILES += \
    txt/Output_errors_description.txt \
    txt/TODO.txt

## OpenCV settings for Unix/Linux


message("*** Video Stabilization: Using OpenCV, OpenMP, Armadillo, LibBoost on Unix\Linux.")
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib \
     -lopencv_imgproc \
     -lopencv_features2d\
     -lopencv_highgui \
     -lopencv_nonfree \
     -lopencv_flann \
     -lopencv_calib3d \
     -lopencv_core \
     -lopencv_video \
     -lboost_system \
     -lboost_filesystem \
     -larmadillo \
     -fopenmp


QMAKE_CXXFLAGS += -fopenmp
