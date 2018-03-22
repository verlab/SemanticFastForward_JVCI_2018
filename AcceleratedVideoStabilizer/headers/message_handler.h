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

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "headers/error_messages.h"

enum Stream{LOG_FILE, SCREEN, BOTH};

class MessageHandler
{
public:
    MessageHandler(std::string log_file_name);
    ~MessageHandler(void);

    /**
     * @brief MessageHandler::reportError Report the specified error.
     * @param error_message
     * @param condition
     */
    void reportError(ErrorMessage error_message);

    /**
     * @brief MessageHandler::reportError Report the specified error if the condition is respected.
     * @param error_message
     * @param condition
     */
    void reportError(ErrorMessage error_message, bool condition);

    /**
     * @brief MessageHandler::reportStatus Reports the given status to the log file, screen, or both.
     * @param status
     * @param stream
     */
    void reportStatus(std::string status, Stream stream);

private:
    std::ofstream log_file;

    /**
     * @brief MessageHandler::printError Prints the specified error.
     * @param error_message
     * @param condition
     */
    void printError(ErrorMessage error_message);
};

#endif // MESSAGEHANDLER_H
