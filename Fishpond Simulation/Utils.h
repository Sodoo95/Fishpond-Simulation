#pragma once 
#ifndef UTILS_H
#define UTILS_H
#include <raylib.h>
#include <math.h>

static inline std::vector<int> GetNeighbours(int index, int cols, int rows)
{
    std::vector<int> neighbors;

    int r = index / cols;
    int c = index % cols;

    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue; // skip self

            int nr = r + dr;
            int nc = c + dc;

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                neighbors.push_back(nr * cols + nc);
            }
        }
    }

    return neighbors;
}

static inline Vector2 GetDirectionOpposite(const Vector2 start, const Vector2 target) {
    Vector2 diff = start - target;
    return Vector2Normalize(diff);
}

static inline Vector2 GetDirectionTo(const Vector2& start, const Vector2& target) {
    Vector2 diff = target - start;
    return Vector2Normalize(diff);
}


#endif // !UTILS_H
