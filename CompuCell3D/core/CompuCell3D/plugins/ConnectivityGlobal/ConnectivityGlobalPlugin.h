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

#ifndef CONNECTIVITYGLOBALPLUGIN_H
#define CONNECTIVITYGLOBALPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ConnectivityGlobalData.h"
#include "ConnectivityGlobalDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{
class Potts3D;
class Automaton;

class CONNECTIVITYGLOBAL_EXPORT ConnectivityGlobalPlugin : public Plugin,public EnergyFunction
{
private:
	// ATTENTION SWIG cant deal with using yet
    //using DataAccessor_t = BasicClassAccessor<ConnectivityGlobalData>;
	typedef BasicClassAccessor<ConnectivityGlobalData> DataAccessor_t;
    DataAccessor_t connectivityGlobalDataAccessor;

    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;

    Potts3D *potts;
    std::vector<double> penaltyVec;
    unsigned char maxTypeId;
    bool doNotPrecheckConnectivity;

public:
    ConnectivityGlobalPlugin();
    virtual ~ConnectivityGlobalPlugin();

    //Plugin interface
    virtual void init ( Simulator *simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;

    DataAccessor_t * getConnectivityGlobalDataPtr()
    {
        return & connectivityGlobalDataAccessor;
    }
    void setConnectivityStrength ( CellG * _cell, double _connectivityStrength );
    double getConnectivityStrength ( CellG * _cell );

    virtual std::string toString() override;

    //EnergyFunction interface
    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell ) override;

    bool checkIfCellIsFragmented ( const CellG * cell,Point3D cellPixel );
    //SteerableObject interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
};
} // end namespace
#endif
