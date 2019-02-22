#include <iostream>
#include <set>
#include <functional>

using namespace std;

int n_hints;
int n_sessions;
int max_n;
const int n = 1 << 17;
const int kTreeSize = (1 << 18) + 10;

set<pair<int, int>, less<>> s_session[n];
set<pair<int, int>, greater<>> l_session[n];
pair<int, int> max_session_tree[kTreeSize];
pair<int, int> min_session_tree[kTreeSize];

pair<int, int> emptyseg = make_pair(0, 0);

pair<int, int> ShortestPair(pair<int, int> p1, pair<int, int> p2) {
    if (p1 == emptyseg) return p2;
    if (p2 == emptyseg) return p1;
    if (p1.second < p2.second) return p1;
    else return p2;
}

pair<int, int> LongestPair(pair<int, int> p1, pair<int, int> p2) {
    if (p1 == emptyseg) return p2;
    if (p2 == emptyseg) return p1;
    if (p1.second > p2.second) return p1;
    else return p2;
}

void InsertShort(int x, pair<int, int> added) {
    int v = n + x - 1;
    if (min_session_tree[v].second > added.second || min_session_tree[v] == emptyseg) min_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (min_session_tree[v].second > added.second || min_session_tree[v] == emptyseg) {
            min_session_tree[v] = added;
        }
    }
}

void UpdateShort(int x, pair<int, int> added) {
    int v = n + x - 1;
    min_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (2 * v < kTreeSize && min_session_tree[2 * v] == added) {
            added = ShortestPair(added, min_session_tree[2 * v + 1]);
            min_session_tree[v] = added;
        } else if (2 * v < kTreeSize && min_session_tree[2 * v + 1] == added) {
            added = ShortestPair(added, min_session_tree[2 * v]);
            min_session_tree[v] = added;
        }
    }
}

pair<int, int> QueryShort(long long a, long long b) {
    if (a > n) a = n;
    if (b > n) b = n;
    long long va = n + a - 1, vb = n + b - 1;
    pair<int, int> res = min_session_tree[va];
    if (va != vb) res = ShortestPair(res, min_session_tree[vb]);
    while (va / 2 != vb / 2) {
        if (va % 2 == 0) res = ShortestPair(res, min_session_tree[va + 1]);
        if (vb % 2 == 1) res = ShortestPair(res, min_session_tree[vb - 1]);
        va /= 2;
        vb /= 2;
    }
    return res;
}

void InsertLong(int x, pair<int, int> added) {
    int v = n + x - 1;
    if (max_session_tree[v].second < added.second || max_session_tree[v] == emptyseg) max_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (max_session_tree[v].second < added.second || max_session_tree[v] == emptyseg) {
            max_session_tree[v] = added;
        }
    }
}

void UpdateLong(int x, pair<int, int> added) {
    int v = n + x - 1;
    max_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (max_session_tree[2 * v] == added) {
            added = LongestPair(added, max_session_tree[2 * v + 1]);
            max_session_tree[v] = added;
        } else if (max_session_tree[2 * v + 1] == added) {
            added = LongestPair(added, max_session_tree[2 * v]);
            max_session_tree[v] = added;
        }
    }
}

pair<int, int> QueryLong(long long a, long long b) {
    if (a > n) a = n;
    if (b > n) b = n;
    long long va = n + a - 1, vb = n + b - 1;
    pair<int, int> res = max_session_tree[va];
    if (va != vb) res = LongestPair(res, max_session_tree[vb]);
    while (va / 2 != vb / 2) {
        if (va % 2 == 0) res = LongestPair(res, max_session_tree[va + 1]);
        if (vb % 2 == 1) res = LongestPair(res, max_session_tree[vb - 1]);
        va /= 2;
        vb /= 2;
    }
    return res;
}

void RemoveAndUpdate(int start, int end, int id) {
    l_session[start].erase(make_pair(end, id));
    s_session[start].erase(make_pair(end, id));
    if (!l_session[start].empty()) {
        UpdateLong(start, make_pair(start, (*l_session[start].begin()).first));
    } else {
        UpdateLong(start, emptyseg);
    }
    if (!s_session[start].empty()) {
        UpdateShort(start, make_pair(start, (*s_session[start].begin()).first));
    } else {
        UpdateShort(start, emptyseg);
    }
}

int In(int a, int b) {
    pair<int, int> result = QueryShort(a, b);
    if (result == emptyseg) return -1;
    if (result.second <= b) {
        int start = result.first;
        int end = result.second;
        auto first_session = s_session[start].begin();
        int found_id = (*first_session).second;
        RemoveAndUpdate(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

int None(int a, int b) {
    pair<int, int> l_result, r_result, result;
    l_result = QueryShort(1, a - 1);
    r_result = QueryShort(b + 1, n);
    if (l_result.first < a && l_result.second < a && l_result != emptyseg) {
        result = l_result;
    } else {
        if (r_result.first > b && r_result.second > b && r_result != emptyseg) {
            result = r_result;
        } else {
            result = emptyseg;
        }
    }
    if (result == emptyseg) return -1;
    else {
        int start = result.first;
        int end = result.second;
        auto first_session = s_session[start].begin();
        int found_id = (*first_session).second;
        RemoveAndUpdate(start, end, found_id);
        return found_id;
    }
}

int Over(int a, int b) {
    pair<int, int> result = QueryLong(1, a);
    if (result == emptyseg) return -1;
    if (result.second >= b) {
        int start = result.first;
        int end = result.second;
        auto first_session = l_session[start].begin();
        int found_id = (*first_session).second;
        RemoveAndUpdate(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

int Some(int a, int b) {
    pair<int, int> result;
    result = QueryLong(1, b);
    if (result.second >= a) {
        int start = result.first;
        int end = result.second;
        auto first_session = l_session[start].begin();
        int found_id = (*first_session).second;
        RemoveAndUpdate(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

void GetSessions() {
    cin >> n_sessions;
    max_n = 0;
    for (int i = 0; i < n_sessions; i++) {
        int start, end;
        cin >> start >> end;
        s_session[start].insert(make_pair(end, i + 1));
        l_session[start].insert(make_pair(end, i + 1));
        max_n = max(max_n, end);
    }
}

void BuildTree() {
    for (int j = 0; j < kTreeSize; j++) {
        max_session_tree[j] = emptyseg;
        min_session_tree[j] = emptyseg;
    }

    for (int i = 0; i <= max_n; i++) {
        if (!s_session[i].empty()) {
            InsertShort(i, make_pair(i, (*s_session[i].begin()).first));
        }
        if (!l_session[i].empty()) {
            InsertLong(i, make_pair(i, (*l_session[i].begin()).first));
        }
    }
}

void GetHints() {
    cin >> n_hints;
    string answer;
    for (int i = 0; i < n_hints; i++) {
        std::string command;
        int start, end;
        int res = -1;

        cin >> command >> start >> end;

        if (command == "in") res = In(start, end);
        if (command == "over") res = Over(start, end);
        if (command == "some") res = Some(start, end);
        if (command == "none") res = None(start, end);

        answer.append(to_string(res));
        answer.append(" ");

    }
    cout << answer;
}

ostream &operator<<(ostream &os, const pair<int, int> &p) {
    os << "<" << p.first << ", " << p.second << ">";
    return os;
}

int main() {
    ios_base::sync_with_stdio(false);
    GetSessions();
    BuildTree();
    GetHints();
}