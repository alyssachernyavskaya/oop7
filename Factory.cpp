#include "Factory.h"
#include "NPC.h"
#include "Robber.h"
#include "Elf.h"
#include "Knight.h"
#include <iostream>
#include <cstring>
#include <random>

/// Создать NPC
std::shared_ptr<NPC> NPCFactory::create(const NpcType type, int x, int y, const std::string& name)
{
	try
	{
		switch (type)
		{
		case RobberType:
			return std::make_shared<Robber>(x, y, name);
		case ElfType:
			return std::make_shared<Elf>(x, y, name);
		case KnightType:
			return std::make_shared<Knight>(x, y, name);
		default:
			std::cerr << "Unexpected type of NPC: " << type << std::endl;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error creating NPC: " << ex.what() << std::endl;
	}
}

/// Сгенерировать имя объекта в зависимости от типа
std::string NPCFactory::generate_name(const NpcType npcType)
{
	// Префикс имени разбойника
	static std::vector<std::string> robberNames =
	{
		"Evil",
		"Cruel",
		"Fierce",
		"Dangerous"
	};
	// Префикс имени эльфа
	static std::vector<std::string> elfNames =
	{
		"Light",
		"Dark",
		"Wise",
		"Quiet"
	};
	// Префикс имени рыцаря
	static std::vector<std::string> knightNames =
	{
		"Fearless",
		"Noble",
		"Mighty",
		"Smart"
	};

	// Имя равно: префикс + _ + случайное число
	static std::random_device random_device;
	static std::mt19937 gen(random_device());
	size_t sz = npcType == RobberType ? robberNames.size()
		: npcType == ElfType ? elfNames.size()
		: knightNames.size();
	std::uniform_int_distribution<> dist(0, sz - 1);
	return (npcType == RobberType ? robberNames[dist(gen)]
		: npcType == ElfType ? elfNames[dist(gen)]
		: knightNames[dist(gen)]) +
		"_" + std::to_string(random_device() / 1000);
}