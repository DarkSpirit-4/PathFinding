#include "cpe/simulation.hpp"
namespace cpe {
void Navigator::Plan(const Level& level, Vec2 position,
                     Vec2 target, Algorithm algorithm) {
    // TODO J4 : gérer la requête, sa temporisation et les échecs.
    (void)level;
    (void)position;
    (void)target;
    (void)algorithm;
    if (requestCooldown > 0.0) return;
    bool changed = !hasTarget || Distance(target, lastTarget) > 0.75;
    bool retry = path.search.status != Status::Found;
    bool stoppedFar = Finished() && Distance(position, target) > 0.1;
    if (!changed && !retry && !stoppedFar) return;
    path = level.navmesh.FindPath(position, target, algorithm);
    if (path.search.status == Status::Found) {
        next = path.points.size() > 1 ? 1 : path.points.size();
        lastTarget = target;
        hasTarget = true;
        requestCooldown = 1.75;
        ++requests;
    }
    else {
        requestCooldown = 1.75;
    }
}
Vec2 Navigator::Follow(const Level& level, Vec2 position, double speed, double dt) {
    // TODO J4 : consommer les waypoints et appliquer un déplacement valide.
    (void)level;
    (void)speed;
    (void)dt;
    if (path.search.status != Status::Found || dt <= 0 || speed <= 0) return position;
    double budget = speed * dt;
    while (!Finished() && budget > 0.0) {
        Vec2 delta = path.points[next] - position;
        double distance = Length(delta);
        if (distance < 1e-8) { ++next; continue; }
        Vec2 displacement;
        if (distance <= budget) {
            displacement = delta;
        }
        else {
            displacement = delta * (budget / distance);
        }

        Vec2 oldPosition = position;

        position = level.MoveSliding(position, displacement, level.agentRadius);

        double moved = Distance(oldPosition, position);
        if (moved < 1e-8) break;

        if (Distance(position, path.points[next]) < 1e-6) {
            ++next;
        }
        else if (moved < distance) {
            break;
        }
    }
    return position;
}
}

