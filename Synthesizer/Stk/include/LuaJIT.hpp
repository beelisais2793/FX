#pragma once

#include <lua.hpp>
#include <iostream>
#include <string>

using std::string;
using std::vector;
using std::cin;

struct LuaJIT
{
	int status;
	lua_State *L;

    LuaJIT() {
		L = luaL_newstate(); // open Lua
		if (!L) {
				std::cout << "Error creating lua\n";
				exit(-1);
		}

		luaL_openlibs(L); // load Lua libraries  		
	}
	LuaJIT(const char * script) {
		L = luaL_newstate(); // open Lua
		if (!L) {
				std::cout << "Error creating lua\n";
				exit(-1);
		}

		luaL_openlibs(L); // load Lua libraries  
		status = luaL_loadfile(L, script);  // load Lua script
		int ret = lua_pcall(L, 0, 0, 0); // tell Lua to run the script
		if (ret != 0) {
			std::cout << "Lua Error: " << lua_tostring(L, -1) << std::endl;
			exit(-1);
		}
	}
	~LuaJIT()
	{
		if(L) lua_close(L);
	}
	

    int LoadFile(const string& filename)
    {
        int status{ luaL_loadfile(L, filename.c_str()) };
        if (status == 0)
        {
            lua_setglobal(L, filename.c_str());
        }
        return status;
    }

    int DoCmd(const string& cmd) {
        return luaL_dostring(L,cmd.c_str());
    }

    int PCall()
    {
        return lua_pcall(L, 0, LUA_MULTRET, 0);
    }

    void NewTable(const string& name)
    {
        lua_newtable(L);
        lua_setglobal(L, name.c_str());
    }

    void GetGlobal(const string& name)
    {
        lua_getglobal(L, name.c_str());
    }

    void PushNumber(double number)
    {
        lua_pushnumber(L, number);
    }
	void PushString(const string& str)
    {
        lua_pushstring(L, str.c_str());
    }

    void SetTableValue(double index, double value)
    {
        PushNumber(index);
        PushNumber(value);
        lua_rawset(L, -3);
    }

    double GetNumber()
    {
        return lua_tonumber(L, -1);
    }

    void Pop(int number)
    {
        lua_pop(L, number);
    }

    void CreateCFunction(const string& name, lua_CFunction function)
    {
        lua_pushcfunction(L, function);
        lua_setglobal(L, name.c_str());
    }
};

class LuaTable
{
private:
    LuaJIT& m_Lua;
    string m_Name;

public:
    LuaTable(LuaJIT& lua, const string& name)
        : m_Lua{ lua }
        , m_Name(name)
    {
        m_Lua.NewTable(m_Name);
    }

    void Set(const vector<int>& values)
    {
        Push();

        for (unsigned int i = 0; i < values.size(); ++i)
        {
            m_Lua.SetTableValue(i + 1, values[i]);
        }

        m_Lua.Pop(1);
    }

    void Push()
    {
        m_Lua.GetGlobal(m_Name);
    }
};

class LuaFunction
{
private:
    LuaJIT& m_Lua;
    string m_Filename;

protected:
    int PCall()
    {
        m_Lua.GetGlobal(m_Filename);
        return m_Lua.PCall();
    }

    double GetReturnValue()
    {
        double result{ m_Lua.GetNumber() };
        m_Lua.Pop(1);
        return result;
    }

public:
    LuaFunction(LuaJIT& lua, const string& filename)
        : m_Lua{ lua }
        , m_Filename(filename)
    {
        int status{ m_Lua.LoadFile(m_Filename) };
    }
};

class PrintTable
    : public LuaFunction
{
public:
    PrintTable(LuaJIT& lua, const string& filename)
        : LuaFunction(lua, filename)
    {

    }

    double Call(LuaTable& table)
    {
        double sum{};

        int status{ LuaFunction::PCall() };
        if (status)
        {
            throw(status);
        }
        else
        {
            sum = LuaFunction::GetReturnValue();
        }

        return sum;
    }
};