#include "cpe/simulation.hpp"
#include <utility>

namespace cpe {
Simulation::Simulation(Level source) : level(std::move(source)) { Reset(); }

void Simulation::Reset() {
    player = Player{};
    player.position = level.playerSpawn;
    enemies.clear();
    elapsed = 0;

    currentWave = -1;
}

void Simulation::SetAlgorithm(Algorithm value) {
    algorithm = value;
    for (auto& enemy : enemies) enemy.navigator.Invalidate();
}

std::size_t Simulation::RequestCount() const {
    std::size_t count = 0;
    for (const auto& enemy : enemies) count += enemy.navigator.requests;
    return count;
}

void Simulation::Update(Vec2 movement, bool actionPressed, double dt) {
    if (!std::isfinite(dt) || dt <= 0) return;
    dt = std::min(dt, 0.05);
    elapsed += dt;

    if (Finite(movement) && Length(movement) > 1e-8) {
        Vec2 direction = Normalized(movement);
        player.facing = direction;
        player.position = level.MoveSliding(
            player.position, direction * (3.5 * dt), level.agentRadius);
    }

    UpdateGameplay(actionPressed, dt);
    for (auto& enemy : enemies) UpdateEnemy(enemy, dt);
}
void Simulation::Attack(bool actionPressed)
{
    for (int i = 0; i < enemies.size(); i++) {
        if (Distance(player.position, enemies[i].position) < player.attackRange) {
            
        }

    }

}
}
