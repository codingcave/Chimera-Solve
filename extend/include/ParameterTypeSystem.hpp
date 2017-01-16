#ifndef PARAMETERTYPESYSTEM_H
#define PARAMETERTYPESYSTEM_H

struct T_MatrixDef {
    const int rows;
    const int cols;
    const bool readonly;
    void* const value;
};

struct T_MatrixRowDef {
    const int row;
    const int length;
    const bool readonly;
    void* const value;
};

struct T_VectorDef {
    const int length;
    const bool readonly;
    void* value;
};

typedef int (*lua_CFunction) (lua_State *L);

class IConnectEventHandler;

class ParameterTypeSystem final
{
    friend class LuaParser;
    friend class LuaFunctionWrapper;
    public:
        virtual ~ParameterTypeSystem();
        static int registerParameter(const std::string& name, const struct T_ParameterDef& pdef);
        static int registerVector(const int& subtype, const struct T_ParameterDef& pdef);
        static int registerMatrix(const int& subtype, const struct T_ParameterDef& pdef);
        static int getParameterID(const std::string& name);
        static int getParameterID(const size_t& base, const size_t& tag);
        static const std::string getParameterName(const int& id);
        static size_t getParameterBase(const int& id);
        static size_t getParameterTag(const int& id);
        static bool pushValue(const struct T_Parameter& value);
        static bool pushReal(const double& value);
        static bool pushString(const std::string& value);
        static bool pushBoolean(const bool& value);
        static bool pushFunction(lua_CFunction value);
        static const struct T_Parameter getValue(const int& index);
        static int getParameterType(const int& index);
        static void deleteValues(vec_t_LuaItem& items);
        static void deleteValue(const struct T_Parameter& value);
        static inline bool isParameterID(const std::string& name, const int& id)
        {
            return id == getParameterID(name);
        }

        static const int pid_nil;
        static const int pid_boolean;
        static const int pid_real;
        static const int pid_string;
        static const int pid_func;
        static const int pid_library;
        static const int pid_luafunc;
        static const int pid_entrypoint;
        static const int pid_registry;
        static const int pid_instance;
        static const int pid_vector;
        static const int pid_matrix;
        static const int pid_matrixrow;
    protected:
    private:
        static ParameterTypeSystem _instance;

        int _lastID;
        std::vector<const ParameterType*>* _typeList;
        IConnectEventHandler* _luaparser;

        ParameterTypeSystem();
        static int registerParameter(const std::string& name, const struct T_ParameterDef& pdef, bool notify);
        static int registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, bool notify);
        static bool pushValue(lua_State* const L, const struct T_Parameter& value);
        static void finalizeValue(lua_State* const L, const struct T_Parameter& value);
        static void deleteSingle(lua_State* const L, const struct T_Parameter& value);
        static ParameterType const * const getParameter(const int& id);
        static void addBaseTypes();
        static void connect(IConnectEventHandler* const c);
        static void disconnect();
};

#endif // PARAMETERTYPESYSTEM_H
