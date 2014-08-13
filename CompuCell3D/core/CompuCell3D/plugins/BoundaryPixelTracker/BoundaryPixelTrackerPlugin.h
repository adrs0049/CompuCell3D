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

#ifndef BOUNDARYPIXELTRACKERPLUGIN_H
#define BOUNDARYPIXELTRACKERPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "BoundaryPixelTracker.h"
#include "BoundaryPixelTrackerDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D
{

class Cell;
class Field3DIndex;
class Potts3D;
template <class T> class Field3D;
template <class T> class WatchableField3D;

class BOUNDARYPIXELTRACKER_EXPORT BoundaryPixelTrackerPlugin : public Plugin, public CellGChangeWatcher
{
    Dim3D fieldDim;
	// ATTENTION SWIG cant deal with using yet
    //using TrackerAccessor_t =  BasicClassAccessor<BoundaryPixelTracker>;
    typedef BasicClassAccessor<BoundaryPixelTracker> TrackerAccessor_t;
	TrackerAccessor_t boundaryPixelTrackerAccessor;
    Simulator *simulator;
    Potts3D* potts;
    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;
    CC3DXMLElement *xmlData;

public:
    BoundaryPixelTrackerPlugin();
    virtual ~BoundaryPixelTrackerPlugin();

    // Field3DChangeWatcher interface
    virtual void field3DChange ( const Point3D &pt, CellG *newCell,
                                 CellG *oldCell ) override;

    //Plugin interface
    virtual void init ( Simulator *_simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( Simulator *_simulators ) override;
    virtual void handleEvent ( CC3DEvent &_event ) override;

    //Steerable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;

    TrackerAccessor_t * getBoundaryPixelTrackerAccessorPtr()
    {
        return & boundaryPixelTrackerAccessor;
    }
    //had to include this function to get set inereation working properly with Python , and Player that has restart capabilities
    BoundaryPixelTrackerData * getBoundaryPixelTrackerData ( BoundaryPixelTrackerData * _psd )
    {
        return _psd;
    }
};
} // end namespace
#endif
