/*******************************************************************\

              Copyright (C) 2003 Joseph Coffland

    This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
        of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
             GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
                           02111-1307, USA.

            For information regarding this software email
                   jcofflan@users.sourceforge.net

\*******************************************************************/


#ifndef BASICEXCEPTION_H
#define BASICEXCEPTION_H

#include "BasicFileLocation.h"

#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include "memory_include.h"

#ifndef HAVE_EXECINFO
#if defined(__linux__)
#define HAVE_EXECINFO 1
#elif defined(__DARWIN_VERS_1050)
#define HAVE_EXECINFO 1
#else
#define HAVE_EXECINFO 0
#endif
#endif

// TEST ONLY
#if HAVE_EXECINFO
#include <execinfo.h>
#include <stdio.h>
#include <cxxabi.h>
#endif

/**
 * BasicException is a general purpose exception class.  It is similar to
 * the java Exception class.  A BasicException can carry a message,
 * a BasicFileLocation and/or a pointer to an exception which was the
 * original cause.
 *
 * There are preprocessor macros that can be used to as a convient way
 * to add the current file, line and column where the exception occured.
 * These are:
 *
 * THROW(const string message)
 *
 * and
 *
 * ASSERT_OR_THROW(const string message, const bool condition)
 *
 * The latter can be used to in place of assert(const bool condition).
 * Throwing an exception instead of aborting overcomes some of the limitations
 * of the standard assert.
 */
class BasicException : public std::exception
{
    std::string message;
    BasicFileLocation location;
    std::shared_ptr<BasicException> cause;
    std::shared_ptr<std::list<std::string> > trace;
    static constexpr std::size_t max_backtrace_size = 128;

    void swap ( BasicException other )
    {
        std::swap ( message, other.message );
        std::swap ( location, other.location );
        std::swap ( cause, other.cause );
        std::swap ( trace, other.trace );
    }

public:
    static unsigned int causePrintLevel;
    static bool enableStackTraces;

    BasicException() : cause ( nullptr ), trace ( nullptr )
    {
        init();
    }

    BasicException ( const std::string& message ) : message {message}, trace ( nullptr )
    {
        init();
    }

    BasicException ( const char* message ) : message {message}, trace ( nullptr )
    {
        init();
    }

    BasicException ( const std::string& message, const BasicFileLocation &location )
        : message {message}, location ( location ), trace ( nullptr )
    {
        init();
    }

    BasicException ( const std::string& _message, BasicException &_cause ) :
        message {_message}, cause {std::make_shared<BasicException> ( _cause ) }, trace ( nullptr )
    {
        init();
    }

    BasicException ( const std::string& _message, const BasicFileLocation &_location,
                     BasicException &_cause ) :
        message {_message}, location ( _location ),
            cause {std::make_shared<BasicException> ( _cause ) }, trace ( nullptr )
    {
        init();
    }

    /// Copy constructor
    BasicException ( const BasicException &e ) :
        message {e.message}, location ( e.location ), cause {e.cause}, trace {e.trace} {}

    BasicException& operator= ( BasicException other )
    {
        BasicException tmp ( other );
        swap ( other );
        return *this;
    }

    virtual ~BasicException() {}

    const std::string getMessage() const
    {
        return message;
    }
    BasicFileLocation getLocation() const
    {
        return location;
    }

    /**
     * @return A BasicSmartPointer to the BasicException that caused this
     *         exception or NULL.
     */
    std::shared_ptr<BasicException> getCause() const
    {
        return cause;
    }

    std::shared_ptr<std::list<std::string> > getTrace() const
    {
        return trace;
    }

    const char* what ( void )
    {
        std::ostringstream stream;
        stream 	<< "Exception in C++ code : "  << message << "\n"
                << "In FILE :"     << location.getFilename() << "\n"
                << "on LINE :"     << location.getLine() << "\n";

        if ( enableStackTraces && trace )
            for ( const auto& message : *trace )
                stream << message;

        // is there a better solution?
        const std::string& tmp = stream.str();

        return tmp.c_str();
    }

    /**
     * Prints the complete exception recuring down to the cause exception if
     * not null.  WARNING: If there are many layers of causes this function
     * could print a very large amount of data.  This can be limited by
     * setting the causePrintLevel variable.
     *
     * @param stream The output stream.
     * @param printLocations Print file locations.
     * @param printLevel The current cause print level.
     *
     * @return A reference to the passed stream.
     */
    std::ostream &print ( std::ostream &stream,
                          bool printLocations = true,
                          unsigned int printLevel = 0 ) const
    {

        if ( printLocations && !location.isEmpty() )
            stream << "@ " << location << " ";

        stream << message;

        if ( enableStackTraces && trace )
            for ( const auto& message : *trace )
                stream << message;

        if ( cause )
        {
            stream << std::endl << " ";

            if ( printLevel > causePrintLevel )
            {
                stream << "Aborting exception dump due to causePrintLevel limit! "
                       << "Increase BasicException::causePrintLevel to see more.";

            }
            else
            {
                stream << "caused by: ";
                cause->print ( stream, printLocations, printLevel );
            }
        }

        return stream;
    }

protected:
    void init()
    {
        if ( enableStackTraces )
        {
            trace = std::make_shared<std::list<std::string>>();

#if HAVE_EXECINFO
            void * callstack[max_backtrace_size];
            std::size_t size = backtrace ( callstack,  max_backtrace_size );

            char ** messages = backtrace_symbols ( callstack,  size );

            //  skip first two stack frame it points here
            for ( unsigned int i = 2; i < size && messages != NULL; ++i )
            {
                char * mangled_name = NULL,  *offset_begin = NULL,  *offset_end = NULL;

                //  find parantheses and +address offset surround mangled name
                for ( char *p = messages[i]; *p; ++p )
                {
                    if ( *p ==  '(' )
                    {
                        mangled_name = p;
                    }
                    else if ( *p ==  '+' )
                    {
                        offset_begin = p;
                    }
                    else if ( *p == ')' )
                    {
                        offset_end = p;
                        break;
                    }
                }

                std::ostringstream backTraceMessageStream;
                //  if the line could be processed,  attempt to demangle the symbol
                if ( mangled_name && offset_begin && offset_end && mangled_name < offset_begin )
                {
                    *mangled_name++ ='\0';
                    *offset_begin++ = '\0';
                    *offset_end++ = '\0';

                    int status;
                    char * real_name = abi::__cxa_demangle ( mangled_name,  0,  0,  &status );

                    //  if demangling is successful,  output the demangled name
                    if ( status ==  0 )
                    {
                        backTraceMessageStream << "[bt]: (" <<  i <<  ") " <<  messages[i] <<  " : "
                                               <<  real_name <<  "+" <<  offset_begin <<  offset_end
                                               << std::endl;

                    }                                           //  otherwise output mangled
                    else
                    {
                        backTraceMessageStream <<  "[bt]: (" <<  i <<  ") " <<  messages[i] <<  " : "
                                               << mangled_name <<  "+" <<  offset_begin <<  offset_end
                                               << std::endl;
                    }
                    free ( real_name );
                }
                else                                            //  otherwise print whole line
                {
                    backTraceMessageStream << "[bt]: (" << i <<  ") " <<  messages[i] <<  std::endl;
                }
                trace->push_back ( backTraceMessageStream.str() );
            }

            free ( messages );

            // When Optimization is turned on functions such as this
            // one are often inlined and not visable to the debugger.
            // This means stack traces for optimized code will often
            // be incomplete.  Here we remove the offset in order
            // to get as much of the stack trace as possible.
#endif
        }
    }

    friend std::ostream &operator<< ( std::ostream &, const BasicException & );
};

/**
 * An stream output operator for BasicException.  This allows you to print the
 * text of an exception to a stream like so:
 *
 * . . .
 * } catch (BasicException &e) {
 *   cerr << e << endl;
 *   return 0;
 * }
 */
inline std::ostream &operator<< ( std::ostream &stream,
                                  const BasicException &e )
{
    e.print ( stream );
    return stream;
}

#define THROW(msg) throw BasicException((msg), FILE_LOCATION)
#define THROWC(msg, cause) throw BasicException((msg), FILE_LOCATION, (cause))
#define ASSERT_OR_THROW(msg, condition) {if (!(condition)) THROW(msg);}

#endif
