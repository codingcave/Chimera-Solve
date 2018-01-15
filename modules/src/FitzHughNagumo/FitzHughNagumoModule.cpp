#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "FitzHughNagumo/FitzHughNagumo.hpp"
#include "FitzHughNagumo/FitzHughNagumoModule.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new FitzHughNagumoModule();
    }

    void UnloadModule(chimera::Module* module)
    {
        delete (FitzHughNagumoModule*)module;
    }
}

/*
int rk_test(lua_State* L)
{
    FitzHughNagumo* inc = *((FitzHughNagumo**)lua_touserdata(L, lua_upvalueindex(1)));
    return 0;
}
*/

FitzHughNagumoModule::FitzHughNagumoModule()
{
    //setValue("keks", {ParameterTypeSystem::getParameterID(Naming::Type_real), (void*)new double(17.349)});
    //registerMethod("test", rk_test);
}

void* FitzHughNagumoModule::getInstance(chimera::vec_t_LuaItem& parameters) const
{
    double a = 0.0;
    double epsilon = 0.0;

    if(parameters.size() > 0 && chimera::systemtypes::PID_NUMBER == parameters[0].getType())
    {
        epsilon = parameters[0];
    }
    if(parameters.size() > 1 && chimera::systemtypes::PID_NUMBER == parameters[1].getType())
    {
        a = parameters[1];
    }

    return new FitzHughNagumo(getChimeraSystem()->getTypeSystem(), epsilon, a);
}

const std::string FitzHughNagumoModule::getGUID() const
{
    return "FitzHugh-Nagumo";
}

void FitzHughNagumoModule::destroyInstance(void* instance) const
{
    delete ((FitzHughNagumo*)instance);
}

const std::string FitzHughNagumoModule::getVersion() const
{
    return "1.0.0";
}
