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

#include "headers/message_handler.h"
#include "definitions/define.h"

MessageHandler::MessageHandler(std::string log_file_name){
    log_file.open ( log_file_name.c_str() , std::ios::out);
    reportError(CANT_CREATE_LOG, !log_file.is_open());
}

MessageHandler::~MessageHandler(void){
    log_file.close ();
}

/**
 * @brief MessageHandler::reportError Report the specified error.
 * @param error_message
 * @param condition
 */
void MessageHandler::reportError(ErrorMessage error_message){
    printError(error_message);
}

/**
 * @brief MessageHandler::reportError Report the specified error if the condition is respected.
 * @param error_message
 * @param condition
 */
void MessageHandler::reportError(ErrorMessage error_message, bool condition){
    if(condition){
        printError(error_message);
    }
}

/**
 * @brief MessageHandler::printError Prints the specified error.
 * @param error_message
 * @param condition
 */
void MessageHandler::printError(ErrorMessage error_message){
    switch (error_message) {
    case WRONG_INPUT:

        break;
    case CANT_OPEN_ACC_VIDEO:

        break;
    case CANT_CREATE_OUT_VIDEO:

        break;
    case CANT_OPEN_ORI_VIDEO:

        break;
    case CANT_OPEN_CSV:

        break;
    case CANT_CREATE_DIR:

        break;
    case CANT_CREATE_LOG:
        //std::cerr << " --(!) ERROR: Can not create file \"" << experiment_settings.log_file_name << "\" to save the experiment log." << std::endl;
        std::cerr << " --(!) ERROR: Can not create file to save the experiment log." << std::endl;
        break;
    case RANGE_WRONGLY_DEFINED:

        break;
    case CANT_OPEN_SEMANTIC_CSV:

        break;
    case LARGE_TRANSITION:

        break;
    default:
        break;
    }
}

/**
 * @brief MessageHandler::reportStatus Reports the given status to the log file, screen, or both.
 * @param status
 * @param stream
 */
void MessageHandler::reportStatus(std::string status, Stream stream){
    switch (stream) {
    case LOG_FILE:
        log_file << status;
        break;
    case SCREEN:
        std::cout << status;
        break;
    case BOTH:
        log_file << status;
        if ( DEBUG_FRAME_STATUS )
            std::cout << status;
        break;
    default:
        break;
    }
}
