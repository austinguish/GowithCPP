//
// Created by 蒋逸伟 on 2021/4/7.
//
#include <iostream>
#ifdef __cplusplus
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#else
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif
const char *lua_script = R"(
version = '1.0.0'
function add(a, b)
    return a + b;
end
)";
void luaTest(){
    lua_State *L = luaL_newstate();
    if (L == nullptr) {
        //std::cerr << "create state failed!" << std::endl;
        exit(-1);
    }

    if (luaL_dostring(L, lua_script) != 0) {
        //std::cerr << "cannot run lua script!" << std::endl;
        exit(-1);
    }

    // get global variable
    lua_getglobal(L, "version");
    const char *version = lua_tostring(L, -1);
    printf("%s\n",version);
    //std::cout << "version: " << version << std::endl;
    lua_pop(L, 1);

    // call global function
    lua_getglobal(L, "add");
    int a = 100;
    int b = 200;
    lua_pushnumber(L, a);
    lua_pushnumber(L, b);
    if (lua_pcall(L, 2, 1, 0) != 0) {
        //std::cerr << "call func failed, got error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        exit(-1);
    }

    int sum = lua_tonumber(L, -1);
    lua_pop(L, 1);
    printf("%d\n",sum);

    //std::cout << a << " + " << b << " = " << sum << std::endl;
    lua_close(L);
}
