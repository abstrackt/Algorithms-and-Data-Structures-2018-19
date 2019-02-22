#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> graph[500001];
set<pair<int, int>> degrees;
int n, m;
int max_deg;


int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    cin >> n >> m;

    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        graph[a].emplace_back(b);
        graph[b].emplace_back(a);
    }

    for (int j = 1; j <= n; j++) {
        degrees.insert(make_pair(graph[j].size(), j));
    }

    while (!degrees.empty()) {
        int min_deg = degrees.begin()->first;

        if (degrees.begin()->first > max_deg) max_deg = degrees.begin()->first;

        for (auto it = degrees.begin(); it != degrees.end();) {
            if (it->first == min_deg) {
                for (auto conn : graph[it->second]) {
                    graph[conn].erase(find(graph[conn].begin(), graph[conn].end(), it->second));
                    degrees.erase(make_pair(graph[conn].size() + 1, conn));
                    degrees.insert(make_pair(graph[conn].size(), conn));
                }
                degrees.erase(it++);
            } else break;
        }
    }

    cout << max_deg;
}