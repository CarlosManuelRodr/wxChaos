#include "UserFormulaParser.h"

#include <mutex>
#include <mpParser.h>

namespace UserFormulaParser
{
    void EnsureInitialized()
    {
        static std::once_flag initializationFlag;
        std::call_once(initializationFlag, []
        {
            const mup::ParserX parser;
        });
    }
}
