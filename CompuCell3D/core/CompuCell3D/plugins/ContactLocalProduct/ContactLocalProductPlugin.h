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

#ifndef CONTACTLOCALPRODUCTPLUGIN_H
#define CONTACTLOCALPRODUCTPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>

#include "ContactLocalProductData.h"
#include "ContactLocalProductDLLSpecifier.h"

class CC3DXMLElement;

namespace CompuCell3D
{

class Simulator;

class Potts3D;
class Automaton;
class ContactLocalProductData;
class ParallelUtilsOpenMP;

class CONTACTLOCALPRODUCT_EXPORT ContactLocalProductPlugin : public Plugin,public EnergyFunction
{
public:
    typedef double ( ContactLocalProductPlugin::*contactEnergyPtr_t ) ( const CellG *cell1, const CellG *cell2 );

	// ATTENTION SWIG doesnt know about using yet
    //using DataAccessor_t = BasicClassAccessor<ContactLocalProductData>;
    typedef BasicClassAccessor<ContactLocalProductData> DataAccessor_t;

private:
    ParallelUtilsOpenMP *pUtils;
    DataAccessor_t contactProductDataAccessor;
    Potts3D *potts;
    Simulator *sim;
    CC3DXMLElement *xmlData;

    //Energy function data
    typedef std::map<int, double> contactEnergies_t;
    typedef std::vector<std::vector<double> > contactSpecificityArray_t;

    contactEnergies_t contactEnergies;
    contactSpecificityArray_t contactSpecificityArray;

    std::string autoName;
    std::string contactFunctionType;
    double depth;

    DataAccessor_t * contactProductDataAccessorPtr;

    Automaton *automaton;
    bool weightDistance;

    contactEnergyPtr_t contactEnergyPtr;

    unsigned int maxNeighborIndex;
    BoundaryStrategyPtr boundaryStrategy;
    float energyOffset;

    //vectorized variables for convenient parallel access
    std::vector<std::string> variableNameVector;
    std::string customExpression; //expression for cad-cad function
    std::vector<double> k1Vec; //used to keep arguments for cad-cad function
    std::vector<double> k2Vec;//used to keep arguments for cad-cad function
    std::vector<mu::Parser> pVec;

public:

    ContactLocalProductPlugin();
    virtual ~ContactLocalProductPlugin();

    DataAccessor_t * getContactProductDataAccessorPtr()
    {
        return & contactProductDataAccessor;
    }

    //EnergyFunction Interface
    virtual double changeEnergy ( const Point3D &pt, const CellG *newCell,
                                  const CellG *oldCell ) override;

    virtual void init ( Simulator *simulator, CC3DXMLElement *_xmlData ) override;

    virtual void extraInit ( Simulator *simulator ) override;

    virtual void handleEvent ( CC3DEvent &_event ) override;

    //Steerrable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;

    virtual std::string toString() override;

    void setJVecValue ( CellG * _cell, unsigned int _index,float _value );
    float getJVecValue ( CellG * _cell, unsigned int _index );

    void setCadherinConcentration ( CellG * _cell, unsigned int _index,float _value );
    float getCadherinConcentration ( CellG * _cell, unsigned int _index );

    //user in serialization and restart
    void setCadherinConcentrationVec ( CellG * _cell, std::vector<float> &_vec );
    std::vector<float> getCadherinConcentrationVec ( CellG * _cell );

    //EnergyFunction fcns
    double contactSpecificity ( const CellG *cell1, const CellG *cell2 );


    double contactEnergyLinear ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyQuadratic ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyMin ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyCustom ( const CellG *cell1, const CellG *cell2 );

    double contactEnergyLinearMediumLocal ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyQuadraticMediumLocal ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyMinMediumLocal ( const CellG *cell1, const CellG *cell2 );
    double contactEnergyCustomMediumLocal ( const CellG *cell1, const CellG *cell2 );

    /**
     * Sets the contact energy for two cell types.  A -1 type is interpreted
     * as the medium.
     */
    void setContactEnergy ( const std::string typeName1,
                            const std::string typeName2, const double energy );

    int getIndex ( const int type1, const int type2 ) const;
};

} // end namespace
#endif