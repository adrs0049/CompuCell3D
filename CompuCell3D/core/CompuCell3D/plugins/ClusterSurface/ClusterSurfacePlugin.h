
#ifndef CLUSTERSURFACEPLUGIN_H
#define CLUSTERSURFACEPLUGIN_H

#include <CompuCell3D/CC3D.h>

#include "ClusterSurfaceDLLSpecifier.h"
#include <BasicUtils/memory_include.h>
#include <CompuCell3D/Boundary/BoundaryStrategyFwd.h>

class CC3DXMLElement;

namespace CompuCell3D {
    class Simulator;

    class Potts3D;
    class Automaton;
	class ClusterSurfaceTrackerPlugin ;
    class ParallelUtilsOpenMP;
    
    template <class T> class Field3D;
    template <class T> class WatchableField3D;

    class CLUSTERSURFACE_EXPORT  ClusterSurfacePlugin : public Plugin ,public EnergyFunction  {
        
    private:
        typedef double (ClusterSurfacePlugin::*changeEnergy_t)(const Point3D &pt, const CellG *newCell,const CellG *oldCell);

		ParallelUtilsOpenMP *pUtils; 
		ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;   
		CC3DXMLElement *xmlData;        
        WatchableField3D<CellG *> *cellFieldG;
        double scaleClusterSurface;
		BoundaryStrategyPtr boundaryStrategy;
		ClusterSurfacePlugin::changeEnergy_t changeEnergyFcnPtr;
		
        Potts3D *potts;
        Simulator *sim;
        Automaton *automaton;
		std::shared_ptr<ClusterSurfaceTrackerPlugin> cstPlugin;
    
        LatticeMultiplicativeFactors lmf;        
        unsigned int maxNeighborIndex;
    
        enum FunctionType {GLOBAL=0,BYCELLTYPE=1,BYCELLID=2};
        FunctionType functionType;

        double targetClusterSurface;
        double lambdaClusterSurface;
        
    public:

        ClusterSurfacePlugin();
        virtual ~ClusterSurfacePlugin();
    
        //Energy function interface
        virtual double changeEnergy(const Point3D &pt, const CellG *newCell,
                                    const CellG *oldCell) override;
        double changeEnergyByCellId(const Point3D &pt, const CellG *newCell, const CellG *oldCell);       
        double changeEnergyGlobal(const Point3D &pt, const CellG *newCell, const CellG *oldCell);       
        
        void setTargetAndLambdaClusterSurface(CellG * _cell,float _targetClusterSurface, float _lambdaClusterSurface);
        std::pair<float, float> getTargetAndLambdaVolume(const CellG * _cell) const; //(targetClusterSurface,lambdaClusterSurface)        
        
        std::pair<double,double> getNewOldClusterSurfaceDiffs(const Point3D &pt, const CellG *newCell,const CellG *oldCell);
        double diffEnergy(double lambda, double targetSurface,double surface,  double diff);

        virtual void init(Simulator *simulator,
                          CC3DXMLElement *_xmlData) override;
        virtual void extraInit(Simulator *simulator) override;

        //Steerrable interface
        virtual void update(CC3DXMLElement *_xmlData,
                            bool _fullInitFlag = false) override;
        virtual std::string steerableName() override;
        virtual std::string toString() override;
    };
};
#endif
        
