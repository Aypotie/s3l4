#include <iostream>
#include <vector>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>

#include "utils/utils.hpp" 

#define N 5
#define numIter 100

using namespace std;

// Переменные для condition_variable и мьютекса
condition_variable cv;
mutex cv_mtx;
bool ready = false;

// Функция, которая выполняет задачу потока, ожидая сигнала от condition_variable
void run() {
    auto start = chrono::steady_clock::now();
    
    // Захватываем мьютекс и ждем, пока ready не станет true
    unique_lock<mutex> lock(cv_mtx); // для синхронизации доступа к переменной ready
    cv.wait(lock, []{ return ready; });
  
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

    this_thread::sleep_for(chrono::milliseconds(100));

    // устанавливаем ready в true и уведомляем все потоки, чтобы они начали выполнение
    {
        lock_guard<mutex> lock(cv_mtx);
        ready = true;
    }
    cv.notify_all(); // уведомляем все потоки о том, что они могут начать выполнение

    for (auto& th : threads) {
        th.join();
    }
}

int main() {
    cout << "Testing Monitor with condition_variable:\n";
    primitive(run);

    return 0;
}
