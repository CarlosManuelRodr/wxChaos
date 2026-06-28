/**
* @file FractalInteractionTool.h
* @brief Defines the shared fractal mouse interaction tools.
*/

#pragma once

/**
* @enum FractalInteractionTool
* @brief Mouse interaction tools exposed by the fractal toolbar.
*/
enum class FractalInteractionTool
{
    Cursor,  ///< Default canvas interactions.
    Hand,    ///< Pan the fractal by dragging.
    Zoom     ///< Drag vertically to zoom around the clicked point.
};
