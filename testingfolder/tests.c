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
    int total_bet;
    int in_play;
    int player_id;
    char player_name[8];
} Player;

typedef struct {
    int round;
    int num_players;
    int button_idx;
    int bet_round;
    int pool;
    int current_player;
    int last_raised;
    int to_call;
    Card Dealer[5];
} GameParams;



const char SPACE100[] = "                                                                                                   ";


void init_deck(CardDeck *a);
void init_card(Card *a);
void init_player(Player *a, int idx);
void init_param(GameParams *a);
void printcard(Card a);
int isAlphabetOnly(char str[]);
void new_lines_n_times(char newline[], int times);
void empty_stdin (void);
int hand_rankings(Player *a);
void bettinground(Player PlayerArray[], GameParams *Params);
int place_bet(Player PlayerArray[], GameParams *Params, int idx);
int num_in_play(Player PlayerArray[], int num_players);
void shuffle(CardDeck *a);
void print_game(Player PlayerArray[], GameParams *Params, char constr1[], char constr2[], int cards, int move_up);
void betting_round_start(GameParams *a);

    
    // printf("\x1b[A"); // this is goes up in the terminal x times, modify the value infront of A to go up xA times
    // Will use this for hiding player cards

int main(void) {
    GameParams Params;   
    init_param(&Params);
    srand(time(NULL));
    rand(); // flushes first call which is weirdly predictable
    do {
        printf("How many Players will be playing? Input an integer from 2-10\n-> ");
        if (!scanf(" %d", &Params.num_players)) {
            printf("Invalid type input\n");
            exit(102);
        }
        if (Params.num_players > 1 && Params.num_players < 11) {
            break;
        } else {
            printf("Invalid input, please input a valid number between 1-10\n");
        }
    } while (1);
    printf("%d players\n", Params.num_players);
    Player PlayerArray[Params.num_players];
    Params.button_idx = rand() % Params.num_players;
    char namearray[10][6] = {"aaaaa", "bbbbb", "ccccc", "ddd", "eeeee", "f", "ggggg", "hhhhh", "iiiii", "mmmmm"};
    for (int i = 0; i<Params.num_players; i++) { // initilizes all players
        Player a;
        init_player(&a, i);
        PlayerArray[i] = a;
        strcpy(PlayerArray[i].player_name, namearray[i]);    // @todo delete when done testing, and uncomment out the code below
    }
    // for (int i = 0; i<Params.num_players; i++) { // assigns names
    //     while(1) {
    //         char temp[6];
    //         printf("Enter Player %d's name (5 characters or less, letters only, no spaces)\n-> ", PlayerArray[i].player_id);
    //         // from https://www.tutorialkart.com/c-programming/how-to-check-if-a-string-contains-only-alphabets-in-c/
    //         // https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf - restricts spaces instead
    //         if (!scanf(" %5[^\n]", &temp)) {
    //             // is it possible to fail this?
    //             printf("Invalid type input\n");
    //             exit(104);
    //         } else {
    //             empty_stdin();
    //             // this removes all characters left over in the scanf
    //             if (isAlphabetOnly(temp)) {
    //                 strcpy(PlayerArray[i].player_name, temp);
    //                 break;
    //             }
    //             printf("Invalid type input - failed alphabet only test\n");
    //         }
    //     }
    // }    
    CardDeck Deck;
    init_deck(&Deck);
    printf("\n\n\n\nGame Start!\n\n\n");

    while(1) { // main game loop
        //remove players who cant play
        Player temp_array[Params.num_players];
        int added_players = 0;
        for (int i = 0; i<Params.num_players; i++) {
            if (PlayerArray[i].balance > 1) {
                temp_array[added_players] = PlayerArray[i];
                added_players++;
            } else {
                printf("Player %s Elimated!\n", PlayerArray[i].player_name);
            }
        }
        Params.num_players = added_players; // all players remaining               
        for (int i = 0; i<Params.num_players; i++) { // reasign temp to main array
            PlayerArray[i] = temp_array[i];
        }
        if (Params.num_players <= 1) { // <Win Condition>
            break;
        }
        //setup for next round
        Params.pool = 0;
        for (int i = 0; i<4; i++) {
            shuffle(&Deck); // just so its really shuffled lol
        }
        if (Params.round) { 
            Params.button_idx = (Params.button_idx+1)%Params.num_players; // pass the button (starting player) to the right if not the first round
            // note; this is technically innacurate due to if the current button player gets eliminated, but I really dont care ;D
            // I dont even know how to simulate it correctly, and this works well enough and WONT produce out-of-bounds errors :D
            Deck.deck_idx = 0; // reset deck idx
        } 
        for (int i = 0; i<Params.num_players; i++) {
            // PlayerArray[i].player_cards[0] = Deck.array[Deck.deck_idx];
            Deck.deck_idx += 1;           
            // PlayerArray[i].player_cards[0] = Deck.array[Deck.deck_idx];
            Deck.deck_idx += 1;          
            PlayerArray[i].in_play = 1; // resets everyone left to be in play for the next betting round
            PlayerArray[i].bet = 0;
            PlayerArray[i].total_bet = 0;
        }
        // first round of betting- button and person to right of button need to bid atleast 2 and 1 respectively
        Params.bet_round = 1;
        print_game(PlayerArray, &Params, "", "", 0, 0);
        bettinground(PlayerArray, &Params);
        if (num_in_play(PlayerArray, Params.num_players) > 1) {
            for (int i = 0; i<3; i++) { // the flop
                Params.Dealer[i] = Deck.array[Deck.deck_idx];
                Deck.deck_idx += 1;
            }
            // print dealer cards
            Params.bet_round = 2;          
            bettinground(PlayerArray, &Params);
            if (num_in_play(PlayerArray, Params.num_players) > 1) {
                Params.Dealer[3] = Deck.array[Deck.deck_idx]; // 4th dealer card
                Deck.deck_idx += 1;        
                // betting round 3
                Params.bet_round = 3;
                bettinground(PlayerArray, &Params); 
                if (num_in_play(PlayerArray, Params.num_players) > 1) {
                    Params.Dealer[4] = Deck.array[Deck.deck_idx]; // 5th, last dealer card
                    Deck.deck_idx += 1;  
                    Params.bet_round = 4;
                    print_game(PlayerArray, &Params, "", "", 1, 1);
                    bettinground(PlayerArray, &Params);
                }
            }
        }
        if (num_in_play(PlayerArray, Params.num_players) == 1) {
            for (int i = 0; i<Params.num_players; i++) {
                if (PlayerArray[i].in_play) {
                    PlayerArray[i].in_play += Params.pool;
                    printf("Player %s wins this round! They won %d coins\n", PlayerArray[i].player_name, Params.pool);
                }
            }
        }

        // reveal turn
        // decide winner
        // apply winnings (player[i].balance += pool)
        for (int k = 0; k<Params.num_players; k++) {
            if (PlayerArray[k].balance == 0) {
                printf("Player %d is all in, balance = 0, bet = %d\n", PlayerArray[k].player_id, PlayerArray[k].total_bet);
            }
        }
        printf("\n\n\n\n\n\n\n");
        Params.round += 1;
    }
    return 0;
}

void init_deck(CardDeck *a) {
    a->deck_idx = 0;
    for (int i = 0; i<52; i++) {
        Card b;
        init_card(&b);
        a->array[i] = b;
    }
}

void init_card(Card *a) {
    a->suit = '\0';
    *a->value = '\0';
}

void init_player(Player *a, int idx) {
    a->balance = 500;
    a->bet = 0;
    a->total_bet = 0;
    a->in_play = 1;
    a->player_id = idx+1;
    Card null_card;
    init_card(&null_card);
    a->player_name[0] = '\0';
    a->player_cards[0] = null_card;
    a->player_cards[1] = null_card;
}

void init_param(GameParams *a) {
    a->bet_round = 0;
    a->button_idx = 0;
    a->current_player = -1;
    a->num_players = 0;
    a->pool = 0;
    a->round = 0;
    a->last_raised = 0;
    a->to_call = 0;
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

void empty_stdin (void) {
    int c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

// @todo, return highest numbers based on hand rank
int hand_rankings(Player *a) { // Kevin's section ... 

    char order[] = "23456789TJQKA";
    int first = 0;
    int second = 0;

        for (int i = 0; i < 13; i++) {
        if (a->player_cards[0].value[0] ==
            order[i]) {    // check first card letter in char order[]
            first = i + 2; // adds 2 to give value to card
        }

        if (a->player_cards[1].value[0] ==
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

void bettinground(Player PlayerArray[], GameParams *Params) {
    Params->to_call = 0; // bet amount
    Params->last_raised = Params->button_idx;  // idx of person who last raised   
    int first_bidder = 1;
    if (Params->bet_round == 1) {
        // printf("First round\n%d button_idx, %d button-1 idx\n\n", button_idx, (button_idx-1+num_players) % num_players);
        PlayerArray[Params->button_idx].balance -= 2;
        PlayerArray[Params->button_idx].bet = 2;
        PlayerArray[(Params->button_idx-1+Params->num_players)%Params->num_players].balance -= 1;
        PlayerArray[(Params->button_idx-1+Params->num_players)%Params->num_players].bet = 1;   
        // had an issue here of negative indexes, adding num_players fixed this.       
        Params->pool += 3;
        Params->to_call = 2;
    }
    betting_round_start(Params);
    int i = 0; // iterator
    i = (Params->button_idx + 1) % Params->num_players;    
    while (i != Params->last_raised && num_in_play(PlayerArray, Params->num_players) > 1) {
        // returns bet placed. also updates player by .in_play if folded, and to_call by bet if raised
        if (first_bidder) {  // makes sure the button player gets to bid
            Params->last_raised = i;
            first_bidder = 0;
        }
        if (PlayerArray[i].in_play) {   
            Params->current_player = i;
            char constcr1[80];
            char constcr2[] = "Enter a (1) character to continue";
            snprintf(constcr1, 80, "Player %d (%s) Are you ready?.", PlayerArray[Params->current_player].player_id, PlayerArray[Params->current_player].player_name);
            print_game(PlayerArray, Params, constcr1, constcr2, 0, 12+Params->num_players);
            char garbage_char;
            if (!scanf(" %c", &garbage_char)) {
                exit(102);
            }
            empty_stdin();
            Params->pool += place_bet(PlayerArray, Params, i);
        }
        i = (i + 1) % Params->num_players;
    }
    for (int i = 0; i<Params->num_players; i++) {
        PlayerArray[i].total_bet += PlayerArray[i].bet;
        PlayerArray[i].bet = 0;
    }
}

int place_bet(Player PlayerArray[], GameParams *Params, int idx) {
    int bid = 0;
    int player_input;        
    int difference;
    int invalid_trigger = 0;
    difference = Params->to_call - PlayerArray[Params->current_player].bet;
    // printf("difference- %d, to call = %d , bid = %d\n",difference,*to_call,player->bet);
    // set bid amount, limited by min bid and balance
    // update in play if not playing this round
    // return bid amount to add to pool
    while(1) {
        int all_in_trigger = 0;
        if (PlayerArray[Params->current_player].balance >= difference) {
            if (PlayerArray[Params->current_player].balance == difference) {
                char constcr1[80];
                char constcr2[] = "To Fold, bid <<-1>>.";

                // https://stackoverflow.com/questions/804288/creating-c-formatted-strings-not-printing-them
                snprintf(constcr1, 80, "Select an Option: To Call (match current bid) bid <<%d>>.", difference);
                print_game(PlayerArray, Params, constcr1, constcr2, 1, 1);
            } else {
                char constcr1[80];
                char constcr2[100];
                // https://stackoverflow.com/questions/804288/creating-c-formatted-strings-not-printing-them
                snprintf(constcr1, 80, "Select an Option: To Call (match current bid) bid <<%d>>.", difference);                
                snprintf(constcr2, 100, "To Raise, bid between <<%d-%d>>. - To Fold, bid <<-1>>.", (difference)+1, PlayerArray[Params->current_player].balance);
                print_game(PlayerArray, Params, constcr1, constcr2, 1, 1);                
            }
        } else {
            char constcr1[] = "You are too Poor to match current call";
            char constcr2[80];
            snprintf(constcr2, 80, "You can either fold (bid <<-1>>) or go All-In (bid <<%d>>)", PlayerArray[Params->current_player].balance);
            all_in_trigger = 1;
            print_game(PlayerArray, Params, constcr1, constcr2, 1, 1);
        }
        // @todo impliment all-ins https://officialpokerrules.com/betting-structures/#incomplete-raises
        // THIS WILL BE PAIN :SOB:
        printf("%s\r-> ", SPACE100);
        if (!scanf(" %d", &player_input)) {
            printf("Invalid type input\n");
            exit(102);
        }
        printf("\x1b[A");
        printf("-> ");
        if (player_input == -1) { // fold
            PlayerArray[Params->current_player].in_play = 0;
            bid = 0;
            break;
        } else if (player_input > difference && player_input <= PlayerArray[Params->current_player].balance) { // raise
            Params->last_raised = idx;
            PlayerArray[Params->current_player].bet = player_input + PlayerArray[Params->current_player].bet;
            Params->to_call = PlayerArray[Params->current_player].bet;
            bid = player_input;
            break;
        } else if (player_input == difference || (all_in_trigger && player_input == PlayerArray[Params->current_player].balance)) { // call OR all in if cant call
            PlayerArray[Params->current_player].bet = player_input + PlayerArray[Params->current_player].bet;
            bid = player_input;
            break;
        } else { // invalid option
            // @todo fix printing moving up a line
            invalid_trigger = 1;
            printf("\r%s\r    <Invalid option.>", SPACE100);
        }
    }
    if (invalid_trigger) {
        printf("\r%s\r-> ", SPACE100);
    }
    PlayerArray[Params->current_player].balance -= bid;
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

void print_game(Player PlayerArray[], GameParams *Params, char constr1[], char constr2[], int cards, int move_up) {   
    // LISTEN. I KNOW THIS SUCKS
    // I KNOW
    // however, this stupid thing still works the way I want so HA
    if (move_up) {
        int num_up = 13+Params->num_players;
        if (move_up > 2) {
            num_up = move_up;
        }
        printf("\x1b[%dA", num_up);
    }
    printf("%s\rRound Number %d\n", SPACE100, Params->round+1);
    printf("%s\r", SPACE100);
    for (int i = 0; i<Params->num_players; i++) {
        char is_button[8] = "       ";
        char is_folded[12] = "\0";
        char is_single_digit = '\0';
        char name_length_spaces[5] = "\0";
        if (Params->button_idx == i) {
            strcpy(is_button, "BUTTON ");
        }
        if (!PlayerArray[i].in_play) {
            strcpy(is_folded, "    FOLDED!");
        }
        if (PlayerArray[i].player_id < 10) {
            is_single_digit = ' ';
        }
        if (strlen(PlayerArray[i].player_name) < 5) {
            for (int k = 0; k<5-strlen(PlayerArray[i].player_name); k++) {
                name_length_spaces[k] = ' ';
            }
            name_length_spaces[5-strlen(PlayerArray[i].player_name)] = '\0';
        }
        if (Params->current_player == i) {
            printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d  -  Bet: %d    !<CURRENT PLAYER>!\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, PlayerArray[i].bet, PlayerArray[i].bet);
        } else {
            if (strlen(is_folded) > 5) {
                printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d  -             %s\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, PlayerArray[i].bet, is_folded);
            } else {
                printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d  -  Bet: %d\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, PlayerArray[i].bet, PlayerArray[i].bet);
            }            
        }
    }
    printf("\033[3D");
    printf("   \n");    
    printf("\nBetting Round %d\n", Params->bet_round);
    printf("%s\rPool: %d\n", SPACE100, Params->pool);

    if (Params->bet_round == 1 && Params->to_call == 2) {
        char first_bet[] = "  <Dealer last rose, however they stil get to bid> ";
        printf("%s\rLast Raised: Player %d (%s) - %d coins%s\n", SPACE100, PlayerArray[Params->button_idx].player_id, PlayerArray[Params->button_idx].player_name, PlayerArray[Params->button_idx].bet, first_bet);
        printf("\n%s\rDEALER CARDS: ", SPACE100);
    } else {
        printf("%s\rLast Raised: Player %d (%s) - %d coins\n", SPACE100, PlayerArray[Params->last_raised].player_id, PlayerArray[Params->last_raised].player_name, PlayerArray[Params->last_raised].bet);
        printf("\n%s\rDEALER CARDS: ", SPACE100);
    }

    // if (Params->bet_round > 1) {
    //     int dealer_cards = 3;
    //     if (Params->bet_round == 3) {
    //         dealer_cards = 4;
    //     } else if (Params->bet_round = 4) {
    //         dealer_cards = 5;
    //     }
    //     for (int i = 0; i <Params->bet_round ; i++) {
    //         printcard(Params->Dealer[i]);
    //     }
        
    // }
    printf("\n%s\rYOUR CARDS: ", SPACE100);
    // if (cards) {
    //     printcard(PlayerArray[Params->current_player].player_cards[0]);   
    //     printcard(PlayerArray[Params->current_player].player_cards[1]);  
    // }
    printf("\n\n%s\r%s\n%s\r%s\n%s\r-> ", SPACE100, constr1, SPACE100, constr2, SPACE100); // print statements
}

void betting_round_start(GameParams *a) {
    printf("\x1b[%dA", a->num_players+12);
    for (int i = 0; i<a->num_players+13; i++) {
        printf("\r%s\n", SPACE100);
    }
    int half = (a->num_players+20)/2;
    printf("\x1b[%dA", half);
    printf("        <ROUND %d>\n\n    <<BETTING ROUND %d>>\n\n        <Pool: %d>", a->round+1, a->bet_round, a->pool);
    for (int i = 0; i<half-3; i++) {
        printf("\n");
    }
    printf("Enter any letter to continue\n-> ");
    char garb;
    scanf(" %c", &garb);
    printf("\x1b[%dA", a->num_players+12);
    for (int i = 0; i<a->num_players+13; i++) {
        printf("\r%s\n", SPACE100);
    }    
}
