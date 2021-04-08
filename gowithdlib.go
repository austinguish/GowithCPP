package main
//#cgo CFLAGS: -I${SRCDIR}/cheader
//#cgo LDFLAGS: -lstdc++ -L${SRCDIR}/clib -llt
//#include "bridge.h"
import "C"

func main() {
	C.LuaTest()
}