#ifndef VISITOR_H
#define VISITOR_H

#include <vector>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <deque>
#include <condition_variable>
#include "NPC.h"
#include "Observer.h"

/// Состояние боя (кто кого может убить?)
enum FightStatus
{
	/// Никто никого
	None = 0,

	/// Первый второго
	First = 1,

	/// Второй первого
	Second = 2,

	/// Оба
	Both = 3
};

/// Бой
struct GameTask
{
	/// Первый воин
	std::weak_ptr<NPC> first;

	/// Второй воин
	std::weak_ptr<NPC> second;

	/// Кто кого?
	FightStatus status;
};

/// Игра
class Game
{
public:
	/// Заканчиваем игру
	static std::atomic<bool> stopFlag;

	/// Набор фигур (NPC)
	static std::vector<std::shared_ptr<NPC>> figures;

	/// Мьютекс блокировки коллекции фигур
	static std::shared_mutex figures_mutex;

	/// Набор боев
	static std::deque<GameTask> queue;

	/// Мьютекс для блокировки очереди боев
	static std::mutex queue_mutex;

	/// Условная переменная для синхронизации потоков
	static std::condition_variable conditionVariable;

	/// Набор обозревателей
	static std::vector<IBattleObserver*> observers;

	/// Мьютекс для блокировки коллекции обозревателей
	static std::mutex observers_mutex;

	/// Добавить обозреватель
	static void addObserver(IBattleObserver* observer);

	/// Сообщить об убийстве
	static void notify_kill(std::shared_ptr<NPC> agressor, std::shared_ptr<NPC> victim);

	/// Сообщить об обоюдном убийстве
	static void notify_double(std::shared_ptr<NPC> figure1, std::shared_ptr<NPC> figure2);

	/// Движение всех фигур по доске (в потоке)
	static void mover();

	/// Бои всех фигур (в потоке)
	static void fighter();

	/// Печатать доску с фигурами (только типами фигур)
	static void printMap();

	/// Дополнительное условие: обциональный запрет убивать соплеменников
	static std::atomic<bool> cannibalismDisabled;
};

#endif