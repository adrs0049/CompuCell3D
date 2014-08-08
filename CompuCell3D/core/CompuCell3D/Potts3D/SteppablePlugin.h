#ifndef STEPPABLE_PLUGIN_H
#define STEPPABLE_PLUGIN_H

namespace CompuCell3D {

  class SteppablePlugin {
  public:
    virtual void step() = 0;
  };
};
#endif
