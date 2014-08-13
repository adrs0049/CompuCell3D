/*******************************************************************\

              Copyright (C) 2003 Joseph Coffland

    This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
        of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
             GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
                           02111-1307, USA.

            For information regarding this software email
                   jcofflan@users.sourceforge.net

\*******************************************************************/

#ifndef BASICPLUGINMANAGER_H
#define BASICPLUGINMANAGER_H

#include "BasicException.h"
#include "BasicPluginInfo.h"
#include "BasicClassFactoryBase.h"

#include <map>
#include <list>
#include <string>
#include <dlfcn.h>
#include <string.h>
#include <glob.h>
#include <memory>

template <class T>
class BasicPluginManager {
public:
	// ATTENTION SWIG doesn't deal with these yet
    //using infos_t = typename std::list<std::shared_ptr<BasicPluginInfo> >;
	typedef typename std::list<std::shared_ptr<BasicPluginInfo> > infos_t;
	
private:
	// ATTENTION SWIG doesn't deal with these yet
	//using factories_t = typename std::map<std::string, std::unique_ptr<BasicClassFactoryBase<T> > >;
    //using infoMap_t   = typename std::map<std::string, std::shared_ptr<BasicPluginInfo> >;
    //using plugins_t   = typename std::map<std::string, std::shared_ptr<T> >;
	//using libraryHandles_t = typename std::map<std::string, void *>;
	
	typedef typename std::map<std::string, std::unique_ptr<BasicClassFactoryBase<T> > > factories_t;
	typedef typename std::map<std::string, std::shared_ptr<BasicPluginInfo> > infoMap_t;
    typedef typename std::map<std::string, std::shared_ptr<T> > plugins_t;
	typedef typename std::map<std::string, void *> libraryHandles_t;
	
    /// A map from plugin name to registered factory.
    factories_t factories;

    /// A map from plugin name to registered info.
    infoMap_t infoMap;

    /// A map from plugin name to plugin instance.
    plugins_t plugins;

    /// A map of loaded library handles.
    libraryHandles_t libraryHandles;

    /// A list of plugin info structures.
    infos_t infos;

    /// Holds exceptions thrown by plugins on dlopen.
    BasicException *pluginException;

    /// If true dependencies will be loaded first.
    bool preloadDeps;

public:
    /// May need to ifdef this for windows dlls.
    static const char libExtension[];

    BasicPluginManager(bool preloadDeps = false) :
        pluginException(nullptr), preloadDeps(preloadDeps) {}

    /**
     * Destruct the plugin manager.
     * Deallocates all plugins and closes all plugin libraries.
     */
    virtual ~BasicPluginManager() {
        unload();
        clearInfos();
        // Deallocate infos
        // infos_t::iterator it;
        // for (it = infos.begin(); it != infos.end(); it++)
        // delete *it;

        // Deallocate factories
		for (auto& factory : factories)
			factory.second.reset(nullptr);
		
//         typename factories_t::iterator fIt;
//         for (fIt = factories.begin(); fIt != factories.end(); fIt++)
//             delete fIt->second;

        closeLibraries();
    }

    /**
     * Initialization function called when a plugin is first loaded.
     * Child classes should over load this function for type specific
     * plugin initialization.
     *
     * @param plugin The plugin to initialize.
     */
    virtual void init(std::shared_ptr<T>& plugin) {}

    /**
     * Deallocate a specific plugin.
     */
    void destroyPlugin(std::string pluginName) {
        if (!plugins[pluginName]) return;

        // Deallocate dependent plugins first.
        // Dependency cycles should be disallowed on plugin registration!
        for (auto& plugin : plugins)
			if (dependsOn(plugin.first, pluginName))
				destroyPlugin(plugin.first);

        // Deallocate the plugin.
//         auto& factory = getPluginFactory(pluginName);
        auto it = plugins.find(pluginName);
		it->second.reset();
//         factory->destroy(it->second);
        plugins.erase(it);
    }

    /// Deallocate all plugins in the correct order.
    void unload() {
        while (!plugins.empty()) {
            if (!plugins.begin()->second) plugins.erase(plugins.begin());
            else destroyPlugin(plugins.begin()->first);
        }
    }

    void clearInfos() {
		for (auto& info : infos)
			info.reset();
    }
    /// Close all shared library handles.
    void closeLibraries() {
		for (auto& libraryHandle : libraryHandles)
			if (libraryHandle.second) dlclose(libraryHandle.second);
		libraryHandles.clear();
    }

    /**
     * If plugin1 has not yet been registered this function
     * will return false;
     *
     * @return True if plugin1 has plugin2 in its dependency list, false
     *         otherwise.
     */
    bool dependsOn(std::string plugin1, std::string plugin2) {
        auto info = infoMap[plugin1];
        if (!info) return false;

        // Assumes dependency cycles have been disallowed on plugin registration!
        std::string dep;
        for (unsigned int i = 0; i < info->getNumDeps(); i++) {
            dep = info->getDependency(i);
            if (dep == plugin2 || dependsOn(dep, plugin2)) return true;
        }
        return false;
    }

    /**
     * Get a pointer to a named plugin.  If an instance has not yet been
     * allocated it will be created.  If the plugin name is unknown a
     * BasicException will be thrown.
     *
     * @param pluginName The name of the plugin.
     *
     * @return A pointer to the named plugin.
     */
    std::shared_ptr<T> get(const std::string pluginName, bool *_pluginAlreadyRegisteredFlag=nullptr) {
        auto plugin = plugins[pluginName];
        if (plugin) {
            if(_pluginAlreadyRegisteredFlag)
                *_pluginAlreadyRegisteredFlag=true;
            return plugin;
        }
        // Get the factory
        auto& factory = getPluginFactory(pluginName);

        if (preloadDeps) {
            // Make sure its dependencies have been initialized first.
            auto info = getPluginInfo(pluginName);
            for (unsigned int i = 0; i < info->getNumDeps(); ++i)
                get(info->getDependency(i));
        }

        // Create the plugin
        plugin = factory->create();
		plugins[pluginName] = plugin;
		
        // Initialize the plugin
        init(plugin);
        if(_pluginAlreadyRegisteredFlag)
            *_pluginAlreadyRegisteredFlag=false;
		
        return  plugin;
    }

    /// @return True if plugin with name @param pluginName is loaded.
    bool isLoaded(std::string pluginName) {
        return plugins[pluginName] != 0;
    }

    /// @return A reference to the named plugins info structure.
    const std::shared_ptr<BasicPluginInfo> getPluginInfo(const std::string pluginName) {
        auto it = infoMap.find(pluginName);
        ASSERT_OR_THROW(std::string("Plugin '") + pluginName + " not found!",
                        it != infoMap.end());

        return it->second;
    }

    /**
     * @return A list of registered plugin information structures.
     */
    infos_t &getPluginInfos() {
        return infos;
    }

    /**
     * Loads all plugin libraries on a ':' separated path list.
     *
     * @param path The plugin library path list.
     */
    void loadLibraries(const std::string path) {
        char *cPath = strdup(path.c_str());

        char *aPath = strtok(cPath, ":");
        while (aPath != NULL) {
            loadSingleLibraryPath(aPath);

            aPath = strtok(NULL, ":");
        }

        free(cPath);
    }

    /**
     * Load a plugin library.
     *
     * @param filename Path to plugin library.
     */
    void loadLibrary(const std::string filename) {
        // Get the library name
        std::string name;
        std::string::size_type pos = filename.find_last_of("/");
        if (pos != std::string::npos) name = filename.substr(pos + 1);
        else name = filename;

        // Exit if already opened.
        if (libraryHandles[name] != NULL) return;

        void *handle = dlopen(filename.c_str(), RTLD_LAZY | RTLD_GLOBAL);

        char *error;
        if ((error = dlerror()))
            THROW(std::string("BasicPluginManager::loadLibrary() ") + error);

        if (handle) libraryHandles[name] = handle;

        if (pluginException) {
            BasicException e = *pluginException;
            delete pluginException;
            throw BasicException(std::string("Exception while loading library '") +
                                 filename + "'", e);
        }
    }

    /**
     * Called by BasicPluginProxy to register itself with the manager.
     *
     * @param info A pointer to a new plugin info structure.
     * @param factory A pointer to a new plugin class factory.
     */
    void registerPlugin(std::shared_ptr<BasicPluginInfo> info,
                        std::unique_ptr<BasicClassFactoryBase<T> > factory) {
        ASSERT_OR_THROW("registerPlugin() Plugin info cannot be null!!",
                        info);
        ASSERT_OR_THROW("registerPlugin() Plugin factory cannot be null!!",
                        factory);

        ASSERT_OR_THROW(std::string("registerPlugin() A plugin with name ") +
                        info->getName() + " was already registered!",
                        infoMap[info->getName()] == NULL);

        // Check for dependency loops.
        std::string dep;
        for (unsigned int i = 0; i < info->getNumDeps(); i++) {
            dep = info->getDependency(i);
            if (dependsOn(dep, info->getName()))
                throw
                BasicException(std::string("BasicPluginManager::registerPlugin() ") +
                               "Detected dependency cycle between plugins '" +
                               info->getName() + "' and '" + dep + "'!");
        }

        // Register the plugin
        infos.push_back(info);
		infoMap[info->getName()] = info;
        factories[info->getName()] = std::move(factory);
    }

    /**
     * Used by statically allocated plugins to pass any exceptions back
     * to the library loader.
     */
    void setPluginException(BasicException e) {
        if (!pluginException)
            pluginException = new BasicException(e);
    }

protected:
    /// @return A reference to the named plugin's factory.
    std::unique_ptr<BasicClassFactoryBase<T> > const&
    getPluginFactory(const std::string pluginName) {
        auto it = factories.find(pluginName);
        ASSERT_OR_THROW(std::string("Plugin '") + pluginName + " not found!",
                        it != factories.end());

        return it->second;
    }

    plugins_t & getPluginMapBPM() {
        return plugins;
    }
    /**
     * Loads all libraries on a single path.
     *
     * @param path A single path.
     */
    void loadSingleLibraryPath(const std::string path) {
        glob_t globbuf;
        std::string pathGlob = path + "/" + "*" + libExtension;

        if (glob(pathGlob.c_str(), 0, NULL, &globbuf) == 0) {
            for (unsigned int i = 0; i < globbuf.gl_pathc; i++)
                loadLibrary(globbuf.gl_pathv[i]);
        }

        globfree(&globbuf);
    }
};

template<class T>
const char BasicPluginManager<T>::libExtension[] = ".so";

#endif
