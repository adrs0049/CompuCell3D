#ifndef PLAYERSETTINGSPLUGIN_H
#define PLAYERSETTINGSPLUGIN_H

#include <CompuCell3D/Plugin.h>

#include <CompuCell3D/plugins/PlayerSettings/PlayerSettings.h>
#include "PlayerSettingsDLLSpecifier.h"

namespace CompuCell3D
{

class PLAYERSETTINGS_EXPORT PlayerSettingsPlugin : public Plugin
{
    CC3DXMLElement *xmlData;
public:
	PlayerSettings *playerSettingsPtr;
    PlayerSettings playerSettings;

    PlayerSettingsPlugin();
    virtual ~PlayerSettingsPlugin();

    ///SimObject interface
    virtual void init ( Simulator* simulator,
                        CC3DXMLElement *_xmlData = nullptr ) override;
    virtual void extraInit ( Simulator* simulator ) override;

    PlayerSettings getPlayerSettings() const
    {
        return * ( playerSettingsPtr );
    }

    //steerable interface
    virtual void update ( CC3DXMLElement *_xmlData,
                          bool _fullInitFlag = false ) override;
    virtual std::string steerableName() override;
    virtual std::string toString() override;
};

} // end namespace
#endif