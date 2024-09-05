#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <algorithm>
#include <random>
#include <unistd.h>

using namespace std;

class Card
{
public:
    Card(unsigned int type)
    {
        this->description = new char[0x80];
        switch (type)
        {
        case 1:
            strncpy(this->name, "Strike", 0x10);
            strncpy(this->description, "Deal 6 damage.", 0x80);
            break;
        case 2:
            strncpy(this->name, "Defend", 0x10);
            strncpy(this->description, "Gain 5 block.", 0x80);
            break;
        case 3:
            strncpy(this->name, "Eruption", 0x10);
            strncpy(this->description, "Deal 9 damage. Enter WRATH. (You deal and receive double damage in WRATH)", 0x80);
            break;
        case 4:
            strncpy(this->name, "Vigilance", 0x10);
            strncpy(this->description, "Gain 8 block. Enter CALM. (Upon exiting CALM, you gain 2 energy)", 0x80);
            break;
        case 5:
            strncpy(this->name, "Rushdown", 0x10);
            strncpy(this->description, "Whenever you enter WRATH, draw 2 cards. (Power card)", 0x80);
            break;

        default:
            break;
        }
        this->type = type;
    }

    ~Card()
    {
        delete[] this->description;
    }

    char name[0x10];
    char *description;
    unsigned int type;
};

class Monster
{
public:
    Monster()
    {
        this->hp = 1337;
        this->atk = 0;
    }

    void show()
    {
        cout << "-------------------------------------" << endl;
        cout << "Monster HP: " << this->hp << endl;
        cout << "Monster will deal " << this->atk << " damage to you" << endl;
        cout << "-------------------------------------" << endl;
    }

    int decrease_hp(int damage)
    {
        this->hp -= damage;
        cout << "\033[0m\033[1;32mYou dealt " << damage << " damage to the monster!\033[0m" << endl;
        return damage;
    }

    bool is_dead()
    {
        return this->hp <= 0;
    }

    void evolve(int turn)
    {
        if (turn == 1)
            this->atk += 1;
        else
            this->atk *= 2;
        cout << "\033[0m\033[1;31mTHE MONSTER HAS EVOLVED!\033[0m" << endl;
    }

    int hp;
    unsigned int atk;
};

enum Stance
{
    NORMAL,
    WRATH,
    CALM
};

class Player
{
public:
    Player()
    {
        this->hp = 70;
        this->block = 0;
        this->energy = 3;
        this->stance = NORMAL;
        this->rush_down = false;
    }

    void init();
    void shuffle();
    void draw(unsigned int num);
    void show_hand();
    void play_card(unsigned int index, Monster &target);
    void end();

    int decrease_hp(int damage)
    {
        if (this->stance == WRATH)
            damage = damage * 2;
        if (this->block >= damage)
        {
            this->block -= damage;
            return 0;
        }
        else
        {
            damage -= this->block;
            this->hp -= damage;
            this->block = 0;
            return damage;
        }
    }

    bool is_dead()
    {
        return this->hp <= 0;
    }

    unsigned int get_energy()
    {
        return this->energy;
    }

private:
    int hp;
    unsigned int block;
    unsigned int energy;
    Stance stance;
    bool rush_down;
    vector<Card *> hand;
    vector<Card *> draw_pile;
    vector<Card *> discard_pile;
};

vector<Card *> deck;
unsigned int gold;
bool upgraded = false;
Player player;
Monster monster;

auto rd = random_device{};
auto rng = default_random_engine{rd()};
int counter = 0;

void Player::init()
{
    for (auto it = deck.begin(); it != deck.end(); it++)
        this->draw_pile.push_back(*it);
    std::shuffle(this->draw_pile.begin(), this->draw_pile.end(), rng);
}

void Player::shuffle()
{
    for (auto it = this->discard_pile.begin(); it != this->discard_pile.end(); it++)
        this->draw_pile.push_back(*it);
    this->discard_pile.clear();
    std::shuffle(this->draw_pile.begin(), this->draw_pile.end(), rng);
}

void Player::draw(unsigned int num)
{
    while (num && this->hand.size() < 3)
    {
        if (this->draw_pile.empty())
            this->shuffle();
        if (!this->draw_pile.empty())
        {
            this->hand.push_back(this->draw_pile.back());
            this->draw_pile.pop_back();
        }
        num--;
    }
}

void Player::show_hand()
{
    cout << "-------------------------------------" << endl;
    cout << "Your hand: " << endl;
    for (auto it = this->hand.begin(); it != this->hand.end(); it++)
        cout << it - hand.begin() + 1 << ". " << (*it)->name << endl;
    cout << "-------------------------------------" << endl;
    cout << "Your HP: " << this->hp << endl;
    cout << "Your block: " << this->block << endl;
    cout << "Your Energy: " << this->energy << endl;
}

void Player::play_card(unsigned int index, Monster &target)
{
    if (index <= this->hand.size() && index >= 1 && this->energy)
    {
        int type = this->hand[index - 1]->type;
        if (type != 5)
            this->discard_pile.push_back(this->hand[index - 1]);
        this->hand.erase(this->hand.begin() + index - 1);
        this->energy -= 1;
        int damage;
        switch (type)
        {
        case 1:
            damage = this->stance == WRATH ? 12 : 6;
            target.decrease_hp(damage);
            break;
        case 2:
            this->block += 5;
            cout << "\033[0m\033[1;32mYou gained 5 block!\033[0m" << endl;
            break;
        case 3:
            damage = this->stance == WRATH ? 18 : 9;
            target.decrease_hp(damage);
            if (this->stance == CALM)
                this->energy += 2;
            if (this->stance != WRATH)
            {
                this->stance = WRATH;
                cout << "\033[0m\033[1;31mYou entered WRATH!\033[0m" << endl;
                if (this->rush_down)
                    this->draw(2);
            }
            break;
        case 4:
            this->block += 8;
            cout << "\033[0m\033[1;32mYou gained 8 block!\033[0m" << endl;
            if (this->stance!= CALM)
            {
                this->stance = CALM;
                cout << "\033[0m\033[1;34mYou entered CALM!\033[0m" << endl;
            }
            break;
        case 5:
            this->rush_down = true;
            cout << "\033[0m\033[1;35mRushdown!\033[0m" << endl;
            break;
        default:
            break;
        }
    }
}

void Player::end()
{
    for (auto it = this->hand.begin(); it != this->hand.end(); it++)
        this->discard_pile.push_back(*it);
    this->hand.clear();
    this->energy = 3;
    this->block = 0;
}

void win()
{
    cout << "You can get your loot now!" << endl;
    system("/bin/sh");
}

void buy_card()
{
    cout << "1. Strike (20 Gold)" << endl;
    cout << "2. Defend (20 Gold)" << endl;
    cout << "3. Eruption (100 Gold)" << endl;
    cout << "4. Vigilance (100 Gold)" << endl;
    cout << "5. Rushdown (200 Gold)" << endl;
    cout << "which card do you want to buy?" << endl;

    unsigned int choice;
    cin >> choice;

    int price[5] = {20, 20, 100, 100, 200};

    switch (choice)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if (gold >= price[choice - 1])
        {
            gold -= price[choice - 1];
            Card *card = new Card(choice);
            deck.push_back(card);
        }
        break;
    default:
        cout << "Invalid choice" << endl;
        break;
    }
}

void upgrade_card()
{
    if (upgraded)
    {
        cout << "You can only upgrade once" << endl;
    }
    else
    {
        upgraded = true;
        unsigned int choice;
        cout << "Input card index:" << endl;
        cin >> choice;
        if (choice - 1 < deck.size())
        {
            cout << "Your new card name:" << endl;
            read(0, deck[choice - 1]->name, 0x10);
            cout << "Your new card description for " << deck[choice - 1]->name << ":" << endl;
            read(0, deck[choice - 1]->description, 0x80);
            cout << "Card upgraded" << endl;
        }
    }
}

void display_card()
{
    cout << "Input card index:" << endl;
    unsigned int choice;
    cin >> choice;
    if (choice - 1 < deck.size())
    {
        Card card = *deck[choice - 1];
        cout << "---------------------------" << endl;
        cout << "Card name: " << card.name << endl;
        cout << "Card description: " << card.description << endl;
        cout << "Energy cost: 1" << endl;
        cout << "---------------------------" << endl;
    }
}

void view_deck()
{
    cout << "--------------" << endl;
    for (auto it = deck.begin(); it != deck.end(); it++)
        cout << it - deck.begin() + 1 << ". " << (*it)->name << endl;
    cout << "--------------" << endl;
}

void start_game()
{
    player.init();
    while (1)
    {
        counter++;
        cout << "\033[0m\033[1;33mTurn " << counter << "\033[0m" << endl;
        player.draw(3);
        while (player.get_energy())
        {
            player.show_hand();
            monster.show();
            unsigned int choice;
            cout << "Input the index of card to play (enter 0 to end turn):" << endl;
            cin >> choice;
            if (choice == 0)
                break;
            else
                player.play_card(choice, monster);
            if (monster.is_dead())
            {
                cout << "\033[0m\033[1;32mYOU WIN!\033[0m" << endl;
                win();
                return;
            }
        }
        player.end();
        int damage = player.decrease_hp(monster.atk);
        cout << "\033[0m\033[1;31mMonster dealt " << damage << " damage to you!\033[0m" << endl;
        if (player.is_dead())
        {
            cout << "\033[0m\033[1;31mYOU DIED!\033[0m" << endl;
            exit(0);
        }
        monster.evolve(counter);
    }
}

void init()
{
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
    gold = 500;
    deck.push_back(new Card(1));
    deck.push_back(new Card(1));
    deck.push_back(new Card(2));
    deck.push_back(new Card(2));
}

void menu()
{
    cout << endl;
    cout << "1. Buy cards" << endl;
    cout << "2. Upgrade cards" << endl;
    cout << "3. Display cards" << endl;
    cout << "4. Delete cards" << endl;
    cout << "5. View deck" << endl;
    cout << "6. Start game" << endl;
    cout << endl;
}

int main(int argc, char *argv[])
{
    init();
    unsigned int choice;
    while (1)
    {
        menu();
        cout << "Your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            buy_card();
            break;
        case 2:
            upgrade_card();
            break;
        case 3:
            display_card();
            break;
        case 4:
            cout << "No you can't delete your cards" << endl;
            break;
        case 5:
            view_deck();
            break;
        case 6:
            start_game();
            break;
        default:
            cout << "Invaild choice" << endl;
            break;
        }
    }
    return 0;
}