# Campus Path Engine — socle étudiant

**C++17 / raylib 5.5 · S1P2 Pathfinding · GTECH 2**

Ce moteur fournit le socle technique pour travailler directement en C++. Les fonctionnalités évaluées restent à développer par les étudiants.

## Ce qui fonctionne dès le lancement

- Fenêtre 3D et caméra orthographique.
- Déplacement du joueur avec flèches, ZQSD ou WASD.
- Collisions physiques avec les murs et les limites du niveau, prise en compte d'un rayon.
- Chargement de deux scènes depuis des fichiers texte.
- Affichage des obstacles, du joueur et de marqueurs d'ennemis.
- Sélection d'une entité, pause, redémarrage et changement de scène.
- Affichage des données de surface, puis des portails et chemins lorsque votre code les produit.
- Sonde de navigation au clic et affichage du statut.
- CMake, scripts de compilation, tests du socle et tests de contrats.

Le socle se lance et permet de déplacer le joueur **avant toute implémentation du pathfinding**.

## Ce que vous devez développer

| Fonctionnalité | Fichier à compléter |
|---|---|
| Dijkstra et A* | src/student/pathfinding.cpp |
| Composant NavMesh : polygones navigables, localisation, graphe, adjacences et portails | src/student/navmesh.cpp |
| Requête de chemin et conversion en trajet géométrique | src/student/navmesh.cpp |
| Suivi de chemin, recalcul et gestion des échecs | src/student/navigation_agent.cpp |
| Perception, décisions et comportements ennemis | src/student/enemy_ai.cpp |
| Combat, vie, dégâts, mort, vagues et fin de partie | src/student/gameplay.cpp |

Les fichiers contiennent des signatures et des TODO. **Il n'y a pas de corrigé, d'implémentation de référence ni de bibliothèque de navigation dans cette archive.**

Les marqueurs ennemis sont immobiles. Espace est transmis au point d'extension de gameplay sans déclencher d'attaque. Une requête de navigation affiche **A IMPLEMENTER**. Ces comportements sont normaux tant que vos modules ne sont pas développés.

## Premier lancement sous Windows — Visual Studio 2026

La solution native **CampusPathEngine.sln** est fournie. Elle utilise MSVC v145 en x64, sans GCC, MinGW ou génération CMake pour le travail en salle.

1. Installer Visual Studio 2026 avec la charge Développement Desktop en C++, les outils MSVC x64/x86 et un SDK Windows 10/11. Le fichier `.vsconfig` décrit la charge requise.
2. Extraire intégralement l’archive dans un dossier local accessible en écriture.
3. Ouvrir **CampusPathEngine.sln** (Fichier > Ouvrir > Projet/Solution).
4. Choisir **Debug | x64**, définir **campus_path_engine** comme projet de démarrage, puis Générer la solution.
5. Lancer avec Ctrl+F5, ou F5 pour déboguer.

Les sources de raylib sont incluses et compilées par le projet `raylib`. Aucun téléchargement de bibliothèque n’est nécessaire lors de la génération. Le pilote graphique doit prendre en charge OpenGL 3.3.

Les exécutables sont produits dans `bin/x64/Debug/` ou `bin/x64/Release/`. Les données sont copiées près de l’exécutable et le répertoire de travail du débogueur est la racine du moteur.

Pour les tests, définir temporairement `cpe_tests` comme projet de démarrage. Les tests de contrat `cpe_student_tests` attendent l’argument `dijkstra`, `astar` ou `navmesh` : voir le guide de compilation du lancement. Un retour 77 signifie « à implémenter », pas « fonctionnalité validée ».

Dans un terminal de développement Visual Studio 2026, `build_windows.bat` compile la solution en Debug, lance les tests du socle puis l’application. La compilation par l’interface reste le parcours de référence.

Les fichiers CMake restent fournis pour la portabilité. Ils ne sont pas nécessaires au parcours `.sln` de la salle. Ne pas mélanger les dossiers de sortie des deux systèmes.

## Linux et macOS

Sous Ubuntu 24.04, installez les outils et les dépendances graphiques :

~~~bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev
bash build.sh --run
~~~

Sur une autre distribution, adaptez les paquets. Une session graphique est nécessaire pour ouvrir la fenêtre.

Sur macOS, utilisez les outils de développement Xcode et CMake, puis les commandes ci-dessous. Le backend natif est sélectionné par raylib ; les paquets Linux ne sont pas nécessaires.

~~~bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/campus_path_engine
~~~

## Commandes

| Commande | Action fournie |
|---|---|
| Flèches, ZQSD ou WASD | Déplacer le joueur avec collisions physiques |
| F1 | Afficher les surfaces sources ou le NavMesh construit par votre code |
| F2 | Afficher les chemins produits par votre code |
| F3 | Afficher ou masquer le HUD |
| Tab | Choisir Dijkstra ou A* pour les requêtes |
| Clic gauche sur le sol | Appeler NavMesh::FindPath depuis le joueur |
| Clic droit sur une entité ou N | Sélectionner un marqueur ennemi |
| F5 | Changer de scène |
| R | Réinitialiser les positions de la scène |
| P | Mettre la simulation en pause |
| Espace | Appeler le point d'extension de gameplay |
| Échap | Fermer |

Le clic de sonde ne déplace pas le joueur. La sonde est recalculée au clic ou au changement d'algorithme. Espace n'a pas d'effet visible dans le socle initial.

## Organisation des sources

~~~text
include/cpe/
  math.hpp           Primitives mathématiques et collisions fournies
  level.hpp          Niveau et collisions physiques
  navigation.hpp     Contrats du pathfinding et du NavMesh
  simulation.hpp     Entités, suivi et points d'extension

src/engine/
  main.cpp           Fenêtre, rendu, commandes et HUD
  level.cpp          Chargement et collisions physiques
  simulation.cpp     Boucle, déplacement de base et appels des modules
  navigation_debug.cpp  Stockage des données sources et noms du debug

src/student/
  pathfinding.cpp
  navmesh.cpp
  navigation_agent.cpp
  enemy_ai.cpp
  gameplay.cpp
~~~

Vous pouvez modifier les en-têtes et ajouter vos propres fichiers C++. Le dossier student regroupe les points de départ, pas une restriction sur l'architecture finale. Ajoutez vos nouveaux fichiers au CMakeLists.txt.

## Commencer

1. Compilez le socle sans modification.
2. Vérifiez le déplacement, les collisions et le changement de scène.
3. Lisez **docs/TRAVAIL_ETUDIANT.md** et les contrats dans les en-têtes.
4. Implémentez Dijkstra, puis A*.
5. Construisez le NavMesh, les requêtes et le suivi.
6. Développez l'IA et les mécaniques du Hack'n Slash.

La bibliothèque de calcul est indépendante de raylib. Elle peut être testée sans ouvrir la fenêtre.

## Lire les résultats de tests

Le kit sépare :

- **cpe_core_tests** : vérifie le socle fourni ; il doit passer dès le départ.
- **student_dijkstra**, **student_astar**, **student_navmesh** : vérifient des résultats attendus, sans contenir les algorithmes.

Au départ, les trois tests étudiants sont signalés **Skipped / A implementer**, car les fonctions sont vides. Ce n'est ni une réussite de vos fonctionnalités ni une panne du moteur. Dès qu'une fonction retourne un résultat implémenté, les vérifications de son contrat sont exécutées.

Pour exécuter une seule famille :

~~~bash
ctest --test-dir build -R student_dijkstra --output-on-failure
~~~

Sous Visual Studio, ajoutez **-C Release**.

Pour travailler sans dépendances graphiques :

~~~bash
cmake -S . -B build-core -DCPE_BUILD_APP=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-core --parallel 2
ctest --test-dir build-core --output-on-failure
~~~

Les tests fournis ne couvrent pas tout le CDC : complétez-les et ajoutez vos scénarios de suivi, d'IA et de combat.

## Niveaux et ressources

Les fichiers de **data/levels/** décrivent les limites, les obstacles, les positions initiales et des surfaces géométriques d'exemple. La lecture de ces surfaces ne calcule ni leur convexité, ni leur adjacence, ni un graphe, ni un chemin.

Le joueur utilise les collisions physiques du moteur et ne dépend pas du NavMesh étudiant. Vos agents devront utiliser votre NavMesh et les collisions physiques de façon cohérente.

Voir **docs/FORMAT_NIVEAUX.md** pour modifier les données. La réalisation de votre arène et de vos données de navigation fait partie du projet.

## Dépannage

- **Ennemis immobiles / A IMPLEMENTER** : comportement attendu avant votre implémentation.
- **Tests étudiants Skipped** : leurs fonctions sont encore vides. Les tests du socle doivent passer.
- **CMake ou compilateur introuvable** : utilisez le terminal adapté et vérifiez l'installation des outils.
- **X11 ou OpenGL introuvable** : installez les paquets de développement graphiques. Les tests avec CPE_BUILD_APP=OFF restent accessibles sans eux.
- **Fichier de niveau refusé** : consultez le message et le numéro de ligne dans le terminal.
- **Données introuvables** : conservez data près de l'exécutable ou utilisez **--data CHEMIN**.
- **Vos nouveaux fichiers ne sont pas compilés** : ajoutez-les à la liste des sources de cpe_core dans CMakeLists.txt.

## Licence et vérification

Le code propre au socle est sous licence MIT. raylib conserve sa licence zlib/libpng et les licences de ses dépendances dans external/raylib.

Ce kit est la **version étudiant**, distincte de la référence formateur précédente. Seuls ce dossier et les fichiers inclus dans cette archive sont nécessaires. Les vérifications et leurs limites figurent dans **docs/VERIFICATION.md**.
