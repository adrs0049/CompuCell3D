#ifndef THREAD_EXCEPTION_H
#define THREAD_EXCEPTION_H
/*
#include <stdexcept>
#include <mutex>

class ThreadException 
{
    std::exception_ptr Ptr;
    std::mutex Lock;
    
public:
    ThreadException() : Ptr(nullptr) {}
    ~ThreadException() {}
    
    void Rethrow()
    {
        if (this->Ptr) std::rethrow_exception(this->Ptr);
    }
    void CaptureException()
    {
        std::unique_lock<std::mutex> guard(this->Lock);
        this->Ptr = std::current_exception();
    }
    
    template <typename Function, typename... Parameters>
    void Run(Function f, Parameters... params)
    {
        try 
        {
            f(params...);
        }
        catch (...)
        {
            CaptureException();
        }
    }
};

ThreadException e;

#pragma omp parallel for
for (int i = 0; i < n; i++)
{
    e.Run([=]{
        // code that might throw
        // ...
    });
}
e.Rethrow()
*/
#endif