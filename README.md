# Metro Connectivity

A program that evaluates how well connected a metro network is by calculating the minimum number of line changes needed to travel between any two stations.

Built by [Inês Costa](https://github.com/inesiscosta) and [Miguel Raposo](https://github.com/migpovrap) for the Analysis and Synthesis of Algorithms course at Instituto Superior Técnico (2024/2025).

## The Problem

The network is an undirected multigraph. Stations are vertices, and each edge is a section of track on one metro line. Two stations can be joined by several edges if more than one line runs between them.

How well connected the network is, is measured by its connectivity index (`mc`):

```
mc = max { ML(u, v) | u, v ∈ Metro }
```

where `ML(u, v)` is the minimum number of line changes needed to travel between stations `u` and `v`.

### Input

- A line with three integers: the number of stations `n` (n ≥ 2), the number of connections `m` (m ≥ 0) and the number of lines `l` (l ≥ 0).
- `m` lines, each with three integers `x y l`, meaning station `x` is directly connected to station `y` on line `l`.

Stations and lines are numbered from 1.

### Output

A single integer: the connectivity index of the network, with two special cases:

- **0** if no line changes are ever needed.
- **-1** if at least two stations can't reach each other at all.

### Example

```
Input        Output
7 8 3        1
3 2 1
2 7 1
7 5 1
2 6 2
6 4 2
4 1 2
4 1 3
1 5 3
```

## How it Works

Changes only happen between lines, so the program searches over lines instead of stations.

- **Line segments.** A union-find splits each line into connected segments. You can ride anywhere on a segment without changing, and a line that appears in two disconnected places becomes two segments.
- **Segment graph.** Two segments are adjacent when they share a station, so one step is one line change. It's stored as station ↔ segment lists rather than a clique per interchange station.
- **Breadth-first search.** A BFS from a station's segments reaches each station first through its closest segment, so that segment's depth is the station's `ML` value.
- **Pruning.** A station whose segments include all of another station's segments is never further from anything, so it's skipped, and stations with identical segments are searched once. The program returns `-1` as soon as a station is isolated or a search misses a station, and stops once the index reaches its upper bound (segments − 1).

## Complexity

**Time complexity:** `O(m log m + n(n + m))`
**Space complexity:** `O(n + m)`

## Usage

Requires a C++11 compiler and Make.

```bash
cd src
make
./main < ../tests/test01.in
```

Or compile it directly with the course's flags:

```bash
g++ -std=c++11 -O3 -Wall -o main src/main.cpp -lm
./main < tests/test01.in
```
