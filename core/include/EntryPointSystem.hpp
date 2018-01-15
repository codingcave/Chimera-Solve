#ifndef ENTRYPOINTSYSTEM_H
#define ENTRYPOINTSYSTEM_H

namespace chimera {
    class ChimeraSystem;

    class EntryPointSystem final:
        public StateSynchrony
    {
        friend class EntryPoint;
        friend class ChimeraSystem;

        public:
            virtual ~EntryPointSystem();
            void addEntryPoint(const std::string& name, EntryPoint* const ep);
            EntryPoint* const getEntryPoint(const std::string& name);
            const std::string findEntryPoint(EntryPoint const * const entrypoint) const;
            EntryPoint::iterator begin();
            EntryPoint::iterator end();

            virtual const std::string getGUID() const override;
            void removeEntryPoint(const std::string& name);
        protected:
        private:
            //void notifyLoad(StateSynchrony* sender, void const * const data) override;
            //void notifyUnload(StateSynchrony* sender, void const * const data) override;
            void notifyDelete(StateSynchrony* sender) override;
            ChimeraSystem* _chSys;
            EntryPointSystem();
            map_t_EntryPoint* _hooks;
    };
}

#endif // ENTRYPOINTSYSTEM_H
