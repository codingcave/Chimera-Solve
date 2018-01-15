#ifndef ILOGGER_H
#define ILOGGER_H

namespace chimera {
    class ILogger
    {
        public:
            ILogger() {}
            virtual ~ILogger() {}
            virtual void CreateLog(const std::string& message) = 0;
            virtual void CreateWarning(const std::string& message) = 0;
            virtual void CreateError(const std::string& message) = 0;
        protected:
        private:
    };
}

#endif // ILOGGER_H
