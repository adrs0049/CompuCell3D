#ifndef DEBUG_HELPERS_H
#define DEBUG_HELPERS_H

#if defined(DEBUG_BUILD) | defined (DEBUG) | defined(_DEBUG)

	#include <iostream>
	using std::cerr;
	using std::cout;
	using std::endl;

	#ifndef DBG_ONLY
		#define DBG_ONLY(...) do { __VA_ARGS__; } while (0)
	#endif
	
#else

	#ifndef DBG_ONLY
		#define DBG_ONLY(x)
	#endif
	
#endif

#endif