#pragma once

struct Vector2Int
{
    int x, y;

    static Vector2Int Zero() { return Vector2Int{0, 0}; }
};
