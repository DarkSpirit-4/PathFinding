#pragma once
#include "level.hpp"
#include <cstddef>

namespace cpe {
struct Player { Vec2 position; Vec2 facing{0,-1};
int hp = 10;
double attackCooldown = 0.0;
double attackRange = 1.6;
double attackCone = 0.5;
double attackDelay = 0.8;
int attackDamage = 2;
};

// Ce composant expose les données au debug ; le suivi est à implémenter.
struct Navigator {
    Path path;
    std::size_t next = 0;
    std::size_t requests = 0;
    Vec2 lastTarget{};
    bool hasTarget = false;
    double requestCooldown = 0.0;
    const bool Finished();
    void Invalidate();
    void Plan(const Level& level, Vec2 position, Vec2 target, Algorithm algorithm);
    Vec2 Follow(const Level& level, Vec2 position, double speed, double dt);
};
enum class State { Waiting, Idle, Chase, Windup, Recover, Dead , Healing};
const char* Name(State value);

// Entités de démonstration immobiles ; aucun comportement n'est fourni.
// Ajoutez ici les données nécessaires à votre propre IA et votre combat.
struct Enemy {
    Vec2 position;
    EnemyKind kind = EnemyKind::Light;
    State state = State::Waiting;
    Navigator navigator;
    int hp = 10;
    int maxHp = 10;
    double timer = 0.0;
    double detectRange = 8.0, loseRange = 10.0, attackRange = 0.7, healRange = 2.0;
    double windupDuration = 0.5, recoverDuration = 0.8, healCooldown = 1.0;
    double speed = 2.5;
    double speedheavy = 2.0;
    double speedsupport = 2.0;
    int damage = 1;
    int damageheavy = 3;
    int heal = 2;
    int healTarget = -1;
};
class Simulation {
public:
    explicit Simulation(Level level);
    Level level;
    Player player;
    std::vector<Enemy> enemies;
    Algorithm algorithm = Algorithm::AStar;
    double elapsed = 0;

    // Fourni : déplacement, collisions, boucle et appels des points d'extension.
    void Update(Vec2 movement, bool actionPressed, double dt);
    void Reset();
    void SetAlgorithm(Algorithm value);
    std::size_t RequestCount() const;
    int closest(Enemy& enemy);

private:
    // À réaliser dans src/student/enemy_ai.cpp et gameplay.cpp.
    void UpdateEnemy(Enemy& enemy, double dt);
    void UpdateGameplay(bool actionPressed, double dt);
};
}
