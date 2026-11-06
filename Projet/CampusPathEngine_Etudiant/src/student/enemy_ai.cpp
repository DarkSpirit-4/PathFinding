#include "cpe/simulation.hpp"
#include <algorithm>
#include <stdexcept>

namespace cpe {
    void Simulation::UpdateEnemy(Enemy& enemy, double dt) {

        // TODO PROJET : perception, transitions, décisions et appel du Navigator.
        // Les ennemis restent immobiles tant que ce module n'est pas implémenté.

        (void)enemy;
        (void)dt;

        if (enemy.hp <= 0) {
            enemy.state = State::Dead;
            enemy.navigator.Invalidate();
            return;
        }

        if (enemy.state == State::Dead) return;
        if (enemy.state == State::Waiting) enemy.state == State::Idle;

        const bool distance = Distance(enemy.position, player.position);
        const bool canHit = distance <= enemy.attackRange;

        if (player.hp <= 0 || distance > enemy.loseRange) {
            enemy.state = State::Idle;
            enemy.timer = 0.0;
            return;
        }

        switch (enemy.state) {
        case State::Idle:
            if (distance <= enemy.detectRange) enemy.state = State::Chase;
            break;

        case State::Chase:
            if (canHit) {
                enemy.navigator.Invalidate();
                enemy.state = State::Windup;
                enemy.timer = enemy.windupDuration;
            }
            else {
                enemy.navigator.Plan(level, enemy.position, player.position, algorithm);
                enemy.position = enemy.navigator.Follow(level, enemy.position, enemy.speed, dt);
            }
            break;

        case State::Windup:
            enemy.timer -= dt;
            if (enemy.timer <= 0) {
                if (canHit) player.hp = player.hp - enemy.damage;
                enemy.state = State::Recover;
                enemy.timer = enemy.recoverDuration;
            }
            break;

        case State::Recover:
            enemy.timer -= dt;
            if (enemy.timer <= 0) enemy.state = State::Chase;
            break;

        case State::Waiting: break;

        case State::Dead: break;
        }

    }
}
