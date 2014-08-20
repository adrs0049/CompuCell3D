#ifndef ENERGYFUNCTIONCALCLUATOR_H
#define ENERGYFUNCTIONCALCLUATOR_H
#include <string>
#include <vector>
#include <map>

#include <BasicUtils/BasicException.h>

class CC3DXMLElement;

namespace CompuCell3D
{

class EnergyFunction;
class CellG;
class Point3D;
class Potts3D;
class Simulator;
class ParseData;

class EnergyFunctionCalculator
{

public:
    EnergyFunctionCalculator ( Simulator* _sim, Potts3D* _potts );
    virtual ~EnergyFunctionCalculator();

    virtual void init ( CC3DXMLElement *_xmlData ) {}
    virtual void registerEnergyFunction ( EnergyFunction *_function );
    virtual void registerEnergyFunctionWithName ( EnergyFunction *_function,std::string _functionName );
    virtual void unregisterEnergyFunction ( std::string _functionName );
    virtual void configureEnergyCalculator ( std::vector<std::string> &_configVector ) {}
    virtual double changeEnergy ( Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell,const unsigned int _flipAttempt );

private:
    void setPotts ( Potts3D * _potts )
    {
        potts=_potts;
    }

public:
    // this should be removed probably
    void setSimulator ( Simulator * _sim )
    {
        ASSERT_OR_THROW ( "sim objects are actually different!\n",sim==_sim );
//         sim=_sim ;
    }
public:
    virtual void setLastFlipAccepted ( bool _accept )
    {
        lastFlipAccepted=_accept;
    }

protected:
    std::vector<EnergyFunction *> energyFunctions;
    std::vector<std::string> energyFunctionsNameVec;

    std::map<std::string,EnergyFunction *> nameToEnergyFuctionMap;
    Potts3D* potts;
    Simulator* sim;

    bool lastFlipAccepted;
};

} // end namespace
#endif