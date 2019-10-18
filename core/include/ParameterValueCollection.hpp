#ifndef PARAMETERVALUECOLLECTION_H
#define PARAMETERVALUECOLLECTION_H

namespace chimera {
    class ParameterValueCollection
    {
        friend class ChimeraSystem;
        public:
            ParameterValueCollection();
            virtual ~ParameterValueCollection();

            void setValue(const std::string& name, ParameterValue& value);
            std::unordered_map<std::string, ParameterValue>::const_iterator beginItems() const;
            std::unordered_map<std::string, ParameterValue>::const_iterator endItems() const;
            ParameterValue operator[](const std::string& name) const;
            virtual ChimeraSystem* getChimeraSystem() const = 0;
        protected:
            //void truncate();
        private:
            map_t_LuaItem* _items;
    };
}

#endif // PARAMETERVALUECOLLECTION_H
