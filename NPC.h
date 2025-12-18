#ifndef NPC_H
#define NPC_H

#include <string>
#include <atomic>

/// Тип объекта NPC
enum NpcType
{
	/// Не известно
	Unknown = 0,

	/// Эльф
	ElfType = 1,

	/// Разбойник
	RobberType = 2,

	/// Рыцарь
	KnightType = 3
};

/// Преобразовать тип объекта в строку
static std::string NpcTypeToString(NpcType type)
{
	switch (type)
	{
	case ElfType:
		return "Elf";
	case RobberType:
		return "Robber";
	case KnightType:
		return "Knight";
	default:
		return "Unknown";
	}
}

/// NPC
class NPC
{
protected:
	/// Имя
	std::string name;

	/// Абсцисса
	int x{ 0 };

	/// Ордината
	int y{ 0 };

	/// Признак жизни
	std::atomic<bool> alive{ true };
public:
	/// Конструктор с инициализацией
	NPC(int _x, int _y, const std::string& _name) : x(_x), y(_y), name(_name)
	{
	}

	/// Деструктор
	virtual ~NPC() = default;

	/// Геттер имени
	const std::string getName() const
	{
		return name;
	}

	/// Геттер типа
	virtual NpcType getType() const = 0;

	/// Геттер абсциссы
	int getX() const
	{
		return x;
	}

	/// Геттер ординаты
	int getY() const
	{
		return y;
	}

	/// Живой?
	bool isAlive() const
	{
		return alive.load();
	}

	/// Убить
	void kill()
	{
		alive.store(false);
	}

	/// Расстояние хода
	virtual int moveDistance() const = 0;

	/// Растояние выстрела
	virtual int killDistance() const = 0;

	/// Куда-нибудь сходить
	virtual void move_randomly();
};

#endif