#include "Factory.h"
#include "Visitor.h"
#include "Observer.h"
#include "NPC.h"
#include <iostream>
#include <thread>
#include <vector>

/// Консольный логгер
class ConsoleLogger : public IBattleObserver
{
private:
    /// Мьютекс для блокировки консоли
    static inline std::mutex cout_mutex;

public:
    /// Один убит
    void on_kill(
        const std::shared_ptr<NPC> killer,
        const std::shared_ptr<NPC> victim)
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[Kill] " << killer->getName() << " killed " << victim->getName() << std::endl;
    }

    /// Оба убиты
    void on_doubleDeath(
        const std::shared_ptr<NPC> figure1,
        const std::shared_ptr<NPC> figure2)
    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[Double death] " << figure1->getName() << " and " << figure2->getName() << " killed each other" << std::endl;
    }
};

/// Опционально любой аргумент командной строки (или несколько) запрещает NPC убивать сородичей
/// Если аргументы командной строки отсутствуют - всё дозволено
int main(int argc, char* argv[])
{
    // Количество фигур на доске
    const int NPC_COUNT = 50;

    // Запрет на убийство сородичей (если надо)
    if (argc > 1)
        Game::cannibalismDisabled = true;

    // Название игры (повеселее)
    std::cout << "=========== Bloody Ocean ==============" << std::endl;

    // Плодим набор фигур
    for (int i = 0; i < NPC_COUNT; i++)
    {
        int x = rand() % 100;
        int y = rand() % 100;
        NpcType type = (NpcType)(rand() % 3 + 1);
        std::shared_ptr<NPC> figure;
        std::string name = NPCFactory::generate_name(type);
        figure = NPCFactory::create(type, x, y, name);

        Game::figures.push_back(figure);
    }

    // Региструруем обозреватель - консольный логгер
    ConsoleLogger logger;
    Game::addObserver(&logger);

    // Регистрируем еще обозреватель - файловый логгер
    FileObserver fileLogger;
    Game::addObserver(&fileLogger);

    // Запуск игры: старт потоков движения фигур и их боев
    std::thread tMove(&Game::mover);
    std::thread tFight(&Game::fighter);

    // Поток вывода игрового поля - каждую секунду
    std::thread tPrint([]
        {
            while (!Game::stopFlag.load())
            {
                Game::printMap();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

    // Главный поток ждет окончания игры
    std::this_thread::sleep_for(std::chrono::seconds(30));
    // Время вышло - стоп игра! О чем сообщить всем потокам
    Game::stopFlag = true;
    Game::conditionVariable.notify_all();

    // Завершение работы всех запущенных потоков
    tMove.join();
    tFight.join();
    tPrint.join();

    // Вывод результатов, награждение победителей
    std::cout << "\n=========Survived===========" << std::endl;
    int n = 0;
    for (auto& figure : Game::figures)
    {
        if (figure->isAlive())
            std::cout << ++n << ". " << "[" << NpcTypeToString(figure->getType()) << "] " << figure->getName()
            << " (" << figure->getX() << ", " << figure->getY() << ")\n";
    }

    std::cout << "========== Game over!==========";

    return 0;
}