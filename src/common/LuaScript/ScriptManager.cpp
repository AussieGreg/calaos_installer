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
#include <ScriptManager.h>
#include <setjmp.h>

#ifdef CALAOS_INSTALLER
#include <QTime>
#endif

using namespace Calaos;

double ScriptManager::start_time = 0.0;
static jmp_buf panic_jmp;

ScriptManager::ScriptManager()
{
#ifndef CALAOS_INSTALLER
        Utils::logger("script.lua") << Priority::DEBUG << "ScriptManager::ScriptManager(): Ok" << log4cpp::eol;
#endif
}

ScriptManager::~ScriptManager()
{
#ifndef CALAOS_INSTALLER
        Utils::logger("script.lua") << Priority::DEBUG << "ScriptManager::~ScriptManager(): Ok" << log4cpp::eol;
#endif
}

bool ScriptManager::ExecuteScript(string script)
{
        bool ret = true;
        errorScript = true;

        const luaL_Reg *l;
        lua_State *L = lua_open();

        /* Load libraries */
        for (l = lua_libs; l->func; l++)
        {
                lua_pushcfunction(L, l->func);
                lua_pushstring(L, l->name);
                lua_call(L, 1, 0);
        }

        /* disable some calls from base lib */
        lua_pushnil(L);
        lua_setglobal(L, "loadfile");
        lua_pushnil(L);
        lua_setglobal(L, "loadstring");
        lua_pushnil(L);
        lua_setglobal(L, "dofile");
        lua_pushnil(L);
        lua_setglobal(L, "dostring");
        lua_pushnil(L);
        lua_setglobal(L, "load");
        lua_pushnil(L);
        lua_setglobal(L, "print"); /* we provide another version */

        /* register new functions */
        lua_register(L, "print", Lua_print);

        //Disable some function from os lib
        lua_getglobal(L, "os");
        lua_pushnil(L);
        lua_setfield(L, -2, "execute");
        lua_pushnil(L);
        lua_setfield(L, -2, "rename");
        lua_pushnil(L);
        lua_setfield(L, -2, "remove");
        lua_pushnil(L);
        lua_setfield(L, -2, "exit");
        lua_pushnil(L);
        lua_setfield(L, -2, "getenv");
        lua_pushnil(L);
        lua_setfield(L, -2, "setlocale");
        lua_pushnil(L);
        lua_setfield(L, -2, "tmpname");
        lua_pop(L, 1);

        Lunar<Lua_Calaos>::Register(L);

        Lua_Calaos lc;

        Lunar<Lua_Calaos>::push(L, &lc);
        lua_setglobal(L, "calaos");

        //Set a hook to kill script in case of a wrong use (infinite loop, ...)
        lua_sethook(L, Lua_DebugHook, LUA_MASKLINE | LUA_MASKCOUNT, 1);

        #ifndef CALAOS_INSTALLER
        start_time = ecore_time_get();
        #else
        QTime t = QTime::currentTime();
        start_time = (double)t.second() + (((double) t.msec()) / 1000);
        #endif

        int err = luaL_loadbuffer(L, script.c_str(), script.length(), "CalaosScript");
        if (err)
        {
                ret = false;
                if (err == LUA_ERRSYNTAX)
                {
                        string msg = lua_tostring(L, -1);
                        #ifndef CALAOS_INSTALLER
                        Utils::logger("script.lua") << Priority::ERROR
                                        << "ScriptManager::ExecuteScript(): Syntax Error: "
                                        << msg << log4cpp::eol;
                        #endif
                        errorMsg = "Syntax Error:\n" + msg;
                }
                else if (err == LUA_ERRMEM)
                {
                        string msg = lua_tostring(L, -1);
                        #ifndef CALAOS_INSTALLER
                        Utils::logger("script.lua") << Priority::ERROR
                                        << "ScriptManager::ExecuteScript(): LUA memory allocation error: "
                                        << msg << log4cpp::eol;
                        #endif
                        errorMsg = "Fatal Error:\nLUA memory allocation error:\n" + msg;
                }
        }
        else
        {
                if (setjmp(panic_jmp) == 1)
                {
                        ret = false;
                        #ifndef CALAOS_INSTALLER
                        Utils::logger("script.lua") << Priority::ERROR
                                        << "ScriptManager::ExecuteScript(): Script panic !"
                                        << log4cpp::eol;
                        #endif
                        errorMsg = "Fatal Error:\nScript panic !";
                }

                if ((err = lua_pcall(L, 0, 1, 0)))
                {
                        ret = false;

                        string errcode;
                        if (err == LUA_ERRRUN) errcode = "Runtime error";
                        else if (err == LUA_ERRSYNTAX) errcode = "Syntax error";
                        else if (err == LUA_ERRMEM) errcode = "Memory allocation error";
                        else if (err == LUA_ERRERR) errcode = "Error";
                        else errcode = "Unknown error";

                        string msg = lua_tostring(L, -1);
                        #ifndef CALAOS_INSTALLER
                        Utils::logger("script.lua") << Priority::ERROR
                                        << "ScriptManager::ExecuteScript(): "
                                        << errcode << " : " << msg
                                        << log4cpp::eol;
                        #endif
                        errorMsg = "Error " + errcode + " :\n\t" + msg;
                }
                else
                {
                        if (!lua_isboolean(L, -1))
                        {
                                ret = false;
                                #ifndef CALAOS_INSTALLER
                                Utils::logger("script.lua") << Priority::ERROR
                                                << "ScriptManager::ExecuteScript(): Script must return either \"true\" or \"false\""
                                                << log4cpp::eol;
                                #endif
                                errorMsg = "Error:\nScript must return either \"true\" or \"false\"";
                        }
                        else
                        {
                                errorScript = false;
                                ret = lua_toboolean(L, -1);
                        }
                }
        }

        lua_close(L);

        return ret;
}
