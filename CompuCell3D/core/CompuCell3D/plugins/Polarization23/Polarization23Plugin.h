
#ifndef POLARIZATION23PLUGIN_H
#define POLARIZATION23PLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>

#include "Polarization23Data.h"
#include "Polarization23DLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{
class Simulator;

class Potts3D;
class Automaton;
class ParallelUtilsOpenMP;

template <class T> class Field3D;
template <class T> class WatchableField3D;

class POLARIZATION23_EXPORT  Polarization23Plugin : public Plugin ,public EnergyFunction
{

private:
    BasicClassAccessor<Polarization23Data> polarization23DataAccessor;
    CC3DXMLElement *xmlData;
    Potts3D *potts;
    Simulator *sim;
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;
    Automaton *automaton;
    BoundaryStrategyPtr boundaryStrategy;
    WatchableField3D<CellG *> *cellFieldG;
    Dim3D fieldDim;

public:

    Polarization23Plugin();
    virtual ~Polarization23Plugin();

    BasicClassAccessor<Polarization23Data> * getPolarization23DataAccessorPtr()
    {
        return & polarization23DataAccessor;
    }

    //Energy function interface
    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell, const CellG *oldCell );

    virtual void init ( Simulator *simulator, CC3DXMLElement *_xmlData=0 );

    virtual void extraInit ( Simulator *simulator );

    void setPolarizationVector ( CellG * _cell,Vector3 & _vec );
    Vector3 getPolarizationVector ( CellG * _cell );
    void setPolarizationMarkers ( CellG * _cell,unsigned char _type1,unsigned char _type2 );
    std::vector<int> getPolarizationMarkers ( CellG * _cell );
    void setLambdaPolarization ( CellG * _cell,double _lambda );
    double getLambdaPolarization ( CellG * _cell );

    //Steerrable interface
    virtual void update ( CC3DXMLElement *_xmlData, bool _fullInitFlag=false );
    virtual std::string steerableName();
    virtual std::string toString();

};
} // end namespace
#endif