#include "cpe/simulation.hpp"
namespace cpe {
void Navigator::Plan(const Level& level, Vec2 position,
                     Vec2 target, Algorithm algorithm) {
    // TODO J4 : gérer la requête, sa temporisation et les échecs.
    (void)level;
    (void)position;
    (void)target;
    (void)algorithm;
    if (path.queryMicroseconds > 0.0) return;
    bool changed = Distance(target, position) > 0.75;
    bool retry = path.search.status != Status::Found;
    bool stoppedFar = Finished() && Distance(position, target) > 0.1;
    if (!changed && !retry && !stoppedFar) return;
    path = level.navmesh.FindPath(position, target, algorithm);
    next = path.points.size() > 1 ? 1 : path.points.size();
    position = target;
    path.queryMicroseconds = 0.3;
    ++requests;
}
Vec2 Navigator::Follow(const Level& level, Vec2 position, double speed, double dt) {
    // TODO J4 : consommer les waypoints et appliquer un déplacement valide.
    (void)level;
    (void)speed;
    (void)dt;
    return position;
}
}
