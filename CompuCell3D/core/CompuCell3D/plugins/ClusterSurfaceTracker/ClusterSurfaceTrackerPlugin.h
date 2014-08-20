
#ifndef CLUSTERSURFACETRACKERPLUGIN_H
#define CLUSTERSURFACETRACKERPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ClusterSurfaceTrackerDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D {
    
    template <class T> class Field3D;
    template <class T> class WatchableField3D;
        
    class Simulator;

    class Potts3D;
    class Automaton;
    class ParallelUtilsOpenMP;

	class PixelTracker;
	class PixelTrackerPlugin;
	class PixelTrackerData;

    class CLUSTERSURFACETRACKER_EXPORT  ClusterSurfaceTrackerPlugin : public Plugin  ,public CellGChangeWatcher {
        ParallelUtilsOpenMP *pUtils;            
        ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;    
		CC3DXMLElement *xmlData;        
        Potts3DPtr potts;
        SimulatorPtr sim;
        AutomatonPtr automaton;
        BoundaryStrategyPtr boundaryStrategy;
        LatticeMultiplicativeFactors lmf;
        cellFieldPtr cellFieldG;
        unsigned int maxNeighborIndex;

		std::shared_ptr<PixelTrackerPlugin> pixelTrackerPlugin;
		std::shared_ptr<BasicClassAccessor<PixelTracker> > pixelTrackerAccessorPtr;
    public:
        ClusterSurfaceTrackerPlugin();
        virtual ~ClusterSurfaceTrackerPlugin();

		// CellChangeWatcher interface
        virtual void field3DChange(const Point3D &pt, CellG *newCell,
                                   CellG *oldCell) override;

                const LatticeMultiplicativeFactors & getLatticeMultiplicativeFactors() const {return lmf;}
		unsigned int getMaxNeighborIndex(){return maxNeighborIndex;}
        void updateClusterSurface(long _clusterId);

        virtual void init(Simulator *simulator,
                          CC3DXMLElement *_xmlData = nullptr) override;

        virtual void extraInit(Simulator *simulator) override;

        //Steerrable interface
        virtual void update(CC3DXMLElement *_xmlData,
                            bool _fullInitFlag = false) override;
        virtual std::string steerableName() override;
        virtual std::string toString() override;
    };
} // end namespace
#endif