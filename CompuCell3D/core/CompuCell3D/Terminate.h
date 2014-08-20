#ifndef TERMINATE_H
#define TERMINATE_H

#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#   define __USE_GNU
#endif

#include <cstdlib>
#include <cstdio>
#include <exception>
#include <stdexcept>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <cxxabi.h>
#include <ucontext.h>
#include <string.h>

#include <mutex>

#include <BasicUtils/BasicException.h>
#include <PublicUtilities/ParallelUtilsOpenMP.h>

namespace Error {
	
typedef struct _sig_ucontext 
{
    unsigned long uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    struct sigcontext uc_mcontext;
    sigset_t uc_sigmask;    
} sig_ucontext_t;
	
extern "C" void signal_handler_compl(int sig_num,  siginfo_t * info,  void * ucontext);
extern "C" void terminate_handler_impl(void); 

class TerminalCatcher
{
	static constexpr std::size_t array_sz = 128;
	struct sigaction sigact;
	
	ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;
	
// 	std::mutex mut;
	
	TerminalCatcher() 
	{
		lockPtr=new ParallelUtilsOpenMP::OpenMPLock_t;
		pUtils->initLock ( lockPtr );
		
 		std::set_terminate(terminate_handler_impl);
		sigact.sa_sigaction = signal_handler_compl;
		sigact.sa_flags = SA_RESTART | SA_SIGINFO;
    
		if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0)
		{
			std::fprintf(stderr, "error setting signal handler for %d (%s)\n",
               SIGSEGV, strsignal(SIGSEGV));
        
			exit(EXIT_FAILURE);
		}
	}
	
	/*void signalHandler(int sig)
	{
		std::fprintf(stderr, "Error: signal %d\n", sig);
		printStackTrace();
		std::abort();
	} */

	
	void printStackTrace()
	{
		//crit_err_hdlr();
		void *array[array_sz];
		std::size_t size = backtrace(array, array_sz);
		backtrace_symbols_fd(array, size, STDERR_FILENO);
	} 
	
	friend void Error::terminate_handler_impl(void);
	
	void terminateHandler()
    {
        std::exception_ptr exptr = std::current_exception();

        if ( exptr != 0 )
        {
            try 
            {
                std::rethrow_exception(exptr);
            }
            catch ( std::exception &e )
            {
                std::fprintf(stderr, "Terminated due to exception: %s\n", e.what());
				printStackTrace();
            }
            catch ( BasicException &e )
			{
				std::fprintf(stderr, "Terminated due to BasicException: %s\n", e.what());
			}
            catch(...)
            {
                std::fprintf(stderr, "Terminated due to unknown reason:(\n");
				printStackTrace();
            }
        }
        else
        {
            std::fprintf(stderr, "Terminated due to unknown reason :(\n");
			printStackTrace();
        }
        std::abort();
    }
	
	friend void Error::signal_handler_compl(int sig_num,  siginfo_t * info,  void * ucontext);
	
	void crit_err_hdlr(int sig_num,  siginfo_t * info,  void * ucontext)
    {
// 		std::lock_guard<std::mutex> hold { mut };
		
        sig_ucontext_t * uc = (sig_ucontext_t *) ucontext;

#if defined(__i386__)
        void * caller_address = (void *) uc->uc_mcontext.eip;
#elif defined(__x86_64__)
        void * caller_address = (void *) uc->uc_mcontext.rip;
#else
#error Unsupported architecture
#endif

		std::cerr    << "signal " <<  sig_num
        << " (" <<  strsignal(sig_num) << "), address is "
        << info->si_addr <<  " from " <<  caller_address
        << std::endl <<  std::endl;

        void * array[array_sz];
        std::size_t size = backtrace(array,  array_sz);

        array[1] = caller_address;

        char ** messages = backtrace_symbols(array,  size);

        //  skip first stack frame it points here
        for (unsigned int i = 1; i < size && messages != NULL; ++i)
        {
            char * mangled_name = NULL,  *offset_begin = NULL,  *offset_end = NULL;

            //  find parantheses and +address offset surround mangled name
            for (char *p = messages[i]; *p; ++p)
            {
                if (*p ==  '(')
                {
                    mangled_name = p;
                }
                else if (*p ==  '+')
                {
                    offset_begin = p;
                }
                else if (*p == ')')
                {
                    offset_end = p;
                    break;
                }

            }
            
            //  if the line could be processed,  attempt to demangle the symbol
            if (mangled_name && offset_begin && offset_end && mangled_name < offset_begin)
            {
                *mangled_name++ ='\0';
                *offset_begin++ = '\0';
                *offset_end++ = '\0';
                
                int status;
                char * real_name = abi::__cxa_demangle(mangled_name,  0,  0,  &status);
                
                //  if demangling is successful,  output the demangled name
                if (status ==  0)
                {
                    std::cerr << "[bt]: (" <<  i <<  ") " <<  messages[i] <<  " : "
                  <<  real_name <<  "+" <<  offset_begin <<  offset_end 
                  << std::endl;
                  
                }                                           //  otherwise output mangled
                else 
                {
                    std::cerr <<  "[bt]: (" <<  i <<  ") " <<  messages[i] <<  " : "
                  << mangled_name <<  "+" <<  offset_begin <<  offset_end 
                  << std::endl;
                }
                free(real_name);
            }
            else                                            //  otherwise print whole line
            {
                std::cerr << "[bt]: (" << i <<  ") " <<  messages[i] <<  std::endl;
            }
        }
        std::cerr <<  std::endl << std::endl;
        
        if (messages) free(messages);
		
        exit(EXIT_FAILURE);
    }
    
public:
	~TerminalCatcher() 
	{
		pUtils->destroyLock ( lockPtr );
		delete lockPtr;
		lockPtr=nullptr;
	}
	
	static void init()
	{
		static TerminalCatcher instance;
	}
};

static TerminalCatcher * signal_object;
// extern "C" void signal_handler(int signum) { signal_object->signalHandler(signum); }

extern "C" void signal_handler_compl(int sig_num,  siginfo_t * info,  void * ucontext)
{ signal_object->crit_err_hdlr(sig_num, info, ucontext); }

extern "C" void terminate_handler_impl(void) { signal_object->terminateHandler(); }

} // end namespace

#endif