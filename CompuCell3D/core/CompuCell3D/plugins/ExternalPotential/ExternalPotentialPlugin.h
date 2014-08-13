#ifndef COMPUCELL3DEXTERNALPOTENTIALPLUGIN_H
#define COMPUCELL3DEXTERNALPOTENTIALPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ExternalPotentialDLLSpecifier.h"

namespace CompuCell3D
{

/**
@author m
*/

class Simlator;
class Potts3D;
class AdjacentNeighbor;
template <class T> class Field3D;
class Potts3D;

class EXTERNALPOTENTIAL_EXPORT ExternalPotentialParam
{
public:
    ExternalPotentialParam()
    {
        lambdaVec.x=0.0;
        lambdaVec.y=0.0;
        lambdaVec.z=0.0;
    }
    Coordinates3D<float> lambdaVec;
    std::string typeName;
};

class EXTERNALPOTENTIAL_EXPORT ExternalPotentialPlugin : public Plugin,public EnergyFunction
{
private:
    Potts3D *potts;
    AdjacentNeighbor  adjNeighbor;
    CC3DXMLElement * xmlData;
    Point3D boundaryConditionIndicator;
    BoundaryStrategyPtr boundaryStrategy;

    //EneryFunction data
    Coordinates3D<float> lambdaVec;

    AdjacentNeighbor  * adjNeighbor_ptr;
    WatchableField3D<CellG *> *cellFieldG;
    Dim3D fieldDim;
    enum FunctionType {GLOBAL=0,BYCELLTYPE=1,BYCELLID=2};
    FunctionType functionType;
    std::vector<ExternalPotentialParam> externalPotentialParamVector;

    typedef double ( ExternalPotentialPlugin::*changeEnergy_t ) ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

    ExternalPotentialPlugin::changeEnergy_t changeEnergyFcnPtr;
    double changeEnergyGlobal ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellType ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellId ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

    //COM based functions
    double changeEnergyGlobalCOMBased ( const Point3D &pt,  const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellTypeCOMBased ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );
    double changeEnergyByCellIdCOMBased ( const Point3D &pt, const CellG *newCell,const CellG *oldCell );

    std::set<unsigned char> participatingTypes;

public:
    ExternalPotentialPlugin();

    ~ExternalPotentialPlugin();
    //plugin interface
    virtual void init ( Simulator *_simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( Simulator *_simulator ) override;
    //energyFunction interface
    virtual double changeEnergy ( const Point3D &pt,
                                  const CellG *newCell,
                                  const CellG *oldCell ) override;

    //steerable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;
    void initData();
};

} // end namespace

#endif