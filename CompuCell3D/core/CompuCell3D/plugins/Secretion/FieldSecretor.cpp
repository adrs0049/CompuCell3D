#include <CompuCell3D/CC3D_plugin.h>
#include <CompuCell3D/plugins/PixelTracker/PixelTrackerPlugin.h>
#include <CompuCell3D/plugins/BoundaryPixelTracker/BoundaryPixelTrackerPlugin.h>
#include <CompuCell3D/plugins/BoundaryPixelTracker/BoundaryPixelTracker.h>
#include <CompuCell3D/Field3D/CC3D_Field3D.h>

using namespace CompuCell3D;
using namespace std;

#include "FieldSecretor.h"

FieldSecretor::FieldSecretor()
    : concentrationFieldPtr(nullptr), boundaryPixelTrackerPlugin(nullptr),
      pixelTrackerPlugin(nullptr), boundaryStrategy(nullptr),
      maxNeighborIndex(0), cellFieldG(nullptr) {}

FieldSecretor::~FieldSecretor()
{}

// NOTICE, exceptions are thrown from the python wrapper functions defined in CompuCellExtraDeclarations.i

bool FieldSecretor::_secreteInsideCell(CellG * _cell, float _amount){
	
	
	if (!pixelTrackerPlugin){		
		ASSERT_OR_THROW("PixelTracker Plugin has been turned off. Cannot execute secreteInsideCell function",pixelTrackerPlugin);
		return false;
	}
	BasicClassAccessor<PixelTracker> *pixelTrackerAccessorPtr=pixelTrackerPlugin->getPixelTrackerAccessorPtr();
	set<PixelTrackerData > & pixelSetRef=pixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;
        for (const auto &elem : pixelSetRef) {

          concentrationFieldPtr->set(
              elem.pixel, concentrationFieldPtr->get(elem.pixel) + _amount);
        }

	return true;
}

bool FieldSecretor::_secreteInsideCellAtBoundary(CellG * _cell, float _amount){

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

        for (const auto &elem : pixelSetRef) {

          concentrationFieldPtr->set(
              elem.pixel, concentrationFieldPtr->get(elem.pixel) + _amount);
        }

	return true;

}

bool FieldSecretor::_secreteInsideCellAtBoundaryOnContactWith(CellG * _cell, float _amount,const std::vector<unsigned char> & _onContactVec){
	
	set<unsigned char> onContactSet(_onContactVec.begin(),_onContactVec.end());

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}

			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if(nCell!=_cell && !nCell && onContactSet.find(0)!=onContactSet.end()){
				//user requested secrete on contact with medium and we found medium pixel
                          concentrationFieldPtr->set(
                              elem.pixel,
                              concentrationFieldPtr->get(elem.pixel) + _amount);
                                break; //after secreting do not try to secrete more
			}
			
			if (nCell!=_cell && nCell && onContactSet.find(nCell->type)!=onContactSet.end() )
			{
				//user requested secretion on contact with cell type whose pixel we have just found
                          concentrationFieldPtr->set(
                              elem.pixel,
                              concentrationFieldPtr->get(elem.pixel) + _amount);
                                break;//after secreting do not try to secrete more
			}

		}

	}

	return true;

}


bool FieldSecretor::_secreteOutsideCellAtBoundary(CellG * _cell, float _amount){

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	set<FieldSecretorPixelData> visitedPixels;

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}
			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if (nCell!=_cell && visitedPixels.find(FieldSecretorPixelData(neighbor.pt))==visitedPixels.end()){

				concentrationFieldPtr->set(nPt,concentrationFieldPtr->get(nPt)+_amount);
				visitedPixels.insert(FieldSecretorPixelData(nPt));

			}

		}		

	}

	return true;


}


bool FieldSecretor::_secreteOutsideCellAtBoundaryOnContactWith(CellG * _cell, float _amount, const std::vector<unsigned char> & _onContactVec){

	set<unsigned char> onContactSet(_onContactVec.begin(),_onContactVec.end());

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	set<FieldSecretorPixelData> visitedPixels;

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}
			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if (nCell!=_cell && visitedPixels.find(FieldSecretorPixelData(neighbor.pt))==visitedPixels.end()){
				if (!nCell && onContactSet.find(0)!=onContactSet.end()){
					//checking if the unvisited pixel belongs to Medium and if Medium is a  cell type listed in the onContactSet
					concentrationFieldPtr->set(nPt,concentrationFieldPtr->get(nPt)+_amount);
					visitedPixels.insert(FieldSecretorPixelData(nPt));
				}

				if (nCell && onContactSet.find(nCell->type)!=onContactSet.end()){
					//checking if the unvisited pixel belongs to a  cell type listed in the onContactSet
					concentrationFieldPtr->set(nPt,concentrationFieldPtr->get(nPt)+_amount);
					visitedPixels.insert(FieldSecretorPixelData(nPt));
				}

			}

		}		

	}

	return true;


}


bool FieldSecretor::secreteInsideCellAtCOM(CellG * _cell, float _amount){
	Point3D pt((int)round(_cell->xCM/_cell->volume),(int)round(_cell->yCM/_cell->volume),(int)round(_cell->zCM/_cell->volume));

	concentrationFieldPtr->set(pt,concentrationFieldPtr->get(pt)+_amount);
	return true;

}

bool FieldSecretor::_uptakeInsideCell(CellG * _cell, float _maxUptake, float _relativeUptake){

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	float currentConcentration;
        for (const auto &elem : pixelSetRef) {
          currentConcentration = concentrationFieldPtr->get(elem.pixel);
                if(currentConcentration*_relativeUptake>_maxUptake){
                  concentrationFieldPtr->set(elem.pixel,
                                             currentConcentration - _maxUptake);
                }else{
                  concentrationFieldPtr->set(
                      elem.pixel, currentConcentration -
                                      currentConcentration * _relativeUptake);
                }				

	}



	return true;
}

bool FieldSecretor::_uptakeInsideCellAtBoundary(CellG * _cell, float _maxUptake, float _relativeUptake){

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	float currentConcentration;
        for (const auto &elem : pixelSetRef) {

          currentConcentration = concentrationFieldPtr->get(elem.pixel);
                if(currentConcentration*_relativeUptake>_maxUptake){
                  concentrationFieldPtr->set(elem.pixel,
                                             currentConcentration - _maxUptake);
                }else{
                  concentrationFieldPtr->set(
                      elem.pixel, currentConcentration -
                                      currentConcentration * _relativeUptake);
                }				

	}

	return true;
}

bool FieldSecretor::_uptakeInsideCellAtBoundaryOnContactWith(CellG * _cell, float _maxUptake, float _relativeUptake,const std::vector<unsigned char> & _onContactVec){

	set<unsigned char> onContactSet(_onContactVec.begin(),_onContactVec.end());

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	float currentConcentration;
        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}

			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if(nCell!=_cell && !nCell && onContactSet.find(0)!=onContactSet.end()){
				//user requested secrete on contact with medium and we found medium pixel
                          currentConcentration =
                              concentrationFieldPtr->get(elem.pixel);
                                if(currentConcentration*_relativeUptake>_maxUptake){
                                  concentrationFieldPtr->set(
                                      elem.pixel,
                                      currentConcentration - _maxUptake);
                                }else{
                                  concentrationFieldPtr->set(
                                      elem.pixel, currentConcentration -
                                                      currentConcentration *
                                                          _relativeUptake);
                                }
				break; //after secreting do not try to secrete more
			}
			
			if (nCell!=_cell && nCell && onContactSet.find(nCell->type)!=onContactSet.end() )
			{
				//user requested secretion on contact with cell type whose pixel we have just found
                          currentConcentration =
                              concentrationFieldPtr->get(elem.pixel);
                                if(currentConcentration*_relativeUptake>_maxUptake){
                                  concentrationFieldPtr->set(
                                      elem.pixel,
                                      currentConcentration - _maxUptake);
                                }else{
                                  concentrationFieldPtr->set(
                                      elem.pixel, currentConcentration -
                                                      currentConcentration *
                                                          _relativeUptake);
                                }

				break;//after secreting do not try to secrete more
			}

		}

	}

	return true;
}



bool FieldSecretor::_uptakeOutsideCellAtBoundary(CellG * _cell, float _maxUptake, float _relativeUptake){

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	set<FieldSecretorPixelData> visitedPixels;

	float currentConcentration;

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}
			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if (nCell!=_cell && visitedPixels.find(FieldSecretorPixelData(neighbor.pt))==visitedPixels.end()){

				currentConcentration=concentrationFieldPtr->get(nPt);
				if(currentConcentration*_relativeUptake>_maxUptake){
					concentrationFieldPtr->set(nPt,currentConcentration-_maxUptake);
				}else{
					concentrationFieldPtr->set(nPt,currentConcentration-currentConcentration*_relativeUptake);
				}				

				
				visitedPixels.insert(FieldSecretorPixelData(nPt));

			}

		}		

	}

	return true;

}

bool FieldSecretor::_uptakeOutsideCellAtBoundaryOnContactWith(CellG * _cell, float _maxUptake, float _relativeUptake,const std::vector<unsigned char> & _onContactVec){

	set<unsigned char> onContactSet(_onContactVec.begin(),_onContactVec.end());

	if (!boundaryPixelTrackerPlugin){
		return false;
	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	set<FieldSecretorPixelData> visitedPixels;

	float currentConcentration;

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}
			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);

			if (nCell!=_cell && visitedPixels.find(FieldSecretorPixelData(neighbor.pt))==visitedPixels.end()){

				if (!nCell && onContactSet.find(0)!=onContactSet.end()){
					//checking if the unvisited pixel belongs to Medium and if Medium is a  cell type listed in the onContactSet
					currentConcentration=concentrationFieldPtr->get(nPt);
					if(currentConcentration*_relativeUptake>_maxUptake){
						concentrationFieldPtr->set(nPt,currentConcentration-_maxUptake);
					}else{
						concentrationFieldPtr->set(nPt,currentConcentration-currentConcentration*_relativeUptake);
					}				
				}

				if (nCell && onContactSet.find(nCell->type)!=onContactSet.end()){
					//checking if the unvisited pixel belongs to a  cell type listed in the onContactSet
					currentConcentration=concentrationFieldPtr->get(nPt);
					if(currentConcentration*_relativeUptake>_maxUptake){
						concentrationFieldPtr->set(nPt,currentConcentration-_maxUptake);
					}else{
						concentrationFieldPtr->set(nPt,currentConcentration-currentConcentration*_relativeUptake);
					}				
				}				
				visitedPixels.insert(FieldSecretorPixelData(nPt));
			}

		}		

	}

	return true;

}



bool FieldSecretor::uptakeInsideCellAtCOM(CellG * _cell, float _maxUptake, float _relativeUptake){
	Point3D pt((int)round(_cell->xCM/_cell->volume),(int)round(_cell->yCM/_cell->volume),(int)round(_cell->zCM/_cell->volume));

	float currentConcentration=concentrationFieldPtr->get(pt);

	
	if(currentConcentration*_relativeUptake>_maxUptake){
		concentrationFieldPtr->set(pt,currentConcentration-_maxUptake);
	}else{
		concentrationFieldPtr->set(pt,currentConcentration-currentConcentration*_relativeUptake);
	}				

	return true;

}

float FieldSecretor::getConcentrationOutsideCellAtBoundary(CellG * _cell)
{
 	if (!boundaryPixelTrackerPlugin){
		cerr<<"BoundaryPixelTrackerPlugin not loaded!!"<<endl;
 		ASSERT_OR_THROW("ERROR: boundaryPixelTrackerPlugin is required by this", 0);
 	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	Point3D nPt;

        CellG *nCell = nullptr;

        Neighbor neighbor;

	set<FieldSecretorPixelData> visitedPixels;

	float totalConcentration(0);

        for (const auto &elem : pixelSetRef) {

                for(unsigned int nIdx=0 ; nIdx <= maxNeighborIndex ; ++nIdx ){
                  neighbor = boundaryStrategy->getNeighborDirect(
                      const_cast<Point3D &>(elem.pixel), nIdx);
                        if(!neighbor.distance){
				//if distance is 0 then the neighbor returned is invalid
				continue;
			}
			nPt=neighbor.pt;
			nCell = cellFieldG->get(neighbor.pt);
			if (nCell!=_cell && visitedPixels.find(FieldSecretorPixelData(neighbor.pt))==visitedPixels.end()){
	
				totalConcentration+=concentrationFieldPtr->get(nPt);
				
				visitedPixels.insert(FieldSecretorPixelData(nPt));

			}

		}		

	}

	return totalConcentration;
}

float FieldSecretor::getConcentrationInsideCellAtBoundary(CellG * _cell)
{
 	if (!boundaryPixelTrackerPlugin){
		cerr<<"BoundaryPixelTrackerPlugin not loaded!!"<<endl;
 		ASSERT_OR_THROW("ERROR: boundaryPixelTrackerPlugin is required by this", 0);
 	}

	BasicClassAccessor<BoundaryPixelTracker> *boundaryPixelTrackerAccessorPtr=boundaryPixelTrackerPlugin->getBoundaryPixelTrackerAccessorPtr();

	std::set<BoundaryPixelTrackerData > & pixelSetRef=boundaryPixelTrackerAccessorPtr->get(_cell->extraAttribPtr)->pixelSet;

	float totalConcentration(0);

        for (const auto &elem : pixelSetRef) {
          totalConcentration += concentrationFieldPtr->get(elem.pixel);
        }

	return totalConcentration;
}
