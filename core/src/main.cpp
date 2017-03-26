#include <iostream>
#include <unordered_map>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/IConnectEventHandler.hpp"
#include "ItemContainer.hpp"
#include "ModuleLoader.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "lua/LuaParser.hpp"
#include "LoggingSystem.hpp"
#include "StandardLogger.hpp"
#include "ParameterType.hpp"
#include "EntryPoints/SystemDynamicEntryPoint.hpp"
#include "EntryPoints/IntegratorEntryPoint.hpp"
#include "EntryPoints/OutputEntryPoint.hpp"

namespace fs = boost::filesystem;

const std::string version("1.0.0");

int main(int argc, char** argv)
{
    // --help
    // --help module
    // --help module str1 str2 ...
    // --version print version

    // -L load module, that does not belong to a EntryPoint, containing Types and EntryPoints
    // --load=path to module

    // -I include in path lookup
    // --lookup=path to lookup

    fs::path data_dir(fs::current_path());
    fs::path full_path( fs::initial_path<fs::path>() );

    full_path = fs::system_complete( fs::path( argv[0] ).parent_path() );
    full_path /= Naming::Path_modules;

    vec_t_pathList lookup;

    lookup.push_back(full_path.string());
    // add all paths of env NETWORK_INCLUDE_PATH

    ModuleLoader loader(lookup);

    StandardLogger stdLog;
    LoggingSystem::AddLogger(&stdLog);

    std::vector<char*> vargv;
    for(int i = 1; i < argc; i++) {
        vargv.push_back(argv[i]);
    }

    std::string filename(argv[1]);
    LuaParser* lp = new LuaParser(filename, vargv, &loader);

    EntryPointSystem::addEntryPoint(new SystemDynamicEntryPoint());
    EntryPointSystem::addEntryPoint(new IntegratorEntryPoint());
    EntryPointSystem::addEntryPoint(new OutputEntryPoint());

    lp->processFile();
    delete lp;
    LoggingSystem::ShutDown();

    return 0;
}
