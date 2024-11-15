#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

#include "utils/utils.hpp"

#define N 5
#define numIter 100

using namespace std;

// SpinWait с использованием atomic_bool
atomic_bool ready(false); // Условие для активного ожидания

void spin_wait() {
    // Ожидаем до тех пор, пока ready не станет true
    while (!ready.load(std::memory_order_acquire)) {
        this_thread::yield(); // Уступаем другим потокам
    }
}

void run() {
    auto start = chrono::steady_clock::now();
    
    spin_wait(); // Активное ожидание

    for (int i = 0; i < numIter; i++) {
        cout << generateRandom() << " ";
    }

    cout << endl;
    
    auto end = chrono::steady_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;
}

template <typename T>
void primitive(T func) {
    vector<thread> threads;
    for (int i = 0; i < N; i++) {
        threads.emplace_back(func);
    }

    // Даем потокам немного времени, чтобы они успели запуститься
    this_thread::sleep_for(chrono::milliseconds(100));

    // Устанавливаем ready в true, чтобы потоки прекратили активное ожидание
    ready.store(true, std::memory_order_release);

    for (auto& th : threads) {
        th.join();
    }
}

int main() {
    primitive(run);
    return 0;
}
