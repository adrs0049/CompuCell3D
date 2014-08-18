
#include <CompuCell3D/CC3D_plugin.h>
using namespace CompuCell3D;
#include "CellTypeMonitorPlugin.h"

CellTypeMonitorPlugin::CellTypeMonitorPlugin()
    : pUtils ( nullptr ), lockPtr ( nullptr ), xmlData ( nullptr ), cellFieldG ( nullptr ),
      boundaryStrategy ( nullptr ), cellTypeArray ( nullptr ), mediumType ( 0 )
{}

CellTypeMonitorPlugin::~CellTypeMonitorPlugin()
{
    pUtils->destroyLock ( lockPtr );
    delete lockPtr;
    lockPtr = nullptr;

    if ( cellTypeArray )
    {
        delete cellTypeArray;
        cellTypeArray = nullptr;
    }
}

void CellTypeMonitorPlugin::init ( Simulator *simulator, CC3DXMLElement *_xmlData )
{
    xmlData=_xmlData;
    sim=simulator;
    potts=simulator->getPotts();
    cellFieldG = potts->getCellFieldG();

    pUtils=sim->getParallelUtils();
    lockPtr=new ParallelUtilsOpenMP::OpenMPLock_t;
    pUtils->initLock ( lockPtr );

    update ( xmlData,true );

    Dim3D fieldDim=cellFieldG->getDim();
    cellTypeArray=new Array3DCUDA<unsigned char> ( fieldDim,mediumType );
    cellIdArray=new Array3DCUDA<float> ( fieldDim,-1 ); //we assume medium cell id is -1 not zero because normally cells in older versions of CC3D we allwoed cells with id 0

    potts->registerCellGChangeWatcher ( this );
    simulator->registerSteerableObject ( this );
}

void CellTypeMonitorPlugin::extraInit ( Simulator *simulator )
{

}

void CellTypeMonitorPlugin::field3DChange ( const Point3D &pt, CellG *newCell, CellG *oldCell )
{
    //This function will be called after each succesful pixel copy - field3DChange does usuall ohusekeeping tasks to make sure state of cells, and state of the lattice is uptdate
    // here we keep track of a cell type at a given position
    if ( newCell )
    {
        cellTypeArray->set ( pt,newCell->type );
        cellIdArray->set ( pt,newCell->id );
    }
    else
    {
        cellTypeArray->set ( pt,0 );
        cellIdArray->set ( pt,0 );
    }
}

void CellTypeMonitorPlugin::update ( CC3DXMLElement *_xmlData, bool _fullInitFlag )
{
    //PARSE XML IN THIS FUNCTION
    //For more information on XML parser function please see CC3D code or lookup XML utils API
    automaton = potts->getAutomaton();
    ASSERT_OR_THROW ( "CELL TYPE PLUGIN WAS NOT PROPERLY INITIALIZED YET. MAKE SURE THIS IS THE FIRST PLUGIN THAT YOU SET", automaton )


    //boundaryStrategy has information aobut pixel neighbors
    boundaryStrategy=BoundaryStrategy::getInstance();

}

std::string CellTypeMonitorPlugin::toString()
{
    return "CellTypeMonitor";
}

std::string CellTypeMonitorPlugin::steerableName()
{
    return toString();
}