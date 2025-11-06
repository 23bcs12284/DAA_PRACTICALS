// User Function Template
class Solution
{
public:
  vector<int> dijkstra(int V, vector<vector<int>> &edges, int src)
  {
    // Code here
    vector<vector<pair<int, int>>> adj(V);
    int e = edges.size();
    for (int i = 0; i < e; i++)
    {
      int u = edges[i][0];
      int v = edges[i][1];
      int w = edges[i][2];

      adj[u].push_back({v, w});
      adj[v].push_back({u, w});
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> dist(V, INT_MAX);
    pq.push({0, src});
    dist[src] = 0;
    while (!pq.empty())
    {
      int u = pq.top().second;
      pq.pop();
      for (auto x : adj[u])
      {
        int v = x.first;
        int weight = x.second;

        if (dist[v] > dist[u] + weight)
        {
          dist[v] = dist[u] + weight;
          pq.push({dist[v], v});
        }
      }
    }
    return dist;
  }
};