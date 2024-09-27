#include <bits/stdc++.h>

using namespace std;

const int INF = 0x3f3f3f3f;

struct Person {
  vector<int> preferences;
  vector<int> removed;
  vector<int> rank;
  int partner = -1;
  int nextProposal = 0;
  bool is_free() const {
    return partner == -1;
  }
};

struct Rotation {
  int idx;

  vector<pair<int, int>> pairs{};
  int weight;
  int level = -1;
};

void galeShapley(vector<Person>& men, vector<Person>& women) {
  int n = men.size();
  deque<int> freeMen;
 
  for (int i = 0; i < n; i++) {
    freeMen.push_back(i);
  }

  while (!freeMen.empty()) {
    int man = freeMen.front(); freeMen.pop_front();
   
    int woman = men[man].preferences[men[man].nextProposal];

    men[man].nextProposal++;

    if (women[woman].partner == -1) {
      women[woman].partner = man;
      men[man].partner = woman;
    } else {
      int currentPartner = women[woman].partner;
      if (women[woman].rank[man] < women[woman].rank[currentPartner]) {
        women[woman].partner = man;
        men[man].partner = woman;

        men[currentPartner].partner = -1;
        freeMen.push_front(currentPartner);
      } else {
        freeMen.push_front(man);
      }
    }
  }
}

void generateShortlists(vector<Person>& men, vector<Person>& women) {
  int n = men.size();

  for (int i = 0; i < n; ++i) {
    int currentWoman = men[i].partner;
    for (int j = 0; j < men[i].rank[currentWoman]; ++j) {
      int woman = men[i].preferences[j];

      men[i].removed[j] = true;
      women[woman].removed[ women[woman].rank[i] ] = true;
    }

    int currentMan = women[i].partner;
    for (int j = women[i].rank[currentMan] + 1; j < n; ++j) {
      int man = women[i].preferences[j];

      women[i].removed[j] = true;
      men[man].removed[ men[man].rank[i] ] = true;
    }
  }
}

void dfs(int node, const vector<vector<int>>& G, vector<bool>& visited, vector<int>& currentPath, vector<vector<int>>& cycles) {
  visited[node] = true;
  currentPath.push_back(node);

  for (int neighbor : G[node]) {
    if (!visited[neighbor]) {
      dfs(neighbor, G, visited, currentPath, cycles);
    } else {
      auto it = find(currentPath.begin(), currentPath.end(), neighbor);
      if (it != currentPath.end()) {
        vector<int> cycle(it, currentPath.end());
        cycles.push_back(cycle);
      }
    }
  }

  currentPath.pop_back();
}

void calculateWeight(Rotation &r, vector<Person>& men, vector<Person>& women) {
  int n = (int)r.pairs.size();
  r.weight = 0;
  for (int i = 0; i < n; ++i) {
    int prev_i = (i == 0 ? n - 1 : i - 1);
    int next_i = (i == n - 1 ? 0 : i + 1);

    auto [mi_1, wi_1] = r.pairs[prev_i];
    auto [mi, wi] = r.pairs[i];
    auto [mip1, wip1] = r.pairs[next_i];

    r.weight += men[mi].rank[wi];
    r.weight -= men[mi].rank[wip1];

    r.weight += women[wi].rank[mi];
    r.weight -= women[wi].rank[mi_1];
  }
}

void eliminateRotation(Rotation& r, vector<Person>& men, vector<Person>& women, vector<vector<int>> &removedBy) {
  int N = men.size();
  int n = (int)r.pairs.size();

  for (int i = 0; i < n; ++i) {
    int prev_i = (i == 0 ? n - 1 : i - 1);

    auto [mi_1, wi_1] = r.pairs[prev_i];
    auto [mi, wi] = r.pairs[i];

    for (int j = women[wi].rank[mi_1] + 1; j < N; ++j) {
      int man = women[wi].preferences[j];

      if (!women[wi].removed[j]) removedBy[man][wi] = r.idx;

      women[wi].removed[j] = true;
      men[man].removed[ men[man].rank[wi] ] = true;
    }
  }

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (men[i].removed[j] == false) {
        men[i].partner = men[i].preferences[j];
        women[ men[i].partner ].partner = i;
        break;
      }
    }
  }

  calculateWeight(r, men, women);
}

vector<Rotation> findRotations(const vector<Person>& men, const vector<Person>& women, vector<vector<int>> &removedBy) {
  vector<Person> Men = men, Women = women;

  int n = Men.size();

  vector<Rotation> rotations;

  int level = 0;
  while (true) {
    vector<vector<int>> G(n, vector<int>());

    for (int i = 0; i < n; ++i) {
      int cnt = 0;
      for (int j = 0; j < n; ++j) {
        if (Men[i].removed[j] == false) {
          ++cnt;
        }

        if (cnt == 2) {
          int scd = Men[i].preferences[j];
          int mate = Women[ scd ].partner;

          G[i].push_back(mate);

          break;
        }
      }
    }

    // find cycles
    vector<bool> visited(n, false);
    vector<vector<int>> cycles;

    for (int i = 0; i < n; ++i) {
      if (!visited[i]) {
        vector<int> currentPath;
        dfs(i, G, visited, currentPath, cycles);
      }
    }

    vector<Rotation> curr_rotations;
    for (auto cycle : cycles) {
      Rotation r;
      // cout << "INICIO ROTACAO\n";
      for (auto p : cycle) {
        r.pairs.push_back({ p, Men[p].partner });
        // cout << p+1 << ' ' << Men[p].partner + 1 << '\n';
      }
      // cout << "----\n";
      curr_rotations.push_back(r);
    }

    if (curr_rotations.size() == 0) break;

    for (auto r : curr_rotations) {
      r.level = level;
      r.idx = rotations.size();
      rotations.push_back(r);

      eliminateRotation(r, Men, Women, removedBy);
    }

    ++level;
  }

  return rotations;
}

vector<vector<int>> constructGraph(vector<Rotation>& rotations, vector<Person>& men, vector<Person>& women, vector<vector<int>> &removedBy) {
  int n = rotations.size();
  int N = women.size();

  vector<vector<int>> G(n, vector<int>()); 

  vector<vector<int>> isInRotation(N, vector<int>(N, -1));

  for (auto r : rotations) {
    for (auto [m, w] : r.pairs) {
      isInRotation[m][w] = r.idx;
    }
  }

  for (int m = 0; m < N; ++m) {
    for (int w = 0; w < N; ++w) {
      int a = isInRotation[m][w];

      if (a != -1) {
        int rnk = men[m].rank[w];
        ++rnk;
        while (rnk < N and men[m].removed[rnk] == true) {
          ++rnk;
        }

        if (rnk < N) {
          int w_linha = men[m].preferences[rnk];

          int b = isInRotation[m][w_linha];
          if (b != -1) {

            G[a].push_back(b);
          }
        }
      } else {
        a = removedBy[m][w];

        if (a == -1) continue;

        int rnk = men[m].rank[w];
        --rnk;

        while (rnk >= 0 and men[m].removed[rnk] == true) {
          --rnk;
        }

        if (rnk >= 0) {
          int w_linha = men[m].preferences[rnk];

          int b = isInRotation[m][w_linha];
          if (b != -1) {

            G[a].push_back(b);
          }
        }
      }
    }
  }

  return G;
}

void reverseBFS(const vector<vector<int>>& G, const vector<vector<int>>& capacity, vector<bool>& reachableFromSink, int t) {
  queue<int> q;
  q.push(t);
  reachableFromSink[t] = true;

  while (!q.empty()) {
    int cur = q.front();
    q.pop();

    for (int prev : G[cur]) {
      if (!reachableFromSink[prev] && capacity[prev][cur] > 0) {
        reachableFromSink[prev] = true;
        q.push(prev);
      }
    }
  }
}

vector<int> findPositiveNodesUncutByMinCut(const vector<vector<int>>& G, const vector<vector<int>>& capacity, const vector<Rotation>& rotations, int t) {
  int n = G.size();
  vector<bool> reachableFromSink(n, false);

  reverseBFS(G, capacity, reachableFromSink, t);

  vector<int> positiveNodesUncut;
  for (int i = 0; i < n - 2; ++i) {
    if (reachableFromSink[i] && rotations[i].weight > 0) {
      positiveNodesUncut.push_back(rotations[i].idx);
    }
  }

  return positiveNodesUncut;
}

void bfsInResidualGraph(const vector<vector<int>>& G, const vector<vector<int>>& capacity, vector<bool>& visited, int s) {
  queue<int> q;
  q.push(s);
  visited[s] = true;

  while (!q.empty()) {
    int cur = q.front();
    q.pop();

    for (int next : G[cur]) {
      if (!visited[next] && capacity[cur][next] > 0) {
        visited[next] = true;
        q.push(next);
      }
    }
  }
}

vector<int> getMinCut(const vector<vector<int>>& G, const vector<vector<int>>& capacity, int s) {
  int n = G.size();
  vector<bool> visited(n, false);

  bfsInResidualGraph(G, capacity, visited, s);

  vector<int> minCutNodes;
  for (int i = 0; i < n; ++i) {
    if (visited[i]) {
      minCutNodes.push_back(i);
    }
  }

  return minCutNodes;
}

int bfs(vector<vector<int>>& graph, int s, int t, vector<int>& parent, vector<vector<int>> capacity) {
  fill(parent.begin(), parent.end(), -1);
  parent[s] = -2;
  queue<pair<int, int>> q;
  q.push({s, INF});

  while (!q.empty()) {
    int cur = q.front().first;
    int flow = q.front().second;
    q.pop();

    for (int next : graph[cur]) {
      if (parent[next] == -1 && capacity[cur][next]) {
        parent[next] = cur;
        int new_flow = min(flow, capacity[cur][next]);
        if (next == t)
          return new_flow;
        q.push({next, new_flow});
      }
    }
  }

  return 0;
}

int fordFulkerson(vector<vector<int>> &graph, vector<vector<int>> &capacity, int s, int t) {
  int n = graph.size();

  int flow = 0;
  vector<int> parent(n);
  int new_flow;

  while (true) {
    new_flow = bfs(graph, s, t, parent, capacity);
    if (new_flow == 0) break;

    flow += new_flow;
    int cur = t;
    while (cur != s) {
      int prev = parent[cur];
      capacity[prev][cur] -= new_flow;
      capacity[cur][prev] += new_flow;
      cur = prev;
    }
  }

  return flow;
}

vector<int> findMaxWeightClosedSubset(vector<Rotation> &rotations, vector<vector<int>>& graph) {
  int n = rotations.size();

  vector<vector<int>> capacity(n + 2, vector<int>(n + 2, 0));
  vector<vector<int>> G(n + 2, vector<int>());

  // 0 to n-1   -> rotation nodes
  // n          -> source node
  // n + 1      -> sink node

  for (int i = 0; i < n; ++i) {
    for (int to : graph[i]) {
      // i -> j
      G[i].push_back(to);
      G[to].push_back(i);
      capacity[i][to] += INF;
    }

    if (rotations[i].weight < 0) {
      // n -> i
      G[n].push_back(i);
      G[i].push_back(n);
      capacity[n][i] += -(rotations[i].weight);
    } else if (rotations[i].weight > 0) {
      // i -> n+1
      G[i].push_back(n+1);
      G[n+1].push_back(i);
      capacity[i][n+1] += +(rotations[i].weight);
    }
  }

  fordFulkerson(G, capacity, n, n+1);

  // cout << "maxFlow(" << maxFlow << ")" << '\n';

  vector<int> minCutNodes = getMinCut(G, capacity, n);
  // cout << "Min-Cut\n";
  // for (int node : minCutNodes) if (0 <= node and node < n) {
  //   cout << rotations[node].idx << ' ' << rotations[node].level << ' ' << rotations[node].weight << '\n';
  // }

  vector<int> positiveNodes = findPositiveNodesUncutByMinCut(G, capacity, rotations, n + 1);
  // cout << "Positive Nodes whose edges into t are uncut\n";
  // for (int node : positiveNodes) if (0 <= node and node < n) {
  //   cout << rotations[node].idx << ' ' << rotations[node].level << ' ' << rotations[node].weight << '\n';
  // }

  return positiveNodes;
}

int calculateMarriageCost(const vector<Person>& men, const vector<Person>& women) {
  int n = men.size();
  int cost = 0;

  for (int m = 0; m < n; ++m) {
    int woman = men[m].partner;
    cost += men[m].rank[woman];
    cost += women[woman].rank[m];
  }

  return cost;
}

int main() {
  int n;
  cin >> n;

  vector<Person> men(n, Person());
  vector<Person> women(n, Person());

  vector<vector<int>> removedBy(n, vector<int>(n, -1));

  for (int i = 0; i < n; ++i) {
    men[i].preferences.resize(n);
    men[i].removed.assign(n, false);
    men[i].rank.resize(n);
    for (int j = 0; j < n; ++j) {
      auto &pref = men[i].preferences[j];
      cin >> pref; pref--;
      men[i].rank[pref] = j;
    }
  }

  for (int i = 0; i < n; ++i) {
    women[i].preferences.resize(n);
    women[i].removed.assign(n, false);
    women[i].rank.resize(n);
    for (int j = 0; j < n; ++j) {
      auto &pref = women[i].preferences[j];
      cin >> pref; pref--;
      women[i].rank[pref] = j;
    }
  }

  galeShapley(men, women);

  generateShortlists(men, women);

  vector<Rotation> rotations = findRotations(men, women, removedBy);

  vector<vector<int>> rotation_graph = constructGraph(rotations, men, women, removedBy);

  vector<vector<int>> reversed_graph(rotation_graph.size(), vector<int>());

  for (auto &r : rotations) {
    calculateWeight(r, men, women);
  }

  for (int i = 0; i < (int)rotation_graph.size(); ++i) {
    for (int to : rotation_graph[i]) {
      reversed_graph[to].push_back(i);
    }
  }

  vector<int> nodes = findMaxWeightClosedSubset(rotations, rotation_graph);

  vector<int> rot;

  queue<int> q;
  for (int x : nodes) {
    q.push(x);
  }

  while (!q.empty()) {
    int u = q.front(); q.pop();

    rot.push_back(u);
    for (int to : reversed_graph[u]) {
      q.push(to);
    }
  }

  for (int r : rot) {
    eliminateRotation(rotations[r], men, women, removedBy);
  }

  for (auto &r : rotations) {
    calculateWeight(r, men, women);
  }

  for (int i = 0; i < n; ++i) {
    cout << "(" << i + 1 << ", " << men[i].partner + 1 << ")\n";
  }

  int optimal_cost = calculateMarriageCost(men, women);
  cout << "MIN_COST: " << optimal_cost << endl;

  return 0;
}
