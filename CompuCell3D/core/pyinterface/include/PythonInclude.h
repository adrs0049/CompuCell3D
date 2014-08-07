// Python Include
#ifndef PYTHON_INCLUDE
#define PYTHON_INCLUDE

// stolen from some llvm review
#if defined(__linux__)
	// features.h will define _POSIX_C_SOURCE if _GNU_SOURCE is defined.  This value
	// may be different from the value that Python defines it to be which results
	// in a warning.  Undefine _POSIX_C_SOURCE before including Python.h  The same
	// holds for _XOPEN_SOURCE.
	#undef _POSIX_C_SOURCE
	#undef _XOPEN_SOURCE
#endif

// Include python
#include <Python.h>

#endif