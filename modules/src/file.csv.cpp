#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
#include "event/StateEventListener.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
#include "EntryPointBase/OutputMultiRegistry.hpp"
//#include "types/LuaFunctionWrapper.hpp"
//#include "EntryPointBase/OutputRegistry.hpp"

#include "file.csv.hpp"

namespace ublas = boost::numeric::ublas;

extern "C"
{
    Registry* LoadModule()
    {
        return new CsvFileRegistry();
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

CsvFileRegistry::CsvFileRegistry()
{
    //ctor
    //registerMethod("open", file_open);
    //registerMethod("close", file_close);
    registerMethod("path", file_path);
}

CsvFileRegistry::~CsvFileRegistry()
{
    //dtor
}

const std::string CsvFileRegistry::getVersion() const
{
    return "1.0.0";
}

void CsvFileRegistry::destroyInstance(void * const instance) const
{
    //delete (CsvFileWriter*)instance;
    //delete (TemplateIntegrator
    //        <double, double>
    //        *)instance;
}

IEventListenerProvider* CsvFileRegistry::getOutputInstance(vec_t_LuaItem& parameters) const
{
    if(parameters.size() > 0)
    {
        if(parameters[0].getType() == ParameterTypeSystem::pid_string)
        {
            std::string* path = (std::string*)parameters[0].getValue();
            return new CsvFileWriter(*path);
        }
    }
    return nullptr;
}

CsvFileWriter::CsvFileWriter(const std::string& path):
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

IEventListener* CsvFileWriter::provideListener(size_t id, void* args)
{
    //std::cout << "Provide: " << id << std::endl;

    switch(id){
    case 1: // StateEventListener
        {
            struct StateProviderArgs* spArgs = (struct StateProviderArgs*)args;
            //std::cout << "Time:  " << spArgs->time_type << " :: " << ParameterTypeSystem::getParameterName(spArgs->time_type) << std::endl;
            //std::cout << "State: " << spArgs->state_type << " :: " << ParameterTypeSystem::getParameterName(spArgs->state_type) << std::endl;
            open();
            return new FileOutput_real_vecReal(_file);
        }
    }

    return nullptr;
}

/*
void CsvFileWriter::notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state)
{
    static const std::string vectorRealMetaName = (std::string(Naming::Type_Vector) + "#" + std::string(Naming::Type_real));
    static const size_t vectorRealType = ParameterTypeSystem::getParameterID(vectorRealMetaName);
    open();
    if(timeType == ParameterTypeSystem::pid_real)
    {
        (*_file) << *((double*)time);
    } else {
        return;
    }

    if(stateType == vectorRealType)
    {
        struct T_VectorDef* vd = (struct T_VectorDef*)state;
        boost::numeric::ublas::vector<double>* v = (boost::numeric::ublas::vector<double>*)vd->value;
        int length = vd->length;
        for(int i = 0; i < length; i++)
        {
            (*_file) << "," << ((*v)(i));
        }
    }
    (*_file) << std::endl;
}
*/

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
