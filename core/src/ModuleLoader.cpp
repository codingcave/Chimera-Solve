#include <iostream>
#include <unordered_map>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dlfcn.h>

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "ModuleLoader.hpp"

namespace fs = boost::filesystem;

ModuleLoader::ModuleLoader(const vec_t_pathList& importLookup)
{
    _importLookup = new vec_t_pathList(importLookup);
    _modulePtr = new map_t_ModuleItem();
    _moduleReg = new map_t_ModuleRegistry();
}

Registry * const ModuleLoader::Load(const std::string& name)
{
    void* lib_handle = nullptr;
    char* error = nullptr;

    // test if module with `name` was already loaded

    map_t_ModuleItem::const_iterator available = _modulePtr->find (name);
    if ( available == _modulePtr->end() ) // if not loaded
    {
        // check all lookup directories for module
        for (std::vector<std::string>::iterator it = _importLookup->begin() ; it != _importLookup->end(); ++it) {
            fs::path loadDir(*it);
            if(fs::exists(loadDir) && fs::is_directory(loadDir))
            {
                fs::path modulePath(loadDir);
                std::string modName(Naming::Module_prefix);
                modName += name;
                modName += Naming::Module_extension;
                modulePath /= modName;

                if(fs::exists(modulePath) && !fs::is_directory(modulePath))
                {
                    // if matching module file name was found, try opening
                    // the library
                    lib_handle = dlopen(modulePath.string().c_str(), RTLD_LAZY);
                    // library was opened successfully:
                    // add to open libraries
                    if (lib_handle)
                    {
                        (*_modulePtr)[name] = lib_handle;
                        break;
                    }
                }
            }
            else
            {
                // warning: directory cannot be searched
            }
        }
    } else {
        // library was already open, return
        return (*_moduleReg)[name];
    }

    // if library was not found, return nothing
    if(lib_handle == nullptr) {
        return nullptr;
    }

    fn_factory_ptr fn;
    // load function symbol `LoadModule` which has to return
    // a Registry pointer
    fn = (fn_factory_ptr)dlsym(lib_handle, Naming::Module_createFunction);
    if ((error = dlerror()) != NULL && fn)
    {
        // on error remove library again and return nothing
        _modulePtr->erase(name);
        return nullptr;
    }

    // set the registry for the loaded module
    Registry* mReg = (*fn)();
    (*_moduleReg)[name] = mReg;
    return mReg;
}

void ModuleLoader::Unload(const std::string& name)
{
    void *lib_handle;

    map_t_ModuleItem::const_iterator available = _modulePtr->find (name);

    // if module was found, remove it
    // if not, do nothing
    if ( available != _modulePtr->end() )
    {
        map_t_ModuleRegistry::const_iterator it2 = _moduleReg->find (name);
        // free Registry space
        delete ((*_moduleReg)[name]);
        // close library file
        lib_handle = (*_modulePtr)[name];
        dlclose(lib_handle);
        // remove pointers
        _modulePtr->erase(available);
        _moduleReg->erase(it2);
    }
}

void ModuleLoader::UnloadAll()
{
    auto mIt = _modulePtr->begin();
    while ( mIt != _modulePtr->end() ) {
        this->Unload(mIt->first);
        mIt = _modulePtr->begin();
    }
}

ModuleLoader::~ModuleLoader()
{
    delete _importLookup;
    UnloadAll();
    delete _modulePtr;
    delete _moduleReg;
}
