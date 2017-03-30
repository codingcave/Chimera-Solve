#ifndef PARAMETERTYPESYSTEM_H
#define PARAMETERTYPESYSTEM_H

struct T_MatrixDef {
    const long unsigned int rows;
    const long unsigned int cols;
    const bool readonly;
    const bool deleteValue;
    void* const value;
};

struct T_MatrixRowDef {
    const long unsigned int row;
    const long unsigned int length;
    const bool readonly;
    void* const value;
};

struct T_VectorDef {
    const long unsigned int length;
    const bool readonly;
    const bool deleteValue;
    void* value;
};

typedef int (*lua_CFunction) (lua_State *L);

class IConnectEventHandler;
class ParameterValue;

class ParameterTypeSystem final
{
    friend class LuaParser;
    friend class LuaFunctionWrapper;
    friend class ParameterValue;
    public:
        virtual ~ParameterTypeSystem();
        static size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef);
        static size_t registerVector(const size_t& subtype, const struct T_ParameterDef& pdef);
        static size_t registerMatrix(const size_t& subtype, const struct T_ParameterDef& pdef);
        static size_t getParameterID(const std::string& name);
        static size_t getParameterID(const size_t& base, const size_t& tag);
        static const std::string getParameterName(const size_t& id);
        static size_t getParameterBase(const size_t& id);
        static size_t getParameterTag(const size_t& id);
        //static bool pushValue(ParameterValue& value);
	static bool pushValue(struct T_Parameter value);
        static bool pushReal(const double& value);
        static bool pushString(const std::string& value);
        static bool pushBoolean(const bool& value);
        static bool pushFunction(lua_CFunction value);
        static ParameterValue getValue(const int& index);
        static size_t getParameterType(const int& index);
        static void deleteValue(ParameterValue& value);
        //static void deleteValue(const struct T_Parameter& value);
        static inline bool isParameterID(const std::string& name, const size_t& id)
        {
            return id == getParameterID(name);
        }
        static bool isReferenced(void* ptr);

        static const size_t pid_nil;
        static const size_t pid_real;
        static const size_t pid_boolean;
        static const size_t pid_string;
        static const size_t pid_table;
        static const size_t pid_func;
        static const size_t pid_library;
        static const size_t pid_luafunc;
        static const size_t pid_entrypoint;
        static const size_t pid_registry;
        static const size_t pid_instance;
        static const size_t pid_vector;
        static const size_t pid_matrix;
        static const size_t pid_matrixrow;
    protected:
    private:
        static ParameterTypeSystem _instance;

        size_t _lastID;
        std::vector<const ParameterType*>* _typeList;
        IConnectEventHandler* _luaparser;
        std::unordered_map<void*, ParameterValue>* _references;

        ParameterTypeSystem();
        static size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, bool notify);
        static size_t registerParameter(const std::string& name, const struct T_ParameterDef& pdef, size_t base, size_t tag, bool notify);
        static bool pushValue(lua_State* const L, ParameterValue& value);
        static void finalizeValue(lua_State* const L, ParameterValue& value);
        static ParameterType const * const getParameter(const size_t& id);
        static void addBaseTypes();
        static void connect(IConnectEventHandler* const c);
        static void disconnect();
};

#endif // PARAMETERTYPESYSTEM_H
