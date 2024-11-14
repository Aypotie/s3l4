#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>
#include <atomic>
#include <future>

using namespace std;

struct MedicalRecord {
    string specialty;  // Специальность врача
    string category;   // Категория здоровья: "А", "Б", "В", "Г", "Д" (используем строку)
};

struct Conscript {
    string fullName;                 // ФИО
    string birthDate;                // Дата рождения
    vector<MedicalRecord> records;   // Список заключений врачей
};

vector<string> names = {"Иванов И.И.", "Сидоров С.С.", "Кузнецов К.К.", "Петров П.П."};
vector<string> birth = {"24.06.2004", "13.05.2005", "04.11.1979", "31.12.2024", "07.11.1978"};
vector<string> doctors = {"Хирург", "Ортопед", "Терапевт", "Офтальмолог"};
vector<string> categories = {"А", "Б", "В", "Г", "Д"};

// Функция для проверки пригодности к службе (категория "А" — пригоден)
bool isFitForService(const Conscript& conscript) {
    for (const auto& record : conscript.records) {
        if (record.category == "А") {  // Сравниваем с строкой "А"
            return true;  // Если хотя бы одно заключение "А", призывник пригоден
        }
    }
    return false;  // Если нет категории "А", не пригоден
}

// Функция обработки без многопоточности
vector<Conscript> processWithoutThreads(const vector<Conscript>& conscripts) {
    vector<Conscript> fitConscripts;
    for (const auto& conscript : conscripts) {
        if (isFitForService(conscript)) {
            fitConscripts.push_back(conscript);
        }
    }
    return fitConscripts;
}

// Функция обработки с многопоточностью
vector<Conscript> processWithThreads(const vector<Conscript>& conscripts, int numThreads) {
    vector<Conscript> fitConscripts;
    vector<future<vector<Conscript>>> futures;

    // Разделяем данные на несколько частей для параллельной обработки
    size_t chunkSize = conscripts.size() / numThreads;
    size_t remainder = conscripts.size() % numThreads;

    auto processChunk = [](const vector<Conscript>& conscripts, size_t start, size_t end) {
        vector<Conscript> result;
        for (size_t i = start; i < end; ++i) {
            if (isFitForService(conscripts[i])) {
                result.push_back(conscripts[i]);
            }
        }
        return result;
    };

    // Создаем потоки
    size_t startIndex = 0;
    for (int i = 0; i < numThreads; ++i) {
        size_t endIndex = startIndex + chunkSize + (i < remainder ? 1 : 0);
        futures.push_back(async(launch::async, processChunk, ref(conscripts), startIndex, endIndex));
        startIndex = endIndex;
    }

    // Собираем результаты из всех потоков
    for (auto& fut : futures) {
        auto result = fut.get();
        fitConscripts.insert(fitConscripts.end(), result.begin(), result.end());
    }

    return fitConscripts;
}

void test(const vector<Conscript>& conscripts) {
    int numThreads = 2;  // Количество параллельных потоков

    // Время обработки без многопоточности
    auto start = chrono::high_resolution_clock::now();
    auto fitConscriptsWithoutThreads = processWithoutThreads(conscripts);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> durationWithoutThreads = end - start;
    cout << "Время обработки без многопоточности: " << durationWithoutThreads.count() << " секунд" << endl;

    // Время обработки с многопоточностью
    start = chrono::high_resolution_clock::now();
    auto fitConscriptsWithThreads = processWithThreads(conscripts, numThreads);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> durationWithThreads = end - start;
    cout << "Время обработки с многопоточностью: " << durationWithThreads.count() << " секунд" << endl;

    // Вывод списка пригодных к службе призывников
    cout << "Пригодные к службе (с использованием многопоточности):" << endl;
    for (const auto& conscript : fitConscriptsWithThreads) {
        cout << conscript.fullName << " (" << conscript.birthDate << ")" << endl;
    }
}

vector<Conscript> generateRandomConscripts(int n) {
    vector<Conscript> conscripts(n);
    
    random_device rd; 
    mt19937 gen(rd());
    uniform_int_distribution<> nameGen(0, names.size() - 1);
    uniform_int_distribution<> birthGen(0, birth.size() - 1);
    uniform_int_distribution<> docGen(0, doctors.size() - 1);
    uniform_int_distribution<> consGen(0, categories.size() - 1);

    for (int i = 0; i < n; i++) {
        Conscript cons;
        cons.fullName = names[nameGen(gen)];
        cons.birthDate = birth[birthGen(gen)];

        vector<MedicalRecord> records(2);
        for (int j = 0; j < 2; j++) {
            MedicalRecord rec;
            rec.category = categories[consGen(gen)];
            rec.specialty = doctors[docGen(gen)];
            records[j] = rec;
        }
        cons.records = records;
        conscripts[i] = cons;
    }

    return conscripts;
}

int main() {
    vector<Conscript> conscripts1 = {
        {"Иванов И.И.", "01.01.2000", {{"Терапевт", "А"}, {"Хирург", "Б"}}},
        {"Петров П.П.", "02.02.1999", {{"Терапевт", "Б"}, {"Невролог", "Г"}}},
        {"Сидоров С.С.", "03.03.2001", {{"Терапевт", "А"}}},
        {"Кузнецов К.К.", "04.04.1998", {{"Терапевт", "Д"}}}
    };
    cout << "Test 1:" << endl;
    test(conscripts1);
    cout << "=========" << endl;
    cout << "Test 2:" << endl;
    auto conscripts2 = generateRandomConscripts(1000);
    test(conscripts2);
    return 0;
}