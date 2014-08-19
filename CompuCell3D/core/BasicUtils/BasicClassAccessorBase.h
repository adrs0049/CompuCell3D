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

#ifndef BASICCLASSACCESSORBASE_H
#define BASICCLASSACCESSORBASE_H

#include "memory_include.h"
#include "BasicClassGroup.h"
#include "BasicException.h"

template <class T>
class BasicClassFactoryBase;

/**
 * The base class for BasicClassAccessor.
 * See BasicClassGroupFactory.
 */
class BasicClassAccessorBase
{
    int id;

public:
    BasicClassAccessorBase() : id ( -1 ) {}
// for debug
    int getId ( void ) const
	{
		return id;
	}
protected:
	~BasicClassAccessorBase() {}
    virtual std::unique_ptr<BasicClassFactoryBase<void>> createClassFactory() = 0;

    /**
     * Called by on registration by BasicClassGroupFactory to set this accessors
     * id.
     *
     * @param id The assigned id.
     */
    void setId ( const int id )
    {
        this->id = id;
    }

    /**
     * Called by BasicClassAccessor to get a pointer to this accessors class in
     * the group.
     */
    std::shared_ptr<void> getClass ( const std::shared_ptr<BasicClassGroup>& group ) const
    {
		ASSERT_OR_THROW(std::string("BasicClassAccessorBase id ( ") + std::to_string(id) + std::string(" ) is not SET!!\n"),id!=-1);
        return group->getClass ( id );
    }

    virtual void deallocateClass ( std::shared_ptr<BasicClassGroup>& group ) const = 0;
    friend class BasicClassGroupFactory;
};

#endif