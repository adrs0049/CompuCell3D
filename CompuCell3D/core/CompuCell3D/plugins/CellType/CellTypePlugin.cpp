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

#include <CompuCell3D/CC3D.h>
#include "CellTypePlugin.h"
#include <ostream>

using namespace CompuCell3D;
using namespace std;

std::string CellTypePlugin::toString()
{
    return "CellType";
}

std::string CellTypePlugin::steerableName()
{
    return toString();
}

CellTypePlugin::CellTypePlugin()
{
    classType = std::make_unique<CellType>();
}

CellTypePlugin::~CellTypePlugin()
{}

void CellTypePlugin::init ( Simulator *simulator, CC3DXMLElement *_xmlData )
{
    potts = simulator->getPotts();
    potts->registerCellGChangeWatcher ( this );
    potts->registerAutomaton ( this );
    update ( _xmlData );
    simulator->registerSteerableObject ( ( SteerableObject* ) this );
}

void CellTypePlugin::update ( CC3DXMLElement *_xmlData, bool _fullInitFlag )
{
    typeNameMap.clear();
    nameTypeMap.clear();
    vector<unsigned char> frozenTypeVec;
	
    CC3DXMLElementList cellTypeVec=_xmlData->getElements ( "CellType" );
    for (const auto& elem : cellTypeVec )
    {
        typeNameMap[elem->getAttributeAsByte ( "TypeId" )]=elem->getAttribute ( "TypeName" );
        nameTypeMap[elem->getAttribute ( "TypeName" )]=elem->getAttributeAsByte ( "TypeId" );

        if ( elem->findAttribute ( "Freeze" ) )
            frozenTypeVec.push_back ( elem->getAttributeAsByte ( "TypeId" ) );
    }

    potts->setFrozenTypeVector ( frozenTypeVec );
}

void CellTypePlugin::init ( Simulator *simulator, ParseData * _pd )
{}

void CellTypePlugin::update ( ParseData *_pd, bool _fullInitFlag )
{}

unsigned char CellTypePlugin::getCellType ( const CellG *cell ) const
{
    if ( !cell ) return 0;
    return cell->type;
}

string CellTypePlugin::getTypeName ( const char type ) const
{
    auto typeNameMapItr = typeNameMap.find ( ( const unsigned char ) type );

    if ( typeNameMapItr!=typeNameMap.end() )
    {
        return typeNameMapItr->second;
    }
    else
    {
        THROW ( string ( "getTypeName: Unknown cell type  " ) + BasicString ( type ) + "!" );
    }
}

unsigned char CellTypePlugin::getMaxTypeId() const 
{
	cerr<<"typeNameMap.size()="<<typeNameMap.size()<<endl;
	if (! typeNameMap.size()){
		return 0;
	}else{
		return (--(typeNameMap.end()))->first; //returning last type number (unsigned char) 
	}
}

unsigned char CellTypePlugin::getTypeId ( const string typeName ) const
{
    auto nameTypeMapItr = nameTypeMap.find ( typeName );

    if ( nameTypeMapItr!=nameTypeMap.end() )
    {
        return nameTypeMapItr->second;
    }
    else
    {
        THROW ( string ( "getTypeName: Unknown cell type  " ) + typeName + "!" );
    }
}
