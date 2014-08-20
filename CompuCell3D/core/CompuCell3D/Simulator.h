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

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <CompuCell3D/CompuCellLibDLLSpecifier.h>

#include "PluginManager.h"
#include "Plugin.h"
#include "PluginBase.h"
#include "Steppable.h"
#include <map>
#include <vector>
#include <string>
#include <boost/concept_check.hpp>
#include <CompuCell3D/Potts3D/Potts3D.h>

#include <BasicUtils/memory_include.h>

#include <CompuCell3D/ParseData.h>
#include <CompuCell3D/PottsParseData.h>
#include <CompuCell3D/ParserStorage.h>
#include <CompuCell3D/CC3DEvents.h>
#include <CompuCell3D/CC3D_fwd.h>

class CC3DXMLElement;
class CustomStreamBufferBase;

namespace CompuCell3D
{
class ClassRegistry;
class BoundaryStrategy;

template <typename Y> class Field3DImpl;
class Serializer;
class PottsParseData;
class ParallelUtilsOpenMP;

class COMPUCELLLIB_EXPORT Simulator
{
    std::unique_ptr<ClassRegistry> classRegistry;

    Potts3D potts;

    int currstep;
    bool simulatorIsStepping;
    bool readPottsSectionFromXML;
    std::map<std::string,Field3D<float>*> concentrationFieldNameMap;
    //map of steerable objects
    std::map<std::string,SteerableObject *> steerableObjectMap;

    std::vector<Serializer*> serializerVec;
    std::string recentErrorMessage;
    bool newPlayerFlag;

    std::streambuf * cerrStreamBufOrig;
    std::streambuf * coutStreamBufOrig;
    CustomStreamBufferBase * qStreambufPtr;

    std::string basePath;
    bool restartEnabled;

public:

    ParserStorage ps;
    PottsParseData * ppdCC3DPtr;
    PottsParseData ppd;
    PottsParseData *ppdPtr;
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP *pUtilsSingle; // stores same information as pUtils but assumes that we use only single CPU - used in modules for which user requests single CPU runs e.g. Potts with large cells

    void setOutputRedirectionTarget ( long  _ptr );
    long getCerrStreamBufOrig();
    void restoreCerrStreamBufOrig ( long _ptr );

    void setRestartEnabled ( bool _restartEnabled )
    {
        restartEnabled=_restartEnabled;
    }
    bool getRestartEnabled()
    {
        return restartEnabled;
    }

    static PluginManager<Plugin> pluginManager;
    static PluginManager<Steppable> steppableManager;
    static BasicPluginManager<PluginBase> pluginBaseManager;
    Simulator();
    virtual ~Simulator();

    //Error handling functions
    std::string getRecentErrorMessage()
    {
        return recentErrorMessage;
    }
    void setNewPlayerFlag ( bool _flag )
    {
        newPlayerFlag=_flag;
    }
    bool getNewPlayerFlag()
    {
        return newPlayerFlag;
    }

    std::string getBasePath()
    {
        return basePath;
    }
    void setBasePath ( std::string _bp )
    {
        basePath=_bp;
    }

    ParallelUtilsOpenMP * getParallelUtils()
    {
        DBG_ONLY ( cerr<<"putils="<<pUtils<<endl; );
        DBG_ONLY ( ASSERT_OR_THROW ( "pUtils is NULL", pUtils!=nullptr ) );
        return pUtils;
    }
    ParallelUtilsOpenMP * getParallelUtilsSingleThread()
    {
        DBG_ONLY ( ASSERT_OR_THROW ( "pUtilsSingle is NULL", pUtilsSingle!=nullptr ) );
        return pUtilsSingle;
    }

    BoundaryStrategyPtr getBoundaryStrategy();
    void registerSteerableObject ( SteerableObject * );
    void unregisterSteerableObject ( const std::string & );
    SteerableObject * getSteerableObject ( const std::string & _objectName );

    void setNumSteps ( unsigned int _numSteps )
    {
        ppdCC3DPtr->numSteps=_numSteps;
    }
    unsigned int getNumSteps()
    {
        return ppdCC3DPtr->numSteps;
    }
    int getStep()
    {
        return currstep;
    }
    bool isStepping()
    {
        return simulatorIsStepping;
    }
    double getFlip2DimRatio()
    {
        return ppdCC3DPtr->flip2DimRatio;
    }
    Potts3D* getPotts()
    {
        return &potts;
    }
//     std::shared_ptr<Simulator> getSimulatorPtr()
//     {
//         return shared_from_this();
//     }
    std::unique_ptr<ClassRegistry>& getClassRegistry()
    {
        return classRegistry;
    }

    void registerConcentrationField ( std::string _name,Field3D<float>* _fieldPtr );
    std::map<std::string,Field3D<float>*> & getConcentrationFieldNameMap()
    {
        return concentrationFieldNameMap;
    }
    void postEvent ( CC3DEvent & _ev );

    std::vector<std::string> getConcentrationFieldNameVector();
    Field3D<float>* getConcentrationFieldByName ( std::string _fieldName );

    void registerSerializer ( Serializer * _serializerPtr )
    {
        serializerVec.push_back ( _serializerPtr );
    }
    virtual void serialize();

    // Begin Steppable interface
    void start();
    void extraInit(); ///initialize plugins after all steppables have been initialized
    void step ( const unsigned int currentStep );
    void finish();
    // End Steppable interface

    //these two functions are necessary to implement proper cleanup after the simulation
    //1. First it cleans cell inventory, deallocating all dynamic attributes - this has to be done before unloading modules
    //2. It unloads dynamic CC3D modules - pluginsd and steppables
    void cleanAfterSimulation();
    //unloads all the plugins - plugin destructors are called
    void unloadModules();

    void initializePottsCC3D ( CC3DXMLElement * _xmlData );
    void processMetadataCC3D ( CC3DXMLElement * _xmlData );

    void initializeCC3D();
    void setPottsParseData ( PottsParseData * _ppdPtr )
    {
        ppdPtr=_ppdPtr;
    }
    CC3DXMLElement * getCC3DModuleData ( std::string _moduleType,std::string _moduleName="" );
    void updateCC3DModule ( CC3DXMLElement *_element );
    void steer();
};

typedef Simulator* SimulatorPtr;

} // end namespace
#endif
