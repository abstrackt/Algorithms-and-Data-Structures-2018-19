#include <iostream>
#include <set>
#include <functional>

using namespace std;

int n_hints;
int n_sessions;
int max_n;
const int n = 1 << 17;
const int k_tree_size = (1 << 18) + 10;

set<pair<int, int>, less<>> s_session[n];
set<pair<int, int>, greater<>> l_session[n];
pair<int, int> max_session_tree[k_tree_size];
pair<int, int> min_session_tree[k_tree_size];

pair<int, int> emptyseg = make_pair(0, 0);

pair<int, int> shortest_pair(pair<int, int> p1, pair<int, int> p2) {
    if (p1 == emptyseg) return p2;
    if (p2 == emptyseg) return p1;
    if (p1.second < p2.second) return p1;
    else return p2;
}

pair<int, int> longest_pair(pair<int, int> p1, pair<int, int> p2) {
    if (p1 == emptyseg) return p2;
    if (p2 == emptyseg) return p1;
    if (p1.second > p2.second) return p1;
    else return p2;
}

void insert_short(int x, pair<int, int> added) {
    int v = n + x - 1;
    if (min_session_tree[v].second > added.second || min_session_tree[v] == emptyseg) min_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (min_session_tree[v].second > added.second || min_session_tree[v] == emptyseg) {
            min_session_tree[v] = added;
        }
    }
}

void update_short(int x, pair<int, int> added) {
    int v = n + x - 1;
    min_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (2 * v < k_tree_size && min_session_tree[2 * v] == added) {
            added = shortest_pair(added, min_session_tree[2 * v + 1]);
            min_session_tree[v] = added;
        } else if (2 * v < k_tree_size && min_session_tree[2 * v + 1] == added) {
            added = shortest_pair(added, min_session_tree[2 * v]);
            min_session_tree[v] = added;
        }
    }
}

pair<int, int> query_short(long long a, long long b) {
    if (a > n) a = n;
    if (b > n) b = n;
    long long va = n + a - 1, vb = n + b - 1;
    pair<int, int> res = min_session_tree[va];
    if (va != vb) res = shortest_pair(res, min_session_tree[vb]);
    while (va / 2 != vb / 2) {
        if (va % 2 == 0) res = shortest_pair(res, min_session_tree[va + 1]);
        if (vb % 2 == 1) res = shortest_pair(res, min_session_tree[vb - 1]);
        va /= 2;
        vb /= 2;
    }
    return res;
}

void insert_long(int x, pair<int, int> added) {
    int v = n + x - 1;
    if (max_session_tree[v].second < added.second || max_session_tree[v] == emptyseg) max_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (max_session_tree[v].second < added.second || max_session_tree[v] == emptyseg) {
            max_session_tree[v] = added;
        }
    }
}

void update_long(int x, pair<int, int> added) {
    int v = n + x - 1;
    max_session_tree[v] = added;
    while (v != 1) {
        v /= 2;
        if (max_session_tree[2 * v] == added) {
            added = longest_pair(added, max_session_tree[2 * v + 1]);
            max_session_tree[v] = added;
        } else if (max_session_tree[2 * v + 1] == added) {
            added = longest_pair(added, max_session_tree[2 * v]);
            max_session_tree[v] = added;
        }
    }
}

pair<int, int> query_long(long long a, long long b) {
    if (a > n) a = n;
    if (b > n) b = n;
    long long va = n + a - 1, vb = n + b - 1;
    pair<int, int> res = max_session_tree[va];
    if (va != vb) res = longest_pair(res, max_session_tree[vb]);
    while (va / 2 != vb / 2) {
        if (va % 2 == 0) res = longest_pair(res, max_session_tree[va + 1]);
        if (vb % 2 == 1) res = longest_pair(res, max_session_tree[vb - 1]);
        va /= 2;
        vb /= 2;
    }
    return res;
}

void remove_and_update(int start, int end, int id) {
    l_session[start].erase(make_pair(end, id));
    s_session[start].erase(make_pair(end, id));
    if (!l_session[start].empty()) {
        update_long(start, make_pair(start, (*l_session[start].begin()).first));
    } else {
        update_long(start, emptyseg);
    }
    if (!s_session[start].empty()) {
        update_short(start, make_pair(start, (*s_session[start].begin()).first));
    } else {
        update_short(start, emptyseg);
    }
}

int in(int a, int b) {
    pair<int, int> result = query_short(a, b);
    if (result == emptyseg) return -1;
    if (result.second <= b) {
        int start = result.first;
        int end = result.second;
        auto first_session = s_session[start].begin();
        int found_id = (*first_session).second;
        remove_and_update(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

int none(int a, int b) {
    pair<int, int> l_result, r_result, result;
    l_result = query_short(1, a - 1);
    r_result = query_short(b + 1, n);
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
        remove_and_update(start, end, found_id);
        return found_id;
    }
}

int over(int a, int b) {
    pair<int, int> result = query_long(1, a);
    if (result == emptyseg) return -1;
    if (result.second >= b) {
        int start = result.first;
        int end = result.second;
        auto first_session = l_session[start].begin();
        int found_id = (*first_session).second;
        remove_and_update(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

int some(int a, int b) {
    pair<int, int> result;
    result = query_long(1, b);
    if (result.second >= a) {
        int start = result.first;
        int end = result.second;
        auto first_session = l_session[start].begin();
        int found_id = (*first_session).second;
        remove_and_update(start, end, found_id);
        return found_id;
    } else {
        return -1;
    }
}

void get_sessions() {
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

void build_tree() {
    for (int j = 0; j < k_tree_size; j++) {
        max_session_tree[j] = emptyseg;
        min_session_tree[j] = emptyseg;
    }

    for (int i = 0; i <= max_n; i++) {
        if (!s_session[i].empty()) {
            insert_short(i, make_pair(i, (*s_session[i].begin()).first));
        }
        if (!l_session[i].empty()) {
            insert_long(i, make_pair(i, (*l_session[i].begin()).first));
        }
    }
}

void get_hints() {
    cin >> n_hints;
    string answer;
    for (int i = 0; i < n_hints; i++) {
        std::string command;
        int start, end;
        int res = -1;

        cin >> command >> start >> end;

        if (command == "in") res = in(start, end);
        if (command == "over") res = over(start, end);
        if (command == "some") res = some(start, end);
        if (command == "none") res = none(start, end);

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
    get_sessions();
    build_tree();
    get_hints();
}
