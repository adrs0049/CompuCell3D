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
#ifndef BASICARRAY_H
#define BASICARRAY_H

#include "BasicException.h"
#include <vector>

/**
 * A generic implementation of a dynamically allocated array.
 *
 * WARNING class constructors and destructors are not called.  Allocated
 * memory is initially set to zero.  This works well with arrays of pointers
 * or basic data types, but will not work for classes such as std::string
 * which requires the constructor be called.
 */
template <class T>
class BasicArray : public std::vector<T> {
public:

    /**
     * Create an empty array.
     *
     * @param initialCapacity Initially allocate this many spaces.
     */
    BasicArray(unsigned int initialCapacity=0) {
        std::vector<T>::clear();
        if(initialCapacity>0)
            std::vector<T>::assign(initialCapacity,T());
    }

    /**
     * Destruct the array and all the data in it.
     */
    ~BasicArray() {}

    /**
     * @return true if size is zero, false otherwise.
     */
    bool isEmpty() const {
        return std::vector<T>::size() == 0;
    }

    /**
     * @return The current size of the array.
     */
    unsigned int getSize() const {
        return std::vector<T>::size();
    }

    /**
     * Will always be >= to getSize().
     *
     * @return The number of slots currently allocated.
     */
    unsigned int getCapacity() const {
        return std::vector<T>::capacity();
    }

    /**
     * Set size to zero.  Does not effect capacity.
     * Memory is not initialized to zero!
     */
    void clear() {
        std::vector<T>::clear();
    }

    /**
     * Add the array element to the end of the array.  The size of the array will
     * be increased by one.  See the notes on the increase() function.
     *
     * @param x A reference to the new array element
     *
     * @return The index of the new array element.
     */
    unsigned int put(const T &x) {
        std::vector<T>::push_back(x);
        return std::vector<T>::size()-1;
    }

    unsigned int put(T&& x)
	{
		std::vector<T>::push_back(x);
		return std::vector<T>::size()-1;
	}
    
    /**
     * If the index is out of range a BasicException will be thrown.
     * Array indexing starts from zero.
     *
     * @param i The index of the array element.
     *
     * @return A reference to the array element at index i.
     */
    T &get(const unsigned int i) {
        return const_cast<T&>(((*this)[i]));
    }

//     BasicArray<T> & operator=(const BasicArray<T> & rhs) {
//         if(this==&rhs) return *this;
//         static_cast<std::vector<T>&>(*this)=rhs;
//         return *this;
//     }
};

#endif // BASICARRAY_H
