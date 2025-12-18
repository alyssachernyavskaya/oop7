#include "NPC.h"
#include <random>
#include <algorithm>

/// Клеток по горизонтали
constexpr int MAP_W = 100;

/// Клеток по вертикали
constexpr int MAP_H = 100;

/// Источник случайных чисел
static std::random_device rd;

/// Генератор случайных чисел
static std::mt19937 rng(rd());

/// Получить случайное целое число
static int randint(int a, int b)
{
	return std::uniform_int_distribution<int>(a, b)(rng);
}

/// Сходить куда-нибудь случайным образом
void NPC::move_randomly()
{
	// Мертвые не ходят
	if (!isAlive())
		return;

	// Дистанция хода
	int distance = moveDistance();
	if (distance < 0)
		return;

	int dx = randint(-distance, distance);
	int max_dy = std::max(0, distance - abs(dx));
	int dy = randint(-max_dy, max_dy);
	x += dx;
	y += dy;
	// Если ушел за пределы доски - втиснуть
	x = std::clamp(x, 0, MAP_W - 1);
	y = std::clamp(y, 0, MAP_H - 1);
}