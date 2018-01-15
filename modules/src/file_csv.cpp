#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "Naming.hpp"
#include "ExtensionNaming.hpp"
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
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "EntryPointBase/OutputMultiModule.hpp"
//#include "types/LuaFunctionWrapper.hpp"
//#include "EntryPointBase/OutputModule.hpp"

#include "file_csv.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new CsvFileModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (CsvFileModule*)reg;
    }
}


int file_open(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //w->open();

    return 0;
}

int file_close(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //w->close();

    return 0;
}

int file_path(lua_State* L)
{
    //CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    //lua_pushstring(L, w->getPath().c_str());
    return 1;
}

CsvFileModule::CsvFileModule()
{
    //ctor
    //registerMethod("open", file_open);
    //registerMethod("close", file_close);
    //registerMethod("path", file_path);
}

CsvFileModule::~CsvFileModule()
{
    //dtor
}

const std::string CsvFileModule::getVersion() const
{
    return "1.0.0";
}

void CsvFileModule::destroyInstance(void * const instance) const
{
    //delete (CsvFileWriter*)instance;
    //delete (TemplateIntegrator
    //        <double, double>
    //        *)instance;
}

chimera::simulation::IEventListenerProvider* CsvFileModule::getOutputInstance(chimera::vec_t_LuaItem& parameters) const
{
    if(parameters.size() > 0)
    {
        if(parameters[0].getType() == chimera::systemtypes::PID_STRING)
        {
            std::string* path = (std::string*)parameters[0].getValue();
            return new CsvFileWriter(getChimeraSystem()->getTypeSystem(), *path);
        }
    }
    return nullptr;
}

const std::string CsvFileModule::getGUID() const
{
    return "file:CSV";
}

CsvFileWriter::CsvFileWriter(chimera::ParameterTypeSystem* ps, const std::string& path):
    _ps(ps),
    _path(path)
{
    _file = nullptr;
}

CsvFileWriter::~CsvFileWriter()
{
    if(_file && _file->is_open())
        _file->close();
}

void CsvFileWriter::open()
{
    if(_file == nullptr)
        _file = new std::ofstream(_path);
}

void CsvFileWriter::close()
{
    if(_file && _file->is_open())
        _file->close();
    if(_file)
        delete _file;
    _file = nullptr;
}

const std::string CsvFileWriter::getPath() const
{
    return _path;
}

chimera::simulation::IEventListener* CsvFileWriter::provideListener(size_t id, void* args)
{
    static const size_t pid_vecReal = _ps->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));
    static const size_t pid_vecvecReal = _ps->getParameterID(std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::simulation::Naming::Type_Vector) + "#" + std::string(chimera::typenames::TYPE_NUMBER));

    switch(id){
    case 1: // StateEventListener
        {
            struct chimera::simulation::StateProviderArgs* spArgs = (struct chimera::simulation::StateProviderArgs*)args;
            //std::cout << "Time:  " << spArgs->time_type << " :: " << _ps->getParameterName(spArgs->time_type) << std::endl;
            //std::cout << "State: " << spArgs->state_type << " :: " << _ps->getParameterName(spArgs->state_type) << std::endl;

            if(spArgs->time_type == chimera::systemtypes::PID_NUMBER)
            {
                if(spArgs->state_type == pid_vecReal)
                {
                    open();
                    return new FileOutput_real_vecReal(_file);
                }
                if(spArgs->state_type == pid_vecvecReal)
                {
                    open();
                    return new FileOutput_real_vecvecReal(_file);
                }
            }
        }
    }

    return nullptr;
}

FileOutput_real_vecReal::FileOutput_real_vecReal(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecReal::~FileOutput_real_vecReal()
{

}

void FileOutput_real_vecReal::notify(const double& time, const boost::numeric::ublas::vector<double>& state)
{
    (*_file) << time;
    for(auto item : state)
    {
        (*_file) << "," << item;
    }
    (*_file) << std::endl;
}

void FileOutput_real_vecReal::notify(void const * const sender, void* args)
{

}

FileOutput_real_vecvecReal::FileOutput_real_vecvecReal(std::ofstream* file)
{
    _file = file;
}

FileOutput_real_vecvecReal::~FileOutput_real_vecvecReal()
{

}

void FileOutput_real_vecvecReal::notify(const double& time, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& state)
{
    (*_file) << time;
    for(auto node : state)
    {
        for(auto item : node)
        {
            (*_file) << "," << item;
        }
    }
    (*_file) << std::endl;
}

void FileOutput_real_vecvecReal::notify(void const * const sender, void* args)
{

}
