#include "Visitor.h"
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

/// Заканчиваем игру
std::atomic<bool> Game::stopFlag;

/// Опциональный запрет убивать таких же
std::atomic<bool> Game::cannibalismDisabled;

/// Набор фигур
std::vector<std::shared_ptr<NPC>> Game::figures;

/// Мьютекс для набора фигур
std::shared_mutex Game::figures_mutex;

/// Все бои
std::deque<GameTask> Game::queue;

/// Мьютекс для очереди боев
std::mutex Game::queue_mutex;

/// Условная переменная для синхронизации потоков
std::condition_variable Game::conditionVariable;

/// Все обозреватели
std::vector<IBattleObserver*> Game::observers;

/// Мьютекс для обозревателей
std::mutex Game::observers_mutex;

/// Вычислить расстояние между фигурами
static double distance(const NPC& figure1, const NPC& figure2)
{
	return _hypot(figure1.getX() - figure2.getX(), figure1.getY() - figure2.getY());
}

/// Бросить кубик
static int rollDice()
{
	static std::mt19937 g(std::random_device{}());
	return std::uniform_int_distribution<int>(1, 6)(g);
}

/// Добавить обозреватель
void Game::addObserver(IBattleObserver* observer)
{
	std::lock_guard<std::mutex> lock(observers_mutex);
	observers.push_back(observer);
}

/// Сообщить об убийстве ккуда следует (т.е. всем обозревателям)
void Game::notify_kill(std::shared_ptr<NPC> killer, std::shared_ptr<NPC> victim)
{
	std::lock_guard<std::mutex> lock(observers_mutex);
	for (auto observer : observers)
	{
		observer->on_kill(killer, victim);
	}
}

/// Сообщить об обоюдном убийстве ккуда следует (т.е. всем обозревателям)
void Game::notify_double(std::shared_ptr<NPC> figure1, std::shared_ptr<NPC> figure2)
{
	std::lock_guard<std::mutex> lock(observers_mutex);
	for (auto observer : observers)
	{
		observer->on_doubleDeath(figure1, figure2);
	}
}

/// Движение всех фигур по доске (в потоке)
void Game::mover()
{
	// Работает, пока извне нет команды остановиться
	while (!stopFlag.load())
	{
		// Блокируем на запись набор фигур
		std::shared_lock<std::shared_mutex> figuresLock(figures_mutex);

		// Двигаем все живые фигуры
		for (auto& figure : figures)
		{
			if (!figure->isAlive())
				continue;
			figure->move_randomly();
		}

		// Собираем все возможные бои
		for (size_t i = 0; i < figures.size() - 1; i++)
		{
			auto& figure1 = figures[i];
			if (!figure1->isAlive())
				continue;

			for (size_t j = i + 1; j < figures.size(); j++)
			{
				auto& figure2 = figures[j];
				if (!figure2->isAlive())
					continue;

				// Если есть запрет на убийство сородичей - проверяем
				if (cannibalismDisabled.load() && (figure1->getType() == figure2->getType()))
					continue;

				// Если одна из фигур может достать до другой
				double dist = distance(*figure1, *figure2);
				if ((dist <= figure1->killDistance()) || (dist <= figure2->killDistance()))
				{
					// Блокируем очередь боев
					std::lock_guard<std::mutex> queueLock(queue_mutex);

					//Кто до кого достает?
					FightStatus fightStatus = None;
					if (distance(*figure1, *figure2) <= figure1->killDistance() && distance(*figure1, *figure2) <= figure2->killDistance())
						fightStatus = Both;
					else if (distance(*figure1, *figure2) <= figure1->killDistance())
						fightStatus = First;
					else
						fightStatus = Second;

					// Новый бой добавляем в очередь
					queue.push_back(GameTask{ figure1, figure2, fightStatus });

					// Разблокировать любой поток
					conditionVariable.notify_one();
				}
			}
		}

		// А наш поток на паузу
		std::this_thread::sleep_for(500ms);
	}
}

/// Бои всех фигур (в потоке)
void Game::fighter()
{
	// Работает, пока извне нет команды остановиться
	while (!stopFlag.load())
	{
		// Блокировка очереди боев
		std::unique_lock<std::mutex> lock(queue_mutex);
		// Ждем, пока очередь пуста  или не пришел сигнал СТОП
		conditionVariable.wait(lock, [] {return !Game::queue.empty() || stopFlag.load(); });

		if (stopFlag.load())
			break;

		// Вытаскиваем из очереди бой и снимаем блокировку
		GameTask task = queue.front();
		queue.pop_front();
		lock.unlock();

		auto figure1 = task.first.lock();
		auto figure2 = task.second.lock();
		if (!figure1 || !figure2 || !figure1->isAlive() || !figure2->isAlive())
			continue;

		// Взвешиваем жребии (кидаем кубик)
		int r1 = rollDice();
		int r2 = rollDice();

		// Если жребии совпали и сил у обоих хватает - обоюдное убийство
		// Если силы есть у одного - он должен быть не хуже по жребию, тогда убил
		// Иначе оба живы
		if (r1 == r2 && task.status == Both)
		{
			figure1->kill();
			figure2->kill();
			Game::notify_double(figure1, figure2);
		}
		if (r1 >= r2 && (task.status == First || task.status == Both))
		{
			figure2->kill();
			Game::notify_kill(figure1, figure2);
		}
		else if (r2 >= r1 && (task.status == Second || task.status == Both))
		{
			figure1->kill();
			Game::notify_kill(figure2, figure1);
		}
	}
}

/// Вывод доски с фигурами
void Game::printMap()
{
	// Доска 100 * 100 полей
	const int MAP_W = 100;
	const int MAP_H = 100;

	// Заполняем точками
	std::vector<std::string> field(MAP_H, std::string(MAP_W, '.'));

	{
		// Блокировка на запись набора фигур
		std::shared_lock<std::shared_mutex> lock(figures_mutex);

		// Все живые фигуры располагаем на доске (только типы фигур)
		for (auto& figure : figures)
		{
			if (!figure->isAlive())
				continue;

			int x = figure->getX();
			int y = figure->getY();
			if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
				continue;

			char ch = figure->getType() == RobberType ? 'R' : figure->getType() == ElfType ? 'E' : 'K';
			field[y][x] = ch;
		}
	}

	// Блокируем консоль
	static std::mutex cout_mutex;
	std::lock_guard<std::mutex> guard(cout_mutex);
	// Чистим экран (для Linux)
	system("clear");
	// Выводим
	std::string header = std::string(MAP_W, '=');
	header.replace((MAP_W - 5) / 2, 5, " MAP ");
	std::string footer = std::string(MAP_W, '=');
	std::cout << header << std::endl;
	for (int y = 0; y < MAP_H; y++)
		std::cout << field[y] << std::endl;
	std::cout << footer << std::endl;
}