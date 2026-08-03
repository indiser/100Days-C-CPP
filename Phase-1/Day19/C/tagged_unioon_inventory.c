#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef enum
{
    ITEM_WEAPON, ITEM_POTION, ITEM_ARMOR

}ItemType;

typedef struct
{
    int damage;
    float durability;
}Weapon;

typedef struct
{
    int defense;
    int weight;
}Armor;

typedef struct
{
    int heal_amount;
    bool isExiler;
}Potion;

typedef struct
{
    ItemType tag;
    union
    {
        Weapon weapon;
        Armor armor;
        Potion potion;
    }data;
    
} Item;

Item create_weapon(int damage, float durability)
{
    Item i;
    i.tag = ITEM_WEAPON;
    i.data.weapon.damage = damage;
    i.data.weapon.durability = durability;

    return i;
}

Item create_armor(int defense, int weight)
{
    Item i;
    i.tag = ITEM_ARMOR;
    i.data.armor.defense = defense;
    i.data.armor.weight = weight;

    return i;
}

Item create_potion(int heal_amount, bool isElixer)
{
    Item i;
    i.tag = ITEM_POTION;
    i.data.potion.heal_amount = heal_amount;
    i.data.potion.isExiler = isElixer;
    
    return i;
}

void Process(Item item)
{
    switch (item.tag)
    {
        case ITEM_WEAPON:
            printf("Damage: %d\n", item.data.weapon.damage);
            printf("Durability: %.2f\n", item.data.weapon.durability);
            break;

        case ITEM_ARMOR:
            printf("Defense: %d\n", item.data.armor.defense);
            printf("Weight: %d\n", item.data.armor.weight);
            break;

        case ITEM_POTION:
            printf("Heal Amount: %d\n", item.data.potion.heal_amount);
            char *elixer;
            if(item.data.potion.isExiler == true) elixer = "YES";
            else elixer = "NO";
            printf("IS Elixer: %s\n", elixer);
            break;

        default:
            printf("NO valide choice\n");
            break;
    }
}

int main()
{
    Item inventory[3];

    inventory[0] = create_weapon(50, 95.5f);
    inventory[1] = create_armor(20, 15);
    inventory[2] = create_potion(100, true);

    for (int i = 0; i < 3; i++)
    {
        printf("Item %d:\n", i + 1);
        Process(inventory[i]);
        printf("\n---\n");
    }

    printf("Size of Item: %zu bytes\n", sizeof(Item));
    printf("Size of union: %zu bytes\n", sizeof(inventory[0].data));

    return 0;
}