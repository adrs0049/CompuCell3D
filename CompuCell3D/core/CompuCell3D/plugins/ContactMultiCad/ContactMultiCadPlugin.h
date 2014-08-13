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

#ifndef CONTACTMULTICADPLUGIN_H
#define CONTACTMULTICADPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include "ContactMultiCadData.h"
#include "ContactMultiCadDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{
class Simulator;

class Potts3D;
class Automaton;
class ContactMultiCadData;

class CONTACTMULTICAD_EXPORT ContactMultiCadPlugin : public Plugin,public EnergyFunction
{
public:
    typedef double ( ContactMultiCadPlugin::*contactEnergyPtr_t ) ( const CellG *cell1, const CellG *cell2 );

private:
	// ATTENTION SWIG cant deal with using yet
    //using DataAccessor_t = BasicClassAccessor<ContactMultiCadData>;
	typedef BasicClassAccessor<ContactMultiCadData> DataAccessor_t;
	
    DataAccessor_t contactMultiCadDataAccessor;
    CC3DXMLElement *xmlData;
    Potts3D *potts;
    Simulator *sim;

	//Energy function data
    typedef std::map<int, double> contactEnergies_t;
    typedef std::vector<std::vector<double> > contactEnergyArray_t;
    typedef std::vector<contactEnergyArray_t> multiSpecificityArray_t;

    std::set<std::string> cadherinNameSet;
    std::vector<std::string> cadherinNameOrderedVector;

    contactEnergies_t contactEnergies;
    contactEnergyArray_t contactEnergyArray;
    contactEnergyArray_t cadherinSpecificityArray;
    std::map<std::string,unsigned int> mapCadNameToIndex;
    unsigned int numberOfCadherins;

    std::string contactFunctionType;
    std::string autoName;
    double depth;

    std::list<CadherinData> cadherinDataList;

    DataAccessor_t * contactMultiCadDataAccessorPtr;

    Automaton *automaton;
    bool weightDistance;

    contactEnergyPtr_t contactEnergyPtr;

    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;
    float energyOffset;

public:

    ContactMultiCadPlugin();
    virtual ~ContactMultiCadPlugin();

    DataAccessor_t * getContactMultiCadDataAccessorPtr()
    {
        return & contactMultiCadDataAccessor;
    }

    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell ) override;

    virtual void init ( Simulator *simulator, CC3DXMLElement *_xmlData ) override;

    virtual void extraInit ( Simulator *simulator ) override;

    //Steerrable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;

    //Energy Function fcns
    double contactEnergy ( const CellG *cell1, const CellG *cell2 );
    /**
    * @return The contact energy between cell1 and cell2.
    */
    double contactEnergyLinear ( const CellG *cell1, const CellG *cell2 );
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