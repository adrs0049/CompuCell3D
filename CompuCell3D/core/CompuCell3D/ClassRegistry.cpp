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

#include "Simulator.h"

#include <BasicUtils/BasicClassFactory.h>
#include <BasicUtils/BasicException.h>
#include <BasicUtils/BasicString.h>

#include "ClassRegistry.h"

#include <string>
using namespace CompuCell3D;
using namespace std;

ClassRegistry::ClassRegistry ( Simulator *simulator )
    : simulator ( simulator )
{}

std::shared_ptr<Steppable> ClassRegistry::getStepper ( string id )
{
    auto stepper = activeSteppersMap[id];
    ASSERT_OR_THROW ( string ( "Stepper '" ) + id + "' not found!", stepper );
    return stepper;
}

void ClassRegistry::extraInit ( Simulator * simulator )
{
    for ( const auto& stepper : activeSteppers )
        stepper->extraInit ( simulator );
}

void ClassRegistry::start()
{
    for ( const auto& stepper : activeSteppers )
    {
        std::cerr << "Starting stepper: " << stepper->toString() << "\n";
        stepper->start();
    }
}

void ClassRegistry::step ( const unsigned int currentStep )
{
    for ( const auto& stepper : activeSteppers )
        if ( stepper->frequency && ( currentStep % stepper->frequency ) == 0 )
            stepper->step ( currentStep );
}

void ClassRegistry::finish()
{
    for ( const auto& stepper : activeSteppers )
    {
        std::cerr << "Finishing stepper: " << stepper->toString() << "\n";
        stepper->finish();
    }
}

void ClassRegistry::addStepper ( std::string _type, std::shared_ptr<Steppable> _steppable )
{
    activeSteppers.push_back ( _steppable );
    activeSteppersMap[_type] = _steppable;
}

void ClassRegistry::initModules ( Simulator *_sim )
{
    std::vector<CC3DXMLElement *> steppableCC3DXMLElementVectorRef = _sim->ps.steppableCC3DXMLElementVector;
    PluginManager<Steppable> &steppableManagerRef=Simulator::steppableManager;

    cerr<<" INSIDE INIT MODULES:"<<endl;
    for ( const auto& elem : steppableCC3DXMLElementVectorRef )
    {
        string type=elem->getAttribute ( "Type" );

        auto steppable = steppableManagerRef.get ( type );
        cerr<<"CLASS REGISTRY INITIALIZING "<<type<<endl;

        steppable->init ( _sim, elem );
        addStepper ( type, steppable );
    }

    for ( auto& stepper : activeSteppers )
        cerr<<"HAVE THIS STEPPER : "<<stepper->getParseData()->moduleName<<endl;;
}