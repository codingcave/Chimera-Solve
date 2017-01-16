#ifndef MODULELOADER_H
#define MODULELOADER_H

//enum EntryPoint { Dynamic, Coupling, Noise, Initialsation, Integrator, Output };

class Registry;

typedef std::vector<std::string> vec_t_pathList;

typedef Registry* (*fn_factory_ptr) ();

typedef std::unordered_map<std::string, Registry*> map_t_ModuleRegistry;
typedef std::unordered_map<std::string, void*> map_t_ModuleItem;
//typedef std::unordered_map<EntryPoint, map_t_ModuleEntry*, std::hash<int> > map_t_ModuleCategory;

//typedef std::unordered_map<std::string,system_factory_ptr> map_t_SystemDynamics;

class ModuleLoader
{
    public:
        ModuleLoader(const vec_t_pathList& importLookup);
        virtual ~ModuleLoader();
        Registry * const Load(const std::string& name);
        void Unload(const std::string& name);
        void UnloadAll();
    protected:
    private:
        std::vector<std::string> *_importLookup;
        map_t_ModuleItem *_modulePtr;
        map_t_ModuleRegistry *_moduleReg;
};

#endif // MODULELOADER_H
