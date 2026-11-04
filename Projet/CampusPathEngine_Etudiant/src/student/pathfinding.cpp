#include "cpe/navigation.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <queue>
#include <utility>

namespace cpe {

    bool ValidNode(const Graph& graph, int id)
    {
        return id >= 0 && id < static_cast<int>(graph.size());
    }

    bool ValidGraph(const Graph& graph)
    {
        for (const auto& neighbors : graph) {
            for (const Edge& edge : neighbors) {
                if (!ValidNode(graph, edge.to) ||
                    !std::isfinite(edge.cost) || edge.cost < 0.0) {
                    return false;
                }
            }
        }
        return true;
    }

SearchResult Dijkstra(const Graph& graph, int start, int goal) {

    using Clock = std::chrono::steady_clock;
    const auto begin = Clock::now();
    SearchResult result;

    auto finish = [&]() {
        result.microseconds =
            std::chrono::duration<double, std::micro>(Clock::now() - begin).count();
        return result;
        };

    if (!ValidGraph(graph) || !ValidNode(graph, start) ||
        !ValidNode(graph, goal)) return finish();

    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> distance(graph.size(), infinity);
    std::vector<int> parent(graph.size(), -1);
    using Entry = std::pair<double, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> open;

    distance[start] = 0.0;
    open.push({ 0.0, start });
    while (!open.empty()) {
        const auto [savedCost, u] = open.top();
        open.pop();
        if (savedCost > distance[u]) continue;
        if (u == goal) {
            result.status = Status::Found;
            result.cost = distance[goal];
            for (int v = goal; v != -1; v = parent[v]) {
                result.nodes.push_back(v);
            }
            std::reverse(result.nodes.begin(), result.nodes.end());
            return finish();
        }
        ++result.expanded;
        for (const Edge& edge : graph[u]) {
            const double candidate = distance[u] + edge.cost;
            if (candidate < distance[edge.to]) {
                distance[edge.to] = candidate;
                parent[edge.to] = u;
                open.push({ candidate, edge.to });
            }
        }
    }
    result.status = Status::NoPath;
    return finish();
}

SearchResult AStar(const Graph& graph, int start, int goal,
    const std::vector<double>& heuristic) {
    using Clock = std::chrono::steady_clock;
    const auto begin = Clock::now();
    SearchResult result;
    auto finish = [&]() {
        result.microseconds =
            std::chrono::duration<double, std::micro>(Clock::now() - begin).count();
        return result;
        };
    if (!ValidGraph(graph) || !ValidNode(graph, start) ||
        !ValidNode(graph, goal)  || heuristic.size() != graph.size()) return finish();
    std::vector<double> h(graph.size());
    for (int i = 0; i < static_cast<int>(graph.size()); ++i) {
        h[i] = heuristic[i];
        if (!std::isfinite(h[i]) || h[i] < 0.0) return finish();
    }
    if (std::abs(h[goal]) > 1e-9) return finish();

    const double infinity = std::numeric_limits<double>::infinity();
    std::vector<double> g(graph.size(), infinity);
    std::vector<int> parent(graph.size(), -1);
    using Entry = std::tuple<double, double, int>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> open;
    g[start] = 0.0;
    open.push({ h[start], 0.0, start });
    while (!open.empty()) {
        const auto [priority, savedG, u] = open.top();
        open.pop();
        (void)priority;
        if (savedG > g[u]) continue;
        if (u == goal) {
            result.status = Status::Found;
            result.cost = g[goal];
            for (int v = goal; v != -1; v = parent[v]) result.nodes.push_back(v);
            std::reverse(result.nodes.begin(), result.nodes.end());
            return finish();
        }
        ++result.expanded;
        for (const Edge& edge : graph[u]) {
            const double candidate = g[u] + edge.cost;
            if (candidate < g[edge.to]) {
                g[edge.to] = candidate;
                parent[edge.to] = u;
                open.push({ candidate + h[edge.to], candidate, edge.to });
            }
        }
    }
    result.status = Status::NoPath;
    return finish();
}
}