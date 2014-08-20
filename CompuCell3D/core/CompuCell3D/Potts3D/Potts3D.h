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

#ifndef POTTS3D_H
#define POTTS3D_H

#include <CompuCell3D/Field3D/CC3D_Field3D.h>
#include <CompuCell3D/Boundary/CC3D_Boundary.h>
#include "DefaultAcceptanceFunction.h"
#include "FirstOrderExpansionAcceptanceFunction.h"
#include "CustomAcceptanceFunction.h"
#include "DynamicAcceptanceFunction.h"

#include <BasicUtils/BasicUtils.h>
#include "CellInventory.h"
#include "Cell.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <BasicUtils/memory_include.h>
#include <CompuCell3D/Boundary/BoundaryTypeDefinitions.h>
#include <CompuCell3D/SteerableObject.h>

#include <PublicUtilities/Units/Unit.h>
#include <muParser/ExpressionEvaluator/ExpressionEvaluator.h>

#include "CC3D_Potts_fwd.h"

class BasicRandomNumberGeneratorNonStatic;

namespace CompuCell3D
{
class EnergyFunction;
class AcceptanceFunction;
class FluctuationAmplitudeFunction;
class CellGChangeWatcher;
class BCGChangeWatcher;
class Stepper;
class FixedStepper;
class Automaton;
class TypeTransition;
class TypeChangeWatcher;
class AttributeAdder;
class EnergyFunctionCalculator;
class Simulator;
class CellTypeMotilityData;
class ParallelUtilsOpenMP;

template<typename T>
class WatchableField3D;

template<typename T>
class Field3D;

template<typename T>
class Field3DImpl;

typedef std::shared_ptr<WatchableField3D<CellG *> > cellFieldPtr;

/**
 * A generic implementation of the potts model in 3D.
 *
 * See http://scienceworld.wolfram.com/physics/PottsModel.html for
 * more information about the potts model.
 */
class /*DECLSPECIFIER*/ Potts3D : public SteerableObject
{
	
private:
    /// The potts field of cell spins.
    cellFieldPtr cellFieldG;
    AttributeAdder * attrAdder;
    std::unique_ptr<EnergyFunctionCalculator> energyCalculator;
	
    /// Cell class aggregator and allocator.
    BasicClassGroupFactory cellFactoryGroup; 	//creates aggregate of objects associated with cell
    //DOES NOT creat group of cells (as a name might suggest)

    /// An array of energy functions to be evaluated to determine energy costs.
    std::vector<EnergyFunction *> energyFunctions;
    EnergyFunction * connectivityConstraint;

    std::map<std::string,EnergyFunction *> nameToEnergyFuctionMap;
    std::set<Point3D> boundaryPixelSet;
    std::vector<BasicRandomNumberGeneratorNonStatic> randNSVec;

    TypeTransition *typeTransition; //fires up automatic tasks associated with type reassignment

    /// An array of potts steppers.  These are called after each potts step.
    std::vector<Stepper *> steppers;
    std::vector<FixedStepper *> fixedSteppers;
    /// The automaton to use.  Assuming one automaton per simulation.
    Automaton* automaton;

//     DefaultAcceptanceFunction defaultAcceptanceFunction;
//     FirstOrderExpansionAcceptanceFunction   firstOrderExpansionAcceptanceFunction;

    /// Used to determine the probablity that a pixel flip should be taken.
	// should be unique
    std::unique_ptr<AcceptanceFunction> acceptanceFunction;

    //ExpressionEvaluatorDepot acceptanceEed;
//     CustomAcceptanceFunction customAcceptanceFunction;
    bool customAcceptanceExpressionDefined;

    std::unique_ptr<FluctuationAmplitudeFunction> fluctAmplFcn;

    /// The current total energy of the system.
    double energy;

    std::string boundary_x; // boundary condition for x axiz
    std::string boundary_y; // boundary condition for y axis
    std::string boundary_z; // boundary condition for z axis

    /// This object keeps track of all cells available in the simulations. It allows for simple iteration over all the cells
    /// It becomes useful whenever one has to visit all the cells. Without inventory one would need to go pixel-by-pixel - very inefficient
    CellInventory cellInventory;

    Point3D flipNeighbor;
    std::vector<Point3D> flipNeighborVec; //for parallel access

    double depth;
    //int maxNeighborOrder;
    std::vector<Point3D> neighbors;
    std::vector<unsigned char> frozenTypeVec;///lists types which will remain frozen throughout the simulation
    unsigned int sizeFrozenTypeVec;

    long recentlyCreatedCellId;
    long recentlyCreatedClusterId;

    unsigned int currentAttempt;
    unsigned int numberOfAttempts;
    unsigned int maxNeighborIndex;
    unsigned int debugOutputFrequency;
    Simulator *sim;
    Point3D minCoordinates;
    Point3D maxCoordinates;
    unsigned int attemptedEC;
    unsigned int flips;
    std::vector<float> cellTypeMotilityVec;

    //units
    Unit massUnit;
    Unit lengthUnit;
    Unit timeUnit;
    Unit energyUnit;
    bool displayUnitsFlag;
    double temperature;
    ParallelUtilsOpenMP *pUtils;

public:

    Potts3D();
    Potts3D ( const Dim3D dim );
    virtual ~Potts3D();

    /**
     * Create the cell field.
     *
     * @param dim The field dimensions.
     */
    void createCellField ( const Dim3D dim );
    void resizeCellField ( const Dim3D dim, Dim3D shiftVec=Dim3D() );
    //void resizeCellField(const std::vector<int> & dim, const std::vector<int> & shiftVec);

    double getTemperature() const
    {
        return temperature;
    }
    unsigned int getCurrentAttempt()
    {
        return currentAttempt;
    }
    unsigned int getNumberOfAttempts()
    {
        return numberOfAttempts;
    }
    unsigned int getNumberOfAttemptedEnergyCalculations()
    {
        return attemptedEC;
    }
    unsigned int getNumberOfAcceptedSpinFlips()
    {
        return flips;
    }
    void registerConnectivityConstraint ( EnergyFunction * _connectivityConstraint );
    EnergyFunction * getConnectivityConstraint();

    bool checkIfFrozen ( unsigned char _type );

    std::set<Point3D> * getBoundaryPixelSetPtr()
    {
        return & boundaryPixelSet;
    }

    LatticeType getLatticeType();

    double getDepth()
    {
        return depth;
    }
    
    void setDepth ( double _depth );
    void setNeighborOrder ( unsigned int _neighborOrder );

    void initializeCellTypeMotility ( std::vector<CellTypeMotilityData> & _cellTypeMotilityVector );
    void setCellTypeMotilityVec ( std::vector<float> & _cellTypeMotilityVec );
    const std::vector<float> & getCellTypeMotilityVec() const
    {
        return cellTypeMotilityVec;
    }

    void setDebugOutputFrequency ( unsigned int _freq )
    {
        debugOutputFrequency=_freq;
    }
    
    void setSimulator ( Simulator *_sim )
    {
        sim=_sim;
    }

    void setFrozenTypeVector ( std::vector<unsigned char> _frozenTypeVec );
    const std::vector<unsigned char> & getFrozenTypeVector()
    {
        return frozenTypeVec;
    }

    void setMassUnit ( const Unit & _unit )
    {
        massUnit=_unit;
    }
    
    void setLengthUnit ( const Unit & _unit )
    {
        lengthUnit=_unit;
    }
    
    void setTimeUnit ( const Unit & _unit )
    {
        timeUnit=_unit;
    }
    
    void setEnergyUnit ( const Unit & _unit )
    {
        energyUnit=_unit;
    }

    Unit getMassUnit()
    {
        return massUnit;
    }
    
    Unit getLengthUnit()
    {
        return lengthUnit;
    }
    
    Unit getTimeUnit()
    {
        return timeUnit;
    }
    
    Unit getEnergyUnit()
    {
        return energyUnit;
    }
    
    bool getDisplayUnitsFlag()
    {
        return displayUnitsFlag;
    }

    void updateUnits ( CC3DXMLElement * );

    Point3D getFlipNeighbor();

    void setBoundaryXName ( std::string const & _xName )
    {
        boundary_x=_xName;
    }
    
    void setBoundaryYName ( std::string const & _yName )
    {
        boundary_y=_yName;
    }
    
    void setBoundaryZName ( std::string const & _zName )
    {
        boundary_z=_zName;
    }
    
    std::string const & getBoundaryXName() const
    {
        return boundary_x;
    }
    
    std::string const & getBoundaryYName() const
    {
        return boundary_y;
    }
    
    std::string const & getBoundaryZName() const
    {
        return boundary_z;
    }

    void setMinCoordinates ( Point3D _minCoord )
    {
        minCoordinates=_minCoord;
    }
    
    void setMaxCoordinates ( Point3D _maxCoord )
    {
        maxCoordinates=_maxCoord;
    }
    
    Point3D getMinCoordinates() const
    {
        return minCoordinates;
    }
    
    Point3D getMaxCoordinates() const
    {
        return maxCoordinates;
    }
    
    TypeTransition *getTypeTransition()
    {
        return typeTransition;
    }
    
    virtual void createEnergyFunction ( std::string _energyFunctionType );
    std::unique_ptr<EnergyFunctionCalculator>& getEnergyFunctionCalculator()
    {
        return energyCalculator;
    }

    CellInventory &getCellInventory()
    {
        return cellInventory;
    }

    virtual void registerAttributeAdder ( AttributeAdder * _attrAdder );
    /// Add an energy function to the list.
    virtual void registerEnergyFunction ( EnergyFunction *function );
    virtual void registerEnergyFunctionWithName ( EnergyFunction *_function,std::string _functionName );
    virtual void unregisterEnergyFunction ( std::string _functionName );

    double getNewNumber()
    {
        return energy;
    }

    /// Add the automaton.
    virtual void registerAutomaton ( Automaton* autom );

    /// Return the automaton for this simulation.
    virtual Automaton* getAutomaton();
    void setParallelUtils ( ParallelUtilsOpenMP *_pUtils )
    {
        pUtils=_pUtils;
    }
    /**
     * Set the acceptance function.  This is DefaultAcceptanceFunction
     * by default.
     */
#ifndef SWIG
    virtual void registerAcceptanceFunction ( std::unique_ptr<AcceptanceFunction> function );
#endif
    virtual void setAcceptanceFunctionByName ( std::string _acceptanceFunctionName );
    virtual std::unique_ptr<AcceptanceFunction>& getAcceptanceFunction()
    {
        return acceptanceFunction;
    }

    virtual void setFluctuationAmplitudeFunctionByName ( std::string _fluctuationAmplitudeFunctionName );

    /// registration of the BCG watcher
    virtual void registerCellGChangeWatcher ( CellGChangeWatcher *_watcher );

    /// Register accessor to a class with a cellGroupFactory. Accessor will access a class which is a mamber of a BasicClassGroup
    virtual void registerClassAccessor ( std::shared_ptr<BasicClassAccessorBase> _accessor );

    /// Add a potts stepper to be called after each potts step.
    virtual void registerStepper(Stepper *stepper);
    virtual void registerFixedStepper(FixedStepper *fixedStepper, bool _front=false);
    virtual void unregisterFixedStepper(FixedStepper *fixedStepper);

    /**
     * @return Current energy.
     */
    double getEnergy();

    /**
     * Allocate a cell.
     *
     * @param pt The point in the cell field to put the new cell.
     *
     * @return The newly created cell.
     */
    virtual CellG *createCellG ( const Point3D pt, long _clusterId=-1 );
    virtual CellG *createCellGSpecifiedIds ( const Point3D pt,long _cellId,long _clusterId=-1 );
    virtual CellG *createCell ( long _clusterId=-1 );
    virtual CellG *createCellSpecifiedIds ( long _cellId,long _clusterId=-1 );
    virtual void destroyCellG ( CellG * cell,bool _removeFromInventory=true );

    BasicClassGroupFactory * getCellFactoryGroupPtr()
    {
        return &cellFactoryGroup;
    };

    /// @return The current number of cells in the field.
    virtual unsigned int getNumCells()
    {
        return cellInventory.getCellInventorySize();
    }

    /**
     * Calculate the current total energy of the system.
     *
     * @return The freshly calculated energy.
     */
    virtual double totalEnergy();

    /**
     * Calculate the energy change of a proposed flip.
     *
     * @param pt The point to flip.
     * @param newCell The new spin.
     * @param oldCell The old spin.
     *
     * @return The energy change.
     */
    virtual double changeEnergy ( Point3D pt, const CellG *newCell,
                                  const CellG *oldCell );


    /**
     * Run the potts metropolis algorithm.
     *
     * @param steps The numer of flip attempts to make.
     * @param temp The current temperature.
     *
     * @return The total number of accepted flips.
     */
    virtual unsigned int metropolis ( const unsigned int steps,
                                      const double temp );

    typedef unsigned int ( Potts3D::*metropolisFcnPtr_t ) ( const unsigned int , const double );

    metropolisFcnPtr_t metropolisFcnPtr;

    unsigned int metropolisList ( const unsigned int steps, const double temp );
    unsigned int metropolisFast ( const unsigned int steps, const double temp );
    unsigned int metropolisBoundaryWalker ( const unsigned int steps, const double temp );
    void setMetropolisAlgorithm ( std::string _algName );

    /// @return A pointer to the potts field of BasicClassGroup.
 	virtual std::shared_ptr<Field3D<CellG *> > getCellField3DG ()
    {
 		return std::static_pointer_cast<Field3D<CellG *> >(cellFieldG);
        //return ( Field3D<CellG *> * ) cellFieldG;
    }

    /// @return A pointer to the potts cell field.
    virtual std::shared_ptr<Field3DImpl<CellG *> > getCellFieldGImpl ()
    {
 		return std::static_pointer_cast<Field3DImpl<CellG *> >(cellFieldG);
//         return ( Field3DImpl<CellG *> * ) cellFieldG;
    }

    virtual cellFieldPtr getCellFieldG ()
	{
		return cellFieldG;
	}
    
    //SteerableObject interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual void runSteppers();
    long getRecentlyCreatedClusterId()
    {
        return recentlyCreatedClusterId;
    }
    long getRecentlyCreatedCellId()
    {
        return recentlyCreatedCellId;
    }
};

typedef Potts3D* Potts3DPtr;

} // end namespace
#endif