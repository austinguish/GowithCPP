# GO with C/CPP

## directly using C

You can directly using `C` in the golang. Just include the headers and the `C` code at the top.

```go
package main

// #include <stdio.h>
// #include <stdlib.h>
//
// static void myprint(char* s) {
//   printf("%s\n", s);
// }
import "C"
import "unsafe"

func main() {
	cs := C.CString("Hello from stdio")
	C.myprint(cs)
	C.free(unsafe.Pointer(cs))
}
```

## using  static library

you should generate the object file with

`gcc -o foo.o foo.c` compile to object file

`ar rcs libfoo.a foo.o` archive the object file

to include and load the static library

```go
package main

//#cgo CFLAGS: -I./cheader
//#cgo LDFLAGS: -L${SRCDIR}/clib -lnumber
//
//#include "number.h"
import "C"
import "fmt"

func main() {
	fmt.Println(C.number_add_mod(10, 5, 12))
}
```

## using  dynamic library(recommend)

the cgo cannot use the cpp directly must be packaged in a c.

let's start with a cpp-lua example.

the luatest.cpp

```cpp
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

```

the luatest.h

```cpp
#ifndef CWITHLUA_LUATEST_H
#define CWITHLUA_LUATEST_H
void luaTest();
#endif //CWITHLUA_LUATEST_H
```

we must use a bridge or interface to package the cpp code

the bridge.h

```cpp

#ifdef __cplusplus
extern "C" {
#endif
void LuaTest();
#ifdef __cplusplus
}
#endif

```

the bridge.c

```cpp
#include "bridge.h"
#include "luatest.h"
#ifdef __cplusplus
extern "C"{
#endif
void LuaTest(){
    luaTest();
}
#ifdef __cplusplus
}
#endif
```

complie to dynamic lib

`g++ luatest.cpp bridge.c -std=c++14 -L/Users/austinguish/Desktop/backend/CWithLua/lib -llua -I/Users/austinguish/Desktop/backend/CWithLua/include  -fPIC -shared -o liblt.so `

to compile the lib we need add `-L` `-I` flags to tell the compiler where the lua header and liblua are.

 the directory must be **absolute directory** not the **relative directory**!!!

write go code to use the dynamic library

```go
package main
//#cgo CFLAGS: -I${SRCDIR}/cheader
//#cgo LDFLAGS: -lstdc++ -L${SRCDIR}/clib -llt
//#include "bridge.h"
import "C"

func main() {
	C.LuaTest()
}
```

go build this go file.

`go build gowithdlib.go`

and run this prog

`./gowithdlib`

you 'll get this error:

`dyld: Library not loaded: liblt.so`

You should change the prog's dynamic lib dir.

On Mac:

1. check the prog

   `otool -L gowithdlib`

2. install the correct dir to this prog

`install_name_tool -change liblt.so  /Users/austinguish/go/src/GoWIthC/clib/liblt.so gowithclib`

3. then run

   `./gowithdlib`

4. the result

   ```shell
   ➜  GoWIthC ./gowithclib
   1.0.0
   300
   ```

   
