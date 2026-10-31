#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct {
    char suit; 
    char value[2];
} Card;

typedef struct {
    Card array[52];
    size_t deck_idx;
} CardDeck;

typedef struct {
    Card player_cards[2];
    int bet;
    int balance;
    int in_play;
    int player_id;
} Player;

void init_card(Card *a);
void shuffle(CardDeck *a);
void init_player(Player *a, int idx);
void init_deck(CardDeck *a);


// @todo, finish main game loop
int main(void) {
    int num_players;
    srand(time(NULL));
    do {
        printf("How many Players will be playing? Input an integer from 2-10\n-> ");
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
    int button_idx;
    button_idx = rand() % num_players;
    for (int i = 0; i<num_players; i++) {
        Player a;
        init_player(&a, i);
        PlayerArray[i] = a;
    }
    CardDeck Deck;
    init_deck(&Deck);
    Card Dealer[5];    
    // printf("\x1b[A"); // this is goes up in the terminal x times, modify the value infront of A to go up xA times
    // Will use this for hiding player cards
    int round = 0;
    while(1) { // main game loop
        prinf("Round %d:\n", round);        
        int dealer_idx = 0; // num dealer cards
        int pool = 0;
        //remove players who cant play

        Player temp_array[num_players];
        int added_players = 0;
        for (int i = 0; i<num_players; i++) {
            if (PlayerArray[i].balance > 1) {
                temp_array[added_players] = PlayerArray[i];
                added_players++;
            } else {
                printf("Player %d Elimated!\n", PlayerArray[i].player_id);
            }
        }
        for (int i = 0; i<num_players; i++) {
            PlayerArray[i] = temp_array[i];
        }
        num_players = added_players; // removed elimiated players
        if (!round) { 
            button_idx = (button_idx+1)%num_players;
        }
        prinf("New First Player - Player %d\n", button_idx);


        for (int i = 0; i<num_players; i++) {
            int current_player = (i + button_idx) % num_players;
            if (PlayerArray[current_player].in_play == 1) {
                PlayerArray[current_player].player_cards[0] = Deck.array[Deck.deck_idx];
                Deck.deck_idx += 1;
                PlayerArray[current_player].player_cards[0] = Deck.array[Deck.deck_idx];
                Deck.deck_idx += 1;
            }
        }

        // first round of betting- dealer and person to left of dealer need to bid atleast 2 and 1 respectively


    }





    return 0;
}


void main_game_loop(Player PlayerArray[], int num_players) {
    CardDeck Deck;
    init_deck(&Deck);
    shuffle(&Deck);

}


void init_deck(CardDeck *a) {

}

void init_card(Card *a) {
    a->suit = '\0';
    *a->value = '\0';
}

void init_player(Player *a, int idx) {
    a->balance = 500;
    a->bet = 0;
    a->in_play = 1;
    a->player_id = idx+1;
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
int hand_rankings(Player *a) { // Kevin's section ... 

    char order[] = "23456789TJQKA" int first = 0 int second = 0

        for (int i = 0; i < 13; i++) {
        if (a->player_cards[0].value[0] ==
            order[i]) {    // check first card letter in char order[]
            first = i + 2; // adds 2 to give value to card
        }

        if (a->player_card[1].value[0] ==
            order[i]) {     // check second card in char order[]
            second = i + 2; // adds 2 to give value to the card
        }
    }
    if (first == 0 ||
        second == 0) { // if a hand is = 0 or just blank. Returns 0
        return 0;
    }
    if (first == second) { // a pair
        return 100 + first; // returns 100 and beats everything else
    }
    if (first > second) { // if true returns first
        return first;
    }
    return second; // otherwise just returns second
    
}

void bettinground(Player *PlayerArray, int num_players, int button_idx, int first_round) {
    int passed = 0;
    int i = 0;
    
    int min_bid = 0;
    while (passed < num_players) {
        int current_player = (i + button_idx) % num_players;
        if (current_player == button_idx && first_round) {
            PlayerArray[current_player].bet = 2;
        }
        if (current_player == ((button_idx-1)%num_players) && first_round) {
            PlayerArray[current_player].bet = 1;
        }
        place_bet(&PlayerArray[current_player], &min_bid);
    }
}

void place_bet(Player *player, int *min_bid) {
    while(1) {

    }
}


// @todo, finish using above structs
void shuffle(CardDeck *a) {
    int CardDecks[260]; // Array to store 260 total cards
    int index =
        0; // Counter to track the current position in the CardDeck array

    srand(
        time(NULL)
    ); // Seeds the random number generator using the system clock

    // for (int deck = 0; deck < 5; deck++) { // Loop 5 times for 5 decks
    //     for (int card = 0; card <= 51;
    //          card++) { // Loop through card values 1 to 13
    //         CardDecks[index] = card; // Store the card value at current index
    //         index++;                // Move to the next array spot            
    //     }
    // }

    for (int i = 0; i < 260; i++) { // Standard Shuffle: Swap each card with a
                                    // card at a random position
        int random_spot =
            rand() % 260; // Pick a random array index from 0 to 259

        int temp = CardDecks[i]; // Swap CardDeck[i] and CardDeck[random_spot]
                                // using a temporary variable
        CardDecks[i] = CardDecks[random_spot];
        CardDecks[random_spot] = temp;
    }
}
