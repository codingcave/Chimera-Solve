#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

typedef struct lua_State lua_State;

class ParameterValue
{
    friend class ParameterTypeSystem;

    public:
        ParameterValue(size_t type, void* value);
        ParameterValue(const ParameterValue& p);
        virtual ~ParameterValue();
        size_t getType() const;
        void* getValue() const;
        //void * getValue() const;
        void* const operator-> ();
        void dispose();
        friend int lua_UserData_gc(lua_State* L);

        ParameterValue operator+(const ParameterValue& p);  // __add
        ParameterValue operator-(const ParameterValue& p);  // __sub
        ParameterValue operator*(const ParameterValue& p);  // __mul
        ParameterValue operator/(const ParameterValue& p);  // __div
        ParameterValue operator%(const ParameterValue& p);  // __mod
        ParameterValue operator^(const ParameterValue& p);  // __pow
        ParameterValue operator-();                         // __unm
        ParameterValue operator&(const ParameterValue& p);  // __band
        ParameterValue operator|(const ParameterValue& p);  // __bor
        ParameterValue XOR(const ParameterValue& p);        // __bxor
        ParameterValue operator~();                         // __bnot
        ParameterValue operator<<(const ParameterValue& p); // __shl
        ParameterValue operator<<(const int& i);            // __shl
        ParameterValue operator>>(const ParameterValue& p); // __shr
        ParameterValue operator>>(const int& i);            // __shr
        ParameterValue concat(const ParameterValue& b);     // __concat
        ParameterValue length();                            // __len
        bool operator==(const ParameterValue& p);           // __eq
        bool operator!=(const ParameterValue& p);           // __eq
        bool operator<(const ParameterValue& p);            // __lt
        bool operator>(const ParameterValue& p);            // __lt
        bool operator<=(const ParameterValue& p);           // __le
        bool operator>=(const ParameterValue& p);           // __le
        // __index
        // __newindex
        std::vector<ParameterValue> operator()(std::vector<ParameterValue> params);

        operator int() const;
        operator double() const;
        operator bool() const;
        operator std::string() const;
        operator std::unordered_map<std::string, ParameterValue>() const;
        explicit operator void*() const;

        friend std::ostream& operator<<(std::ostream& os, const ParameterValue& obj);
    private:
        size_t _type;
        bool _disposed;
        void * _value;
        int* _references;
        bool* _luaref;
};

#endif // PARAMETERVALUE_H
