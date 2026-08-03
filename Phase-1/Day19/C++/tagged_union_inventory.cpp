#include <iostream>
#include <variant>
#include <vector>

struct Weapon {
    int damage;
    float durability;
};

struct Armor {
    int defense;
    int weight;
};

struct Potion {
    int heal_amount;
    bool is_elixir;
};

using Item = std::variant<Weapon, Armor, Potion>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void process(const Item& item) {
    std::visit(overloaded {
        [](const Weapon& w) {
            std::cout << "Weapon | Damage: " << w.damage 
                      << " | Durability: " << w.durability << "\n";
        },
        [](const Armor& a) {
            std::cout << "Armor  | Defense: " << a.defense 
                      << " | Weight: " << a.weight << "\n";
        },
        [](const Potion& p) {
            std::cout << "Potion | Heal: " << p.heal_amount 
                      << " | Elixir: " << (p.is_elixir ? "YES" : "NO") << "\n";
        }
    }, item);
}

int main() {
    std::vector<Item> inventory;
    inventory.push_back(Weapon{50, 95.5f});
    inventory.push_back(Armor{20, 15});
    inventory.push_back(Potion{100, true});

    for (size_t i = 0; i < inventory.size(); ++i) {
        std::cout << "Item " << i + 1 << ": ";
        process(inventory[i]);
    }

    std::cout << "---\n";
    std::cout << "Size of Item variant: " << sizeof(Item) << " bytes\n";

    return 0;
}