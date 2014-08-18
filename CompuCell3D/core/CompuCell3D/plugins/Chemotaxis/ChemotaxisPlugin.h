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

#ifndef CHEMOTAXISPLUGIN_H
#define CHEMOTAXISPLUGIN_H

#include <CompuCell3D/CC3D_plugin.h>
#include <CompuCell3D/Field3D/CC3D_Field3D.h>
#include "ChemotaxisData.h"
#include "ChemotaxisDLLSpecifier.h"

class CC3DXMLElement;
namespace CompuCell3D {

class Simulator;
class Automaton;

template <class T>
class Field3DImpl;

template <class T>
class Field3D;
class Potts3D;

class CHEMOTAXIS_EXPORT ChemotaxisPlugin : public Plugin,public EnergyFunction 
{
public:
    typedef float (ChemotaxisPlugin::*chemotaxisEnergyFormulaFcnPtr_t)(float,float,ChemotaxisData &);

    typedef double (ChemotaxisPlugin::*changeEnergyEnergyFormulaFcnPtr_t)(const Point3D &, const CellG *,
            const CellG *);
private:

	SimulatorPtr sim;
    CC3DXMLElement * xmlData;
    AutomatonPtr automaton;
    //EnergyFunction data

    Potts3DPtr potts;
	changeEnergyEnergyFormulaFcnPtr_t algorithmPtr;
	
    std::vector<Field3D<float> *> fieldVec;
    std::vector<std::string> fieldNameVec;

    std::vector<std::vector<ChemotaxisData> > vecVecChemotaxisData;

    float simpleChemotaxisFormula(float _flipNeighborConc,float _conc,ChemotaxisData & _chemotaxisData);
    float saturationChemotaxisFormula(float _flipNeighborConc,float _conc,ChemotaxisData & _chemotaxisData);
    float saturationLinearChemotaxisFormula(float _flipNeighborConc,float _conc,ChemotaxisData & _chemotaxisData);
    //bool okToChemotact(unsigned int fieldIdx,unsigned char cellType);
    std::string chemotaxisAlgorithm;

	// ATTENTION SWIG doesnt know about using yet
	//using chemotaxisDataAccessor_t = BasicClassAccessor< std::map<std::string,ChemotaxisData> >;
	typedef BasicClassAccessor< std::map<std::string,ChemotaxisData> > chemotaxisDataAccessor_t;
    chemotaxisDataAccessor_t chemotaxisDataAccessor;

    std::map<std::string,chemotaxisEnergyFormulaFcnPtr_t> chemotaxisFormulaDict;
    double haptoHelper(unsigned int, const Point3D &);

public:
	ChemotaxisPlugin();
    virtual ~ChemotaxisPlugin();

    virtual void init(Simulator *simulator,
                      CC3DXMLElement *_xmlData = nullptr) override;
    virtual void extraInit(Simulator *simulator) override;

    //EnergyFunction interface
    virtual double changeEnergy(const Point3D &pt, const CellG *newCell,
                                const CellG *oldCell) override;

    //Steerable interface
    virtual void update(CC3DXMLElement *_xmlData,
                        bool _fullInitFlag = false) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;

    //EnergyFunction Methods
    double regularChemotaxis(const Point3D &pt, const CellG *newCell,
                             const CellG *oldCell);

    double merksChemotaxis(const Point3D &pt, const CellG *newCell,
                           const CellG *oldCell);

    double haptoChemotaxis(const Point3D &pt, const CellG *newCell,
                           const CellG *oldCell);
            
    ChemotaxisData * addChemotaxisData(CellG * _cell,std::string _fieldName);
    ChemotaxisData * getChemotaxisData(CellG * _cell,std::string _fieldName);

    std::vector<std::string> getFileNamesWithChemotaxisData(CellG * _cell);
};
} // end namespace
#endif