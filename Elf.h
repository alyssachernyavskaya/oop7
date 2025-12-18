#pragma once
#include "NPC.h"
#include "Visitor.h"

/// Эльф
class Elf : public NPC
{
	using NPC::NPC;
public:
	/// Геттер типа
	NpcType getType() const override;

	/// Дистанция хода
	int moveDistance() const override;

	/// Дистанция выстрела
	int killDistance() const override;
};
