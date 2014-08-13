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

#include <CompuCell3D/Field3D/Point3D.h>
#include <CompuCell3D/Field3D/Dim3D.h>
#include <CompuCell3D/Field3D/Neighbor.h>
#include <CompuCell3D/Field3D/NeighborFinder.h>
#include <map>
#include <Utils/Coordinates3D.h>
#include "BoundaryStrategy.h"
#include <CompuCell3D/Field3D/Field3DImpl.h> //Field3DImpl.h includes boundary Strategy and for this reason has to be listed after #define EXP_STL
#include "Boundary.h"
#include "BoundaryFactory.h"
#include "AlgorithmFactory.h"
#include "Algorithm.h"

#define roundf(a) ((fmod(a,1)<0.5)?floor(a):ceil(a))

using namespace std;
using namespace CompuCell3D;

// Singleton
BoundaryStrategyPtr BoundaryStrategy::instance = nullptr;
std::mutex BoundaryStrategy::_mutex;

// Default constructor
BoundaryStrategy::BoundaryStrategy()
{
    strategy_x = BoundaryFactory::createBoundary ( BoundaryFactory::no_flux );
    strategy_y = BoundaryFactory::createBoundary ( BoundaryFactory::no_flux );
    strategy_z = BoundaryFactory::createBoundary ( BoundaryFactory::no_flux );
    algorithm = AlgorithmFactory::createAlgorithm ( AlgorithmFactory::Default,0,0,"None" );

    regular = true;
    neighborListsInitializedFlag=false;
    latticeType=SQUARE_LATTICE;

#ifdef _DEBUG
    const unsigned int maxHexArraySize=6;
    cerr<<"maxHexArraySize="<<maxHexArraySize<<endl;

    cerr<<"\t\t\t\t\t\t\t CALLING DEFAULT CONSTRUCTOR FOR BOUNDARY STRATEGY"<<endl;
#endif

}

// Constructor
BoundaryStrategy::BoundaryStrategy ( string boundary_x, string boundary_y,
                                     string boundary_z, string alg, int index,
									 int size, string inputfile,LatticeType latticeType )
{
    strategy_x = BoundaryFactory::createBoundary ( boundary_x );
    strategy_y = BoundaryFactory::createBoundary ( boundary_y );
    strategy_z = BoundaryFactory::createBoundary ( boundary_z );
    algorithm = AlgorithmFactory::createAlgorithm ( alg, index, size, inputfile );
    regular = true;
    neighborListsInitializedFlag=false;
    this->latticeType=latticeType;

#ifdef _DEBUG
    const unsigned int maxHexArraySize=6;
    cerr<<"\t\t\t\t\t\t\t CALLING SPECILIZED CONSTRUCTOR FOR BOUNDARY STRATEGY"<<endl;
    cerr<<"maxHexArraySize="<<maxHexArraySize<<endl;

#endif

}

// Destructor
BoundaryStrategy::~BoundaryStrategy()
{
     instance.reset();
}

LatticeMultiplicativeFactors BoundaryStrategy::generateLatticeMultiplicativeFactors ( LatticeType _latticeType,Dim3D dim )
{
    LatticeMultiplicativeFactors lFactors;
    if ( _latticeType==HEXAGONAL_LATTICE )
    {
        if ( dim.x==1 ||dim.y==1 || dim.z==1 ) //2D case for hex lattice // might need to tune it further to account for 1D case
        {
            //area of hexagon with edge l = 6*sqrt(3)/4 * l^2

            lFactors.volumeMF=1.0;
            lFactors.surfaceMF=sqrt ( 2.0/ ( 3.0*sqrt ( 3.0 ) ) );
            lFactors.lengthMF=lFactors.surfaceMF*sqrt ( 3.0 );
            return lFactors;
        }
        else  //3D case for hex lattice
        {
            //Volume of rhombic dodecahedron = 16/9 *sqrt(3)*b^3
            //Surface of rhombic dodecahedron = 9*sqrt(2)*b^2
            //b - rhomb edge length
            lFactors.volumeMF=1.0;
            lFactors.surfaceMF=8.0/12.0*sqrt ( 2.0 ) *pow ( 9.0/ ( 16.0*sqrt ( 3.0 ) ),1.0/3.0 ) *pow ( 9.0/ ( 16.0*sqrt ( 3.0 ) ),1.0/3.0 );
            lFactors.lengthMF=2.0*sqrt ( 2.0/3.0 ) *pow ( 9.0/ ( 16.0*sqrt ( 3.0 ) ),1.0/3.0 );
            return lFactors;
        }
    }
    else
    {
        lFactors.volumeMF=1.0;
        lFactors.surfaceMF=1.0;
        lFactors.lengthMF=1.0;
        return lFactors;
    }
}

/*
 * Check to see if the given point lies inside the field dimensions
 *
 * @param pt Point3D
 *
 * @return bool
 */

bool BoundaryStrategy::isValid ( const Point3D &pt ) const
{
    // check to see if the point lies in the dimensions before applying the
    // shape algorithm

    if ( 0 <= pt.x && pt.x < dim.x &&
            0 <= pt.y && pt.y < dim.y &&
            0 <= pt.z && pt.z < dim.z )
    {
        return algorithm->inGrid ( pt );
    }

    return false;
}

/*
 * Check to see if the given point lies inside the field dimensions
 *
 * @param pt Point3D
 * @param customDim Dim3D*
 * @return bool
 */

bool BoundaryStrategy::isValidCustomDim ( const Point3D &pt, const Dim3D & customDim ) const
{
    // check to see if the point lies in the dimensions before applying the
    // shape algorithm

    if ( 0 <= pt.x && pt.x < customDim.x &&
            0 <= pt.y && pt.y < customDim.y &&
            0 <= pt.z && pt.z < customDim.z )
    {
        return algorithm->inGrid ( pt );
    }

    return false;
}

/*
 * Check to see if the given coordinate  lies inside the
 * max value for that axis
 *
 * @param coordinate for a given axis
 * @param max_value for a that axis
 *
 * @return bool
 */
bool BoundaryStrategy::isValid ( const int coordinate, const int max_value ) const
{

    return ( 0 <= coordinate && coordinate < max_value );
}

/*
 * Set dimensions
 *
 * @param dim Dim3D
 */
void BoundaryStrategy::setDim ( const Dim3D theDim )
{
//     cerr<<"calling setDim again"<<endl;
    Dim3D oldDim ( dim );

    dim = theDim;
    algorithm->setDim ( theDim );
    if ( ! neighborListsInitializedFlag )
    {
        prepareNeighborLists();
        neighborListsInitializedFlag=true;
    }

    if ( latticeType==HEXAGONAL_LATTICE )
    {
        latticeSizeVector.x=dim.x;
        latticeSizeVector.y=dim.y*sqrt ( 3.0 ) /2.0;
        latticeSizeVector.z=dim.z*sqrt ( 6.0 ) /3.0;

        latticeSpanVector.x=dim.x-1;
        latticeSpanVector.y= ( dim.y-1 ) *sqrt ( 3.0 ) /2.0;
        latticeSpanVector.z= ( dim.z-1 ) *sqrt ( 6.0 ) /3.0;

    }
    else
    {
        latticeSizeVector.x=dim.x;
        latticeSizeVector.y=dim.y;
        latticeSizeVector.z=dim.z;

        latticeSpanVector.x=dim.x-1;
        latticeSpanVector.y=dim.y-1;
        latticeSpanVector.z=dim.z-1;
    }
}

/*
 * Set current step
 *
 *  @param int currentStep
 */
void BoundaryStrategy::setCurrentStep ( const int theCurrentStep )
{
    currentStep = theCurrentStep;
    algorithm->setCurrentStep ( theCurrentStep );
}

/*
 * Set Irregular
 *
 */
void BoundaryStrategy::setIrregular()
{
    regular = false;
}

/*
 * Retrieves a neighbor for a given point after applying
 * boundary conditions as configured
 *
 * @param pt Point3D
 * @param token int
 * @param distance int
 * @param checkBounds bool
 *
 * @return Point3D Valid Neighbor
 */
Point3D BoundaryStrategy::getNeighbor ( const Point3D& pt, unsigned int& token, double& distance, bool checkBounds ) const
{
    Neighbor n;
    Point3D p;
    int x;
    int y;
    int z;
    bool x_bool;
    bool y_bool;
    bool z_bool;

    NeighborFinder::destroy();

    while ( true )
    {
        // Get a neighbor from the NeighborFinder
        n = NeighborFinder::getInstance()->getNeighbor ( token );
        x = ( pt + n.pt ).x;
        y = ( pt + n.pt ).y;
        z = ( pt + n.pt ).z;

        token++;

        if ( !checkBounds || isValid ( pt + n.pt ) )
        {
            // Valid Neighbor
            break;
        }
        else
        {
            if ( regular )
            {
                // For each coordinate, if it is not valid, apply condition
                x_bool = ( isValid ( x,dim.x ) ? true : strategy_x->applyCondition ( x, dim.x ) );
                y_bool = ( isValid ( y,dim.y ) ? true : strategy_y->applyCondition ( y, dim.y ) );
                z_bool = ( isValid ( z,dim.z ) ? true : strategy_z->applyCondition ( z, dim.z ) );

                // If all the coordinates of the neighbor are valid then return the
                // neighbor
                if ( x_bool && y_bool && z_bool )
                {
                    break;
                }
            }
        }
    }

    distance = n.distance;
    p.x = x;
    p.y = y;
    p.z = z;

    return p;

}

/*
 * Retrieves a neighbor for a given point after applying
 * boundary conditions as configured
 *
 * @param pt Point3D
 * @param token int
 * @param distance int
 * @param customDim Dim3D
 * @param checkBounds bool
 *
 * @return Point3D Valid Neighbor
 */
// this function returns neighbor but takes extra dim as an argument  menaning we can use it for lattices of size different than simulation dim. used in prepareOffsets functions
Point3D BoundaryStrategy::getNeighborCustomDim ( const Point3D& pt, unsigned int& token,double& distance, const Dim3D & customDim, bool checkBounds ) const
{

    Neighbor n;
    Point3D p;
    int x;
    int y;
    int z;
    bool x_bool;
    bool y_bool;
    bool z_bool;

    NeighborFinder::destroy();

    while ( true )
    {
        // Get a neighbor from the NeighborFinder
        n = NeighborFinder::getInstance()->getNeighbor ( token );
        x = ( pt + n.pt ).x;
        y = ( pt + n.pt ).y;
        z = ( pt + n.pt ).z;

        token++;

        if ( !checkBounds || isValidCustomDim ( pt + n.pt,customDim ) )
        {
            // Valid Neighbor
            break;
        }
        else
        {
            if ( regular )
            {
                // For each coordinate, if it is not valid, apply condition
                x_bool = ( isValid ( x,customDim.x ) ? true : strategy_x->applyCondition ( x, customDim.x ) );
                y_bool = ( isValid ( y,customDim.y ) ? true : strategy_y->applyCondition ( y, customDim.y ) );
                z_bool = ( isValid ( z,customDim.z ) ? true : strategy_z->applyCondition ( z, customDim.z ) );

                // If all the coordinates of the neighbor are valid then return the
                // neighbor
                if ( x_bool && y_bool && z_bool )
                {
                    break;
                }
            }
        }
    }

    distance = n.distance;
    p.x = x;
    p.y = y;
    p.z = z;

    return p;

}


int BoundaryStrategy::getNumPixels ( int x, int y, int z ) const
{

    return algorithm->getNumPixels ( x, y, z );

}

//returns true if _offset is stacked in _offsetVec
//false otherwise
bool BoundaryStrategy::checkIfOffsetAlreadyStacked ( Point3D & _ptToCheck , std::vector<Point3D> & _offsetVec ) const
{

    for ( auto &elem : _offsetVec )
    {
        if ( elem.x == _ptToCheck.x && elem.y == _ptToCheck.y &&
                elem.z == _ptToCheck.z )
            return true;
    }
    return false;
}

double BoundaryStrategy::calculateDistance ( Coordinates3D<double> & _pt1 , Coordinates3D<double> & _pt2 ) const
{
    return sqrt ( ( double ) ( _pt1.x-_pt2.x ) * ( _pt1.x-_pt2.x ) + ( _pt1.y-_pt2.y ) * ( _pt1.y-_pt2.y ) + ( _pt1.z-_pt2.z ) * ( _pt1.z-_pt2.z ) );
}

bool BoundaryStrategy::checkEuclidianDistance ( Coordinates3D<double> & _pt1,Coordinates3D<double> & _pt2, float _distance ) const
{
    //checks if distance between two points is smaller than _distance
    //used to eliminate in offsetVec offsets that come from periodic conditions (opposite side of the lattice)
    return calculateDistance ( _pt1,_pt2 ) <_distance+0.1;

}




bool BoundaryStrategy::precisionCompare ( float _x,float _y,float _prec )
{
    return fabs ( _x-_y ) <_prec;
}


Coordinates3D<double> BoundaryStrategy::HexCoord ( const Point3D & _pt ) const
{
    if ( ( _pt.z%3 ) ==1 ) //odd z e.g. z=1
    {
        //(-0.5,+sqrt(3)/6)
        if ( _pt.y%2 )
            return Coordinates3D<double> ( _pt.x+0.5 , sqrt ( 3.0 ) /2.0* ( _pt.y+2.0/6.0 ), _pt.z*sqrt ( 6.0 ) /3.0 );
        else//even
            return Coordinates3D<double> ( _pt.x ,  sqrt ( 3.0 ) /2.0* ( _pt.y+2.0/6.0 ) , _pt.z*sqrt ( 6.0 ) /3.0 );

    }
    else if ( ( _pt.z%3 ) ==2 ) //e.g. z=2
    {

        if ( _pt.y%2 )
            return Coordinates3D<double> ( _pt.x+0.5 , sqrt ( 3.0 ) /2.0* ( _pt.y-2.0/6.0 ), _pt.z*sqrt ( 6.0 ) /3.0 );
        else//even
            return Coordinates3D<double> ( _pt.x ,  sqrt ( 3.0 ) /2.0* ( _pt.y-2.0/6.0 ) , _pt.z*sqrt ( 6.0 ) /3.0 );
    }
    else //z divible by 3 - includes z=0
    {
        if ( _pt.y%2 )
            return Coordinates3D<double> ( _pt.x , sqrt ( 3.0 ) /2.0*_pt.y, _pt.z*sqrt ( 6.0 ) /3.0 );
        else//even
            return Coordinates3D<double> ( _pt.x+0.5 ,  sqrt ( 3.0 ) /2.0*_pt.y , _pt.z*sqrt ( 6.0 ) /3.0 );
    }
}

Point3D BoundaryStrategy::Hex2Cartesian(const Coordinates3D<double> & _coord)const{
    //this transformation takes coordinates of a point on ahex lattice and returns integer coordinates of cartesian pixel that is nearest given point on hex lattice 
    //It is the inverse transformation of the one coded in HexCoord 
   int z_segments = (int) roundf( _coord.z/(sqrt(6.0)/3.0) ) ;

    if ((z_segments%3) == 1) {    
        int y_segments =  (int)roundf ( _coord.y/ (sqrt(3.0)/2.0) -2.0/6.0 ) ;
        
        if (y_segments%2){
          
            return Point3D((int)roundf(_coord.x-0.5),y_segments , z_segments);            
        }else{
          
            return Point3D((int)roundf(_coord.x),y_segments , z_segments);            
        }
         
    }else if  ((z_segments%3) == 2){
    
        int y_segments =  (int)roundf ( _coord.y/ (sqrt(3.0)/2.0) +2.0/6.0 ) ;
        
    
        if (y_segments%2){
          
            return Point3D( (int)roundf (_coord.x-0.5) ,y_segments , z_segments);            
        }else{
          
            return Point3D((int)roundf(_coord.x),y_segments , z_segments);            
        }

    }else{    
        
        int y_segments =  (int)roundf ( _coord.y/ (sqrt(3.0)/2.0) ) ;
        if (y_segments%2){
          
            return Point3D((int)roundf(_coord.x),y_segments , z_segments);            
        }else{
          
            return Point3D((int)roundf(_coord.x-0.5),y_segments , z_segments);            
        }
        

    }   
}

Coordinates3D<double> BoundaryStrategy::calculatePointCoordinates ( const Point3D & _pt ) const
{
    if ( latticeType==HEXAGONAL_LATTICE )
    {
        Coordinates3D<double> hexCoord=HexCoord ( _pt );
        return hexCoord;
    }
    else
    {
        return Coordinates3D<double> ( _pt.x,_pt.y,_pt.z );
    }
}

void BoundaryStrategy::prepareNeighborListsHex ( float _maxDistance )
{
#ifdef _DEBUG
    cerr<<"INSIDE prepareNeighborListsHex"<<endl;
#endif
    const unsigned int maxHexArraySize=6;

    hexOffsetArray.assign ( maxHexArraySize,vector<Point3D>() );
    hexDistanceArray.assign ( maxHexArraySize,vector<float>() );
    hexNeighborOrderIndexArray.assign ( maxHexArraySize,vector<unsigned int>() );

    char a='0';

    vector<Point3D> offsetVecTmp;
    vector<float> distanceVecTmp;
    Dim3D tmpFieldDim;

    tmpFieldDim=dim;
    if ( dim.z !=1 && tmpFieldDim.z<15 ) // to generate coorect offsets we need to have tmpField which is large enouigh for our algorithm in non-flad z dimension
    {
        tmpFieldDim.z=15;
    }

    if ( dim.y !=1 && tmpFieldDim.y<10 ) // to generate coorect offsets we need to have tmpField which is large enouigh for our algorithm in non-flad y dimension
    {
        tmpFieldDim.z=10;
    }

    if ( dim.x !=1 && tmpFieldDim.x<10 ) // to generate coorect offsets we need to have tmpField which is large enouigh for our algorithm in non-flad z dimension
    {
        tmpFieldDim.x=10;
    }

    Field3DImpl<char> tempField ( tmpFieldDim,a );
    Point3D ctPt ( tmpFieldDim.x/2 , tmpFieldDim.y/2 , tmpFieldDim.z/2 );
    cerr<<"_maxDistance="<<_maxDistance<<endl;

#ifdef _DEBUG
    cerr<<"center point="<<ctPt<<endl;
#endif
    Point3D ctPtTmp;
    unsigned int indexHex;
    //For hex lattice we have four different offset lists
    //y-even z_even

    ctPtTmp=ctPt;

    //there are 3 layers of z planes which are interlaced therefore we need to consider pt.z%3
    //indexHexFormula=(pt.z%3)*2+(pt.y%2);

    //indexHex=Y_EVEN|Z_EVEN;
    indexHex=0; // e.g. z=21,y=20

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {
        ctPtTmp.y+=ctPtTmp.y % 2; //make it even
        ctPtTmp.z+=3-ctPtTmp.z % 3;// make it divisible by 3 in case it is not
#ifdef _DEBUG
        cerr<<"ctPtTmp.y % 2 ="<<ctPtTmp.y % 2 << " ctPtTmp.y % 2="<<ctPtTmp.y % 2<<endl;
        cerr<<"  WILL USE CENTER POINT="<<ctPtTmp<<"Y_EVEN|Z_EVEN "<< ( Y_EVEN|Z_EVEN ) <<endl;
#endif
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }
    else  //2D case
    {
#ifdef _DEBUG
        cerr<<"ctPtTmp.y % 2 ="<<ctPtTmp.y % 2 <<endl;
#endif
        ctPtTmp.y+=ctPtTmp.y % 2; //make it even
        ctPtTmp.z+=0;// make it divisible by 3 in case it is not

#ifdef _DEBUG
        cerr<<"even even ctPtTmp="<<ctPtTmp<<endl;
#endif
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }

    //y-odd z_even
    ctPtTmp=ctPt;
    indexHex=1; //e.g. z=21 y=21
    //indexHex=Y_ODD|Z_EVEN;

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {

        ctPtTmp.y+= ( ctPtTmp.y % 2-1 ); //make it odd
        ctPtTmp.z+=3-ctPtTmp.z % 3;// make it divisible by 3 in case it is not

#ifdef _DEBUG
        cerr<<"ctPtTmp.y % 2 ="<<ctPtTmp.y % 2 << " !ctPtTmp.y % 2="<<! ( ctPtTmp.y % 2 ) <<endl;

        cerr<<"  WILL USE CENTER POINT="<<ctPtTmp<<"Y_ODD|Z_EVEN "<< ( Y_ODD|Z_EVEN ) <<endl;
#endif
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }
    else  //2D case
    {
#ifdef _DEBUG
        cerr<<"ctPtTmp.y % 2 ="<<ctPtTmp.y % 2 << " !ctPtTmp.y % 2="<<! ( ctPtTmp.y % 2 ) <<endl;
#endif

        ctPtTmp.y+= ( ctPtTmp.y % 2-1 ); //make it odd
        ctPtTmp.z+=0;   // make it divisible by 3 in case it is not

#ifdef _DEBUG
        cerr<<"odd even ctPtTmp="<<ctPtTmp<<endl;
#endif
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }

    ctPtTmp=ctPt;
    indexHex=2;// e.g. z=22 y=20

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {

        ctPtTmp.y+=ctPtTmp.y % 2; //make it even

        ctPtTmp.z+=3-ctPtTmp.z % 3-2;// make it divisible by 3 with z%3=1 in case it is not

        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }
    else  //2D case
    {
        //ignore this case
    }

    //y-even z_odd
    ctPtTmp=ctPt;

    indexHex=3;

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {

        ctPtTmp.y+= ( ctPtTmp.y % 2-1 ); //make it odd
        ctPtTmp.z+=3-ctPtTmp.z % 3-2;// make it divisible by 3 with z%3=1 in case it is not
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );
    }
    else  //2D case
    {
        //ignore this case
    }

    ctPtTmp=ctPt;
    indexHex=4;

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {

        ctPtTmp.y+=ctPtTmp.y % 2; //make it even
        ctPtTmp.z+=3-ctPtTmp.z % 3-1;// make it divisible by 3 with z%3=2 in case it is not
        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }
    else  //2D case
    {
        //ignore this case
    }

    ctPtTmp=ctPt;
    indexHex=5;

    if ( dim.z > 1 ) //make sure not 2D with z direction flat
    {

        ctPtTmp.y+= ( ctPtTmp.y % 2-1 ); //make it odd
        ctPtTmp.z+=3-ctPtTmp.z % 3-1;// make it divisible by 3 with z%3=2 in case it is not

        getOffsetsAndDistances ( ctPtTmp,_maxDistance,tempField,hexOffsetArray[indexHex],hexDistanceArray[indexHex],hexNeighborOrderIndexArray[indexHex] );

    }
    else  //2D case
    {
        //ignore this case
    }

    //we will copy arrays 0 and 1 to (2,4) (3,5) respectively for 2D case
    {
        maxOffset=6;
        if ( dim.z == 1 )
        {
            maxOffset=6;

            hexOffsetArray[2]=hexOffsetArray[0];
            hexOffsetArray[4]=hexOffsetArray[0];
            hexOffsetArray[3]=hexOffsetArray[1];
            hexOffsetArray[5]=hexOffsetArray[1];

            hexDistanceArray[2]=hexDistanceArray[0];
            hexDistanceArray[4]=hexDistanceArray[0];
            hexDistanceArray[3]=hexDistanceArray[1];
            hexDistanceArray[5]=hexDistanceArray[1];

            hexNeighborOrderIndexArray[2]=hexNeighborOrderIndexArray[0];
            hexNeighborOrderIndexArray[4]=hexNeighborOrderIndexArray[0];
            hexNeighborOrderIndexArray[3]=hexNeighborOrderIndexArray[1];
            hexNeighborOrderIndexArray[5]=hexNeighborOrderIndexArray[1];

        }
        else
        {
            maxOffset=12;
        }
    }

#ifdef _DEBUG

    indexHex=0;
    for ( indexHex=0 ; indexHex<maxHexArraySize; ++indexHex )
    {

        cerr<<"INDEX HEX="<<indexHex<<" hexOffsetArray[indexHex].size()="<<hexOffsetArray[indexHex].size() <<endl;

        for ( int i = 0 ; i < hexOffsetArray[indexHex].size() ; ++i )
        {
            cerr<<" This is offset["<<i<<"]="<<hexOffsetArray[indexHex][i]<<" distance="<<hexDistanceArray[indexHex][i]<<endl;
        }
    }

    Neighbor n;
    Point3D testPt ( 10,10,0 );
    unsigned int idx=3;
    n=getNeighborDirect ( testPt,idx );

    cerr<<"Neighbor="<<n<<endl;
    testPt = Point3D ( 10,11,0 );
    n=getNeighborDirect ( testPt,idx );
    cerr<<"Neighbor="<<n<<endl;
    testPt = Point3D ( 11,11,0 );
    n=getNeighborDirect ( testPt,idx );
    cerr<<"Neighbor="<<n<<endl;

    cerr<<"\n\n\n ****************************Checking Bondary "<<endl;

    testPt = Point3D ( 0,0,0 );
    cerr<<"HexCoord(testPt)="<<HexCoord ( testPt ) <<endl;
    for ( int i =0 ; i<6 ; ++i )
    {
        n=getNeighborDirect ( testPt,i );
        if ( n.distance>0 )
        {
            cerr<<"Neighbor="<<n<<endl;
        }
        else
        {
            cerr<<"************************Not a neighbor= "<<n<<endl;
        }
    }

    cerr<<"\n\n\n *****************Checkup Boundary"<<endl;

    testPt = Point3D ( 0,dim.y-1,0 );
    cerr<<"HexCoord(testPt)="<<HexCoord ( testPt ) <<endl;
    for ( int i =0 ; i<6 ; ++i )
    {
        n=getNeighborDirect ( testPt,i );
        if ( n.distance>0 )
        {
            cerr<<"Neighbor="<<n<<endl;
        }
        else
        {
            cerr<<"*****************Not a neighbor= "<<n<<endl;
        }
    }

    for ( int i =1 ; i<=11 ; ++i )
    {
        unsigned int maxIdx=getMaxNeighborIndexFromNeighborOrder ( i );
        cerr<<"NEIGHBOR ORDER ="<<i<<" maxIdx="<<maxIdx<<endl;

    }

#endif
}

void BoundaryStrategy::getOffsetsAndDistances (
    Point3D ctPt,
    float maxDistance,
    Field3DImpl<char> const& tempField,
    vector<Point3D> & offsetVecTmp,
    vector<float> &distanceVecTmp,
    vector<unsigned int> &neighborOrderIndexVecTmp
) const
{
    Point3D n;

    unsigned int token = 0;
    double distance = 0;
    Coordinates3D<double> ctPtTrans, nTrans;
    Point3D offset;
    double distanceTrans=0.0;

    offsetVecTmp.clear();
    distanceVecTmp.clear();
    neighborOrderIndexVecTmp.clear();

    if ( latticeType==HEXAGONAL_LATTICE )
    {
        ctPtTrans=HexCoord ( ctPt );
    }
    else
    {
        ctPtTrans=Coordinates3D<double> ( ctPt.x,ctPt.y,ctPt.z );
    }

    Dim3D tmpFieldDim = tempField.getDim();

    while ( true )
    {
        // calling  getNeighbor via field interface changes checkBounds from false to true...
        // calling getNeighbor directly requires does not set checkBounds to true
        n=getNeighborCustomDim ( ctPt, token, distance,tmpFieldDim, true ); // notice that we cannot in general use regular getNeighbor because this fcn assumes that dimension of the thmField are same as the dimensions of simulation field
        // n = tempField.getNeighbor(ctPt, token, distance, false); // calling  getNeighbor via field interface changes checkBounds from false to true...
        // n = getNeighbor(ctPt, token, distance, true);
        // cerr<<"distance="<<distance<<endl;
        if ( distance > maxDistance*2.0 ) break; //2.0 factor is to ensure you visit enough neighbors for different kind of lattices

        offset=n-ctPt;

        if ( latticeType==HEXAGONAL_LATTICE )
        {
            //the transformations formulas for hex latice are written in such a way that distance between pixels is set to 1
            ctPtTrans=HexCoord ( ctPt );
            nTrans=HexCoord ( n );
            distanceTrans=calculateDistance ( ctPtTrans,nTrans );
        }
        else
        {
            ctPtTrans=Coordinates3D<double> ( ctPt.x,ctPt.y,ctPt.z );
            nTrans=Coordinates3D<double> ( n.x,n.y,n.z );
            distanceTrans=distance;
        }

        if ( !checkIfOffsetAlreadyStacked ( offset,offsetVecTmp ) && distanceTrans<maxDistance+0.1 )
        {
//          cerr<<"distanceTrans="<<distanceTrans<<" offset="<<offset<<endl;
            offsetVecTmp.push_back ( offset );
            distanceVecTmp.push_back ( distanceTrans );
        }
    }

    //at this point we have all the offsets for the given simulation but they are unsorted.
    //Sorting  neighbors
    multimap<float,Point3D> sortingMap;
    for ( unsigned int i = 0 ; i < offsetVecTmp.size() ; ++i )
    {
        sortingMap.insert ( make_pair ( distanceVecTmp[i],offsetVecTmp[i] ) );
    }

    //clearing offsetVecTmp and distanceVecTmp
    offsetVecTmp.clear();
    distanceVecTmp.clear();
    //Writing sorted  by distance content of offsetVecTmp and distanceVecTmp
    for ( auto &elem : sortingMap )
    {
        distanceVecTmp.push_back ( elem.first );
        offsetVecTmp.push_back ( elem.second );
    }
#ifdef _DEBUG
    cerr<<"distanceVecTmp.size()="<<distanceVecTmp.size() <<endl;
#endif
    //creating a vector indexed by neighbor order  - entries of this vector are the highest indices of offsets for a
    //given neighbor order
    float currentDistance=1.0;

    for ( unsigned int i = 0 ; i < distanceVecTmp.size() ; ++i )
    {
        if ( currentDistance<distanceVecTmp[i] )
        {
            neighborOrderIndexVecTmp.push_back ( i-i );
            currentDistance=distanceVecTmp[i];
        }
    }

}

void BoundaryStrategy::prepareNeighborListsSquare ( float _maxDistance )
{
    char a='0';
    Field3DImpl<char> tempField ( dim,a );
    Point3D ctPt ( dim.x/2,dim.y/2,dim.z/2 );
    getOffsetsAndDistances ( ctPt,_maxDistance,tempField,offsetVec,distanceVec,neighborOrderIndexVec );

#ifdef _DEBUG
    for ( unsigned int i = 0 ; i < offsetVec.size() ; ++i )
    {
        cerr<<" This is offset["<<i<<"]="<<offsetVec[i]<<" distance="<<distanceVec[i]<<endl;
    }
#endif

}

void BoundaryStrategy::prepareNeighborLists ( float _maxDistance )
{
    maxDistance=_maxDistance;
    lmf=generateLatticeMultiplicativeFactors ( latticeType,dim );
    if ( latticeType==HEXAGONAL_LATTICE )
    {
        prepareNeighborListsHex ( _maxDistance );
    }
    else
    {
        prepareNeighborListsSquare ( _maxDistance );
    }
}

unsigned int  BoundaryStrategy::getMaxNeighborOrder() const
{

    //determining max neighborOrder
    unsigned int maxNeighborOrder=1;
    unsigned int previousMaxIdx=0;
    unsigned int currentMaxIdx=0;

    while ( true )
    {
        currentMaxIdx=getMaxNeighborIndexFromNeighborOrder ( maxNeighborOrder );

        if ( previousMaxIdx==currentMaxIdx )
            break;

        previousMaxIdx=currentMaxIdx;
        ++maxNeighborOrder;
    }

    return --maxNeighborOrder;
}

void BoundaryStrategy::prepareNeighborListsBasedOnNeighborOrder ( unsigned int _neighborOrder )
{

    unsigned int maxNeighborOrder=getMaxNeighborOrder();

    while ( ( maxNeighborOrder-4 ) <_neighborOrder ) //making sure there is enough higher order neighbors in the list
    {
        prepareNeighborLists ( 2.0*maxDistance );
        maxNeighborOrder=getMaxNeighborOrder();
    }
}

unsigned int BoundaryStrategy::getMaxNeighborIndexFromNeighborOrder ( unsigned int _neighborOrder ) const
{
    //Now determine max neighbor index from a list of neighbor offsets
    unsigned int maxNeighborIndex=0;
    unsigned int orderCounter=1;

    if ( latticeType==HEXAGONAL_LATTICE )
    {
        //unsigned int indexHex=Y_EVEN|Z_EVEN;
        unsigned int indexHex=0;
        double currentDepth=hexDistanceArray[indexHex][0];

        for ( unsigned int i = 0 ; i < hexDistanceArray[indexHex].size() ; ++i )
        {

            ++maxNeighborIndex;
            if ( hexDistanceArray[indexHex][i]> ( currentDepth+0.005 ) ) //0.005 is to account for possible numerical approximations in double or float numbers
            {
                currentDepth = hexDistanceArray[indexHex][i];
                ++orderCounter;
                if ( orderCounter>_neighborOrder )
                {
                    maxNeighborIndex=i-1;
                    return maxNeighborIndex;
                }
            }
        }
        return --maxNeighborIndex;

    }
    else
    {
        double currentDepth=distanceVec[0];

        for ( unsigned int i = 0 ; i < distanceVec.size() ; ++i )
        {
            if ( distanceVec[i]> ( currentDepth+0.005 ) ) //0.005 is to account for possible numerical approximations in double or float numbers
            {
                currentDepth=distanceVec[i];
                ++orderCounter;
                if ( orderCounter>_neighborOrder )
                {
                    maxNeighborIndex=i-1;
                    return maxNeighborIndex;
                }
            }
        }
        return --maxNeighborIndex;
    }
}

unsigned int BoundaryStrategy::getMaxNeighborIndexFromDepth ( float depth ) const
{
    //Now determine max neighbor index from a list of neighbor offsets

    unsigned int maxNeighborIndex=0;

    if ( latticeType==HEXAGONAL_LATTICE )
    {
        //unsigned int indexHex=Y_EVEN|Z_EVEN;
        unsigned int indexHex=0;

        for ( unsigned int i = 0 ; i < hexDistanceArray[indexHex].size() ; ++i )
        {
            maxNeighborIndex=i;
            if ( hexDistanceArray[indexHex][i]>depth )
            {
                maxNeighborIndex=i-1;
                break;
            }
        }
        return maxNeighborIndex;

    }
    else
    {
        for ( unsigned int i = 0 ; i < distanceVec.size() ; ++i )
        {
            maxNeighborIndex=i;
            if ( distanceVec[i]>depth )
            {
                maxNeighborIndex=i-1;
                break;
            }
        }
        return maxNeighborIndex;
    }
}

Neighbor BoundaryStrategy::getNeighborDirect ( Point3D & pt,unsigned int idx, bool checkBounds,bool calculatePtTrans ) const
{
    Neighbor n;

    unsigned int indexHex = 0;

    if ( latticeType==HEXAGONAL_LATTICE )
    {
        indexHex= ( pt.z%3 ) *2+ ( pt.y%2 );
        n.pt=pt+hexOffsetArray[indexHex][idx];
    }
    else
    {
        n.pt=pt+offsetVec[idx];
    }

    //Here I will add condition  if (flagField[pt] ) ...

    if ( !checkBounds || isValid ( n.pt ) )
    {

        // Valid Neighbor
        n.ptTrans=calculatePointCoordinates ( n.pt );
        if ( latticeType==HEXAGONAL_LATTICE )
        {
            n.distance=hexDistanceArray[indexHex][idx];
            if ( calculatePtTrans )
                n.ptTrans=HexCoord ( n.pt );

        }
        else
        {
            n.distance= distanceVec[idx]*lmf.lengthMF;
            if ( calculatePtTrans )
                n.ptTrans=Coordinates3D<double> ( pt.x,pt.y,pt.z );
        }

        return n;
    }
    else
    {
        if ( regular )
        {
            bool x_bool;
            bool y_bool;
            bool z_bool;
            int x=n.pt.x;
            int y=n.pt.y;
            int z=n.pt.z;

            // For each coordinate, if it is not valid, apply condition
            x_bool = ( isValid ( x,dim.x ) ? true : strategy_x->applyCondition ( x, dim.x ) );
            y_bool = ( isValid ( y,dim.y ) ? true : strategy_y->applyCondition ( y, dim.y ) );
            z_bool = ( isValid ( z,dim.z ) ? true : strategy_z->applyCondition ( z, dim.z ) );

            // If all the coordinates of the neighbor are valid then return the
            // neighbor
            if ( x_bool && y_bool && z_bool )
            {
                n.pt.x=x;
                n.pt.y=y;
                n.pt.z=z;
                n.ptTrans=calculatePointCoordinates ( n.pt );
                if ( latticeType==HEXAGONAL_LATTICE )
                {
                    n.distance=hexDistanceArray[indexHex][idx];
                }
                else
                {
                    n.distance= distanceVec[idx]*lmf.lengthMF;
                }
                return n;

            }
            else
            {
                //requesed neighbor does not belong to the lattice
                n.distance=0.0;
                return n;
            }

        }
        else     // can reach no return type of non-void function
        {
            n.distance = 0.0;
            return n;
        }
    }
}