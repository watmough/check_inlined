# README.md

## check_inlined

Header-only (macro) to *quickly* check if this function is inlined.

### Motivation

I have recently been benchmarking some code that uses it's own string 
implementation, which is heavily used, so knowing how many calls, 
what's inlined etc., is useful to the process of documenting whether
it would be worth tearing out the code and switching to std::string.

However, whilst timing functions is quite easy, identifying whether a 
particular function has been inlined or not can be surprisingly tricky. 

There is some good material around:

[StackOverflow - How will i know whether inline function is actually replaced at the place where it is called or not?](https://stackoverflow.com/a/62343845/33758)

[JohnysSWLab - make-your-programs-run-faster-avoid-function-calls](https://johnysswlab.com/make-your-programs-run-faster-avoid-function-calls/#bonus)


However, I needed a method that I could use for both instance and static
class functions, and I wanted something super light.

### How to Use the Library

Just include it, then you can do stuff like this:

```C++
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


///////////////////////////////////////////////////////////////////////////////
// main.cpp
//
// Simple demo of check_inlined.hpp.
// Creates a class and each function is able to check whether in-lined or not.
//

#include "./check_inlined.hpp"
#include <cstdio>

int GLOBALX{0};

class SimpleInlineTest {
public:
	// func3 - INLINED
	static inline __attribute((always_inline)) void func3()
	{
		CHECK_INLINED(SimpleInlineTest,func3,fINLINED)
		printf("%40s %6s inlined.\n", __PRETTY_FUNCTION__, fINLINED ? "IS" : "IS NOT");
		GLOBALX++;
	}

	// func2 - NOT INLINED
	static void __attribute((noinline)) func2()
	{
		CHECK_INLINED(SimpleInlineTest,func2,fINLINED)
		printf("%40s %6s inlined.\n", __PRETTY_FUNCTION__, fINLINED ? "IS" : "IS NOT");
		GLOBALX++;
		func3();
	}

	// func1 - NOT INLINED (can be switched)
	// func1 - INLINED (can be switched)
	inline void __attribute((always_inline)) func1()
	//void __attribute((noinline)) func1()
	{
		CHECK_INLINED(SimpleInlineTest,func1,fINLINED)
		printf("%40s %6s inlined.\n", __PRETTY_FUNCTION__, fINLINED ? "IS" : "IS NOT");
		GLOBALX++;
		func2();
	}

	// func0 - NOT INLINED
	void __attribute((noinline)) func0()
	{
		CHECK_INLINED(SimpleInlineTest,func0,fINLINED)
		printf("%40s %6s inlined.\n", __PRETTY_FUNCTION__, fINLINED ? "IS" : "IS NOT");
		GLOBALX++;
		func1();
	}
};

int main()
{
	GLOBALX=0;
	SimpleInlineTest simpleTest;
	printf("\nmain.cpp: A test of check_inlined.hpp macro: \n\nCHECK_INLINED(CLASSNAME,FUNCTION,IS_INLINED)\n"
			"if (IS_INLINED) { DoThis(); } else { DoThat(); };\n\n");
	simpleTest.func0();
	printf("\nTry building with -O0 or -O3 or -Os.\n\n");
}

```

### Example `main.cpp` Output for Different Optimization Settings

#### -O0

```
(base) [jonathan@centos7-dev check_inlined]$ g++ main.cpp -O0
(base) [jonathan@centos7-dev check_inlined]$ ./a.out 

main.cpp: A test of check_inlined.hpp macro: 
--------------------------------------------
CHECK_INLINED(CLASSNAME,FUNCTION,IS_INLINED)
if (IS_INLINED) { DoThis(); } else { DoThat(); };

          void SimpleInlineTest::func0() IS NOT inlined.
          void SimpleInlineTest::func1()     IS inlined.
   static void SimpleInlineTest::func2() IS NOT inlined.
   static void SimpleInlineTest::func3()     IS inlined.

Try building with -O0 or -O3 or -Os.
```

#### -O3

```
(base) [jonathan@centos7-dev check_inlined]$ g++ main.cpp -O3
(base) [jonathan@centos7-dev check_inlined]$ ./a.out 

main.cpp: A test of check_inlined.hpp macro: 
--------------------------------------------
CHECK_INLINED(CLASSNAME,FUNCTION,IS_INLINED)
if (IS_INLINED) { DoThis(); } else { DoThat(); };

          void SimpleInlineTest::func0()     IS inlined.
          void SimpleInlineTest::func1()     IS inlined.
   static void SimpleInlineTest::func2() IS NOT inlined.
   static void SimpleInlineTest::func3()     IS inlined.

Try building with -O0 or -O3 or -Os.
```

#### -Os

```
(base) [jonathan@centos7-dev check_inlined]$ g++ main.cpp -Os
(base) [jonathan@centos7-dev check_inlined]$ ./a.out 

main.cpp: A test of check_inlined.hpp macro: 
--------------------------------------------
CHECK_INLINED(CLASSNAME,FUNCTION,IS_INLINED)
if (IS_INLINED) { DoThis(); } else { DoThat(); };

          void SimpleInlineTest::func0() IS NOT inlined.
          void SimpleInlineTest::func1()     IS inlined.
   static void SimpleInlineTest::func2() IS NOT inlined.
   static void SimpleInlineTest::func3()     IS inlined.

Try building with -O0 or -O3 or -Os.

(base) [jonathan@centos7-dev check_inlined]$ 
```

