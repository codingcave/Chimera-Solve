#ifndef PARAMETERVALUE_H
#define PARAMETERVALUE_H

class ParameterValue
{
    public:
        ParameterValue(size_t type, void* value);
        ParameterValue(const ParameterValue& p);
        virtual ~ParameterValue();
        size_t getType() const;
        //void * getValue() const;
        void* const& operator* ();
        void dispose();
    private:
        size_t _type;
        bool _disposed;
        void * _value;
        int* _references;
        bool* _luaref;
};

#endif // PARAMETERVALUE_H
