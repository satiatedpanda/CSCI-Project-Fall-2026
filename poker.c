/**
 * @file poker.c
 * @author Jacob, Devin, Kevin
 * @brief Small program to run a poker game in the terminal
 * @version 1.0
 * @date 2026-11-01
 *
 * @copyright Copyright (c) 2026
 *
 * rules based off of https://officialpokerrules.com
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char suit; // suit of card
    char value; // value of card
} Card;

typedef struct {
    Card array[52]; // card deck
    size_t deck_idx; // index of last delt card
} CardDeck;

typedef struct {
    Card player_cards[2]; // cards
    int balance; // coin balance
    int bet; // current bet for bet round
    int total_bet; // total bet in overall round
    int in_play; // if not folded
    int player_id; // id
    char player_name[8]; // name
} Player;

typedef struct {
    int round; // round number
    int num_players; // number of players STILL in game
    int button_idx; // idx of button player
    int bet_round; // bet round number
    int pool; // total of player's total_bets
    int current_player; // current player idx in betting round
    int last_raised; // last raised player idx in betting round
    int to_call; // highest bid by player in betting round
    Card Dealer[5]; // dealer's cards
} GameParams;

const char SPACE100[] = "                                                                                                   ";

void init_deck(CardDeck *a);
void init_card(Card *a);
void init_player(Player *a, int idx);
void init_param(GameParams *a);
void printcard(Card a);
int isAlphabetOnly(char str[]);
void empty_stdin (void);
int hand_rankings(Player* a, GameParams* d);
void bettinground(Player PlayerArray[], GameParams *Params);
int place_bet(Player PlayerArray[], GameParams *Params, int idx);
int num_in_play(Player PlayerArray[], int num_players);
void shuffle(CardDeck *a);
void print_game(Player PlayerArray[], GameParams *Params, char constr1[], char constr2[], int cards, int move_up);
void betting_round_start(GameParams *a);
char* hand_name(int score);

    /**
     * @brief Main function, includes the main game itself
     * Will continue to loop in main game loop until only a single player remains
     */
int main(void) {
    printf("NOTE:\nPlease enlarge your terminal before playing, the game will malfunction if the terminal window is too small\n\n");
    GameParams Params;
    init_param(&Params);
    srand(time(NULL));
    rand(); // flushes first call which is weirdly predictable
    printf("\n\nDo you need to read the rules? Y for yes, anything else for no\n-> ");
    char check1; // garbage string just for this first check
    scanf(" %c", &check1);
    empty_stdin();
    if (check1 == 'Y') {
        printf("Read the rules here:\nGeneral Rules-> https://beginnerpoker.com/holdem/rules.html\nHand Rankings-> https://beginnerpoker.com/fundamentals/hand-rankings.html\n\n");
        // im sorry I ran out of time, and this explains the rules better than I could anyways
    }
    do {
        printf("How many Players will be playing? Input an integer from 2-10\n-> ");
        if (!scanf(" %d", &Params.num_players)) {
            printf("Invalid type input\n");
            exit(102);
        }
        empty_stdin();
        if (Params.num_players > 1 && Params.num_players < 11) {
            break;
        } else {
            printf("Invalid input, please input a valid number between 1-10\n");
        }
    } while (1);
    printf("%d players\n", Params.num_players);
    printf("\n\nPre-Assign names? Y for yes, anything else for no\n-> ");
    char check2; // garbage string just for this second check
    scanf(" %c", &check2);
    empty_stdin();
    Player PlayerArray[Params.num_players];
    Params.button_idx = rand() % Params.num_players;
    char namearray[10][6] = {"Adam", "Bob", "Tim", "Emily", "James", "Ash", "Shay", "Jack", "Mary", "Cindy"}; // mainly used for testing purposes
    for (int i = 0; i<Params.num_players; i++) { // initilizes all players
        Player a;
        init_player(&a, i);
        PlayerArray[i] = a;
        if (check2 == 'Y') {
            strcpy(PlayerArray[i].player_name, namearray[i]);
        }
    }
    if (check2 != 'Y') {
        for (int i = 0; i<Params.num_players; i++) { // assigns names
            while(1) {
                char temp[6]; // temp string to check if its valid
                printf("\nEnter Player %d's name (5 characters or less, letters only, no spaces)\n-> ", PlayerArray[i].player_id);
                // https://stackoverflow.com/questions/1247989/how-do-you-allow-spaces-to-be-entered-using-scanf - restricts spaces instead
                if (!scanf(" %5[^\n]", temp)) {
                    // is it possible to fail this?
                    printf("Invalid type input\n");
                    exit(104);
                } else {
                    empty_stdin();
                    // this removes all characters left over in the scanf
                    if (isAlphabetOnly(temp)) {
                        strcpy(PlayerArray[i].player_name, temp);
                        break;
                    }
                    printf("Invalid type input - failed alphabet only test\n");
                }
            }
        }
    }
    CardDeck Deck; // main game deck
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
                if (Params.button_idx == i) {
                    Params.button_idx = i-1; // fixes button skipping players if the current button player gets eliminated
                }
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
            Deck.deck_idx = 0; // reset deck idx
        }
        for (int i = 0; i<Params.num_players; i++) {
            PlayerArray[i].player_cards[0] = Deck.array[Deck.deck_idx];
            Deck.deck_idx += 1;
            PlayerArray[i].player_cards[1] = Deck.array[Deck.deck_idx];
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
        print_game(PlayerArray, &Params, "", "", 0, 1); // removes shown player cards
        printf("\n\n\n\n\n");
        printf("Round Over!\n\nEnter any letter to reveal winner!\n-> ");
        char garbs;
        scanf(" %c", &garbs);
        empty_stdin();
        printf("- - - - - - - - - - - - - - - - - - - - - - - - - -\n\n");
        // find inital winner(s)
        if (num_in_play(PlayerArray, Params.num_players) == 1) {
            for (int i = 0; i<Params.num_players; i++) {
                if (PlayerArray[i].in_play) {
                    PlayerArray[i].balance += Params.pool;
                    printf("\nPlayer %s wins this round! They won %d chips\n", PlayerArray[i].player_name, Params.pool);
                }
            }
        } else { // reveal turn, decide winner
            int player_scores[10] = {0};
            int winners[10] = {0};
            int highestscore = 0;
            int num_highest = 1;
            printf("Dealer's Cards: ");
            for (int k = 0; k<5; k++) {
                printcard(Params.Dealer[k]);
            }
            printf("\n");
            for (int k = 0; k<Params.num_players; k++) {
                if (PlayerArray[k].in_play) {
                    player_scores[k] = hand_rankings(&PlayerArray[k], &Params); // all folded players are 0 anyways so they dont matter- this preserves idx
                    printf("%s's (P%d) Cards: ", PlayerArray[k].player_name, PlayerArray[k].player_id);
                    printcard(PlayerArray[k].player_cards[0]);
                    printcard(PlayerArray[k].player_cards[1]);
                    int score = hand_rankings(&PlayerArray[k], &Params);
                    printf("      %s", hand_name(score));
                    printf("\n");
                }
            }
            for (int k = 0; k<Params.num_players; k++) {
                if (player_scores[k] >= highestscore) {
                    if (highestscore == player_scores[k]) {
                        num_highest++;
                    } else {
                        highestscore = player_scores[k];
                        num_highest = 1;
                    }
                }
            }
                // @todo impliment all-ins https://officialpokerrules.com/betting-structures/#incomplete-raises
            int n = 0;
            for (int k = 0; k<Params.num_players; k++) {
                if (highestscore == player_scores[k]) {
                    winners[n] = k; // save idx for later
                    n++;
                    if (n == num_highest) {
                        break;
                    }
                }
            }
            int indx_after_button = 15;
            for (int k = 0; k<Params.num_players; k++) {
                int current_player = (Params.button_idx+1+k)%Params.num_players;
                int match = 0;
                for (int i = 0; i<num_highest; i++) {
                    if (current_player == winners[i]) {
                        match = 1;
                        break;
                    }
                }
                if (match) {
                    indx_after_button = current_player;
                    break;
                }
            }
            if (indx_after_button == 15) {
                printf("idx not matched");
                exit(192);
            }
            // apply winnings (player[i].balance += pool)
            int avg_winnings = Params.pool / num_highest;
            int extra = Params.pool % num_highest;
            for (int k = 0; k<num_highest; k++) {
                // int current_player = (k+Params.button_idx)%Params.num_players;
                int total_earned = 0;
                if (winners[k] == indx_after_button) {
                    total_earned = avg_winnings + extra;
                } else {
                    total_earned = avg_winnings;
                }
                PlayerArray[winners[k]].balance += total_earned;
                if (num_highest != 1) {
                    printf("\nPlayer %d (%s) Tied! They won %d chips.\nHand: %s\n", PlayerArray[winners[k]].player_id, PlayerArray[winners[k]].player_name, total_earned, hand_name(player_scores[winners[k]]));
                } else {
                    printf("\nPlayer %d (%s) Won! They earn %d chips.\nHand: %s\n", PlayerArray[winners[k]].player_id, PlayerArray[winners[k]].player_name, total_earned, hand_name(player_scores[winners[k]]));
                }
            }
        }
        printf("\n\nEnter any letter to continue\n-> ");
        char garb;
        scanf(" %c", &garb);
        empty_stdin();
        printf("\n\n\n\n\n\n\n");
        Params.round += 1;
        }

    printf("\n\n\nGame Over! Last Player standing, %s (P%d) Wins!\n", PlayerArray[0].player_name, PlayerArray[0].player_id);
    return 0;
}

    /**
     * @brief Initilizes a CardDeck a with base values
     * @param a A Deck of Cards, with idx of 0-51
     * Made by: Devin
     */
void init_deck(CardDeck *a) {
    char order[] = "23456789TJQKA";
    char suits[] = "HDCS";
    a->deck_idx = 0;
    for (int i = 0; i<52; i++) {
        Card b;
        init_card(&b);
        a->array[i] = b;
        a->array[i].suit = suits[i/13];
        a->array[i].value = order[i%13];
    }
}

    /**
     * @brief Initilizes a Card a with base values
     * @param a A Card, with empty char values
     */
void init_card(Card *a) {

    a->suit = '\0';
    a->value = '\0';
}

    /**
     * @brief Initilizes a Player a with base values
     * @param a A Player object to be initilized
     * @param idx Player ID value, no ids can match
     */
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

    /**
     * @brief Initilizes Game Paramaters a with base values
     * @param a Struct of values critical for the game
     */
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

    /**
     * @brief Prints a card, in form [SV] where S is the suit and V is the value
     */
void printcard(Card a) {
    if (a.value == 'T') {
        printf("[%c%s] ", a.suit, "10");
    } else {
        printf("[%c%c] ", a.suit, a.value);
    }
}

    /**
     * @brief Returns true if the passed str has only alphabet characters
     * @param str String to be checked
     * found at: https://www.tutorialkart.com/c-programming/how-to-check-if-a-string-contains-only-alphabets-in-c/
     */
int isAlphabetOnly(char str[]) {
    int i = 0;
    while (str[i] != '\0') {
        if (!isalpha(str[i])) {
            return 0; // Non-alphabet character found
        }
        i++;
    }
    return 1; // String contains only alphabets
}

/**
 * @brief Removes all leftover characters in buffer from scanf
 */
void empty_stdin (void) {
    int c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

/**
 * @brief The entire current betting round
 *
 * @param PlayerArray Array of players
 * @param Params General game paramaters
 */
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
    betting_round_start(Params); // prints a nice thing showing the start of the betting round
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
            char constcr2[] = "Enter a single character to continue";
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

/**
 * @brief Asks for user input, places and assigns user bet for betting round
 *
 * @param PlayerArray Array of players
 * @param Params General game paramaters
 * @param idx current player
 * @return int bet made
 */
int place_bet(Player PlayerArray[], GameParams *Params, int idx) {
    int bid = 0;
    int player_input;
    int difference;
    int invalid_trigger = 0;
    difference = Params->to_call - PlayerArray[Params->current_player].bet;
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
        if (invalid_trigger) {
            printf("%s\r            <Invalid Option>\r-> ", SPACE100);
        } else {
            printf("%s\r-> ", SPACE100);
        }

        if (!scanf(" %d", &player_input)) {
            printf("Invalid type input\n");
            exit(102);
        }
        empty_stdin();
        if (player_input == -1) { // fold
            printf("\x1b[A");
            printf("-> ");
            PlayerArray[Params->current_player].in_play = 0;
            bid = 0;
            break;
        } else if (player_input > difference && player_input <= PlayerArray[Params->current_player].balance) { // raise
            printf("\x1b[A");
            printf("-> ");
            Params->last_raised = idx;
            PlayerArray[Params->current_player].bet = player_input + PlayerArray[Params->current_player].bet;
            Params->to_call = PlayerArray[Params->current_player].bet;
            bid = player_input;
            break;
        } else if (player_input == difference || (all_in_trigger && player_input == PlayerArray[Params->current_player].balance)) { // call OR all in if cant call
            printf("\x1b[A");
            printf("-> ");
            PlayerArray[Params->current_player].bet = player_input + PlayerArray[Params->current_player].bet;
            bid = player_input;
            break;
        } else { // invalid option
            // this stupid invalid trigger caused a lot of pain to add
            invalid_trigger = 1;
        }
    }
    PlayerArray[Params->current_player].balance -= bid;
    return bid;
}

/**
 * @brief Returns number of players that are still in play
 *
 * @param PlayerArray Array of Players
 * @param num_players Number of Players in Array
 * @return int number of players active in current round
 */
int num_in_play(Player PlayerArray[], int num_players) {
    int total = 0;
    for (int i=0; i<num_players; i++) {
        if (PlayerArray[i].in_play == 1) {
            total++;
        }
    }
    return total;
}


/**
 * @brief Shuffles the deck using  Fisher-Yates shuffle
 *
 * @param a CardDeck a
 *Made by Devin
 */
void shuffle(CardDeck *a) {
    for (int i = 0; i < 52; i++) {
        int random_spot = rand() % 52;
        Card temp = a->array[i];
        a->array[i] = a->array[random_spot];
        a->array[random_spot] = temp;
    }
}

/**
 * @brief Handles 90% of the printing of the game, all in one place for nice printing
 *
 * @param PlayerArray Array of Players
 * @param Params Game Paramaters
 * @param constr1 Question string 1
 * @param constr2 Question string 2
 * @param cards Bool to print player cards or not
 * @param move_up int deciding if you override previous prints or not, and by how many lines up (0=no override, 1= override, 2+ = how many lines of override)
 */
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
            for (size_t k = 0; k<5-strlen(PlayerArray[i].player_name); k++) {
                name_length_spaces[k] = ' ';
            }
            name_length_spaces[5-strlen(PlayerArray[i].player_name)] = '\0';
        }
        int inPool = 0;
        inPool = PlayerArray[i].bet + PlayerArray[i].total_bet;
        if (Params->current_player == i) {
            printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d  -  Bet: %d    !<CURRENT PLAYER>!\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, inPool, PlayerArray[i].bet);
        } else {
            if (strlen(is_folded) > 5) {
                printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d                %s\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, inPool, is_folded);
            } else {
                printf("%s\r%sPlayer %d%c (%s)%s - Balance: [%d] - inPool: %d  -  Bet: %d\n", SPACE100, is_button, PlayerArray[i].player_id, is_single_digit, PlayerArray[i].player_name, name_length_spaces, PlayerArray[i].balance, inPool, PlayerArray[i].bet);
            }
        }
    }
    printf("\033[3D");
    printf("   \n%s\r", SPACE100);
    printf("\n%s\rBetting Round %d\n", SPACE100, Params->bet_round);
    printf("%s\rPool: %d\n", SPACE100, Params->pool);

    if (Params->bet_round == 1 && Params->to_call == 2) {
        char first_bet[] = "  <Button last rose, however they stil get to bid> ";
        printf("%s\rLast Raised: Player %d (%s) - %d chips%s\n", SPACE100, PlayerArray[Params->button_idx].player_id, PlayerArray[Params->button_idx].player_name, PlayerArray[Params->button_idx].bet, first_bet);
    } else {
        printf("%s\rLast Raised: Player %d (%s) - %d chips\n", SPACE100, PlayerArray[Params->last_raised].player_id, PlayerArray[Params->last_raised].player_name, PlayerArray[Params->last_raised].bet);
    }
    printf("\r%s\n%s\rDEALER CARDS: ", SPACE100, SPACE100);
    if (Params->bet_round > 1) {
        int dealer_cards = 3;
        if (Params->bet_round == 3) {
            dealer_cards = 4;
        } else if (Params->bet_round == 4) {
            dealer_cards = 5;
        }
        for (int i = 0; i <dealer_cards ; i++) {
            printcard(Params->Dealer[i]);
        }
    }
    printf("\n%s\rYOUR CARDS: ", SPACE100);
    if (cards) {
        printcard(PlayerArray[Params->current_player].player_cards[0]);
        printcard(PlayerArray[Params->current_player].player_cards[1]);
    }
    printf("\n%s\n", SPACE100); // empty space
    printf("%s\r%s\n", SPACE100, constr1); // question line 1
    printf("%s\r%s\n", SPACE100, constr2);  // question line 2
    printf("%s\r-> ", SPACE100); // answer line
}

/**
 * @brief Prints a nice thing showing a new betting round has started
 *
 * @param a Game Paramaters
 */
void betting_round_start(GameParams *a) {
    printf("\x1b[%dA", a->num_players+13);
    for (int i = 0; i<a->num_players+14; i++) {
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
    empty_stdin();
    printf("\x1b[%dA", a->num_players+13);
    for (int i = 0; i<a->num_players+14; i++) {
        printf("\r%s\n", SPACE100);
    }
}

/**
 * @brief Looks for 5 cards in a row
 *
 * @param have have[5] is 1 if we have a 5, 0 if we don't.
 * @return int Gives back the TOP card of the straight, or 0 if theres not one.
 * Made by Kevin
 */
int find_straight(int have[15]) {
    // Try each possible top card, from ace (14) down to 5.
    for (int top = 14; top >= 5; top--) {
        int in_a_row = 0;

        // Count how many of the 5 cards (top, top-1, top-2, top-3, top-4) we have.
        for (int down = 0; down < 5; down++) {
            int number = top - down;
            if (number == 1) {
                number = 14; // the ace can also be low: A-2-3-4-5
            }
            if (have[number] == 1) {
                in_a_row++;
            }
        }

        if (in_a_row == 5) {
            return top;
        }
    }
    return 0; // no straight
}


/**
 * @brief Gives the players best hand a score. Bigger score = better hand.
 *
 * @param a Current Player
 * @param d Game Paramater d (for flop cards)
 * @return int Score of hand, from 10000-100000, highest being the best hand
 * Made by Kevin
 */
int hand_rankings(Player* a, GameParams* d) {
    char order[] = "23456789TJQKA"; // weakest to strongest (spot + 2 = the card's number)
    char suits[] = "HDCS";

    int number[7] = {0}; // the 7 cards as numbers (2 to 14)
    char suit[7];        // the 7 cards suits

    //  Get the 7 cards: 2 from the player and 5 from the dealer.
    for (int card = 0; card < 7; card++) {
        char letter;

        if (card < 2) {
            letter = a->player_cards[card].value;
            suit[card] = a->player_cards[card].suit;
        } else {
            letter = d->Dealer[card - 2].value; // dealer cards start at 0, so -2
            suit[card] = d->Dealer[card - 2].suit;
        }

        // Turn the letter into a number.
        for (int spot = 0; spot < 13; spot++) {
            if (letter == order[spot]) {
                number[card] = spot + 2;
            }
        }
    }

    //  Count the cards. count[9] = how many 9s we have. have[9] = 1 if we have a 9.
    int count[15] = {0};
    int have[15] = {0};
    for (int card = 0; card < 7; card++) {
        count[number[card]]++;
        have[number[card]] = 1;
    }

    //  Check for a flush (5 or more cards of the same suit).
    int flush = 0;             // 1 if we have a flush
    int flush_have[15] = {0};  // the numbers we have in the flush suit
    int flush_high = 0;        // the biggest card in the flush

    for (int which_suit = 0; which_suit < 4; which_suit++) {
        int how_many = 0;
        for (int card = 0; card < 7; card++) {
            if (suit[card] == suits[which_suit]) {
                how_many++;
            }
        }

        if (how_many >= 5) {
            flush = 1;
            for (int card = 0; card < 7; card++) {
                if (suit[card] == suits[which_suit]) {
                    flush_have[number[card]] = 1;
                    if (number[card] > flush_high) {
                        flush_high = number[card];
                    }
                }
            }
        }
    }

    // Check for straights.
    int straight_top = find_straight(have);  // 5 in a row, any suits
    int straight_flush_top = 0;              // 5 in a row, all the same suit
    if (flush == 1) {
        straight_flush_top = find_straight(flush_have);
    }

    // Check for four of a kind, three of a kind, and pairs (biggest cards first).
    int four = 0;       // the card we have four of (0 = none)
    int three = 0;      // the card we have three of (0 = none)
    int pairs = 0;      // how many pairs we have
    int high_pair = 0;  // bigger pair
    int low_pair = 0;   // smaller pair

    for (int value = 14; value >= 2; value--) {
        if (count[value] >= 4) {
            four = value;
        } else if (count[value] == 3 && three == 0) {
            three = value;
        } else if (count[value] >= 2) {
            // a pair (a second three of a kind counts as a pair too)
            pairs++;
            if (pairs == 1) {
                high_pair = value;
            }
            if (pairs == 2) {
                low_pair = value;
            }
        }
    }
    // Checking  the hands from best to worst
    // The first one that fits is the answer.
    // The score is hand number * 10000 + main card * 100 + second card. A higher hand number always wins. The cards only break ties.


    // Royal flush: a straight flush with an ace on top
    if (straight_flush_top == 14) {
        return 10 * 10000;
    }
    // Straight flush
    if (straight_flush_top > 0) {
        return 9 * 10000 + straight_flush_top * 100;
    }
    //  Four of a kind
    if (four > 0) {
        return 8 * 10000 + four * 100;
    }
    // Full house (three of a kind + a pair)
    if (three > 0 && pairs >= 1) {
        return 7 * 10000 + three * 100 + high_pair;
    }
    // Flush
    if (flush == 1) {
        return 6 * 10000 + flush_high * 100;
    }
    //  Straight
    if (straight_top > 0) {
        return 5 * 10000 + straight_top * 100;
    }
    // Three of a kind
    if (three > 0) {
        return 4 * 10000 + three * 100;
    }
    // Two pair
    if (pairs >= 2) {
        return 3 * 10000 + high_pair * 100 + low_pair;
    }
    // One pair
    if (pairs == 1) {
        return 2 * 10000 + high_pair * 100;
    }

    // High card: the score is just our biggest card
    int highest = 0;
    for (int card = 0; card < 7; card++) {
        if (number[card] > highest) {
            highest = number[card];
        }
    }
    return 1 * 10000 + highest * 100;
}


/**
 * @brief Turns the score back into the hand's name to print it.
 *
 * @param score player's best hand score
 * @return char* string of the hand name
 */
char* hand_name(int score) {
    int hand = score / 10000; // the hand number is the front of the score

    if (hand == 10) return "Royal flush";
    if (hand == 9) return "Straight flush";
    if (hand == 8) return "Four of a kind";
    if (hand == 7) return "Full house";
    if (hand == 6) return "Flush";
    if (hand == 5) return "Straight";
    if (hand == 4) return "Three of a kind";
    if (hand == 3) return "Two pair";
    if (hand == 2) return "One pair";
    return "High card";
}