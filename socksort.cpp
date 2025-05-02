#include <vector>
#include <random>
#include <utility>

// I came up with this algorithm while trying to find socks in a laundry basket.
// My sock finding method is to pick two socks and search through the laundry for a match.
// The rest came from metaphors in my head that I don't remember now.
// I haven't touched sorting algorithms in years but this seemed like a fun
// challenge. I'm sure its not even remotely optimal or even original but it works and it's mine :)
// Stavros P.
void sock_sort(std::vector<int>& laundry) {
    std::vector<std::pair<int, int>> stack;
    stack.push_back({0, (int)laundry.size() - 1});

    std::mt19937 rng(std::random_device{}());

    while (!stack.empty()) {
        int lo = stack.back().first;
        int hi = stack.back().second;
        stack.pop_back();

        if (lo >= hi) continue;

        std::uniform_int_distribution<> dist(lo, hi);
        int sock_a = dist(rng);
        int sock_b = dist(rng);

        std::swap(laundry[lo], laundry[sock_a]);
        std::swap(laundry[hi], laundry[sock_b]);

        if (laundry[lo] > laundry[hi]) {
            std::swap(laundry[lo], laundry[hi]);
        }

        int L = laundry[lo];
        int R = laundry[hi];

        int left = lo + 1;
        int right = hi - 1;
        int pos = lo + 1;

        // split into left right and middle
        while (pos <= right) {
            if (laundry[pos] < L) {
                std::swap(laundry[left++], laundry[pos++]);
            } else if (laundry[pos] > R) {
                std::swap(laundry[pos], laundry[right--]);
            } else {
                pos++;
            }
        }

        left--;
        right++;

        // put pivots in correct positions
        std::swap(laundry[lo], laundry[left]);
        std::swap(laundry[hi], laundry[right]);


        stack.push_back({right + 1, hi});       // > r
        stack.push_back({left + 1, right - 1}); // [l, r
        stack.push_back({lo, left - 1});        // < l
    }
}
