#ifndef OBSERVER_H
#define OBSERVER_H

#include "NPC.h"
#include <memory>
#include <mutex>
#include <fstream>

/// Абстрактный класс обозревателя
class IBattleObserver
{
public:
	/// Конструктор
	virtual ~IBattleObserver() = default;

	/// Событие Убил
	virtual void on_kill(
		const std::shared_ptr<NPC> killer,
		const std::shared_ptr<NPC> victim) = 0;

	/// Событие Оба убиты
	virtual void on_doubleDeath(
		const std::shared_ptr<NPC> figure1,
		const std::shared_ptr<NPC> figure2) = 0;
};

/// Консольный обозреватель
class TextObserver : public IBattleObserver
{
private:
	/// Мьютекс для блокировки консоли
	static std::mutex cout_mutex;

public:
	/// Убил
	void on_kill(
		const std::shared_ptr<NPC> killer,
		const std::shared_ptr<NPC> victim) override;

	/// Оба убиты
	void on_doubleDeath(
		const std::shared_ptr<NPC> figure1,
		const std::shared_ptr<NPC> figure2) override;
};

/// Класс обозреватель в файл лога
class FileObserver : public IBattleObserver
{
private:
	/// Мьютекс для блокировки файла
	static std::mutex file_mutex;

	/// Поток вывода в файл
	std::ofstream file;

public:
	/// Конструктор
	FileObserver();

	/// Убил
	void on_kill(
		const std::shared_ptr<NPC> killer,
		const std::shared_ptr<NPC> victim) override;

	/// Оба
	void on_doubleDeath(
		const std::shared_ptr<NPC> figure1,
		const std::shared_ptr<NPC> figure2) override;
};

#endif