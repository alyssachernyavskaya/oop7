#include "Elf.h"

/// Эльф
class Elf;

/// Геттер типа
NpcType Elf::getType() const
{
	return ElfType;
}

/// Дистанция хода
int Elf::moveDistance() const
{
	return 10;
}

/// Дистанция выстрела
int Elf::killDistance() const
{
	return 50;
}