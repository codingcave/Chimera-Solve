#ifndef NAMING_HPP_INCLUDED
#define NAMING_HPP_INCLUDED

namespace Naming
{
    const char * const Path_modules = "modules";

    const char * const Module_prefix = "";
    const char * const Module_extension = ".mod";
    const char * const Module_createFunction = "LoadModule";

    const char * const EntryPoint_dynamics = "dynamics";
    const char * const EntryPoint_integrator = "integrator";

    const char * const Logging_log = "";
    const char * const Logging_warning = "[WARNING] ";
    const char * const Logging_error = "[ERROR] ";

    const char * const Lua_reg_instance_LuaParser = "simulation:LuaParser";
    const char * const Lua_reg_instance_ModuleLoader = "simulation:moduleloader";
    const char * const Lua_reg_references = "simulation:com:references";
    const char * const Lua_reg_functions = "simulation:com:luafunctions";
    const char * const Lua_reg_simulation = "simulation:com:simulations";
    const char * const Lua_reg_argv = "simulation:argv";
    const char * const Lua_reg_systemNames = "simulation:system_names";

    const char * const Lua_name_EntryPoint = "EntryPoint";
    const char * const Lua_name_Registry = "Registry";
    const char * const Lua_name_Instance = "Instance";
    const char * const Lua_name_Simulation = "Simulation";

    const char * const Lua_global_log = "log";
    const char * const Lua_global_print = "print";
    const char * const Lua_global_write = "write";
    const char * const Lua_global_writeline = "writeln";
    const char * const Lua_global_error = "error";
    const char * const Lua_global_warning = "warning";
    const char * const Lua_global_exit = "exit";
    const char * const Lua_global_simulation = "simulation";
    const char * const Lua_global_argv = "argv";
    const char * const Lua_global_complex = "complex";
    const char * const Lua_global_random = "random";
    const char * const Lua_global_math = "math";
    const char * const Lua_global_matrix = "matrix";


    const char * const Type_null = "nil";
    const char * const Type_boolean = "boolean";
    const char * const Type_real = "real";
    const char * const Type_string = "string";
    const char * const Type_function = "function";
    const char * const Type_UserFunction = "LuaFunction";
    const char * const Type_Library = "Library";
    const char * const Type_EntryPoint = "EntryPoint";
    const char * const Type_Registry = "Registry";
    const char * const Type_Instance = "Instance";
    const char * const Type_Simulation = "Simulation";
    const char * const Type_Complex = "Complex";
    const char * const Type_Vector = "Vector";
    const char * const Type_Matrix = "Matrix";
    const char * const Type_MatrixRow = "MatrixRow";
}
#endif // NAMING_HPP_INCLUDED
