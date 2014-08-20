#ifndef REALPLASTICITYTRACKERPLUGIN_H
#define REALPLASTICITYTRACKERPLUGIN_H
#include <CompuCell3D/CC3D_plugin.h>
#include "PlasticityTracker.h"
#include "PlasticityTrackerDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D
{

class Cell;
class Field3DIndex;
template <class T> class Field3D;
template <class T> class WatchableField3D;
class CellInventory;

class PLASTICITYTRACKER_EXPORT PlasticityTrackerPlugin : public Plugin, public CellGChangeWatcher
{
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;
    cellFieldPtr cellFieldG;
	CellInventory * cellInventoryPtr;
    PlasticityTrackerAccessor_t plasticityTrackerAccessor;
	BoundaryStrategyPtr boundaryStrategy;
    CC3DXMLElement *xmlData;
    SimulatorPtr simulator;
   
    bool initialized;
	unsigned int maxNeighborIndex;
	Dim3D fieldDim;

public:
    PlasticityTrackerPlugin();
    virtual ~PlasticityTrackerPlugin();

    // SimObject interface
    virtual void init ( SimulatorPtr _simulator, CC3DXMLElement *_xmlData=0 );
    virtual void extraInit ( SimulatorPtr simulator );

    // BCGChangeWatcher interface
    virtual void field3DChange ( const Point3D &pt, CellG *newCell,
                                 CellG *oldCell );

    PlasticityTrackerAccessor_t getPlasticityTrackerAccessorPtr()
    {
        return plasticityTrackerAccessor;
    }
    //had to include this function to get set inereation working properly with Python , and Player that has restart capabilities
    PlasticityTrackerData * getPlasticityTrackerData ( PlasticityTrackerData * _psd )
    {
        return _psd;
    }
    void initializePlasticityNeighborList();
    void addPlasticityNeighborList();

protected:
    std::set<std::string> plasticityTypesNames;
    std::set<unsigned char> plasticityTypes;
};
} // end namespace
#endif
