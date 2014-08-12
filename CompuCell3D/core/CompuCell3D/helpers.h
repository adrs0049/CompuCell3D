#ifndef HELPER_H_H
#define HELPER_H_H

#include <memory>
// #include <CompuCell3D/Simulator.h>

namespace CompuCell3D {

class Simulator;
class Potts3D;
	
template <typename Plugin>
std::shared_ptr<Plugin>
get_plugin(const std::string pluginName, bool * flag)
{
	return std::static_pointer_cast<Plugin>(Simulator::pluginManager.get(pluginName, flag));
}
	
template <typename Steppable>
std::shared_ptr<Steppable>
get_steppable(const std::string steppableName, bool * flag)
{
	return std::static_pointer_cast<Steppable>(Simulator::steppableManager.get(steppableName, flag));
}

template <typename Accessor>
void
registerClassOnCell(Potts3D * potts, Accessor& TheAccessor)
{
	potts->getCellFactoryGroupPtr()->registerClass(std::make_shared<Accessor>(TheAccessor));
}

}; // end namespace

#endif