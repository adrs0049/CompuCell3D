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

#include "BasicClassGroupFactory.h"

#include "BasicClassFactoryBase.h"
#include "BasicClassAccessorBase.h"
#include "BasicClassGroup.h"

#include <vector>
#include "memory_include.h"

#include "BasicException.h"

BasicClassGroupFactory::~BasicClassGroupFactory()
{
	// DO I EVEN NEED THIS??
//  	for (auto& factory : classFactories)
//  		factory.reset(nullptr);
}

void BasicClassGroupFactory::registerClass(std::shared_ptr<BasicClassAccessorBase> accessor)
{
    classFactories.emplace_back(accessor->createClassFactory());
    accessor->setId(classFactories.size()-1);
    classAccessors.push_back(accessor);
}

std::shared_ptr<BasicClassGroup> BasicClassGroupFactory::create()
{
	std::vector<std::shared_ptr<void> > classes {};
	classes.reserve(classFactories.size());
	
	for (auto& factory : classFactories)
		classes.emplace_back( factory->create() );
			
    return std::make_shared<BasicClassGroup>(classes);
}

void BasicClassGroupFactory::destroy(std::shared_ptr<BasicClassGroup>& group) 
{
    ASSERT_OR_THROW("BasicClassGroupFactory NULL group pointer!", group);

	for (auto& accessor : classAccessors)
		accessor->deallocateClass(group);

    group.reset();
}
