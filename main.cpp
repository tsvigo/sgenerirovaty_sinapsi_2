// #include <QCoreApplication>

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);

//     return a.exec();
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <vector>
#include <gmpxx.h>
#include <fstream>
#include <random>
#include <QFile>
constexpr size_t NUM_SYNAPSES = 10105;
const mpz_class MAX_VALUE("18446744073709551615");
const std::string FILE_PATH = "/home/viktor/my_projects_qt_2/sgenerirovaty_sinapsi/random_sinapsi.bin";
QString filePath = "/home/viktor/my_projects_qt_2/sgenerirovaty_sinapsi/random_sinapsi.bin";
std::vector<mpz_class> list_of_synapses(10105);
// Функции:
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeVectorToFile(const std::vector<mpz_class>& vec, const std::string& filename) {
    FILE* outFile = fopen(filename.c_str(), "wb");
    if (!outFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return;
    }

    for (const auto& num : vec) {
        mpz_out_raw(outFile, num.get_mpz_t());
    }

    fclose(outFile);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void generateRandomNumbers(std::vector<mpz_class>& list_of_synapses) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, MAX_VALUE.get_ui());

    for (size_t i = 0; i < NUM_SYNAPSES; ++i) {
        list_of_synapses[i] = dis(gen);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void writeVectorToFile2(const std::vector<mpz_class>& vec, const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Cannot open file for writing:" << file.errorString();
        return;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);

    // Записываем размер вектора
    out << static_cast<quint32>(vec.size());

    for (const auto& number : vec) {
        // Получаем размер числа в байтах
        size_t size = (mpz_sizeinbase(number.get_mpz_t(), 2) + 7) / 8;

        // Выделяем буфер для хранения числа
        std::vector<unsigned char> buffer(size);

        // Записываем число в буфер
        mpz_export(buffer.data(), &size, 1, 1, 0, 0, number.get_mpz_t());

        // Записываем размер числа и само число
        out << static_cast<quint32>(size);
        out.writeRawData(reinterpret_cast<const char*>(buffer.data()), size);

        // Отладочная информация
        //   qDebug() << "Записан элемент с размером:" << size;
    }

    file.close();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printVector(const std::vector<mpz_class>& list_of_synapses) {
    for (const auto& value : list_of_synapses) {
        qDebug() << QString::fromStdString(value.get_str());
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<mpz_class> readVectorFromFile(const std::string& filename) {
   // std::vector<mpz_class> vec;
    FILE* inFile = fopen(filename.c_str(), "rb");
    if (!inFile) {
        std::cerr << "Error opening file for reading." << std::endl;
        return list_of_synapses;
    }

    while (!feof(inFile)) {
        mpz_class num;
        if (mpz_inp_raw(num.get_mpz_t(), inFile) == 0) {
            break; // EOF or error
        }
        list_of_synapses.push_back(num);
    }

    fclose(inFile);
    return list_of_synapses;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
 //   writeToFile(list_of_synapses, FILE_PATH);
    // Запись вектора в файл
    writeVectorToFile(list_of_synapses, FILE_PATH);
    readVectorFromFile(FILE_PATH)  ;
//writeVectorToFile2(list_of_synapses, filePath);
    // Чтение чисел из бинарного файла
  //  std::vector<mpz_class> read_synapses(NUM_SYNAPSES);
  //  readFromFile(read_synapses, FILE_PATH);

    // Проверка прочитанных чисел
    // for (size_t i = 0; i < NUM_SYNAPSES; ++i) {
    //     if (list_of_synapses[i] != read_synapses[i]) {
    //         qCritical() << "Ошибка: значения не совпадают на позиции" << i;
    //         return -1;
    //     }
    // }

    // qDebug() << "Все значения совпадают.";

    // // Вывод значений вектора в консоль
    printVector(list_of_synapses);

    return a.exec();
}
