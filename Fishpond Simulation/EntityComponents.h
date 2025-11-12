#ifndef ENTITYCOMPONENTS_H
#define ENTITYCOMPONENTS_H
#include <raylib.h>
struct TagFish {};
struct TagKoi {};
struct PositionComponent { Vector2 pos; };
struct RotationComponent { float angle; };
struct CellID { int id; };
struct ForwardDirection { Vector2 dir; };
struct CellPosDir { Vector2 pos; Vector2 dir; };
struct Neighbours { std::vector<Vector2> pos; };
struct NeiDirection { std::vector<Vector2> dir; };
struct FishSpeed { float speed; };

#endif // !ENTITYCOMPONENTS_H

