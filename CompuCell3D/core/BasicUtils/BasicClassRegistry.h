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

#ifndef BASICCLASSREGISTRY_H
#define BASICCLASSREGISTRY_H

#include "BasicException.h"
#include "BasicClassFactoryBase.h"
#include "memory_include.h"

#include <string>
#include <map>

template <class T>
class BasicClassRegistry {
     using factoryMap_t = std::map<std::string, std::unique_ptr<BasicClassFactoryBase<T> > >;
     factoryMap_t factoryMap;

public:
     BasicClassRegistry() {}

     ~BasicClassRegistry() {}

     std::unique_ptr<BasicClassFactoryBase<T>> unregisterFactory ( const std::string id ) 
	 {
          auto factory = std::move(factoryMap[id]);
          factoryMap.erase ( id );
          return factory;
     }

     void registerFactory ( std::unique_ptr<BasicClassFactoryBase<T> > factory,
                            const std::string id ) {
          ASSERT_OR_THROW ( std::string ( "registerFactory() Factory with id '" ) +
                            id + "' already registered",
                            factoryMap.find ( id ) == factoryMap.end() );

          factoryMap[id] = std::move(factory);
     }

     std::unique_ptr<T> create ( const std::string id ) {
          auto factory = factoryMap[id];
          ASSERT_OR_THROW ( std::string ( "create() Factory '" ) + id + "' not found!",
                            factory );

          return std::make_unique<T> ( factory->create() );
     }
};

#endif
