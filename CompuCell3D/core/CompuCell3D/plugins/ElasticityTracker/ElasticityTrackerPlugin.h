#ifndef ELASTICITYTRACKERPLUGIN_H
#define ELASTICITYTRACKERPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ElasticityTracker.h"
#include "ElasticityTrackerDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D
{
class Cell;
class Field3DIndex;
template <class T> class Field3D;
template <class T> class WatchableField3D;
class CellInventory;

class ELASTICITYTRACKER_EXPORT ElasticityTrackerPlugin : public Plugin, public CellGChangeWatcher
{
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;

    WatchableField3D<CellG *> *cellFieldG;
    Dim3D fieldDim;

public:
	// ATTENTION SWIG doesnt know about using yet
    //using TrackerAccessor_t = BasicClassAccessor<ElasticityTracker>;
	typedef BasicClassAccessor<ElasticityTracker> TrackerAccessor_t;

private:
    TrackerAccessor_t elasticityTrackerAccessor;
    Simulator *simulator;
    CellInventory * cellInventoryPtr;
    bool initialized;

    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;
    CC3DXMLElement *xmlData;
    bool manualInit;
public:
    ElasticityTrackerPlugin();
    virtual ~ElasticityTrackerPlugin();

    // SimObject interface
    virtual void init ( Simulator *_simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( Simulator *simulator ) override;

    // BCGChangeWatcher interface
    virtual void field3DChange ( const Point3D &pt, CellG *newCell,
                                 CellG *oldCell ) override;

    TrackerAccessor_t * getElasticityTrackerAccessorPtr()
    {
        return & elasticityTrackerAccessor;
    }
    //had to include this function to get set inereation working properly with Python , and Player that has restart capabilities
    ElasticityTrackerData * getElasticityTrackerData ( ElasticityTrackerData * _psd )
    {
        return _psd;
    }
    ElasticityTrackerData * findTrackerData ( CellG * _cell1 ,CellG * _cell2 );
    void assignElasticityPair ( CellG * _cell1 ,CellG * _cell2 );
    void addNewElasticLink ( CellG * _cell1 ,CellG * _cell2,float _lambdaElasticityLink=0.0,float _targetLinkLength=0.0 );
    void removeElasticityPair ( CellG * _cell1 ,CellG * _cell2 );
    void initializeElasticityNeighborList();

protected:
    std::set<std::string> elasticityTypesNames;
    std::set<unsigned char> elasticityTypes;

};
} // end namespace
#endif
