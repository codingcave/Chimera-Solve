#ifndef ITEMCONTAINER_H
#define ITEMCONTAINER_H

class ItemContainer
{
    friend class LuaParser;
    public:
        ItemContainer();
        virtual ~ItemContainer();

        void setValue(const std::string& name, ParameterValue& value);
        std::unordered_map<std::string, ParameterValue>::const_iterator beginItems() const;
        std::unordered_map<std::string, ParameterValue>::const_iterator endItems() const;
        //ItemAssigner& operator[](const std::string& name);
        ParameterValue operator[](const std::string& name) const;
    protected:
        void truncate();
    private:
        map_t_LuaItem* _items;
};

#endif // ITEMCONTAINER_H
