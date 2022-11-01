/*
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <random>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "LoggingSystem.hpp"
#include "lua/lua_random.hpp"

std::default_random_engine generator(clock1::now().time_since_epoch().count());
std::uniform_int_distribution<int> uniIntDist;
std::uniform_real_distribution<double> uniRealDist;
std::normal_distribution<double> normalDist;
std::poisson_distribution<int> poissonDist;
std::gamma_distribution<double> gammaDist;
std::cauchy_distribution<double> cauchyDist;
std::chi_squared_distribution<double> chiSquaredDist;

void push_random_library(lua_State* const L)
{
    void** lib = (void**)lua_newuserdata(L, sizeof(void*));
    *lib = nullptr;
    lua_newtable(L);
    lua_pushcfunction (L, lua_ignore_newindex);
    lua_setfield(L, -2, "__newindex");
    lua_pushstring (L, "Library");
    lua_setfield(L, -2, "__name");
    lua_pushinteger (L, ParameterTypeSystem::pid_library);
    lua_setfield(L, -2, "__type");
    lua_pushstring(L, "__index");
    lua_newtable(L);
    lua_pushcfunction (L, lua_random_seed);
    lua_setfield(L, -2, "seed");
    lua_pushcfunction (L, lua_random_int);
    lua_setfield(L, -2, "int");
    lua_pushcfunction (L, lua_random_uniform);
    lua_setfield(L, -2, "rand");
    lua_pushcfunction (L, lua_random_normal);
    lua_setfield(L, -2, "norm");
    lua_pushcfunction (L, lua_random_poisson);
    lua_setfield(L, -2, "poisson");
    lua_pushcfunction (L, lua_random_gamma);
    lua_setfield(L, -2, "gamma");
    lua_pushcfunction (L, lua_random_cauchy);
    lua_setfield(L, -2, "cauchy");
    lua_pushcfunction (L, lua_random_chiSquared);
    lua_setfield(L, -2, "chi_squared");
    lua_rawset(L, -3);
    lua_setmetatable(L, -2);
}

int lua_random_seed(lua_State* const L)
{
    if(lua_gettop(L) == 1)
    {
        int isnum;
        int seed = lua_tointegerx(L, 1, &isnum);
        if(isnum && seed >= 0)
        {
            if(seed == 0)
            {
                generator.seed(clock1::now().time_since_epoch().count());
            }
            else
            {
                generator.seed(seed);
            }
        }
        else
        {
            LoggingSystem::Error("Invalid seed supplied to random.seed");
        }

    }
    else
    {
        LoggingSystem::Error("invalid call to random.seed");
    }
    return 0;
}

int lua_random_int(lua_State* const L)
{
    switch(lua_gettop(L))
    {
    case 0:
        {
            lua_pushinteger(L, uniIntDist(generator));
            return 1;
        }
    case 1:
        {
            int isnum;
            int a = lua_tointegerx(L, 1, &isnum);
            if(isnum)
            {
                lua_pushinteger(L, uniIntDist(generator) % a);
                return 1;
            }
        }
    case 2:
        {
            int isnum;
            int a = lua_tointegerx(L, 1, &isnum);
            if(isnum)
            {
                int b = lua_tointegerx(L, 2, &isnum);
                if(isnum)
                {
                    lua_pushinteger(L, a + uniIntDist(generator) % b);
                    return 1;
                }
            }
        }
    }
    LoggingSystem::Error("Invalid call to random.int");
    return 0;
}

int lua_random_uniform(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushnumber(L, uniRealDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}

int lua_random_normal(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushnumber(L, normalDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}

int lua_random_poisson(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushinteger(L, poissonDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}

int lua_random_gamma(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushnumber(L, gammaDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}

int lua_random_cauchy(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushnumber(L, cauchyDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}

int lua_random_chiSquared(lua_State* const L)
{
    if(lua_gettop(L) == 0)
    {
        lua_pushnumber(L, chiSquaredDist(generator));
        return 1;
    }
    LoggingSystem::Error("Invalid call to random.uniform");
    return 0;
}
*/
