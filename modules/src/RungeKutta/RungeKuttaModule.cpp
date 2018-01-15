#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/EventManager.hpp"
#include "event/StateEventListener.hpp"
#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/AbstractIntegrator.hpp"
#include "EntryPointBase/TemplateIntegrator.hpp"
#include "EntryPointBase/IntegratorModule.hpp"
#include "RungeKutta/RungeKuttaSystem.hpp"
#include "RungeKutta/RungeKutta_double_vecDouble.hpp"
#include "RungeKutta/RungeKutta_double_vecvecDouble.hpp"
#include "RungeKutta/RungeKuttaModule.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new RungeKuttaModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (RungeKuttaModule*)reg;
    }
}

RungeKuttaModule::RungeKuttaModule()
{
    _init = nullptr;
}

RungeKuttaModule::~RungeKuttaModule()
{

}

//void* RungeKuttaModule::getInstance(vec_t_LuaItem& parameters) const
//{
//    return new RungeKutta(nullptr);
//}

const std::string RungeKuttaModule::getVersion() const
{
    return "1.0.0";
}

void RungeKuttaModule::destroyInstance(void * const instance) const
{
    delete (chimera::simulation::AbstractIntegrator*)instance;
}

void RungeKuttaModule::load(chimera::EntryPoint const * const entryPoint, void const * const params)
{
    _init = (chimera::EntryPoint*)params;
}

chimera::simulation::AbstractIntegrator* RungeKuttaModule::getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t vectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorVectorRealMetaName);

    static const size_t basetype = chimera::systemtypes::PID_INSTANCE;
    static const size_t tagtype = (size_t)(getChimeraSystem()->getEntryPointSystem()->getEntryPoint(chimera::simulation::Naming::EntryPoint_dynamics));

    if(parameters.size() > 0) {
        double dt = 0.1;
        if(parameters.size() > 1 && parameters[1].getType() == chimera::systemtypes::PID_NUMBER)
        {
            dt = *((double*)parameters[1].getValue());
            if(dt <= 0.0) dt = .1;
        }

        if(getChimeraSystem()->getTypeSystem()->getParameterBase(parameters[0].getType()) == basetype &&
           getChimeraSystem()->getTypeSystem()->getParameterTag(parameters[0].getType()) == tagtype)
        {
            chimera::simulation::AbstractSystemDynamic* dyn = (chimera::simulation::AbstractSystemDynamic*)parameters[0].getValue();
            auto features = dyn->getFeatures();

            auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
            auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
            if(timeType != features.end() && stateType != features.end()) {
                if(timeType->second == chimera::systemtypes::PID_NUMBER) {
                    if(stateType->second == vectorRealType) {
                        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>(dyn);
                        if(sys != nullptr) {
                            return new RungeKutta_double_vecDouble(getChimeraSystem()->getTypeSystem(), _init, sys, dt);
                        }
                    }
                    if(stateType->second == vectorVectorRealType) {
                        chimera::simulation::TemplateOdeSystem<double, vec_vec_real>* sys = dynamic_cast<chimera::simulation::TemplateOdeSystem<double, vec_vec_real>*>(dyn);
                        if(sys != nullptr) {
                            return new RungeKutta_double_vecvecDouble(getChimeraSystem()->getTypeSystem(), _init, sys, dt);
                            //std::cout << "RETURN new RungeKutta_double_vecvecDouble(sys, dt);" << std::endl;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

bool RungeKuttaModule::checkFeatures(const chimera::map_t_size& features) const
{
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const std::string vectorVectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t vectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorRealMetaName);
    static const size_t vectorVectorRealType = getChimeraSystem()->getTypeSystem()->getParameterID(vectorVectorRealMetaName);

    auto timeType = features.find(chimera::simulation::Naming::Feature_time_type);
    auto stateType = features.find(chimera::simulation::Naming::Feature_state_type);
    if(timeType != features.end() && stateType != features.end()) {
        if(timeType->second == chimera::systemtypes::PID_NUMBER) {
            if(stateType->second == vectorRealType || stateType->second == vectorVectorRealType) {
                return true;
            }
        }
    }

    return false;
}

const std::string RungeKuttaModule::getGUID() const
{
    return "Runge-Kutta";
}

const std::string RungeKuttaModule::getSystemName() const
{
    return "ode";
}
