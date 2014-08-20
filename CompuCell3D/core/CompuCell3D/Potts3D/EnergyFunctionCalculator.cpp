#include "EnergyFunctionCalculator.h"
#include <BasicUtils/BasicException.h>
#include "EnergyFunction.h"
#include <sstream>

using namespace CompuCell3D;
using namespace std;

EnergyFunctionCalculator::EnergyFunctionCalculator ( Simulator* _sim, Potts3D* _potts )
    : potts ( _potts ), sim ( _sim )
{
    ASSERT_OR_THROW ( "EnergyFunctionCalculator() Sim is NULL!\n", sim!=nullptr );
    ASSERT_OR_THROW ( "EnergyFunctionCalculator() Potts3D is NULL!\n", potts!=nullptr );
    DBG_ONLY ( cerr<<"Constructing EnergyFunctionCalculator"<<endl; );
}

EnergyFunctionCalculator::~EnergyFunctionCalculator()
{}

void EnergyFunctionCalculator::registerEnergyFunction ( EnergyFunction *_function )
{
    ASSERT_OR_THROW ( "registerEnergyFunction() function cannot be NULL!",
                      _function );

    ASSERT_OR_THROW ( "Sim Pointer  cannot be NULL!",
                      sim );

    ASSERT_OR_THROW ( "Potts3D Pointer  cannot be NULL!",
                      potts );

    ostringstream automaticNameStream;
    automaticNameStream<<"EnergyFuction_"<<energyFunctions.size()-1;
    string functionName;

    functionName=automaticNameStream.str();
    nameToEnergyFuctionMap.insert ( make_pair ( functionName,_function ) );

    energyFunctions.push_back ( _function );
    energyFunctionsNameVec.push_back ( functionName );
}

void EnergyFunctionCalculator::registerEnergyFunctionWithName ( EnergyFunction *_function,std::string _functionName )
{
    ASSERT_OR_THROW ( "registerEnergyFunction() function cannot be NULL!",
                      _function );

    ASSERT_OR_THROW ( "Sim Pointer  cannot be NULL!",
                      sim );

    ASSERT_OR_THROW ( "Potts3D Pointer  cannot be NULL!",
                      potts );

    ostringstream automaticNameStream;
    automaticNameStream<<"EnergyFuction_"<<energyFunctions.size()-1;
    string functionName;
    if ( _functionName.empty() )
    {
        functionName=automaticNameStream.str();
    }
    else
    {
        functionName=_functionName;
    }
    nameToEnergyFuctionMap.insert ( make_pair ( functionName,_function ) );

    energyFunctions.push_back ( _function );
    energyFunctionsNameVec.push_back ( functionName );
}

void EnergyFunctionCalculator::unregisterEnergyFunction ( std::string _functionName )
{
    auto mitr = nameToEnergyFuctionMap.find ( _functionName );

    if ( mitr==nameToEnergyFuctionMap.end() )
    {
        cerr<<"Sorry, Could not find "<<_functionName<<" energy Function"<<endl;
        return; //plugin name not found
    }
    energyFunctions.erase ( remove ( energyFunctions.begin(),energyFunctions.end(),mitr->second ),energyFunctions.end() );
    energyFunctionsNameVec.erase ( remove ( energyFunctionsNameVec.begin(),energyFunctionsNameVec.end(),_functionName ),energyFunctionsNameVec.end() );
}

double EnergyFunctionCalculator::changeEnergy ( Point3D &pt, const CellG *newCell,const CellG *oldCell,const unsigned int _flipAttempt )
{
    double change {0};
    for ( auto &elem : energyFunctions )
        change += elem->changeEnergy ( pt, newCell, oldCell );

    return change;
}