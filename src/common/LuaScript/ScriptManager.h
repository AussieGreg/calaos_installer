/******************************************************************************
**  Copyright (c) 2007-2008, Calaos. All Rights Reserved.
**
**  This file is part of Calaos Home.
**
**  Calaos Home is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  Calaos Home is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Foobar; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/
#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <Utils.h>

#ifndef CALAOS_INSTALLER
#include <Ecore.h>
#endif

#include <ScriptBindings.h>

namespace Calaos
{

//The maximum number of second a script can take to do his duty
//After that, it will be stopped.
#define SCRIPT_MAX_EXEC_TIME    1.0

class ScriptManager
{
private:
    ScriptManager();

    bool errorScript;
    string errorMsg;

public:
    static ScriptManager &Instance()
    {
        static ScriptManager sm;
        return sm;
    }

    ~ScriptManager();

    /** Execute script and return true or false depending on
                  * the return value of the script
                  */
    bool ExecuteScript(string script);

    /** Retrieve the last error message
                  */
    string getErrorMsg() { return errorMsg; }

    bool hasError() { return errorScript; }

    static double start_time;
    static bool abortTimeout;
};

}
#endif
