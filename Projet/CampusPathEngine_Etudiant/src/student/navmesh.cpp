#include "cpe/navigation.hpp"
#include <stdexcept>

namespace cpe {
    //Complet
    bool NavMesh::Build() {

        built_ = false;
        polygons_.clear();
        graph_.clear();

        for (int i = 0; i < source_.size(); i++) {
            if (source_[i].size() < 3) throw std::invalid_argument("Au moins trois sommets requis");
            Vec2 sum{};
            for (std::size_t j = 0;j < source_[i].size();++j) {
                if (!Finite(source_[i][j]))
                    throw std::invalid_argument("Coordonnees invalides.");
                for (std::size_t k = 0;k < j;++k) {
                    if (Near(source_[i][j], source_[i][k]))
                        throw std::invalid_argument("Sommet repete.");
                }
                sum = sum + source_[i][j];
            }
            const Vec2 center = sum * (1.0 / static_cast<double>(source_[i].size()));
            polygons_.push_back({ source_[i], center });
        }

        for (int l = 0; l < portals_.size(); l++) {
            if (!ValidNode(graph_,
                portals_[l].a) ||
                !ValidNode(graph_, portals_[l].b) ||
                portals_[l].a == portals_[l].b) {
                throw std::invalid_argument(
                    "Indices de portail invalides."
                );
            }

            if (!Finite(portals_[l].p) || !Finite(portals_[l].q) || Distance(portals_[l].p, portals_[l].q) < 1e-8) {
                throw std::invalid_argument(
                    "Portail invalide ou de longueur nulle.");
            }

            int regionA = portals_[l].a;
            int regionB = portals_[l].b;

            // Les deux extrémités doivent appartenir aux deux régions.
            if (!Contains(regionA, portals_[l].p) ||
                !Contains(regionA, portals_[l].q) ||
                !Contains(regionB, portals_[l].p) ||
                !Contains(regionB, portals_[l].q)) {
                throw std::invalid_argument(
                    "Le portail n'appartient pas aux deux regions."
                );
            }

            const Vec2 middle = portals_[l].Middle();

            const double cost = Distance(polygons_[regionA].center, middle) + Distance(middle, polygons_[regionB].center);

           AddUndirected(graph_, portals_[l].a, portals_[l].b, cost);


            // TODO J3 : valider et structurer source_, calculer les adjacences/portails.
            // Remplir polygons_, portals_, graph_, puis définir built_=true.

        }

        built_ = true;
        return true;

    }

int NavMesh::Locate(Vec2 point) const {
    // TODO J3 : localiser le point dans un polygone navigable.
    (void)point;
    if (!Finite(point)) return -1;
    for (int i = 0;i < static_cast<int>(polygons_.size());++i) {
        if (Contains(i, point)) return i;
    }
    return -1;
}
bool NavMesh::Contains(int polygon, Vec2 point) const {
    // TODO J3 : test point dans polygone.
    (void)polygon;
    (void)point;
    if (!Finite(point) || polygons_[polygon].vertices.size() < 3) return false;
    constexpr double epsilon = 1e-8;
    bool positive = false, negative = false;
    const auto& vertices = polygons_[polygon].vertices;
    for (std::size_t i = 0;i < vertices.size();++i) {
        const Vec2 a = vertices[i];
        const Vec2 b = vertices[(i + 1) % vertices.size()];
        const double side = Cross(b - a, point - a);
        if (side >= epsilon) positive = true;
        if (side < -epsilon) negative = true;
        if (positive && negative) return false;
    }
    return true;
}

bool NavMesh::SegmentInside(Vec2 from, Vec2 to) const {
    // TODO J3 : vérifier qu'un segment reste dans la surface navigable.
    (void)from;
    (void)to;
    if (!Finite(from) || !Finite(to)) return false;
    std::vector<std::pair<double, double>> intervals;
    const Vec2 direction = to - from;
    for (const Polygon& polygon : polygons_) {
        const auto& v = polygon.vertices;
        double orientation = Cross(v[1] - v[0], v[2] - v[1]) > 0 ? 1.0 : -1.0;
        double low = 0.0, high = 1.0;
        bool valid = true;
        for (std::size_t i = 0; i < v.size(); ++i) {
            Vec2 edge = v[(i + 1) % v.size()] - v[i];
            double base = orientation * Cross(edge, from - v[i]);
            double slope = orientation * Cross(edge, direction);
            // base + slope*t >= -epsilon
            if (std::abs(slope) < 1e-12) {
                if (base < -1e-9) { valid = false; break; }
            }
            else {
                double bound = (-1e-9 - base) / slope;
                if (slope > 0) low = std::max(low, bound);
                else high = std::min(high, bound);
            }
        }
        if (valid && low <= high) intervals.push_back({ low,high });
    }
    std::sort(intervals.begin(), intervals.end());
    double covered = 0.0;
    for (auto [low, high] : intervals) {
        if (low > covered + 1e-8) return false;
        covered = std::max(covered, high);
        if (covered >= 1.0 - 1e-8) return true;
    }
    return false;
}

const Portal* NavMesh::Between(int a, int b) const {
    for (const Portal& portal : portals_) {
        const bool forward =
            portal.a == a && portal.b == b;

        const bool backward =
            portal.a == b && portal.b == a;

        if (forward || backward) {
            return &portal;
        }
    }

    return nullptr;
}

Path NavMesh::FindPath(Vec2 from, Vec2 to, Algorithm algorithm) const {
    // TODO J3 : relier localisation, recherche sur graphe et trajet géométrique.
    (void)from;
    (void)to;
    (void)algorithm;
    Path result;

    if (!Finite(from) || !Finite(to)) {
        return result;
    }

    const int startRegion = Locate(from);
    const int goalRegion = Locate(to);

    if (startRegion < 0 || goalRegion < 0) {
        result.search.status = Status::OutsideNavMesh;
        return result;
    }

    const std::vector<double> h = [&]() {
        std::vector<double> heurist;

        for (int n = 0; n < polygons_.size(); ++n) {
            heurist.push_back(
                Distance(
                    polygons_[n].center,
                    polygons_[goalRegion].center
                )
            );
        }

        return heurist;
        }();

    if (Algorithm::AStar == algorithm) {
        result.search = AStar(graph_, startRegion, goalRegion, h);
    }
    else {
        result.search = Dijkstra(graph_, startRegion, goalRegion);
    }

    if (result.search.status != Status::Found) {
        result.search.status = Status::NoPath;
        return result;
    }

    auto append = [&](Vec2 point) {
        if (result.points.empty() ||
            !Near(result.points.back(), point)) {
            result.points.push_back(point);
        }
        };

    append(from);

    if (startRegion != goalRegion) {
        append(polygons_[startRegion].center);

        for (std::size_t i = 1;
            i < result.search.nodes.size();
            ++i) {
            const int previous = result.search.nodes[i - 1];
            const int next = result.search.nodes[i];

            const Portal* portal =
                Between(previous, next);

            if (portal == nullptr) {
                return {};
            }

            append((portal->p + portal->q) * 0.5);
            append(polygons_[next].center);
        }
    }

    append(to);

    result.length = 0.0;

    for (std::size_t i = 1; i < result.points.size(); ++i) {
        result.length += Distance(
            result.points[i - 1],
            result.points[i]
        );
    }
    return result;
}
// à voir
void AddUndirected(Graph& graph_, int a, int b, double cost) {
    // Précondition : a et b sont des indices valides.
    graph_.at(a).push_back({ b, cost });
    graph_.at(b).push_back({ a, cost });
}

void AddDirected(Graph& graph_, int a, int b, double cost) {
    // Précondition : a et b sont des indices valides.
    graph_.at(a).push_back({ b, cost });
}

}