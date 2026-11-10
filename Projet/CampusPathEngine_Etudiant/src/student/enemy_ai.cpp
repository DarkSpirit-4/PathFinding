#include "cpe/simulation.hpp"
#include <algorithm>
#include <stdexcept>

namespace cpe {
    void Simulation::UpdateEnemy(Enemy& enemy, double dt) {

        // TODO PROJET : perception, transitions, décisions et appel du Navigator.
        // Les ennemis restent immobiles tant que ce module n'est pas implémenté.

        (void)enemy;
        (void)dt;

        enemy.navigator.requestCooldown = std::max(0.0, enemy.navigator.requestCooldown - dt);

        if (enemy.hp <= 0) {
            enemy.state = State::Dead;
            enemy.navigator.Invalidate();
            return;
        }

        if (enemy.state == State::Dead) return;
        if (enemy.state == State::Waiting) enemy.state = State::Idle;

        double distance = Distance(enemy.position, player.position);

        if (player.hp <= 0) {
            enemy.state = State::Idle;
            enemy.timer = 0.0;
            enemy.navigator.Invalidate();
            return;
        }

        switch (enemy.state) {
        case State::Idle:
            if (distance <= enemy.detectRange) {
                enemy.state = State::Chase;
                enemy.navigator.Invalidate();
            }
            break;

        case State::Chase:
            if (distance > enemy.loseRange) {
                enemy.state = State::Idle;
                enemy.navigator.Invalidate();
                break;
            }
            if (distance <= enemy.attackRange && level.navmesh.SegmentInside(player.position, enemy.position)) {
                enemy.navigator.Invalidate();
                enemy.state = State::Windup;
                enemy.timer = enemy.windupDuration;
                break;
            }
            else {
                enemy.navigator.Plan(level, enemy.position, player.position, algorithm);
                enemy.position = enemy.navigator.Follow(level, enemy.position, enemy.speed, dt);
            }
            break;

        case State::Windup:
            distance = Distance(enemy.position, player.position);
            if (distance > enemy.attackRange) {
                enemy.state = State::Chase;
                enemy.timer = 0.0;
                enemy.navigator.Invalidate();
                break;
            }
            enemy.timer -= dt;
            if (enemy.timer <= 0.0) {
                distance = Distance(enemy.position, player.position);
                if (distance <= enemy.attackRange && level.navmesh.SegmentInside(player.position, enemy.position)) {
                    player.hp = player.hp - enemy.damage;
                    enemy.state = State::Recover;
                    enemy.timer = enemy.recoverDuration;
                }
                else {
                    enemy.state = State::Chase;
                    enemy.navigator.Invalidate();
                }

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
