#include "TypeTransition.h"
#include "TypeChangeWatcher.h"
using namespace CompuCell3D;
using namespace std;

void TypeTransition::registerTypeChangeWatcher(TypeChangeWatcher* _watcher){
      typeChangeWatcherVec.push_back(_watcher);
}

void TypeTransition::setType(CellG * _cell,CellG::CellType_t _newType){
  for (auto &elem : typeChangeWatcherVec) {
    elem->typeChange(_cell, _newType);
   }

   if(_cell){
      _cell->type=_newType;
   }
}