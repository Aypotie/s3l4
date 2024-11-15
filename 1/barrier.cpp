#include <iostream>
#include <thread>
#include <barrier>
#include <vector>
#include <chrono>

#include "utils/utils.hpp"

#define N 5
#define numIter 100

using namespace std;

barrier sync_pointt(N);

void run() {
    auto start = chrono::steady_clock::now();
    sync_pointt.arrive_and_wait();
    for(int i = 0; i < numIter; i++) {
        cout << generateRandom() << " ";
    }
    cout << endl;
    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;
}

template <typename T>
void primitive(T func){
    vector<thread> threads;
    for( int i = 0; i < N; i++) {
        threads.emplace_back(func);
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

int main() {
    primitive(run);
    return 0;
}