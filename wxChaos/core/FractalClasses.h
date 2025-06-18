#pragma once
#ifndef FRACTAL_CLASSES_H
#define FRACTAL_CLASSES_H

#include "Fractal.h"
#include "RenderFractal.h"
#include "SystemUtils.h"

            }
        }
    }

    watchdog.LaunchThreads();
    watchdog.launch();

    if (waitRoutine)
    {
        watchdog.wait();
    }
}

#endif
