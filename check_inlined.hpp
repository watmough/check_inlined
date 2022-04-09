///////////////////////////////////////////////////////////////////////////////
// M.I.T. License
//
// Copyright (c) 2022 Jonathan Adams Watmough
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files
// (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CHECK_INLINED_HPP
#define CHECK_INLINED_HPP

#include <cstdint>									// needed for uintptr_t

// CHECK_INLINE_HPP
//
// Include this file to provide the macro: CHECK_INLINED(CLASSNAME,FUNCTION,BOOLEAN)
//
// CLASSNAME ... Name of the  containing class
// FUNCTION  ... Name of the current function
// BOOLEAN   ... Name of a bool variable to be created

// GENSYM
// 
// call this macro to generate a symbol
// https://stackoverflow.com/a/1597129/33758
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define GENSYM TOKENPASTE2(gensym_,__LINE__)

// // PRINTSYM
// //
// // call to print a GENSYM
// #define PRINTSYM(P) \
// 	printf("%s","" #P "");

// // TEST_PRINTSYM
// //
// // test code to verify that PRINTSYM works
// #define TEST_PRINTSYM(X,VAL) \
// 	auto X = VAL; \
//     printf("Test. Assigned "); { PRINTSYM(X) } printf(": %d\n",X);

// uintptr_t __attribute((noinline)) RETURN_ADDRESS()
//
// call to get the return address / current executing location.
uintptr_t __attribute((noinline)) RETURN_ADDRESS() {
	return (uintptr_t) __builtin_extract_return_addr (__builtin_return_address (0));
}

// SIMPLE EXAMPLE 1 - just checking current function address versus where the
// program counter is. [Simple case for static functions.]
// 
//  uintptr_t PC_ADDR = executing_address();
//  printf("FUNCTION: %lx executing at: %lx inlined: %s\n", FUNCTION,
//  	   PC_ADDR, (PC_ADDR-(uintptr_t)FUNCTION>0x40) ? "INLINED" : "NOT INLINED");

// SIMPLE EXAMPLE 2 - just checking current function address versus where the
// program counter is. [More complex for instance functions.]
//
//	uintptr_t executing_at = executing_address();
//	void (dummy::*addr)() = &dummy::func1;
//	uintptr_t vaddr = (uintptr_t)(void*&)addr;
//	printf("func1: %lx executing at: %lx inlined: %s\n", 
//		(uintptr_t)vaddr, executing_at, 
//		(executing_at-vaddr>0x40) ? "INLINED" : "NOT INLINED");

// TRACK_INLINED0
//
// Blackadder: "As cunning as a fox who's just been appointed Professor of 
// Cunning at Oxford University?"
#define CHECK_INLINED0(CLASSNAME,FUNCTION,VAR,BOOLEAN)					\
	uintptr_t EXEC_ADDR = RETURN_ADDRESS(); /*Do this first!*/			\
	uintptr_t FUNC_ADDR ; bool BOOLEAN;									\
/*	Testing code to compare error messages for using decltype()     */	\
/*	Works for both static and regular class functions.			    */	\
/*	{          auto I{32};             	I = &CLASS::FUNC;}			*/	\
/*	{          decltype (&CLASS::FUNC) 	F = I           ;}			*/	\
	decltype (&CLASSNAME::FUNCTION) VADDR = &CLASSNAME::FUNCTION;		\
	                            FUNC_ADDR = (uintptr_t)(void*&)VADDR;	\
/*	  printf("%s. %s::%s executing at: %lx ",__PRETTY_FUNCTION__,   */  \
/*	  		 "" #CLASS "","" #FUNC "",PC); { PRINTSYM(VAR) };       */  \
/*	  printf("Address: %lx Diff: %lx Inlined: %s\n", VAR, PC-VAR,   */  \
/*		 	 (PC-VAR>0x40) ? "yes" : "no");                         */  \
      BOOLEAN = (EXEC_ADDR-FUNC_ADDR>0x40);

// CHECK_INLINED(CLASSNAME,FUNCTION,BOOLEAN)
//
// call this macro from a class static or instance function to identify whether
// we are executing within  64 bytes of the start of the function (not inlined).
// otherwise, the function code has been copied somewhere else, and we are inlined.
//
#define CHECK_INLINED(CLASSNAME,FUNCTION,BOOLEAN) \
		CHECK_INLINED0(CLASSNAME,FUNCTION,GENSYM,BOOLEAN)

#endif /* CHECK_INLINED_HPP */