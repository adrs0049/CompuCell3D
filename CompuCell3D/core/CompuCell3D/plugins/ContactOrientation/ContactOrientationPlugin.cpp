#include <CompuCell3D/CC3D_plugin.h>
using namespace CompuCell3D;
#include "ContactOrientationPlugin.h"

ContactOrientationPlugin::ContactOrientationPlugin()
    : pUtils(nullptr), lockPtr(nullptr), xmlData(nullptr),
      angularTermDefined(false), cellFieldG(nullptr), boundaryStrategy(nullptr),
      automaton(nullptr),
      angularTermFcnPtr(&ContactOrientationPlugin::singleTermFormula) {}

ContactOrientationPlugin::~ContactOrientationPlugin() {}

void ContactOrientationPlugin::init(Simulator *simulator, CC3DXMLElement *_xmlData) {
    xmlData=_xmlData;
    sim=simulator;
    potts=simulator->getPotts();
    cellFieldG = (WatchableField3D<CellG *> *)potts->getCellFieldG();
    fieldDim=cellFieldG->getDim();
    pUtils=sim->getParallelUtils();

    update(xmlData,true);

    potts->getCellFactoryGroupPtr()->registerClass(std::make_shared<BasicClassAccessor<ContactOrientationData> >(contactOrientationDataAccessor));
    potts->registerEnergyFunctionWithName(this,"ContactOrientation");

	simulator->registerSteerableObject(this);
}

void ContactOrientationPlugin::extraInit(Simulator *simulator) {}

double ContactOrientationPlugin::singleTermFormula(double _alpha,double _theta) {
    return _alpha*fabs(cos(_theta));
}

double ContactOrientationPlugin::angularTermFunction(double _alpha,double _theta) 
{
    int currentWorkNodeNumber=pUtils->getCurrentWorkNodeNumber();
    ExpressionEvaluator & ev=eed[currentWorkNodeNumber];
    double angularTerm=0.0;

    ev[0]=_alpha;
    ev[1]=_theta;
    angularTerm=ev.eval();

    return angularTerm;
}

double ContactOrientationPlugin::changeEnergy(const Point3D &pt,
											  const CellG *newCell,
											  const CellG *oldCell) 
{
    double energy {0};
    Point3D n;
    CellG *nCell = nullptr;
    WatchableField3D<CellG *> *fieldG =(WatchableField3D<CellG *> *) potts->getCellFieldG();
    Neighbor neighbor;

    //precalculating COMs before and after flip
    Coordinates3D<double> centroidOldAfter;
    Coordinates3D<double> centroidNewAfter;

    Vector3 comOldAfter;
    Vector3 comNewAfter;
    Vector3 comOldBefore;
    Vector3 comNewBefore;

    if(oldCell) {
        comOldBefore=Vector3(oldCell->xCOM,oldCell->yCOM,oldCell->zCOM);

        if(oldCell->volume>1) {
            centroidOldAfter=precalculateCentroid(pt, oldCell, -1,fieldDim,boundaryStrategy);
            comOldAfter=Vector3(centroidOldAfter.X()/(float)(oldCell->volume-1), centroidOldAfter.Y()/(float)(oldCell->volume-1),centroidOldAfter.Z()/(float)(oldCell->volume-1));
        } else {
            comOldAfter=Vector3(oldCell->xCOM,oldCell->yCOM,oldCell->zCOM);
        }
    }

    if(newCell) {
        comNewBefore=Vector3(newCell->xCOM,newCell->yCOM,newCell->zCOM);
        centroidNewAfter=precalculateCentroid(pt, newCell, 1,fieldDim,boundaryStrategy);
        comNewAfter=Vector3(centroidNewAfter.X()/(float)(newCell->volume+1),centroidNewAfter.Y()/(float)(newCell->volume+1),centroidNewAfter.Z()/(float)(newCell->volume+1));
    }

    for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ) {
        neighbor=boundaryStrategy->getNeighborDirect(const_cast<Point3D&>(pt),nIdx);
        if(!neighbor.distance) {
            //if distance is 0 then the neighbor returned is invalid
            continue;
        }

        nCell = fieldG->get(neighbor.pt);
        Vector3 ptVec(pt.x,pt.y,pt.z);

        if(nCell!=oldCell) {

          if ((nCell != nullptr) && (oldCell != nullptr)) {
                double termN=0.0;
                double termOld=0.0;

                Vector3 comNBefore(nCell->xCOM,nCell->yCOM,nCell->zCOM);
                Vector3 oldCellPolVector=getOriantationVector(oldCell);
                Vector3 nCellPolVector=getOriantationVector(nCell);
                double alphaOld=getAlpha(oldCell);
                double alphaN=getAlpha(nCell);
                Vector3 oldDistVec=ptVec-comOldBefore;
                Vector3 nDistVec=ptVec-comNBefore;

                double thetaOld=oldDistVec.Angle(oldCellPolVector);
                double thetaN=nDistVec.Angle(nCellPolVector);

                if (oldCell->volume>1) {
                    //termOld=alphaOld*fabs(cos(thetaOld));
                    //termOld=alphaOld*cos(thetaOld);
                    // termOld=singleTermFormula(alphaOld,thetaOld);
                    termOld=(this->*angularTermFcnPtr)(alphaOld,thetaOld);
                } else {
                    termOld=0.0;
                }

                termN=(this->*angularTermFcnPtr)(alphaN,thetaN);

                if((nCell->clusterId) != (oldCell->clusterId)) 
                    energy-=termOld+termN;
            } 
        }

        //watch for case nCell=oldCell - use oldCellCOMAfter for calculations
        if(nCell!=newCell) {

          if ((newCell != nullptr) && (nCell != nullptr)) {

                double termN=0.0;
                double termNew=0.0;
                Vector3 comNAfter;
                //notice, that if oldCell is about to disappear - it has Vol=1 before spin flip
                //then it will never be picked as a after-flip neighbor of the new cell and consequently
                // statement : comNAfter=comOldAfter; will never be called so there is no issue what COM of oldCell after it disappears should be
                if (nCell==oldCell) {
                    comNAfter=comOldAfter;
                } else {
                    comNAfter=Vector3(nCell->xCOM,nCell->yCOM,nCell->zCOM);
                }

                Vector3 newCellPolVector=getOriantationVector(newCell);
                Vector3 nCellPolVector=getOriantationVector(nCell);
                double alphaNew=getAlpha(newCell);
                double alphaN=getAlpha(nCell);

                Vector3 newDistVec=ptVec-comNewAfter;
                Vector3 nDistVec=ptVec-comNAfter;

                double thetaNew=newDistVec.Angle(newCellPolVector);
                double thetaN=nDistVec.Angle(nCellPolVector);

                termNew=(this->*angularTermFcnPtr)(alphaNew,thetaNew);
                termN=(this->*angularTermFcnPtr)(alphaN,thetaN);

                if((nCell->clusterId) != (newCell->clusterId)) 
                    energy+=termNew+termN;

            }
        }
    }

    return energy;
}

void ContactOrientationPlugin::handleEvent(CC3DEvent & _event) {
    if (_event.id==CHANGE_NUMBER_OF_WORK_NODES) 
        update(xmlData);
}

void ContactOrientationPlugin::update(CC3DXMLElement *_xmlData, bool _fullInitFlag) {
    //PARSE XML IN THIS FUNCTION
    //For more information on XML parser function please see CC3D code or lookup XML utils API
    automaton = potts->getAutomaton();
    ASSERT_OR_THROW("CELL TYPE PLUGIN WAS NOT PROPERLY INITIALIZED YET. MAKE SURE THIS IS THE FIRST PLUGIN THAT YOU SET", automaton)

    angularTermDefined=false;

    if (_xmlData->findElement("AngularTerm")) {
        unsigned int maxNumberOfWorkNodes=pUtils->getMaxNumberOfWorkNodesPotts();
        eed.allocateSize(maxNumberOfWorkNodes);
        vector<string> variableNames;
        variableNames.push_back("Alpha");
        variableNames.push_back("Theta");

        eed.addVariables(variableNames.begin(),variableNames.end());
        eed.update(_xmlData->getFirstElement("AngularTerm"));
        angularTermDefined=true;
        angularTermFcnPtr=&ContactOrientationPlugin::angularTermFunction;
    } else {
        angularTermDefined=false;
        angularTermFcnPtr=&ContactOrientationPlugin::singleTermFormula;
    }

    //Here I initialize max neighbor index for direct acces to the list of neighbors
    boundaryStrategy=BoundaryStrategy::getInstance();
    maxNeighborIndex=0;

    if(_xmlData->getFirstElement("Depth")) {
        maxNeighborIndex=boundaryStrategy->getMaxNeighborIndexFromDepth(_xmlData->getFirstElement("Depth")->getDouble());
        //cerr<<"got here will do depth"<<endl;
    } else {
        //cerr<<"got here will do neighbor order"<<endl;
        if(_xmlData->getFirstElement("NeighborOrder")) {

            maxNeighborIndex=boundaryStrategy->getMaxNeighborIndexFromNeighborOrder(_xmlData->getFirstElement("NeighborOrder")->getUInt());
        } else {
            maxNeighborIndex=boundaryStrategy->getMaxNeighborIndexFromNeighborOrder(1);

        }

    }

    cerr<<"Contact maxNeighborIndex="<<maxNeighborIndex<<endl;
}

double ContactOrientationPlugin::contactEnergy(const CellG *cell1, const CellG *cell2) {
    return contactEnergyArray[cell1 ? cell1->type : 0][cell2? cell2->type : 0];
}

void ContactOrientationPlugin::setContactEnergy(const string typeName1,const string typeName2, const double energy) {
    char type1 = automaton->getTypeId(typeName1);
    char type2 = automaton->getTypeId(typeName2);

    int index = getIndex(type1, type2);

    auto it = contactEnergies.find(index);
    ASSERT_OR_THROW(string("Contact energy for ") + typeName1 + " " + typeName2 +
                    " already set!", it == contactEnergies.end());

    contactEnergies[index] = energy;
}

int ContactOrientationPlugin::getIndex(const int type1, const int type2) const {
    if (type1 < type2) return ((type1 + 1) | ((type2 + 1) << 16));
    else return ((type2 + 1) | ((type1 + 1) << 16));
}

void ContactOrientationPlugin::setOriantationVector(CellG * _cell,double _x, double _y, double _z) {
    contactOrientationDataAccessor.get(_cell->extraAttribPtr)->oriantationVec=Vector3(_x,_y,_z);
}

Vector3 ContactOrientationPlugin::getOriantationVector(const CellG * _cell) {
    return contactOrientationDataAccessor.get(_cell->extraAttribPtr)->oriantationVec;
}

void ContactOrientationPlugin::setAlpha(CellG * _cell,double _alpha) {
    contactOrientationDataAccessor.get(_cell->extraAttribPtr)->alpha=_alpha;
}
double ContactOrientationPlugin::getAlpha(const CellG * _cell) {
    return contactOrientationDataAccessor.get(_cell->extraAttribPtr)->alpha;
}

std::string ContactOrientationPlugin::toString() {
    return "ContactOrientation";
}

std::string ContactOrientationPlugin::steerableName() {
    return toString();
}