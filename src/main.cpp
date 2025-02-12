#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

// A "segment" is a connected piece of a single metro line. A passenger can
// ride between any two stations of the same segment without changing lines,
// so the problem reduces to shortest paths between segments, where two
// segments are adjacent if they share a station.

struct Edge {
  int line, a, b;
  bool operator<(const Edge& other) const { return line < other.line; }
};

static std::vector<int> parent;

static int find(int x) {
  while (parent[x] != x) x = parent[x] = parent[parent[x]];
  return x;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int num_stations, num_connections, num_lines;
  std::cin >> num_stations >> num_connections >> num_lines;

  std::vector<Edge> edges(num_connections);
  for (Edge& e : edges) {
    std::cin >> e.a >> e.b >> e.line;
    --e.a; --e.b; // Convert to 0-based indexing.
  }
  std::sort(edges.begin(), edges.end());

  // Split every line into segments with a union-find over its stations.
  parent.resize(num_stations);
  std::iota(parent.begin(), parent.end(), 0);
  std::vector<int> segment_of_root(num_stations, -1);
  std::vector<std::vector<int>> station_segments(num_stations);
  std::vector<std::vector<int>> segment_stations;

  for (size_t start = 0; start < edges.size();) {
    size_t end = start;
    while (end < edges.size() && edges[end].line == edges[start].line) ++end;

    for (size_t i = start; i < end; ++i)
      parent[find(edges[i].a)] = find(edges[i].b);
    for (size_t i = start; i < end; ++i) {
      for (int station : {edges[i].a, edges[i].b}) {
        int root = find(station);
        if (segment_of_root[root] == -1) {
          segment_of_root[root] = segment_stations.size();
          segment_stations.emplace_back();
        }
        int segment = segment_of_root[root];
        segment_stations[segment].push_back(station);
        station_segments[station].push_back(segment);
      }
    }
    // Reset only what this line touched.
    for (size_t i = start; i < end; ++i) {
      for (int station : {edges[i].a, edges[i].b}) {
        segment_of_root[station] = -1;
        parent[station] = station;
      }
    }
    start = end;
  }

  const int num_segments = segment_stations.size();
  std::vector<bool> has_single_segment_station(num_segments, false);
  for (int s = 0; s < num_stations; ++s) {
    std::vector<int>& segs = station_segments[s];
    if (segs.empty()) { // A station with no connections can't be reached.
      std::cout << -1 << '\n';
      return 0;
    }
    std::sort(segs.begin(), segs.end());
    segs.erase(std::unique(segs.begin(), segs.end()), segs.end());
    if (segs.size() == 1) has_single_segment_station[segs[0]] = true;
  }
  for (std::vector<int>& stations : segment_stations) {
    std::sort(stations.begin(), stations.end());
    stations.erase(std::unique(stations.begin(), stations.end()), stations.end());
  }

  // Starting points for the searches. A station whose segments are a superset
  // of another station's segments is never further from anything, so it can be
  // skipped. In particular, any station sharing a segment with a station that
  // only has that segment is skipped, and identical sets are searched once.
  std::vector<std::vector<int>> sources;
  for (int seg = 0; seg < num_segments; ++seg)
    if (has_single_segment_station[seg]) sources.push_back({seg});
  for (int s = 0; s < num_stations; ++s) {
    const std::vector<int>& segs = station_segments[s];
    if (segs.size() < 2) continue;
    bool dominated = false;
    for (int seg : segs)
      if (has_single_segment_station[seg]) { dominated = true; break; }
    if (!dominated) sources.push_back(segs);
  }
  std::sort(sources.begin(), sources.end());
  sources.erase(std::unique(sources.begin(), sources.end()), sources.end());

  // Breadth-first search over segments from each source. A station's number
  // of line changes is the distance of the first segment that reaches it.
  std::vector<int> segment_mark(num_segments, -1), station_mark(num_stations, -1);
  std::vector<int> changes(num_segments), queue(num_segments);
  int connectivity_index = 0;

  for (int run = 0; run < (int) sources.size(); ++run) {
    int head = 0, tail = 0, reached = 0;
    for (int seg : sources[run]) {
      segment_mark[seg] = run;
      changes[seg] = 0;
      queue[tail++] = seg;
    }
    while (head < tail) {
      int seg = queue[head++];
      for (int station : segment_stations[seg]) {
        if (station_mark[station] == run) continue;
        station_mark[station] = run;
        ++reached;
        connectivity_index = std::max(connectivity_index, changes[seg]);
        for (int next : station_segments[station]) {
          if (segment_mark[next] == run) continue;
          segment_mark[next] = run;
          changes[next] = changes[seg] + 1;
          queue[tail++] = next;
        }
      }
    }
    if (reached < num_stations) { // Some station is unreachable.
      std::cout << -1 << '\n';
      return 0;
    }
    if (connectivity_index == num_segments - 1) break; // Can't get any higher.
  }

  std::cout << connectivity_index << '\n';
  return 0;
}
