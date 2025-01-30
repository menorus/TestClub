#include <iostream>  // Библиотека для ввода/вывода
#include <fstream>   // Библиотека для работы с файлами
#include <sstream>   // Библиотека для строковых потоков (для парсинга)
#include <vector>    // Библиотека для работы с векторами
#include <map>       // Библиотека для работы с ассоциативными массивами (словарями)
#include <queue>     // Библиотека для работы с очередями
#include <iomanip>   // Библиотека для манипуляций с выводом (например, для форматирования)
#include <set>       // Библиотека для работы с множествами
#include <string>    // Библиотека для работы со строками

// Структура для представления событий
struct Event {
    std::string time;   // Время события
    int type = 0;       // Тип события (приход, посадка, ожидание, уход)
    std::string client; // Имя клиента
    int tableId = -1;   // Номер стола, за который посадили клиента (для события типа 2)
    int timeSpent = 0;  // Время, которое клиент провел за столом (в минутах)
};

// Класс для представления клуба
class ComputerClub {
public:
    // Конструктор, который инициализирует все основные параметры клуба
    ComputerClub(int tables, const std::string& openTime, const std::string& closeTime, int rate)
        : tableCount(tables), openTime(openTime), closeTime(closeTime), rate(rate) {
        // Инициализация данных для каждого стола
        for (int i = 1; i <= tableCount; ++i) {
            tableStatus[i] = "";  // Все столы свободны
            tableRevenue[i] = 0;  // Начальная выручка равна нулю
            tableTimeUsed[i] = 0; // Начальное время за столом равно нулю
        }
    }

    // Основная функция для обработки событий
    void processEvents(const std::vector<Event>& events) {
        std::cout << openTime << std::endl; // Начало работы клуба

        for (const auto& event : events) {
            // Проверка правильности формата времени события
            if (!isValidTime(event.time)) {
                printError(event, "IncorrectFormat");
                return;
            }

            // Обработка события в зависимости от его типа
            switch (event.type) {
            case 1:
                handleClientArrival(event); // Приход клиента
                break;
            case 2:
                handleClientSit(event);     // Посадка клиента за стол
                break;
            case 3:
                handleClientWait(event);    // Ожидание свободного стола
                break;
            case 4:
                handleClientLeave(event);   // Уход клиента
                break;
            default:
                printError(event, "UnknownEvent"); // Неизвестное событие
                return;
            }
        }

        closeClub(); // Завершение работы клуба
    }

private:
    int tableCount;               // Количество столов в клубе
    std::string openTime, closeTime; // Время открытия и закрытия клуба
    int rate;                     // Тариф (стоимость за час)
    std::map<int, std::string> tableStatus; // Статус столов (кто за столом)
    std::map<int, int> tableRevenue;       // Выручка по каждому столу
    std::map<int, int> tableTimeUsed;      // Время использования каждого стола
    std::map<std::string, std::string> clientStatus; // Статус клиента (в клубе/за столом/в очереди)
    std::queue<std::string> waitQueue;     // Очередь клиентов, ожидающих стол
    std::map<int, std::string> tableStartTime; // Время начала использования каждого стола

    // Функция проверки корректности времени (формат HH:MM)
    bool isValidTime(const std::string& time) {
        if (time.size() != 5 || time[2] != ':') return false; // Проверка формата
        int hours = std::stoi(time.substr(0, 2));  // Извлечение часов
        int minutes = std::stoi(time.substr(3, 2)); // Извлечение минут
        return hours >= 0 && hours < 24 && minutes >= 0 && minutes < 60; // Проверка диапазона
    }

    // Функция преобразования времени в минутах
    int timeToMinutes(const std::string& time) {
        int hours = std::stoi(time.substr(0, 2));  // Часы
        int minutes = std::stoi(time.substr(3, 2)); // Минуты
        return hours * 60 + minutes; // Преобразуем в минуты
    }

    // Функция преобразования минут в формат времени HH:MM
    std::string minutesToTime(int totalMinutes) {
        int hours = totalMinutes / 60;     // Часы
        int minutes = totalMinutes % 60;   // Минуты
        std::ostringstream oss;
        // Форматируем в строку с ведущими нулями, если необходимо
        oss << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << minutes;
        return oss.str();
    }

    // Функция для вывода ошибки
    void printError(const Event& event, const std::string& error) {
        std::cout << event.time << " " << event.type << " " << event.client;
        if (event.tableId != -1) std::cout << " " << event.tableId;
        std::cout << std::endl << event.time << " 13 " << error << std::endl;
    }

    // Обработка события прихода клиента
    void handleClientArrival(const Event& event) {
        // Преобразуем время события в минуты
        int eventTime = timeToMinutes(event.time);
        int openTimeMinutes = timeToMinutes(openTime);
        int closeTimeMinutes = timeToMinutes(closeTime);

        // Проверка, что время события в пределах рабочего времени клуба
        if (eventTime < openTimeMinutes || eventTime >= closeTimeMinutes) {
            printError(event, "NotOpenYet"); // Ошибка, если клуб еще не открылся или уже закрыт
            return;
        }

        // Проверка, что клиент не находится в клубе
        if (clientStatus.count(event.client)) {
            printError(event, "YouShallNotPass"); // Ошибка, если клиент уже в клубе
            return;
        }

        clientStatus[event.client] = "in_club"; // Клиент зашел в клуб
        std::cout << event.time << " 1 " << event.client << std::endl;
    }

    // Обработка посадки клиента за стол
    void handleClientSit(const Event& event) {
        if (clientStatus[event.client] != "in_club") {
            printError(event, "ClientUnknown"); // Ошибка, если клиент не в клубе
            return;
        }

        // Проверка, свободен ли стол
        if (!tableStatus[event.tableId].empty()) {
            printError(event, "PlaceIsBusy"); // Ошибка, если стол занят
            return;
        }

        // Успешная посадка клиента за стол
        std::cout << event.time << " 2 " << event.client << " " << event.tableId << std::endl;
        tableStatus[event.tableId] = event.client; // Заполняем стол клиентом
        clientStatus[event.client] = "sitting";    // Устанавливаем статус клиента
        tableStartTime[event.tableId] = event.time; // Устанавливаем время начала использования стола
    }

    // Обработка клиента, ожидающего стол
    void handleClientWait(const Event& event) {
        bool hasFreeTables = false;
        // Проверяем, есть ли свободные столы
        for (const auto& pair : tableStatus) {
            if (pair.second.empty()) {
                hasFreeTables = true;
                break;
            }
        }

        // Если есть свободные столы, клиент не должен ждать
        if (hasFreeTables) {
            printError(event, "ICanWaitNoLonger!"); // Ошибка, если столы все же есть
            return;
        }

        // Проверка, не в очереди ли уже клиент
        if (clientStatus[event.client] == "waiting") return;

        // Если очередь переполнена
        if (waitQueue.size() >= static_cast<size_t>(tableCount)) {
            std::cout << event.time << " 11 " << event.client << std::endl; // Клиент уходит
        }
        else {
            std::cout << event.time << " 3 " << event.client << std::endl;  // Клиент заходит в очередь
            waitQueue.push(event.client); // Добавляем клиента в очередь
            clientStatus[event.client] = "waiting"; // Статус клиента - в ожидании
        }
    }

    // Обработка события ухода клиента
    void handleClientLeave(const Event& event) {
        std::cout << event.time << " 4 " << event.client << std::endl;

        // Проверка, существует ли клиент в клубе
        if (clientStatus[event.client] != "in_club" && clientStatus[event.client] != "sitting") {
            printError(event, "ClientUnknown"); // Ошибка, если клиент не найден
            return;
        }

        // Обработка освобождения стола
        for (auto& [tableId, client] : tableStatus) {
            if (client == event.client) {
                int startMinutes = timeToMinutes(tableStartTime[tableId]);
                int endMinutes = timeToMinutes(event.time);
                int duration = endMinutes - startMinutes;

                if (duration > 0) {
                    tableTimeUsed[tableId] += duration;  // Накопление времени
                }

                tableStatus[tableId] = ""; // Стол освобождается
                tableStartTime.erase(tableId); // Удаляем информацию о времени начала

                // Пересаживаем следующего клиента из очереди, если есть
                if (!waitQueue.empty()) {
                    std::string nextClient = waitQueue.front();
                    waitQueue.pop();
                    tableStatus[tableId] = nextClient;
                    tableStartTime[tableId] = event.time;
                    clientStatus[nextClient] = "sitting";

                    std::cout << event.time << " 12 " << nextClient << " " << tableId << std::endl;
                }

                break;
            }
        }

        clientStatus.erase(event.client); // Удаляем клиента из клуба
    }

    // Завершение работы клуба
    void closeClub() {
        std::set<std::string> remainingClients;
        for (const auto& [client, status] : clientStatus) {
            if (status == "sitting") {
                for (const auto& [tableId, occupant] : tableStatus) {
                    if (occupant == client) {
                        int startMinutes = timeToMinutes(tableStartTime[tableId]);
                        int endMinutes = timeToMinutes(closeTime);
                        int duration = endMinutes - startMinutes;

                        if (duration > 0) {
                            tableTimeUsed[tableId] += duration; // Накопление времени
                        }
                    }
                }
            }
            remainingClients.insert(client); // Клиенты, которые остались
        }

        // Выводим, кто ушел после закрытия
        for (const auto& client : remainingClients) {
            std::cout << closeTime << " 11 " << client << std::endl;
        }

        std::cout << closeTime << std::endl;

        // Выводим выручку по столам
        for (int i = 1; i <= tableCount; ++i) {
            int totalTime = tableTimeUsed[i]; // Общее время за столом
            int hoursToCharge = (totalTime + 59) / 60; // Округление времени в большую сторону
            tableRevenue[i] = hoursToCharge * rate;  // Выручка за стол

            int hours = totalTime / 60;
            int minutes = totalTime % 60;
            std::cout << i << " " << tableRevenue[i] << " "
                << std::setw(2) << std::setfill('0') << hours << ":"
                << std::setw(2) << std::setfill('0') << minutes << std::endl;
        }
    }

};

// Основная функция
int main() {
    std::ifstream inputFile("input.txt"); // Открытие входного файла
    if (!inputFile.is_open()) { // Проверка на успешность открытия
        std::cerr << "Ошибка: Не удалось открыть входной файл!" << std::endl;
        return 1;
    }

    int tableCount;
    std::string openTime, closeTime;
    int rate;
    inputFile >> tableCount >> openTime >> closeTime >> rate; // Чтение данных о клубе

    std::vector<Event> events;
    std::string line;
    // Чтение событий из файла
    while (std::getline(inputFile, line)) {
        if (line.empty()) continue; // Пропускаем пустые строки
        std::istringstream iss(line);
        Event event;
        iss >> event.time >> event.type >> event.client;
        if (event.type == 2) { // Для посадки за стол, также читаем номер стола
            iss >> event.tableId;
        }
        events.push_back(event); // Добавляем событие в список
    }

    ComputerClub club(tableCount, openTime, closeTime, rate); // Создание объекта клуба
    club.processEvents(events); // Обработка событий

    return 0;
}
