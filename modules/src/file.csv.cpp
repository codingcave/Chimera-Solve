#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <boost/numeric/ublas/vector.hpp>
#include "lua.hpp"

#include "Naming.hpp"
#include "ParameterType.hpp"
#include "def.hpp"
#include "ParameterTypeSystem.hpp"
#include "ItemContainer.hpp"
#include "Registry.hpp"
//#include "types/LuaFunctionWrapper.hpp"
#include "interfaces/IEventListener.hpp"
#include "event/StepEventListener.hpp"
#include "EntryPointBase/OutputRegistry.hpp"

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
    CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    w->open();

    return 0;
}

int file_close(lua_State* L)
{
    CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    w->close();

    return 0;
}

int file_path(lua_State* L)
{
    CsvFileWriter* w = *((CsvFileWriter**)lua_touserdata(L, lua_upvalueindex(1)));
    lua_pushstring(L, w->getPath().c_str());
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
    delete (CsvFileWriter*)instance;
    //delete (TemplateIntegrator
    //        <double, double>
    //        *)instance;
}

IEventListener* CsvFileRegistry::getOutputInstance(vec_t_LuaItem& parameters) const
{
    if(parameters.size() > 0)
    {
        if(parameters[0].type == ParameterTypeSystem::pid_string)
        {
            std::string* path = (std::string*)parameters[0].value;
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
