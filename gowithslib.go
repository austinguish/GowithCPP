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