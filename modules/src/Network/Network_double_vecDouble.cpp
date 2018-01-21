#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
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
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"

#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingEntryPoint.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "EntryPointBase/SystemDynamicModule.hpp"
#include "Network/Network_double_vecDouble.hpp"

Network_double_vecDouble::Network_double_vecDouble(chimera::ParameterTypeSystem* ps, chimera::ParameterValue& unit, const int& number, chimera::ParameterValue& coupling):
    _ps(ps)
{
    _unitSys = (chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*)unit.getValue();
    _unitValue = new chimera::ParameterValue(unit);
    _number = number;
    _coupling = (TemplateCoupling<boost::numeric::ublas::vector<double> >*)coupling.getValue();
    _couplingValue = new chimera::ParameterValue(coupling);
}

Network_double_vecDouble::~Network_double_vecDouble()
{
    delete _unitValue;
    delete _couplingValue;
}

void Network_double_vecDouble::operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& dxdt, const double& t)
{
    boost::numeric::ublas::vector<double> tmp(x[0].size());
    unsigned int vSize = dxdt.size();
    //std::cout << " >>step: " << t << std::endl;
    for(unsigned int i = 0; i < vSize; i++)
    {
        dxdt[i].resize(x[i].size());
        (*_unitSys)(x[i], dxdt[i], t);
        for(unsigned int j = 0; j < vSize; j++)
        {
            (*_coupling)(x, i, j, tmp);
            for(unsigned int inner = 0; inner < x[i].size(); inner++)
            {
                dxdt[i][inner] = dxdt[i][inner] + tmp[inner];
            }
        }
    }
}

std::unordered_map<std::string, size_t> Network_double_vecDouble::getFeatures() const
{
    std::unordered_map<std::string, size_t> features;
    static const std::string vectorRealMetaName = (std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    features[chimera::simulation::Naming::Feature_time_type] = chimera::systemtypes::PID_NUMBER;
    features[chimera::simulation::Naming::Feature_state_type] = _ps->getParameterID(vectorRealMetaName);
    features[chimera::simulation::Naming::Feature_size] = _number;

    auto unitFeatures = _unitSys->getFeatures();
    auto unitSize = unitFeatures.find(chimera::simulation::Naming::Feature_size);
    if(unitSize != unitFeatures.end())
    {
        features[chimera::simulation::Naming::Feature_unit_size] = unitSize->second;
    }
    return features;
}

const std::string Network_double_vecDouble::getSystemName() const
{
    return "ode";
}
