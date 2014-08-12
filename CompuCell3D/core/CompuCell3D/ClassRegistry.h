/*************************************************************************
 *    CompuCell - A software framework for multimodel simulations of     *
 * biocomplexity problems Copyright (C) 2003 University of Notre Dame,   *
 *                             Indiana                                   *
 *                                                                       *
 * This program is free software; IF YOU AGREE TO CITE USE OF CompuCell  *
 *  IN ALL RELATED RESEARCH PUBLICATIONS according to the terms of the   *
 *  CompuCell GNU General Public License RIDER you can redistribute it   *
 * and/or modify it under the terms of the GNU General Public License as *
 *  published by the Free Software Foundation; either version 2 of the   *
 *         License, or (at your option) any later version.               *
 *                                                                       *
 * This program is distributed in the hope that it will be useful, but   *
 *      WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    *
 *             General Public License for more details.                  *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *     along with this program; if not, write to the Free Software       *
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
 *************************************************************************/

#ifndef CLASSREGISTRY_H
#define CLASSREGISTRY_H

#include <CompuCell3D/CompuCellLibDLLSpecifier.h>
#include "Steppable.h"
#include <BasicUtils/BasicClassRegistry.h>

#include <map>
#include <list>
#include <string>
#include <vector>

namespace CompuCell3D {
class Simulator;

class COMPUCELLLIB_EXPORT ClassRegistry : public Steppable 
{
    BasicClassRegistry<Steppable> steppableRegistry;

    using ActiveSteppers_t = std::list<std::shared_ptr<Steppable> >;
    ActiveSteppers_t activeSteppers;

    using ActiveSteppersMap_t = std::map<std::string, std::shared_ptr<Steppable> >;
    ActiveSteppersMap_t activeSteppersMap;

    Simulator *simulator;

    std::vector<ParseData *> steppableParseDataVector;

public:
    ClassRegistry(Simulator *simulator);
    virtual ~ClassRegistry() {}

    std::shared_ptr<Steppable> getStepper(std::string id);

    void addStepper(std::string _type, std::shared_ptr<Steppable> _steppable);
    
	// Begin Steppable interface
    virtual void extraInit(Simulator *simulator) override;

    virtual void start() override;
    virtual void step(const unsigned int currentStep) override;
    virtual void finish() override;
    // End Steppable interface

    virtual void initModules(Simulator *_sim);
};

}; // end namespace CompuCell3D
#endif
