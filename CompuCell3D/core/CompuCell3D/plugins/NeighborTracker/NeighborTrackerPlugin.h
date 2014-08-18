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

#ifndef NEIGHBORTRACKERPLUGIN_H
#define NEIGHBORTRACKERPLUGIN_H
#include <CompuCell3D/CC3D_plugin.h>
#include "NeighborTracker.h"
#include "NeighborTrackerDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D
{

class Cell;
class Field3DIndex;
template <class T> class Field3D;
template <class T> class WatchableField3D;

class CellInventory;

class NEIGHBORTRACKER_EXPORT NeighborTrackerPlugin : public Plugin, public CellGChangeWatcher
{
public:
	// ATTENTION SWIG doesnt know about using yet
    //using NeighborTracker_t = BasicClassAccessor<NeighborTracker>;
	typedef BasicClassAccessor<NeighborTracker> NeighborTracker_t;
	
private:
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;
    cellFieldPtr cellFieldG;
    Dim3D fieldDim;
    NeighborTracker_t neighborTrackerAccessor;
    SimulatorPtr simulator;
    bool periodicX,periodicY,periodicZ;
    CellInventory * cellInventoryPtr;
    bool checkSanity;
    unsigned int checkFreq;
    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;

public:
    NeighborTrackerPlugin();
    virtual ~NeighborTrackerPlugin();


    // Field3DChangeWatcher interface
    virtual void field3DChange ( const Point3D &pt, CellG *newCell,
                                 CellG *oldCell ) override;

    //Plugin interface
    virtual void init ( SimulatorPtr _simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual std::string toString() override;

    NeighborTracker_t * getNeighborTrackerAccessorPtr()
    {
        return & neighborTrackerAccessor;
    }
    // End XMLSerializable interface
    int returnNumber()
    {
        return 23432;
    }
    short getCommonSurfaceArea ( NeighborSurfaceData * _nsd )
    {
        return _nsd->commonSurfaceArea;
    }


protected:
    double distance ( double,double,double,double,double,double );

    virtual void testLatticeSanityFull();
    bool isBoundaryPixel ( Point3D pt );
    bool watchingAllowed;
    AdjacentNeighbor adjNeighbor;
    long maxIndex; //maximum field index
    long changeCounter;
};
} // end namespace
#endif
