#ifndef FACTORY_H
#define FACTORY_H

#include "NPC.h"
#include <string>
#include <memory>

/// Создатель объектов NPC
class NPCFactory
{
public:
	/// Создать объект данного типа с данными именем и координатами
	static std::shared_ptr<NPC> create(const NpcType type, int x, int y, const std::string& name);

	/// Сгенерировать имя объекта в зависимости от типа
	static std::string generate_name(const NpcType npcType);
};

#endif