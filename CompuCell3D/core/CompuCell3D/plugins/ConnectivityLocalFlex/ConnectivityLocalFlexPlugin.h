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

#ifndef CONNECTIVITYLOCALFLEXPLUGIN_H
#define CONNECTIVITYLOCALFLEXPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ConnectivityLocalFlexData.h"
#include "ConnectivityLocalFlexDLLSpecifier.h"

namespace CompuCell3D
{

class CellG;
class Potts3D;
class Automaton;
class BoundaryStrategy;
class Simulator;

class CONNECTIVITYLOCALFLEX_EXPORT  ConnectivityLocalFlexPlugin : public Plugin,public EnergyFunction
{
public:
	// ATTENTION SWIG cant deal with using yet
	//using DataAccessor_t = BasicClassAccessor<ConnectivityLocalFlexData>;
    typedef BasicClassAccessor<ConnectivityLocalFlexData> DataAccessor_t;
	
private:
    DataAccessor_t connectivityLocalFlexDataAccessor;
    //Energy Function data
    Potts3D *potts;
    unsigned int numberOfNeighbors;
    std::vector<int> offsetsIndex; //this vector will contain indexes of offsets in the neighborListVector so that accessing
    //them using offsetsindex will ensure correct clockwise ordering
    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;

public:
    ConnectivityLocalFlexPlugin();
    virtual ~ConnectivityLocalFlexPlugin();

    //EnergyFunction interface
    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell ) override;
    //Plugin interface
    virtual void init ( Simulator *simulator, CC3DXMLElement *_xmlData ) override;

    DataAccessor_t * getConnectivityLocalFlexDataPtr()
    {
        return & connectivityLocalFlexDataAccessor;
    }
    void setConnectivityStrength ( CellG * _cell, double _connectivityStrength );
    double getConnectivityStrength ( CellG * _cell );

    //Steerable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;

    //EnergyFunction methods
protected:
    /**
     * @return The index used for ordering connectivity energies in the map.
     */
    void addUnique ( CellG*,std::vector<CellG*> & );
    void initializeNeighborsOffsets();
    void orderNeighborsClockwise ( Point3D & _midPoint, const std::vector<Point3D> & _offsets, std::vector<int> & _offsetsIndex );
};
} // end namespace
#endif