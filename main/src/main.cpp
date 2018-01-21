#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <complex>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "lua.hpp"

#include "Naming.hpp"
#include "RuntimeNames.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "TypeLookup.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "lua/lua_math.hpp"
#include "lua/types/lua_complex.hpp"
#include "lua/types/lua_vector.hpp"
#include "lua/types/lua_matrixrow.hpp"
#include "lua/types/lua_matrix.hpp"
#include "lua/types/lua_Simulation.hpp"
#include "lua/lua_global.hpp"
#include "types/lua_basetypes.hpp"
#include "ModuleLoader.hpp"
#include "ChimeraRuntime.hpp"

#include "StandardLogger.hpp"
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
    full_path /= chimera::runtime::Naming::Path_modules;

    chimera::runtime::vec_t_pathList lookup;
    lookup.push_back(full_path.string());
    // add all paths of env NETWORK_INCLUDE_PATH

    chimera::runtime::ModuleLoader loader(lookup);

    //StandardLogger stdLog;
    //LoggingSystem::AddLogger(&stdLog);

    std::vector<char*> vargv;
    for(int i = 1; i < argc; i++) {
        vargv.push_back(argv[i]);
    }

    StandardLogger* stdLog = new StandardLogger();

    std::string filename(argv[1]);
    chimera::runtime::ChimeraRuntime* runtime = new chimera::runtime::ChimeraRuntime(filename, vargv, &loader);

    runtime->getLoggingSystem()->addLogger(0, stdLog);

    //if(luaL_dofile(runtime->getLuaState(), filename.c_str()))
    //{
    //    std::cout << "LUA ERROR? -.-" << std::endl;
    //}
    int status, result;
    status = luaL_loadfile(runtime->getLuaState(), filename.c_str());
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        std::string s("Couldn't load file: ");
        s += lua_tostring(runtime->getLuaState(), -1);
        std::cout << "Lua Error: " << s << std::endl;
        return false;
    }

    result = lua_pcall(runtime->getLuaState(), 0, LUA_MULTRET, 0);

    if (result) {
        std::string s("Failed to run script: ");
        s += lua_tostring(runtime->getLuaState(), -1);
        std::cout << "Lua Error: " << s << std::endl;
        return false;
    }

    lua_settop(runtime->getLuaState(), 0);

    delete runtime;

    //LoggingSystem::ShutDown();

    return 0;
}
