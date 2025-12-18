#include "Robber.h"

/// Разбойник
class Robber;

/// Геттер типа
NpcType Robber::getType() const
{
	return RobberType;
}

/// Дистанция хода
int Robber::moveDistance() const
{
	return 10;
}

/// Дистанция выстрела
int Robber::killDistance() const
{
	return 10;
}