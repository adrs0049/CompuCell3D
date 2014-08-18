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

#ifndef CONTACTLOCALFLEXPLUGIN_H
#define CONTACTLOCALFLEXPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ContactLocalFlexData.h"
#include "ContactLocalFlexDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{

class Simulator;
class Potts3D;
class Automaton;
class ContactLocalFlexDataContainer;

class CONTACTLOCALFLEX_EXPORT ContactLocalFlexPlugin : public Plugin ,public CellGChangeWatcher,public EnergyFunction
{
public:
	// ATTENTION SWIG doesnt know about using yet
    //using DataAccessor_t = BasicClassAccessor<ContactLocalFlexDataContainer>;
	typedef BasicClassAccessor<ContactLocalFlexDataContainer> DataAccessor_t;

private:
    ParallelUtilsOpenMP *pUtils;
    ParallelUtilsOpenMP::OpenMPLock_t *lockPtr;
	
    DataAccessor_t contactDataContainerAccessor;
    Potts3DPtr potts;
    SimulatorPtr sim;
    void updateContactEnergyData ( CellG *_cell );
    bool initializadContactData;

    // EnergyFunction Data
    typedef std::map<int, double> contactEnergies_t;
    typedef std::vector<std::vector<double> > contactEnergyArray_t;

    contactEnergies_t contactEnergies;
    contactEnergyArray_t contactEnergyArray;

    std::string autoName;
    double depth;

    AutomatonPtr automaton;
    bool weightDistance;

    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;
    CC3DXMLElement *xmlData;

public:
    ContactLocalFlexPlugin();
    virtual ~ContactLocalFlexPlugin();

    DataAccessor_t * getContactDataContainerAccessorPtr()
    {
        return & contactDataContainerAccessor;
    }
    void initializeContactLocalFlexData();

    //CellGCellwatcher interface
    virtual void field3DChange ( const Point3D &pt, CellG *newCell,
                                 CellG *oldCell ) override;
    //EnergyFunction interface
    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell ) override;
    //Plugin interface
    virtual void init ( SimulatorPtr simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( SimulatorPtr simulator ) override;
    virtual std::string toString() override;

    //Steerrable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;

    //void initializeContactEnergy(CC3DXMLElement *_xmlData);

    /**
     * @return The contact energy between cell1 and cell2.
     */
    double contactEnergy ( const CellG *cell1, const CellG *cell2 );

    double defaultContactEnergy ( const CellG *cell1, const CellG *cell2 );

    /**
     * Sets the contact energy for two cell types.  A -1 type is interpreted
     * as the medium.
     */
    void setContactEnergy ( const std::string typeName1,
                            const std::string typeName2, const double energy );

protected:
    /**
     * @return The index used for ordering contact energies in the map.
     */
    int getIndex ( const int type1, const int type2 ) const;
};

} // end namespace

#endif
