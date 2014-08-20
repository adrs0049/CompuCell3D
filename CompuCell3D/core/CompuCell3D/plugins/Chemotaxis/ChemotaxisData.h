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

#ifndef CHEMOTAXISDATA_H
#define CHEMOTAXISDATA_H
#include <CompuCell3D/CC3D_plugin.h>
#include "ChemotaxisDLLSpecifier.h"

namespace CompuCell3D
{
class ChemotaxisPlugin;

class CHEMOTAXIS_EXPORT  ChemotaxisData
{
public:
    ChemotaxisData ( float _lambda = 0.0, float _saturationCoef = 0.0,
                     std::string _typeName = "" )
        : lambda ( _lambda ), saturationCoef ( _saturationCoef ),
          typeName ( std::move ( _typeName ) ), formulaPtr ( nullptr ),
          formulaName ( "SimpleChemotaxisFormula" ),
          chemotaxisFormulaDictPtr ( nullptr ), automaton ( nullptr ) {}

    typedef float ( ChemotaxisPlugin::*chemotaxisEnergyFormulaFcnPtr_t ) ( float,float,ChemotaxisData & );
          
    float lambda;
    float saturationCoef;
	std::string typeName;
	chemotaxisEnergyFormulaFcnPtr_t  formulaPtr;
    std::string formulaName;
    std::map<std::string,chemotaxisEnergyFormulaFcnPtr_t> *chemotaxisFormulaDictPtr;
	Automaton * automaton;

	std::vector<unsigned char> chemotactTowardsTypesVec;
    std::string chemotactTowardsTypesString;

	void setLambda ( float _lambda )
    {
        lambda=_lambda;
    }
    float getLambda()
    {
        return lambda;
    }
    void setType ( std::string _typeName )
    {
        typeName=_typeName;
    }
    void setChemotactTowards ( std::string _chemotactTowardsTypesString )
    {
        chemotactTowardsTypesString=_chemotactTowardsTypesString;
    }

    void setChemotaxisFormulaByName ( std::string _formulaName )
    {
        formulaName=_formulaName;
        if ( chemotaxisFormulaDictPtr )
        {
            std::map<std::string,chemotaxisEnergyFormulaFcnPtr_t>::iterator mitr;
            mitr=chemotaxisFormulaDictPtr->find ( _formulaName );
            if ( mitr!=chemotaxisFormulaDictPtr->end() )
            {
                formulaName=_formulaName;
                formulaPtr=mitr->second;
            }
        }


    }

    void initializeChemotactTowardsVectorTypes ( std::string _chemotactTowardsTypesString )
    {
        chemotactTowardsTypesVec.clear();
        std::vector<std::string> vecTypeNamesTmp;
        parseStringIntoList ( _chemotactTowardsTypesString,vecTypeNamesTmp,"," );

        for ( auto &elem : vecTypeNamesTmp )
        {

            unsigned char typeTmp = automaton->getTypeId ( elem );
            chemotactTowardsTypesVec.push_back ( typeTmp );
        }
    }

    void assignChemotactTowardsVectorTypes ( std::vector<int> _chemotactTowardsTypesVec )
    {
        chemotactTowardsTypesVec.clear();
        for ( auto &elem : _chemotactTowardsTypesVec )
        {
            chemotactTowardsTypesVec.push_back ( elem );
        }
    }

    std::vector<int> getChemotactTowardsVectorTypes()
    {
        std::vector<int> chemotactTowardsTypesVecInt;

        for ( auto &elem : chemotactTowardsTypesVec )
        {
            chemotactTowardsTypesVecInt.push_back ( elem );
        }
        return chemotactTowardsTypesVecInt;
    }

    void setSaturationCoef ( float _saturationCoef )
    {
        saturationCoef=_saturationCoef;

        if ( chemotaxisFormulaDictPtr )
        {
            std::map<std::string,chemotaxisEnergyFormulaFcnPtr_t>::iterator mitr;
            mitr=chemotaxisFormulaDictPtr->find ( "SaturationChemotaxisFormula" );
            if ( mitr!=chemotaxisFormulaDictPtr->end() )
            {
                formulaName="SaturationChemotaxisFormula";
                formulaPtr=mitr->second;
            }
        }

    }
    void setSaturationLinearCoef ( float _saturationCoef )
    {
        saturationCoef=_saturationCoef;

        if ( chemotaxisFormulaDictPtr )
        {
            std::map<std::string,chemotaxisEnergyFormulaFcnPtr_t>::iterator mitr;
            mitr=chemotaxisFormulaDictPtr->find ( "SaturationLinearChemotaxisFormula" );
            if ( mitr!=chemotaxisFormulaDictPtr->end() )
            {
                formulaName="SaturationLinearChemotaxisFormula";
                formulaPtr=mitr->second;
            }
        }
    }

    void outScr()
    {
        using namespace std;
        cerr<<"**************ChemotaxisData**************"<<endl;
        cerr<<"formulaPtr="<<formulaPtr<<endl;
        cerr<<"lambda="<<lambda<<" saturationCoef="<<saturationCoef<<" typaName="<<typeName<<endl;
        cerr<<"chemotactTowards="<<chemotactTowardsTypesString<<endl;
        cerr<<"Chemotact towards types:"<<endl;
        for ( auto &elem : chemotactTowardsTypesVec )
        {
            cerr << "chemotact Towards type id=" << ( int ) elem << endl;
        }
        cerr<<"**************ChemotaxisData END**************"<<endl;
    }

    bool okToChemotact ( const CellG * _oldCell )
    {
        if ( chemotactTowardsTypesVec.size() ==0 ) //chemotaxis always enabled for this cell
        {
            return true;
        }
        //will chemotact towards only specified cell types

        unsigned char type= ( _oldCell?_oldCell->type:0 );

        for ( auto &elem : chemotactTowardsTypesVec )
        {
            if ( type == elem )
                return true;
        }

        return false;
    }
};

// ATTENTION SWIG doesnt know about using yet
//using chemotaxisDataAccessor_t = BasicClassAccessor< std::map<std::string,ChemotaxisData> >;
typedef std::shared_ptr<BasicClassAccessor< std::map<std::string,ChemotaxisData> > > chemotaxisDataAccessor_t;

class  ChemotaxisFieldData
{
public:
    ChemotaxisFieldData()
    {}
    std::string chemicalFieldSource;
    std::string chemicalFieldName;
    std::vector<ChemotaxisData> vecChemotaxisData;
    ChemotaxisData *ChemotaxisByType()
    {
        vecChemotaxisData.push_back ( ChemotaxisData() );
        return & vecChemotaxisData[vecChemotaxisData.size() -1];
    }
    void Source ( std::string _chemicalFieldSource )
    {
        chemicalFieldSource=_chemicalFieldSource;
    }
    void Name ( std::string _chemicalFieldName )
    {
        chemicalFieldName=_chemicalFieldName;
    }
    ChemotaxisData * getChemotaxisData ( int _index )
    {
        if ( _index<0 || _index>vecChemotaxisData.size()-1 )
            return nullptr;
        return &vecChemotaxisData[_index];
    }
};

} // end namespace

#endif
