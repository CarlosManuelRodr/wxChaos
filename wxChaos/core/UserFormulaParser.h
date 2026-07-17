#pragma once

namespace UserFormulaParser
{
    /**
     * @brief Initializes muParserX's lazily created global packages on one thread.
     *
     * The bundled muParserX package singletons are not safe to initialize
     * concurrently. Call this before starting formula-render worker threads.
     */
    void EnsureInitialized();
}
