
#ifndef CELLTYPEMONITORPLUGIN_H
#define CELLTYPEMONITORPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "CellTypeMonitorDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D {
    class Simulator;

    class Potts3D;
    class Automaton;
    class ParallelUtilsOpenMP;
    
    template <class T> class Field3D;
    template <class T> class WatchableField3D;

    class CELLTYPEMONITOR_EXPORT CellTypeMonitorPlugin : public Plugin  ,public CellGChangeWatcher {
    private:    
        CC3DXMLElement *xmlData;        
        Potts3D *potts;
        Simulator *sim;
        ParallelUtilsOpenMP *pUtils;            
        ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;        
        Automaton *automaton;

        BoundaryStrategyPtr boundaryStrategy;
        WatchableField3D<CellG *> *cellFieldG;
        Array3DCUDA<unsigned char> * cellTypeArray;
        // Array3DCUDA<int> * cellIdArray; // this should have been Array3DCUDA<long> but openCL on windows does not "like" longs so I am using int . 
        Array3DCUDA<float> * cellIdArray; // this should have been Array3DCUDA<long> but openCL on windows does not "like" longs so I am using float . 
        unsigned char mediumType;
    public:

        CellTypeMonitorPlugin();
        virtual ~CellTypeMonitorPlugin();

        virtual Array3DCUDA<unsigned char> * getCellTypeArray(){return cellTypeArray;}
        virtual Array3DCUDA<float> * getCellIdArray(){return cellIdArray;}
        
        // CellChangeWatcher interface
        virtual void field3DChange(const Point3D &pt, CellG *newCell,
                                   CellG *oldCell) override;

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