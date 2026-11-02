#include "cpe/navigation.hpp"
namespace cpe {
bool NavMesh::Build() {
    // TODO J3 : valider et structurer source_, calculer les adjacences/portails.
    // Remplir polygons_, portals_, graph_, puis définir built_=true.
    return false;
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
}
