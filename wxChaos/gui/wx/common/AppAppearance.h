#pragma once

/**
 * @enum AppAppearance
 * @brief Application appearance preference persisted in the settings file.
 */
enum class AppAppearance
{
    System, ///< Follows the operating system application appearance.
    Light,  ///< Forces the light application appearance.
    Dark    ///< Forces the dark application appearance.
};
