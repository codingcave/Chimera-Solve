#ifndef STATESYNCHRONY_H
#define STATESYNCHRONY_H

namespace chimera {
    class StateSynchrony
    {
        public:
            StateSynchrony();
            virtual ~StateSynchrony();
            void addListener(StateSynchrony* listener);
            void removeListener(StateSynchrony* listener);
            bool isLoaded() const;
            void stateLoaded();
            void stateLoaded(void const * const data);
            void stateUnloaded();
            void stateUnloaded(void const * const data);
            void itemAdded(void const * const item, void const * const data);
            void itemRemoved(void const * const item, void const * const data);
            virtual void notifyLoad(StateSynchrony* sender, void const * const data);
            virtual void notifyUnload(StateSynchrony* sender, void const * const data);
            virtual void notifyItemAdded(StateSynchrony* sender, void const * const item, void const * const data);
            virtual void notifyItemRemoved(StateSynchrony* sender, void const * const item, void const * const data);
            virtual void notifyItemLoaded(StateSynchrony* sender, void const * const item, void const * const data);
            virtual void notifyItemUnloaded(StateSynchrony* sender, void const * const item, void const * const data);
            virtual void notifyDelete(StateSynchrony* sender);

            virtual const std::string getGUID() const = 0;
        protected:
        private:
            void itemLoaded(void const * const item, void const * const data);
            void itemUnloaded(void const * const item, void const * const data);
            std::unordered_set<StateSynchrony*>* _listeners;
            std::unordered_set<StateSynchrony*>* _linkedItems;
            bool _loaded;
    };
}

#endif // STATESYNCHRONY_H
