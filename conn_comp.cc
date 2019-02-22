/* A program to compute the number of connected components in a graph.
 *
 * Each vertex is assigned with an identifier (a positive integer between
 * 1 and 10e9. Two vertices are connected if and only if their identifiers
 * produce a non-zero value when compared with bitwise AND (&) operator.
 */

#include <iostream>
#include <vector>

int n_vertices;
std::vector<int> components;

/* Function which clusters vertices into groups which will later be merged
 * to produce the final result. Due to the identifier's upper bound no more
 * than 29 elements exist within the components vector at any given
 * moment. A group is represented by a single integer, vertex x is added
 * into the group if it produces a non-zero value when compared with bitwise
 * OR (|) operator. The value of the group's identifier after addition of x is:
 *
 * old_identifier | x_identifier
 *
 * At this stage, duplicates may and possibly will occur.
 */
void cluster() {
    for (int i = 0; i < n_vertices; i++) {
        bool matched = false;
        int current;
        std::cin >> current;

        for (int &comp : components) {
            int res = current & comp;
            if (res != 0) {
                comp = comp | current;
                matched = true;
            }
        }

        if (!matched) components.emplace_back(current);
    }
}

/* To remove duplicates we merge the clustered components. Since the vector
 * contains no more than 29 elements, the merging process is done in O(1) time
 * (29^2 comparisons made in the worst case).
 */
void reduce() {
    for (auto it1 = components.begin(); it1 != components.end(); it1++) {
        // Erasing merged components while iterating through the vector.
        for (auto it2 = next(it1); it2 != components.end();) {
            int res = *it1 & *it2;
            if (res != 0) {
                *it1 = *it1 | *it2;
                it2 = components.erase(it2);
            } else {
                it2++;
            }
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    std::cin >> n_vertices;

    cluster();
    reduce();

    std::cout << components.size();
}