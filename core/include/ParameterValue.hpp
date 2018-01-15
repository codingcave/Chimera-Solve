#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

typedef struct lua_State lua_State;

namespace chimera {
    class ChimeraSystem;
    class ParameterReferenceInteger;
    class ParameterReferenceString;

    struct ParameterValueData {
        ChimeraSystem* cmSys;
        size_t type;
        void* value;
        int references;
        lua_State* luaref;
    };

    class ParameterValue
    {
        friend class ItemContainer;
        friend class ParameterTypeSystem;
        friend class ParameterReferenceInteger;
        friend class ParameterReferenceString;

        public:
            ParameterValue(const ParameterValue& p);
            ParameterValue(const double& value);
            ParameterValue(const int& value);
            ParameterValue(const bool& value);
            ParameterValue(const std::string& value);
            ParameterValue(const char value[]);
            //ParameterValue(const std::unordered_map<std::string, ParameterValue>& value);
            //ParameterValue(const std::vector<ParameterValue>& value);
            ParameterValue(const ParameterReferenceInteger& value);
            ParameterValue(const ParameterReferenceString& value);

            virtual ~ParameterValue();
            size_t getType() const;
            void* getValue() const;
            void* const operator-> ();
            void dispose();

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
            ParameterReferenceInteger operator[](std::size_t idx);
            const ParameterReferenceInteger operator[](std::size_t idx) const;
            ParameterReferenceString operator[](std::string idx);
            const ParameterReferenceString operator[](std::string idx) const;

            std::vector<ParameterValue> operator()(std::vector<ParameterValue>& params);
            ParameterValue operator()(ParameterValue p1);
            ParameterValue operator()(ParameterValue p1, ParameterValue p2);
            ParameterValue operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3);
            ParameterValue operator()(ParameterValue p1, ParameterValue p2, ParameterValue p3, ParameterValue p4);

            bool isCallable() const;

            operator int() const;
            operator double() const;
            operator bool() const;
            operator std::string() const;
            //operator std::unordered_map<std::string, ParameterValue>() const;
            //operator std::vector<ParameterValue>() const;
            explicit operator void*() const;

            friend std::ostream& operator<<(std::ostream& os, const ParameterValue& obj);
        protected:
            void bind(ChimeraSystem* chSys) const;
        private:
            friend int chimera::lua_UserData_gc(lua_State* L);
            ParameterValue(size_t type, void* value);
            ParameterValue(ChimeraSystem* cmSys, size_t type, void* value);
            ParameterValue call_single(std::vector<ParameterValue>& params);
            static lua_State* getGlobalState(struct ParameterValueData* data);
            lua_State* getGlobalState() const;
            static ChimeraSystem* getChimeraSystem(struct ParameterValueData* data);
            ChimeraSystem* getChimeraSystem() const;

            static ParameterValue arith1(const ParameterValue& p1, int op);
            static ParameterValue arith2(const ParameterValue& p1, const ParameterValue& p2, int op);
            static ParameterValue compare(const ParameterValue& p1, const ParameterValue& p2, int op);

            bool _disposed;

            struct ParameterValueData* _data;
    };

    class ParameterReferenceInteger
    {
        friend class ParameterValue;
        public:
            virtual ~ParameterReferenceInteger();

            ParameterValue getValue() const;
            ParameterReferenceInteger& operator=(const ParameterValue& other);
            friend std::ostream& operator<<(std::ostream& os, const ParameterReferenceInteger& obj);
        protected:
        private:
            ParameterReferenceInteger(const ParameterValue& value, size_t idx);
            ParameterValue _inner;
            size_t _idx;
    };

    class ParameterReferenceString
    {
        friend class ParameterValue;
        public:
            virtual ~ParameterReferenceString();
            ParameterValue getValue() const;
            ParameterReferenceString& operator=(const ParameterValue& other);
            friend std::ostream& operator<<(std::ostream& os, const ParameterReferenceString& obj);
        protected:
        private:
            ParameterReferenceString(const ParameterValue& value, std::string idx);
            ParameterValue _inner;
            std::string _idx;
    };

    std::ostream& operator<<(std::ostream& os, const chimera::ParameterValue& obj);
    std::ostream& operator<<(std::ostream& os, const chimera::ParameterReferenceInteger& obj);
    std::ostream& operator<<(std::ostream& os, const chimera::ParameterReferenceString& obj);
}

#endif // PARAMETERVALUE_H
