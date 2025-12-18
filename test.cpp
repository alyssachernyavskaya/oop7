#include "gtest/gtest.h"
#include "Factory.h"
#include "NPC.h"
#include "Dragon.h"
#include "Elf.h"
#include "Druid.h"
#include <memory>
#include <fstream>
#include <filesystem>

// создание NPC
bool test_factory() 
{
    auto npc = factory(DragonType, 100, 200, "TestDragon");
    return npc != nullptr && npc->getType() == DragonType;
}

// расстояние
bool test_distance()
{
    auto a = factory(ElfType, 0, 0, "A");
    auto b = factory(ElfType, 3, 4, "B");
    return a->is_close(b, 5) && !a->is_close(b, 4);
}

// 3. имена
bool test_names() 
{
    std::string name = generate_name(DragonType);
    return !name.empty() && name.find('_') != std::string::npos;
}

// 4. бой
bool test_fight() 
{
    auto dragon = std::make_shared<Dragon>(0, 0, "D");
    auto elf = std::make_shared<Elf>(0, 0, "E");
    return dragon->fight(elf) && !elf->fight(dragon);
}

// 5. сохранение
bool test_save() 
{
    set_t s;
    s.insert(factory(DruidType, 1, 2, "Test"));
    save(s, "micro_test.txt");

    std::ifstream f("micro_test.txt");
    bool ok = f.good();
    f.close();
    std::remove("micro_test.txt");
    return ok;
}