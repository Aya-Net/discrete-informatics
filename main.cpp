#include <iostream>
#include <queue>
#include <string>
#include <random>
#include <chrono>

#include "model/sufficient_bv.h"

std::string random_gen(int x) {
    std::string result;
    result.reserve(x);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (int i = 0; i < x; ++i) {
        result += std::to_string(dis(gen));
    }

    return result;
}
int sl[10000010];
void select(const std::string& s) {
    int cnt = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '1') {
            cnt++;
            sl[cnt] = i;
        }
    }
}
int rk[10000010];
void rank(const std::string& s) {
    for (int i = 0; i < s.length(); i++) {
        if (i > 0) rk[i] = rk[i - 1];
        if (s[i] == '1') {
            rk[i]++;
        }
    }
}

int main() {
    std::string s = random_gen(10000000);
    sufficient_bv sbv(s);
    rank(s);
    select(s);
    sl[rk[s.length() - 1] + 1] = -1;
    // Timing rank function
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < sbv.size(); i++) {
        if (sbv.rank(i) != rk[i]) {
            std::cout << "rank error " << i << ": " << rk[i] << ' ' << sbv.rank(i) << std::endl;
            return 0;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken by sbv.rank: " << duration.count() << " seconds" << std::endl;


    // Timing select function
    start = std::chrono::high_resolution_clock::now();
    for (int i = 1; i <= sbv.rank() + 1; i++) {
        if (sbv.select(i) != sl[i]) {
            std::cout << "select error " << i << ": " << sl[i] <<' '<< sbv.select(i) << std::endl;
            return 0;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Time taken by sbv.select: " << duration.count() << " seconds" << std::endl;

    return 0;
}