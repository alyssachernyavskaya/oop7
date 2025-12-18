#include "Observer.h"
#include <iostream>

/// Мьютекс для блокировки консоли
std::mutex TextObserver::cout_mutex;

/// Мьютекс для блокировки файла
std::mutex FileObserver::file_mutex;

/// Консольный обозреватель
TextObserver textObserver;

/// Убил
void TextObserver::on_kill(
	const std::shared_ptr<NPC> killer,
	const std::shared_ptr<NPC> victim)
{
	std::lock_guard<std::mutex> lock(cout_mutex);
	std::cout << NpcTypeToString(killer->getType()) << " " << killer->getName() << " killed "
		<< NpcTypeToString(victim->getType()) << " " << victim->getName() << std::endl;
}

/// Оба
void TextObserver::on_doubleDeath(
	const std::shared_ptr<NPC> figure1,
	const std::shared_ptr<NPC> figure2)
{
	std::lock_guard<std::mutex> lock(cout_mutex);
	std::cout << NpcTypeToString(figure1->getType()) << " " << figure1->getName() << " and "
		<< NpcTypeToString(figure2->getType()) << " " << figure2->getName() << " killed each other" << std::endl;
}

/// Конструктор с инициализацией
/// Имя файла лога - log.txt
FileObserver::FileObserver() : file("log.txt")
{
}

/// Убил
void FileObserver::on_kill(
	const std::shared_ptr<NPC> killer,
	const std::shared_ptr<NPC> victim)
{
	std::lock_guard<std::mutex> lock(file_mutex);
	file << NpcTypeToString(killer->getType()) << " " << killer->getName() << " (x=" << killer->getX() << ", y=" << killer->getY() << ")" << " killed "
		<< NpcTypeToString(victim->getType()) << " " << victim->getName() << " (x=" << victim->getX() << ", y=" << victim->getY() << ")" << std::endl;
}

/// Оба
void FileObserver::on_doubleDeath(
	const std::shared_ptr<NPC> figure1,
	const std::shared_ptr<NPC> figure2)
{
	std::lock_guard<std::mutex> lock(file_mutex);
	file << NpcTypeToString(figure1->getType()) << " " << figure1->getName() << " (x=" << figure1->getX() << ", y=" << figure1->getY() << ")" << " and "
		<< NpcTypeToString(figure2->getType()) << " " << figure2->getName() << " (x=" << figure2->getX() << ", y=" << figure2->getY() << ")"
		<< " killed each other" << std::endl;
}