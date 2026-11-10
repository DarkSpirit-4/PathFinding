#include "cpe/simulation.hpp"
namespace cpe {
void Simulation::UpdateGameplay(bool actionPressed, double dt) {
    // TODO PROJET : attaques, dégâts, vie, mort, vagues, victoire et défaite.
    // Espace alimente actionPressed ; aucun effet de jeu n'est fourni.
    (void)actionPressed;
    (void)dt;

    player.attackCooldown = std::max(0.0, player.attackCooldown - dt);

    if (player.hp <= 0) {
        player.hp = 0;
        return;
    }

    if (actionPressed && player.attackCooldown <= 0.0) {
        player.attackCooldown = player.attackDelay;

        for (Enemy& enemy : enemies) {
            if (enemy.hp <= 0) continue;

            const Vec2 toEnemy = enemy.position - player.position;
            const double distance = Length(toEnemy);
            if (distance > player.attackRange) continue;

            if (distance > 1e-8 &&
                Dot(player.facing, toEnemy * (1.0 / distance)) < player.attackCone) continue;

            enemy.hp -= player.attackDamage;
        }
    }

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const Enemy& e) { return e.hp <= 0; }), enemies.end());
}
}