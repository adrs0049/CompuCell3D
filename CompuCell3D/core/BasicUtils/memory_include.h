#ifndef MEMORY_H_H
#define MEMORY_H_H

#include <memory>

#if __cplusplus <= 201103L

namespace std {

template<typename T>
struct MakeUnique { typedef unique_ptr<T> single_object; };

template<typename T>
struct MakeUnique<T[]> { typedef unique_ptr<T[]> array; };

template<typename T, size_t bound>
struct MakeUnique<T[bound]> { struct invalid_type { }; };
    
template<typename T, typename ...Args>
inline typename MakeUnique<T>::single_object
make_unique( Args&& ...args)
{
    return unique_ptr<T>( new T(forward<Args>(args)... ));
}

template<typename T>
inline typename MakeUnique<T>::array
make_unique( size_t size )
{
    return unique_ptr<T>( new typename remove_extent<T>::type[size]() );
}

template<typename T, typename ...Args>
inline typename MakeUnique<T>::invalid_type
make_unique(Args&& ...) = delete;

} // end namespace

#endif

#endif