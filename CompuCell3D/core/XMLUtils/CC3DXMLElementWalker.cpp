#include <iostream>
#include "CC3DXMLElement.h"
#include "CC3DXMLElementWalker.h"

using namespace std;

CC3DXMLElementWalker::CC3DXMLElementWalker()
{}	

void CC3DXMLElementWalker::iterateCC3DXMLElement(CC3DXMLElement* _element)
{
	cerr<<"ITERATION ROOT ELEMENT="<<_element->getName()<<endl;
	CC3DXMLElementList childrenList=_element->getElements();
	if(!childrenList.empty())
	{
		cerr<<"ELEMENT: "<<_element->getName()<<" HAS CHILDREN"<<endl;
		for (auto& elem : childrenList)
		{
			cerr<<"child address="<<&(elem)<<" childName="<<elem->getName()<<endl;
			iterateCC3DXMLElement(elem);
		}
	}else{
		cerr<<"ELEMENT: "<<_element->getName()<<" CDATA="<<_element->getData()<<endl;
	}

}

