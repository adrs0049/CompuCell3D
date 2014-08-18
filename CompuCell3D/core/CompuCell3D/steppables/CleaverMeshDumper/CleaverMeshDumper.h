#ifndef CLEAVERMESHDUMPERSTEPPABLE_H
#define CLEAVERMESHDUMPERSTEPPABLE_H

#include <CompuCell3D/CC3D.h>
#include "CleaverMeshDumperDLLSpecifier.h"
#include <Cleaver/ScalarField.h>
#include <Cleaver/BoundingBox.h>

namespace Cleaver
{
class TetMesh;
};

namespace CompuCell3D
{

template <class T> class Field3D;
template <class T> class WatchableField3D;

class Potts3D;
class Automaton;
class CellInventory;
class CellG;

class CLEAVERMESHDUMPER_EXPORT CellFieldCleaverSimulator : public Cleaver::ScalarField
{
public:
    CellFieldCleaverSimulator();
    ~CellFieldCleaverSimulator();

    virtual float valueAt ( float x, float y, float z ) const;
    virtual Cleaver::BoundingBox bounds() const;

    mutable float minValue,maxValue;
    void setFieldDim ( Dim3D _dim );
    void setCellFieldPtr ( cellFieldPtr _cellField )
    {
        cellField=_cellField;
    }
    void setIncludeCellTypesSet ( std::set<unsigned char> & _cellTypeSet )
    {
        includeCellTypesSet=_cellTypeSet;
        end_sitr=includeCellTypesSet.end();
    }
private:
    Cleaver::BoundingBox m_bounds;
    Dim3D fieldDim;
    Dim3D paddingDim;
    cellFieldPtr cellField;
    std::set<unsigned char> includeCellTypesSet;
    std::set<unsigned char>::iterator end_sitr;
};

class CLEAVERMESHDUMPER_EXPORT CleaverMeshDumper : public Steppable
{
    cellFieldPtr cellFieldG;
    SimulatorPtr  sim;
    Potts3DPtr potts;
    CC3DXMLElement *xmlData;
    AutomatonPtr automaton;
    BoundaryStrategyPtr boundaryStrategy;
    CellInventory * cellInventoryPtr;

    Dim3D fieldDim;

public:
    CleaverMeshDumper ();
    virtual ~CleaverMeshDumper ();
    // SimObject interface
    virtual void init ( SimulatorPtr simulator, CC3DXMLElement *_xmlData=0 );
    virtual void extraInit ( SimulatorPtr simulator );

    //steppable interface
    virtual void start();
    virtual void step ( const unsigned int currentStep );
    virtual void finish() {}

    //SteerableObject interface
    virtual void update ( CC3DXMLElement *_xmlData, bool _fullInitFlag=false );
    virtual std::string steerableName();
    virtual std::string toString();

    void simulateCleaverMesh();

private:
    std::string outputFileName;
    std::string meshOutputFormat;
    bool outputMeshSurface;
    std::set<unsigned char> cellTypesSet;
    bool verbose;
};
} // end namespace
#endif
