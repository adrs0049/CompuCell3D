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

#ifndef BASICCLASSGROUP_H
#define BASICCLASSGROUP_H

#include <vector>
#include "memory_include.h"
#include "BasicException.h"

/**
 * A group of classes allocated by BasicClassGroupFactory.
 * Should not be accessed directly, but rather through an instance of
 * BasicClassGroupAccessor.
 */
class BasicClassGroup {
public:
	virtual ~BasicClassGroup() {}

protected:
	/**
     * @param classes The class array.
     */
    BasicClassGroup(std::vector<std::shared_ptr<void> > classes)
        : classes{classes}
    {}
	
    std::vector<std::shared_ptr<void> > classes;
	std::size_t size() const { return classes.size(); }
    
    /**
     * A BasicException will be thrown if the id is invalid.
     *
     * @param id A class id as assigned by BasicClassGroupFactory.
     *
     * @return A pointer to the allocate memory for this class id.
     */
    std::shared_ptr<void> getClass(const unsigned int id) {
        ASSERT_OR_THROW("BasicClassGroup id out of range!",
                        0 <= id && id < classes.size());
        return classes[id];
    }

    friend class BasicClassGroupFactory;
    friend class BasicClassAccessorBase;
};

#endif
