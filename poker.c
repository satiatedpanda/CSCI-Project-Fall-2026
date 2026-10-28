#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char suit;
    char value[2];
} Card;

typedef struct {
    Card array[52 * sizeof(Card)];
    size_t deck_idx;
} CardDeck;

typedef struct {
    Card player_cards[2 * sizeof(Card)];
    unsigned int bet;
    unsigned int balance;
} Player;

void init_card(Card* a);
void shuffle(CardDeck* a);

// @todo, finish main game loop
int main(void) {
    int num_players;
    do {
        printf(
            "How many Players will be playing? Input an integer from 2-10\n-> "
        );
        if (scanf(" %d", &num_players) == 0) {
            printf("Invalid type input\n");
            exit(102);
        }
        if (num_players > 1 && num_players < 11) {
            break;
        } else {
            printf("Invalid input, please input a valid number between 1-10\n");
        }
    } while (1);
    printf("%d players\n", num_players);
    Player PlayerArray[num_players * sizeof(Player)];
    for (int i = 0; i < num_players; i++) {
        Player a;
        init_player(&a);
        PlayerArray[i] = a;
    }

    return 0;
}
void init_deck(CardDeck* a) {
    for (int i = 0; i < 52; i++) {
        a->array[i]
    }
}

void init_card(Card* a) {
    a->suit = '/0';
    *a->value = '/0';
}

void init_player(Player* a) {
    a->balance = 100;
    a->bet = 0;
    Card null_card;
    init_card(&null_card);
    a->player_cards[0] = null_card;
    a->player_cards[1] = null_card;
}

void printcard(Card a, int newline) {
    if (newline == 0) {
        printf("[%c%s] ", a.suit, a.value);
    } else {
        printf("[%c%s]\n", a.suit, a.value);
    }
}

// @todo, return highest numbers based on hand rank
int hand_rankings(Player* a) { return 0; }

void place_bet(Player* a) {}

// @todo, finish using above structs
void shuffle(CardDeck* a) {

    for (int i = 0; i < 52; i++) {
        int random_spot = rand() % 52;
        Card temp = a->array[i];
        a->array[i] = a->array[random_spot];
        a->array[random_spot] = temp;
    }
}
