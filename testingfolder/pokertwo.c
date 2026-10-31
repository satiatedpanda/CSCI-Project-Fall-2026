#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

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
    int balance;
    int bet;
    int in_play;
    int player_id;
    char player_name[20];
} Player;

const char SPACE80[] = "                                                                               ";

void init_deck(CardDeck *a);
void init_card(Card *a);
void init_player(Player *a, int idx);
void printcard(Card a);
int isAlphabetOnly(char str[]);
void new_lines_n_times(char newline[], int times);
int hand_rankings(Player *a);
void bettinground(Player PlayerArray[], int num_players, int *pool, int button_idx, int first_round);
int place_bet(Player *player, int *to_call, int *last_raised, int idx);
int num_in_play(Player PlayerArray[], int num_players);
void shuffle(CardDeck *a);


    
    // printf("\x1b[A"); // this is goes up in the terminal x times, modify the value infront of A to go up xA times
    // Will use this for hiding player cards


// @todo, finish main game loop
int main(void) {
    int num_players;
    srand(time(NULL));
    rand(); // flushes first call which is weirdly predictable
    do {
        printf("How many Players will be playing? Input an integer from 2-10\n-> ");
        if (!scanf(" %d", &num_players)) {
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
    Player PlayerArray[num_players];
    int button_idx;
    button_idx = rand() % num_players;
    for (int i = 0; i<num_players; i++) { // initilizes all players
        Player a;
        init_player(&a, i);
        PlayerArray[i] = a;
    }
    for (int i = 0; i<num_players; i++) { // assigns names
        while(1) {
            char temp[20];
            printf("Enter Player %d's name (Under 15 characters, letters only, no spaces)\n-> ", PlayerArray[i].player_id);
            // from https://www.tutorialkart.com/c-programming/how-to-check-if-a-string-contains-only-alphabets-in-c/
            // https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf - restricts spaces instead
            if (!scanf(" %19[^\n]", &temp)) {
                printf("Invalid type input\n");
                exit(104);                
            } else {
                if (isAlphabetOnly(temp)) {
                    strcpy(PlayerArray[i].player_name, temp);
                    break;
                }
                printf("Invalid type input - failed alphabet only test\n");
            }
        }
    }
    CardDeck Deck;
    init_deck(&Deck);
    Card Dealer[5];
    printf("\n\n\n\nGame Start!\n\n");
    int round = 0;
    while(1) { // main game loop
        //remove players who cant play
        Player temp_array[num_players];
        int added_players = 0;
        for (int i = 0; i<num_players; i++) {
            if (PlayerArray[i].balance > 1) {
                temp_array[added_players] = PlayerArray[i];
                added_players++;
            } else {
                printf("Player %s Elimated!\n", PlayerArray[i].player_name);
            }
        }
        num_players = added_players; // all players remaining               
        for (int i = 0; i<num_players; i++) { // reasign temp to main array
            PlayerArray[i] = temp_array[i];
        }
        if (num_players <= 1) { // <Win Condition>
            break;
        }          
        //setup for next round
        printf("Round %d:\n", round+1);
        int dealer_idx = 0; // num dealer cards
        int pool = 0;
        for (int i = 0; i<4; i++) {
            shuffle(&Deck); // just so its really shuffled lol
        }
        if (round) { 
            printf("test round increase Round = %d\n", round);
            button_idx = (button_idx+1)%num_players; // pass the button (starting player) to the right if not the first round
            Deck.deck_idx = 0; // reset deck idx
        }     
        printf("New First Player - Player %s\n", PlayerArray[button_idx].player_name);
        for (int i = 0; i<num_players; i++) {
            PlayerArray[i].player_cards[0] = Deck.array[Deck.deck_idx];
            Deck.deck_idx += 1;
            PlayerArray[i].player_cards[0] = Deck.array[Deck.deck_idx];
            Deck.deck_idx += 1;
            PlayerArray[i].in_play = 1; // resets everyone left to be in play for the next betting round
            PlayerArray[i].bet = 0;
        }

        // first round of betting- button and person to right of button need to bid atleast 2 and 1 respectively
        printf("Betting round 1\n\n");
        bettinground(PlayerArray, num_players, &pool, button_idx, 1);
        if (num_in_play(PlayerArray, num_players) > 1) {
            for (int i = 0; i<3; i++) { // the flop
                Dealer[i] = Deck.array[Deck.deck_idx];
                Deck.deck_idx += 1;
            }
            // print dealer cards
            // betting round 2            
            printf("\n\nBetting round 2\n");
            bettinground(PlayerArray, num_players, &pool, button_idx, 0);
            if (num_in_play(PlayerArray, num_players) > 1) {
                Dealer[3] = Deck.array[Deck.deck_idx]; // 4th dealer card
                Deck.deck_idx += 1;        
                // betting round 3
                printf("\n\nBetting round 3\n");
                bettinground(PlayerArray, num_players, &pool, button_idx, 0); 
                if (num_in_play(PlayerArray, num_players) > 1) {
                    Dealer[4] = Deck.array[Deck.deck_idx]; // 5th, last dealer card
                    Deck.deck_idx += 1;  
                    // betting round 4 (last round)   
                    printf("\n\nBetting round 4\n");
                    bettinground(PlayerArray, num_players, &pool, button_idx, 0);
                }
            }
        }
        if (num_in_play(PlayerArray, num_players) == 1) {
            for (int i = 0; i<num_players; i++) {
                if (PlayerArray[i].in_play) {
                    PlayerArray[i].in_play += pool;
                    printf("Player %s wins this round! They won %d coins\n", PlayerArray[i].player_name, pool);
                }
            }
        }

        // reveal turn
        // decide winner
        // apply winnings (player[i].balance += pool)
        round++;
    }


    return 0;
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

void printcard(Card a) {
    printf("[%c%s] ", a.suit, a.value);
}

int isAlphabetOnly(char str[]) {
    //https://www.tutorialkart.com/c-programming/how-to-check-if-a-string-contains-only-alphabets-in-c/
    int i = 0;
    while (str[i] != '\0') {
        if (!isalpha(str[i])) {
            return 0; // Non-alphabet character found
        }
        i++;
    }
    return 1; // String contains only alphabets
}

void new_lines_n_times(char newline[], int times) {
    for (int i = 0; i<times; i++) { 
        newline[i] = '\n';
    }
    newline[times] = '\0';
}


// @todo, return highest numbers based on hand rank
int hand_rankings(Player *a) {
    
    
    
    return 0;
}

void bettinground(Player PlayerArray[], int num_players, int *pool, int button_idx, int first_round) {
    int to_call = 0; // bet amount
    int last_raised = button_idx;  // idx of person who last raised   
    int first_bidder = 1;
    if (first_round) {
        printf("First round\n%d button_idx, %d button-1 idx\n\n", button_idx, (button_idx-1+num_players) % num_players);
        PlayerArray[button_idx].balance -= 2;
        PlayerArray[button_idx].bet = 2;
        PlayerArray[(button_idx-1+num_players)%num_players].balance -= 1;
        PlayerArray[(button_idx-1+num_players)%num_players].bet = 1;   
        // had an issue here of negative indexes, adding num_players fixed this.       
        *pool += 3;        
        to_call = 2;
    }
    int i = 0; // iterator
    i = (button_idx + 1) % num_players;    
    while (i != last_raised) {
        // returns bet placed. also updates player by .in_play if folded, and to_call by bet if raised
        if (first_bidder) {  // makes sure the button player gets to bid
            last_raised = i;
            first_bidder = 0;
        }
        printf("\n\nPool = %d\n", *pool);
        if (PlayerArray[i].in_play) {   
            printf("Player %d (%s)\n", PlayerArray[i].player_id, PlayerArray[i].player_name);
            *pool += place_bet(&PlayerArray[i], &to_call, &last_raised, i);
            printf("i= %d, last_raised = %d, %d %d Player %d\n", i, last_raised, PlayerArray[i].balance, PlayerArray[i].in_play, PlayerArray[i].player_id);
            for (int k = 0; k<num_players; k++) {
                printf("%d ",PlayerArray[k].bet);
            }
            printf("\n");
        }
        i = (i + 1) % num_players;
    }
    for (int i = 0; i<num_players; i++) {
        PlayerArray[i].bet = 0;
    }
}

int place_bet(Player *player, int *to_call, int *last_raised, int idx) {
    int bid = 0;
    int player_input;        
    int difference;
    difference = *to_call - player->bet;
    printf("difference- %d, to call = %d , bid = %d\n",difference,*to_call,player->bet);
    // set bid amount, limited by min bid and balance
    // update in play if not playing this round
    // return bid amount to add to pool
    while(1) {
        if (player->balance >= difference) {
            if (player->balance == difference) {
                printf("Select an Option: To Call (match current bid) bid %d. - To Fold, bid -1.\n", difference);
            } else {
                printf("Select an Option: To Call (match current bid) bid %d. - To Raise, bid between %d-%d. - To Fold, bid -1.\n", difference, (difference)+1, player->balance);
            }
        } else {
            printf("You are too Poor to match current call, you fold\n");
            player->in_play = 0;
            break;
        }
        printf("%s\r-> ", SPACE80);
        if (!scanf(" %d", &player_input)) {
            printf("Invalid type input\n");
            exit(102);
        }        
        if (player_input == -1) { // fold
            player->in_play = 0;
            break;
        } else if (player_input > difference && player_input <= player->balance) { // raise
            *last_raised = idx;
            player->bet = player_input + player->bet;
            *to_call = player->bet;
            bid = player_input;
            break; 
        } else if (player_input == difference) { // call
            player->bet = player_input + player->bet;            
            bid = player_input;
            break;
        } else { // invalid option
            printf("\nInvalid option.\n");
        }
    }   
    player->balance -= bid;
    return bid;
}

int num_in_play(Player PlayerArray[], int num_players) {
    int total = 0;
    for (int i=0; i<num_players; i++) {
        if (PlayerArray[i].in_play == 1) {
            total++;
        }
    }
    return total;
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
