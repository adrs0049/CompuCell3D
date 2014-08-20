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

#ifndef PIXELTRACKERPLUGIN_H
#define PIXELTRACKERPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "PixelTracker.h"
#include "PixelTrackerDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D {

class Cell;
class Field3DIndex;
template <class T> class Field3D;
template <class T> class WatchableField3D;

typedef std::shared_ptr<BasicClassAccessor<PixelTracker> > pixelTrackerAccessor_t;

class PIXELTRACKER_EXPORT PixelTrackerPlugin : public Plugin, public CellGChangeWatcher {
public:
	// ATTENTION SWIG doesnt know about using yet
	//using pixelTrackerAccessor_t = BasicClassAccessor<PixelTracker>;
		
private:
    //WatchableField3D<CellG *> *cellFieldG;
    Dim3D fieldDim;
    pixelTrackerAccessor_t pixelTrackerAccessor;
    SimulatorPtr simulator;
    Potts3DPtr potts;

public:
    PixelTrackerPlugin();
    virtual ~PixelTrackerPlugin();

	// Field3DChangeWatcher interface
    virtual void field3DChange(const Point3D &pt, CellG *newCell,
                               CellG *oldCell) override;

    //Plugin interface
    virtual void init(SimulatorPtr _simulator,
                      CC3DXMLElement *_xmlData = nullptr) override;
    virtual std::string toString() override;
    virtual void handleEvent(CC3DEvent &_event) override;

    pixelTrackerAccessor_t getPixelTrackerAccessorPtr() {
        return pixelTrackerAccessor;
    }
    //had to include this function to get set itereation working properly with Python , and Player that has restart capabilities
    PixelTrackerData * getPixelTrackerData(PixelTrackerData * _psd) {
        return _psd;
    }
};
} // end namespace
#endif
