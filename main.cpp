// #include <QCoreApplication>

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);

//     return a.exec();
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QCoreApplication>
#include <QDebug>
#include <vector>
#include <gmpxx.h>
#include <fstream>
#include <random>

constexpr size_t NUM_SYNAPSES = 10105;
const mpz_class MAX_VALUE("18446744073709551615");
const std::string FILE_PATH = "/home/viktor/my_projects_qt_2/sgenerirovaty_sinapsi/random_sinapsi.bin";

void generateRandomNumbers(std::vector<mpz_class>& list_of_synapses) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, MAX_VALUE.get_ui());

    for (size_t i = 0; i < NUM_SYNAPSES; ++i) {
        list_of_synapses[i] = dis(gen);
    }
}

void writeToFile(const std::vector<mpz_class>& list_of_synapses, const std::string& filePath) {
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        qCritical() << "Ошибка открытия файла для записи";
        return;
    }

    for (const auto& value : list_of_synapses) {
        std::string strValue = value.get_str();
        size_t size = strValue.size();
        outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
        outFile.write(strValue.c_str(), size);
    }

    outFile.close();
}

void readFromFile(std::vector<mpz_class>& list_of_synapses, const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile) {
        qCritical() << "Ошибка открытия файла для чтения";
        return;
    }

    for (size_t i = 0; i < NUM_SYNAPSES; ++i) {
        size_t size;
        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<char> buffer(size);
        inFile.read(buffer.data(), size);
        list_of_synapses[i].set_str(std::string(buffer.begin(), buffer.end()), 10);
    }

    inFile.close();
}

void printVector(const std::vector<mpz_class>& list_of_synapses) {
    for (const auto& value : list_of_synapses) {
        qDebug() << QString::fromStdString(value.get_str());
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Создание вектора для хранения случайных чисел
    std::vector<mpz_class> list_of_synapses(NUM_SYNAPSES);

    // Генерация случайных чисел
    generateRandomNumbers(list_of_synapses);
    // Вывод размера вектора list_of_synapses
    qDebug() << "Размер list_of_synapses:" << list_of_synapses.size();
    // Запись чисел в бинарный файл
    writeToFile(list_of_synapses, FILE_PATH);

    // Чтение чисел из бинарного файла
    std::vector<mpz_class> read_synapses(NUM_SYNAPSES);
    readFromFile(read_synapses, FILE_PATH);

    // Проверка прочитанных чисел
    for (size_t i = 0; i < NUM_SYNAPSES; ++i) {
        if (list_of_synapses[i] != read_synapses[i]) {
            qCritical() << "Ошибка: значения не совпадают на позиции" << i;
            return -1;
        }
    }

    qDebug() << "Все значения совпадают.";

    // Вывод значений вектора в консоль
    printVector(read_synapses);

    return a.exec();
}
