#include "Knight.h"

/// Рыцарь
class Knight;

/// Геттер типа
NpcType Knight::getType() const
{
	return KnightType;
}

/// Дистанция хода
int Knight::moveDistance() const
{
	return 30;
}

/// Дистанция выстрела
int Knight::killDistance() const
{
	return 10;
}