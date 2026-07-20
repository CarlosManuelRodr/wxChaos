#pragma once

/**
* @enum TutorialStatus
* @brief Persisted lifecycle state for the guided first-run tutorial.
*/
enum class TutorialStatus
{
    Pending,
    Completed,
    Dismissed
};
