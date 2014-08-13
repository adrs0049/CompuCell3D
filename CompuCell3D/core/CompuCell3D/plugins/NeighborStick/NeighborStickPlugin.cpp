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
#include <CompuCell3D/plugins/NeighborTracker/NeighborTrackerPlugin.h>
#include <limits>
using namespace CompuCell3D;
#include "NeighborStickPlugin.h"

NeighborStickPlugin::NeighborStickPlugin() : thresh(0), xmlData(nullptr) {}

NeighborStickPlugin::~NeighborStickPlugin() {
  
}

void NeighborStickPlugin::init(Simulator *simulator, CC3DXMLElement *_xmlData) {

  xmlData=_xmlData;
  potts=simulator->getPotts();
  simulator->getPotts()->registerEnergyFunctionWithName(this,"NeighborStick");
  simulator->registerSteerableObject(this);

   bool pluginAlreadyRegisteredFlag;
   auto neighborTrackerPluginPtr = get_plugin<NeighborTrackerPlugin>("NeighborTracker", &pluginAlreadyRegisteredFlag);
   if (!pluginAlreadyRegisteredFlag){
      neighborTrackerPluginPtr->init(simulator);
      ASSERT_OR_THROW("NeighborTracker plugin not initialized!", neighborTrackerPluginPtr);
      neighborTrackerAccessorPtr=neighborTrackerPluginPtr->getNeighborTrackerAccessorPtr();
      ASSERT_OR_THROW("neighborAccessorPtr  not initialized!", neighborTrackerAccessorPtr);
   }
}

void NeighborStickPlugin::extraInit(Simulator *simulator){
	update(xmlData,true);
}

void NeighborStickPlugin::update(CC3DXMLElement *_xmlData, bool _fullInitFlag){
   
   automaton = potts->getAutomaton();
   set<unsigned char> cellTypesSet;

	
	thresh=_xmlData->getFirstElement("Threshold")->getDouble();
	typeNamesString=_xmlData->getFirstElement("Types")->getText();
	parseStringIntoList(typeNamesString , typeNames , ",");
	cerr<<"NEIGHBOR STICK typeNamesString="<<typeNamesString<<endl;

   std::vector<std::string> temp;

   for (auto &elem : typeNames) {
     temp.push_back(elem);
     cerr << "typeNames[i]=" << elem << endl;
   }
   //typeNames.clear();
   //typeNames=temp;
   //typeNames.pop_back(); //delete empyt element
   for (auto &elem : typeNames) {
     idNames.push_back(automaton->getTypeId(elem));

     cerr << "adding type ID = " << automaton->getTypeId(elem) << endl;
   }   



}

double NeighborStickPlugin::changeEnergy(const Point3D &pt,
                                  const CellG *newCell,
                                  const CellG *oldCell) {
//    cerr<<"ChangeEnergy"<<endl;
   
   
  double energy = 0;
  unsigned int token = 0;
  double distance = 0;
  Point3D n;
  int totalArea = 0;
  CellG *nCell = nullptr;
  WatchableField3D<CellG *> *fieldG = (WatchableField3D<CellG *> *)potts->getCellFieldG();
  Neighbor neighbor;
  
  std::set<NeighborSurfaceData> * neighborData;
  std::set<NeighborSurfaceData >::iterator sitr;
  
//   cerr << "Threshold: " << thresh << endl;
//   cerr << "  SIZE: " << typeNames.size() << endl;
//   for(int i = 0; i < typeNames.size(); i++) {
//      cerr << "Name: " << typeNames[i] << " ID: " << (int)automaton->getTypeId(typeNames[i]) << " and again: " << idNames[i] << endl;
//   }
  
  
  
     

   if(oldCell) {
      neighborData = &(neighborTrackerAccessorPtr->get(oldCell->extraAttribPtr)->cellNeighbors);
      
      for(sitr=neighborData->begin() ; sitr != neighborData->end() ; ++sitr){
         
          //cerr << "Type: " << (int)oldCell->type << " ID: " << oldCell->id << endl;
         nCell= sitr->neighborAddress;
         if(nCell){
            int nType = (int)nCell->type;
//             cerr << "Neighbor Type: " << nType << endl;
            for (auto &elem : idNames) {
              if (nType == elem) {
                  //cerr << "SAME TYPE step: " << i << endl;
                  //cerr << "Common Surface Area: " << (int)sitr->commonSurfaceArea << endl;
                  
                  totalArea +=(int)sitr->commonSurfaceArea;
                  break;
               }
//               else {
//                  continue;
////                   cerr << "Wrong Type" << endl;
//               }
            }
         }
      
      }
   }
	//cerr<<"totalArea="<<totalArea<<" thresh="<<thresh<<endl;
   if((totalArea < thresh) & (totalArea != 0)){
		return (std::numeric_limits<float>::max)()/2.0;
   }
   else {
      return 0;
   }
}




std::string NeighborStickPlugin::toString(){

   return "NeighborStick";

}

std::string NeighborStickPlugin::steerableName(){

   return toString();

}


