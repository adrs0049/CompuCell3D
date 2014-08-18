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

#include <CompuCell3D/CC3D_plugin.h>

using namespace CompuCell3D;

using namespace std;

#include "PixelTrackerPlugin.h"

PixelTrackerPlugin::PixelTrackerPlugin() : simulator(nullptr), potts(nullptr) {}

PixelTrackerPlugin::~PixelTrackerPlugin() {}

void PixelTrackerPlugin::init(Simulator *_simulator, CC3DXMLElement *_xmlData)
{
    simulator=_simulator;
    potts = simulator->getPotts();

    ///************************************************************************************************
    ///REMARK. HAVE TO USE THE SAME BASIC CLASS ACCESSOR INSTANCE THAT WAS USED TO REGISTER WITH FACTORY
    ///************************************************************************************************
    potts->getCellFactoryGroupPtr()->registerClass(std::make_shared<pixelTrackerAccessor_t>(pixelTrackerAccessor));
    potts->registerCellGChangeWatcher(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PixelTrackerPlugin::field3DChange(const Point3D &pt, CellG *newCell,CellG *oldCell) {
    if (newCell==oldCell) //this may happen if you are trying to assign same cell to one pixel twice
        return;

    if(newCell) {
        std::set<PixelTrackerData > & pixelSetRef=pixelTrackerAccessor.get(newCell->extraAttribPtr)->pixelSet;
        pixelSetRef.insert(PixelTrackerData(pt));
    }

    if(oldCell) {
        std::set<PixelTrackerData > & pixelSetRef=pixelTrackerAccessor.get(oldCell->extraAttribPtr)->pixelSet;
        auto sitr=pixelSetRef.find(PixelTrackerData(pt));

        ASSERT_OR_THROW("Could not find point:"+pt+" inside cell of id: "+BasicString(oldCell->id)+" type: "+BasicString((int)oldCell->type),
                        sitr!=pixelSetRef.end());

        pixelSetRef.erase(sitr);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PixelTrackerPlugin::handleEvent(CC3DEvent & _event) {
    if (_event.id!=LATTICE_RESIZE) {
        return;
    }

    CC3DEventLatticeResize ev = static_cast<CC3DEventLatticeResize&>(_event);

    Dim3D shiftVec=ev.shiftVec;

    CellInventory &cellInventory = potts->getCellInventory();
    CellInventory::cellInventoryIterator cInvItr;
    CellG * cell;

    for(cInvItr=cellInventory.cellInventoryBegin() ; cInvItr !=cellInventory.cellInventoryEnd() ; ++cInvItr )
    {
        cell=cInvItr->second;
        std::set<PixelTrackerData > & pixelSetRef=pixelTrackerAccessor.get(cell->extraAttribPtr)->pixelSet;
        for (const auto &elem : pixelSetRef) {
          Point3D &pixel = const_cast<Point3D &>(elem.pixel);
            pixel.x+=shiftVec.x;
            pixel.y+=shiftVec.y;
            pixel.z+=shiftVec.z;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string PixelTrackerPlugin::toString() {
    return "PixelTracker";
}
