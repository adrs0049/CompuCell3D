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
#include <CompuCell3D/plugins/PlasticityTracker/PlasticityTrackerPlugin.h>
#include <CompuCell3D/plugins/PlasticityTracker/PlasticityTracker.h>

using namespace CompuCell3D;
using namespace std;

#include "PlasticityPlugin.h"

PlasticityPlugin::PlasticityPlugin() :
    cellFieldG ( 0 ),
    pluginName ( "Plasticity" ),
    targetLengthPlasticity ( 0.0 ),
    lambdaPlasticity ( 0.0 ),
    maxLengthPlasticity ( 100000000000.0 ),
    diffEnergyFcnPtr ( &PlasticityPlugin::diffEnergyGlobal ),
    boundaryStrategy ( 0 )
{}

PlasticityPlugin::~PlasticityPlugin()
{}

void PlasticityPlugin::init ( SimulatorPtr simulator, CC3DXMLElement *_xmlData )
{
    Potts3DPtr potts = simulator->getPotts();
    cellFieldG = potts->getCellFieldG();
    pluginName=_xmlData->getAttribute ( "Name" );
    potts->registerEnergyFunctionWithName ( this,"PlasticityEnergy" );
    simulator->registerSteerableObject ( this );
    update ( _xmlData,true );
}

void PlasticityPlugin::extraInit ( SimulatorPtr simulator )
{
    Potts3DPtr potts = simulator->getPotts();
    cellFieldG = potts->getCellField3DG();

    fieldDim=cellFieldG ->getDim();
    boundaryStrategy=BoundaryStrategy::getInstance();

    bool pluginAlreadyRegisteredFlag;
    auto trackerPlugin = get_plugin<PlasticityTrackerPlugin> ( "PlasticityTracker", &pluginAlreadyRegisteredFlag );
    if ( !pluginAlreadyRegisteredFlag )
        trackerPlugin->init ( simulator );
    plasticityTrackerAccessorPtr=trackerPlugin->getPlasticityTrackerAccessorPtr() ;
}

void PlasticityPlugin::update ( CC3DXMLElement *_xmlData, bool _fullInitFlag )
{
    if ( _xmlData->findElement ( "Local" ) )
    {
        diffEnergyFcnPtr=&PlasticityPlugin::diffEnergyLocal;
    }
    else
    {
        diffEnergyFcnPtr=&PlasticityPlugin::diffEnergyGlobal;
        if ( _xmlData->findElement ( "TargetLengthPlasticity" ) )
            targetLengthPlasticity=_xmlData->getFirstElement ( "TargetLengthPlasticity" )->getDouble();
        if ( _xmlData->findElement ( "LambdaPlasticity" ) )
            lambdaPlasticity=_xmlData->getFirstElement ( "LambdaPlasticity" )->getDouble();
        if ( _xmlData->findElement ( "MaxPlasticityLength" ) )
            maxLengthPlasticity=_xmlData->getFirstElement ( "MaxPlasticityLength" )->getDouble();
    }
}

double PlasticityPlugin::diffEnergyGlobal ( float _deltaL,float _lBefore,const PlasticityTrackerData * _plasticityTrackerData,const CellG *_cell )
{
    if ( _cell->volume>1 )
    {
        if ( _lBefore<maxLengthPlasticity )
        {
            return lambdaPlasticity*_deltaL* ( 2* ( _lBefore-targetLengthPlasticity ) +_deltaL );
        }
        else
        {
            return 0.0;
        }
    }
    else  //after spin flip oldCell will disappear so the only contribution from before spin flip i.e. -(l-l0)^2
    {
        if ( _lBefore<maxLengthPlasticity )
        {
            return -lambdaPlasticity* ( _lBefore-targetLengthPlasticity ) * ( _lBefore-targetLengthPlasticity );
        }
        else
        {
            return 0.0;
        }
    }

}

double PlasticityPlugin::diffEnergyLocal ( float _deltaL,float _lBefore,const PlasticityTrackerData * _plasticityTrackerData,const CellG *_cell )
{

    float lambdaLocal=_plasticityTrackerData->lambdaLength;
    float targetLengthLocal=_plasticityTrackerData->targetLength;

    if ( _cell->volume>1 )
    {
        return lambdaLocal*_deltaL* ( 2* ( _lBefore-targetLengthLocal ) +_deltaL );
    }
    else  //after spin flip oldCell will disappear so the only contribution from before spin flip i.e. -(l-l0)^2
    {
        return -lambdaLocal* ( _lBefore-targetLengthLocal ) * ( _lBefore-targetLengthLocal );
    }

}

double PlasticityPlugin::changeEnergy ( const Point3D &pt,
                                        const CellG *newCell,
                                        const CellG *oldCell )
{
    float energy=0.0;
    Coordinates3D<double> centroidOldAfter;
    Coordinates3D<double> centroidNewAfter;
    Coordinates3D<float> centMassOldAfter;
    Coordinates3D<float> centMassNewAfter;
    Coordinates3D<float> centMassOldBefore;
    Coordinates3D<float> centMassNewBefore;

    if ( oldCell )
    {
        centMassOldBefore.XRef() =oldCell->xCM/ ( float ) oldCell->volume;
        centMassOldBefore.YRef() =oldCell->yCM/ ( float ) oldCell->volume;
        centMassOldBefore.ZRef() =oldCell->zCM/ ( float ) oldCell->volume;

        if ( oldCell->volume>1 )
        {
            centroidOldAfter=precalculateCentroid ( pt, oldCell, -1,fieldDim,boundaryStrategy );
            centMassOldAfter.XRef() =centroidOldAfter.X() / ( float ) ( oldCell->volume-1 );
            centMassOldAfter.YRef() =centroidOldAfter.Y() / ( float ) ( oldCell->volume-1 );
            centMassOldAfter.ZRef() =centroidOldAfter.Z() / ( float ) ( oldCell->volume-1 );
        }
        else
        {
            centroidOldAfter.XRef() =oldCell->xCM;
            centroidOldAfter.YRef() =oldCell->yCM;
            centroidOldAfter.ZRef() =oldCell->zCM;
            centMassOldAfter.XRef() =centroidOldAfter.X() / ( float ) ( oldCell->volume );
            centMassOldAfter.YRef() =centroidOldAfter.Y() / ( float ) ( oldCell->volume );
            centMassOldAfter.ZRef() =centroidOldAfter.Z() / ( float ) ( oldCell->volume );
        }
    }

    if ( newCell )
    {
        centMassNewBefore.XRef() =newCell->xCM/ ( float ) newCell->volume;
        centMassNewBefore.YRef() =newCell->yCM/ ( float ) newCell->volume;
        centMassNewBefore.ZRef() =newCell->zCM/ ( float ) newCell->volume;

        centroidNewAfter=precalculateCentroid ( pt, newCell, 1,fieldDim,boundaryStrategy );
        centMassNewAfter.XRef() =centroidNewAfter.X() / ( float ) ( newCell->volume+1 );
        centMassNewAfter.YRef() =centroidNewAfter.Y() / ( float ) ( newCell->volume+1 );
        centMassNewAfter.ZRef() =centroidNewAfter.Z() / ( float ) ( newCell->volume+1 );

    }

    //will loop over neighbors of the oldCell and calculate Plasticity energy
    set<PlasticityTrackerData> * plasticityNeighborsTmpPtr;
    set<PlasticityTrackerData>::iterator sitr;
    CellG *nCell;
    float deltaL;
    float lBefore;
    float oldVol;
    float newVol;
    float nCellVol;
    if ( oldCell )
    {
        oldVol=oldCell->volume;
        plasticityNeighborsTmpPtr=&plasticityTrackerAccessorPtr->get ( oldCell->extraAttribPtr )->plasticityNeighbors ;

        for ( sitr=plasticityNeighborsTmpPtr->begin() ; sitr != plasticityNeighborsTmpPtr->end() ; ++sitr )
        {
            nCell=sitr->neighborAddress;
            nCellVol=nCell->volume;

            if ( nCell!=newCell )
            {
                lBefore=distInvariantCM ( centMassOldBefore.X(),centMassOldBefore.Y(),centMassOldBefore.Z(),nCell->xCM/nCellVol,nCell->yCM/nCellVol,nCell->zCM/nCellVol,fieldDim,boundaryStrategy );
                deltaL=
                    distInvariantCM ( centMassOldAfter.X(),centMassOldAfter.Y(),centMassOldAfter.Z(),nCell->xCM/nCellVol,nCell->yCM/nCellVol,nCell->zCM/nCellVol,fieldDim,boundaryStrategy )
                    -lBefore;
            }
            else
            {
                lBefore=distInvariantCM ( centMassOldBefore.X(),centMassOldBefore.Y(),centMassOldBefore.Z(),centMassNewBefore.X(),centMassNewBefore.Y(),centMassNewBefore.Z(),fieldDim,boundaryStrategy );
                deltaL=
                    distInvariantCM ( centMassOldAfter.X(),centMassOldAfter.Y(),centMassOldAfter.Z(),centMassNewAfter.X(),centMassNewAfter.Y(),centMassNewAfter.Z(),fieldDim,boundaryStrategy )
                    -lBefore;

            }
            energy+= ( this->*diffEnergyFcnPtr ) ( deltaL,lBefore,& ( *sitr ),oldCell );

        }
    }

    if ( newCell )
    {
        newVol=newCell->volume;
        plasticityNeighborsTmpPtr=&plasticityTrackerAccessorPtr->get ( newCell->extraAttribPtr )->plasticityNeighbors ;
        for ( sitr=plasticityNeighborsTmpPtr->begin() ; sitr != plasticityNeighborsTmpPtr->end() ; ++sitr )
        {
            nCell=sitr->neighborAddress;
            nCellVol=nCell->volume;

            if ( nCell!=oldCell )
            {
                lBefore=distInvariantCM ( centMassNewBefore.X(),centMassNewBefore.Y(),centMassNewBefore.Z(),nCell->xCM/nCellVol,nCell->yCM/nCellVol,nCell->zCM/nCellVol,fieldDim,boundaryStrategy );
                deltaL=
                    distInvariantCM ( centMassNewAfter.X(),centMassNewAfter.Y(),centMassNewAfter.Z(),nCell->xCM/nCellVol,nCell->yCM/nCellVol,nCell->zCM/nCellVol,fieldDim,boundaryStrategy )
                    -lBefore;
            }
            energy+= ( this->*diffEnergyFcnPtr ) ( deltaL,lBefore,& ( *sitr ),newCell );
        }
    }
    return energy;
}

std::string PlasticityPlugin::toString()
{
    return pluginName;
    //return "Plasticity";
}

std::string PlasticityPlugin::steerableName()
{
    return toString();
}