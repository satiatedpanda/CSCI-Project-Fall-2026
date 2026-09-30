#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*  blackjack.c
    Program runs 1 player blackjack in the terminal
    Rules fround from: 
    https://bicyclecards.com/how-to-play/blackjack
*/

int card_value(int cardnum);
void print_card(int cardnum);
int score_cards(int cards[], int card_idx);
void swap(int *a, int *b); // not used
void player_turn(int CardDecks[], int * deck_idx, int * balance, int * bet, int dealer_cards[], int player_cards[5][11], int num_player_cards[], int depth);
// I did not have an idea of how to make this work without recursion (or a lot of if statements)


int main(void) {
    int inProgress = 0; // overall loop condition, either 0 or 1
    int player_cards[10][11] = {{0}}; // player hands
    int dealer_cards[11] = {0}; // dealer hand
    int deck_idx = 0; // overall index for postion in the deck
    int balance; // the initial amount of cents 10000 = 100$
    int bet;     // holds bet value
    int CardDecks[260]; // Array to store 260 total cards
    int index =
        0; // Counter to track the current position in the CardDeck array

    srand(
        time(NULL)
    ); // Seeds the random number generator using the system clock

    for (int deck = 0; deck < 5; deck++) { // Loop 5 times for 5 decks
        for (int card = 0; card <= 51;
             card++) { // Loop through card values 1 to 13
            CardDecks[index] = card; // Store the card value at current index
            index++;                // Move to the next array spot            
        }
    }

    for (int i = 0; i < 260; i++) { // Standard Shuffle: Swap each card with a
                                    // card at a random position
        int random_spot =
            rand() % 260; // Pick a random array index from 0 to 259

        int temp = CardDecks[i]; // Swap CardDeck[i] and CardDeck[random_spot]
                                // using a temporary variable
        CardDecks[i] = CardDecks[random_spot];
        CardDecks[random_spot] = temp;
    }
    printf("     Welcome to Blackjack!\n\nThis game is about trying to get to 21 points from cards.\n");
    printf("Each Card is its value, except for 10-K being worth 10.\nAces are worth either 1 or 10\n");
    printf("You have 4 options each round, Either Hit, Stand, Double Down, or Split\n");
    printf("\nHit-> Get another Card\nStand-> End your turn\nDouble Down -> Double your bet, and get another card\n");
    printf("Split-> Only if your starting cards have the same value, Create a new Hand with another card\n\n");


    printf("Enter starting balance: \n-> "); // User enters betting amount (Kevin)
    if (scanf("%d%*c", &balance) != 1) { // !=1 assures 1 integer value is entered
        return 1; // reports error / stops program due to user not entering an
                  // integer
    }

    while (inProgress == 0 && balance > 0) { // Main game loop Edited by Kevin
        printf(
            "Your balance: $%d\n", balance
        ); // reports balance as long as balance > 0

        printf(
            "Enter bet (type 0 to quit): \n-> "
        ); // User will enter bet or enter 0 to quit
        if (scanf("%d%*c", &bet) !=
            1) {      // User input gets assigned to the integer bet
            return 1; // if user type anything besides an integer. Program quits
        }
        if (bet == 0) { // If user enters '0' program will break
            break;      // Exits loop. Skips else if statements and prints final
                        // balance
        }
        // if bet is not == 0. The following statements are considered
        else if (bet < 0) {
            printf("You do not have enough money\n");
        } else if (bet > balance) {
            printf("You do not have enough money\n");
        }
        // Once the previous statements have been verified.
        // The following statement continues
        else {
            balance = balance - bet; // New balance value substracting from bet
            printf("You placed a bet of $%d.\n", bet); // Shows the bet entered
        }
        for (int i = 0; i < 11; i++) { // Resets cards to no value
            dealer_cards[i] = -1;
            for (int j = 0; j < 10; j++)
            {
                player_cards[j][i] = -1;
            }               
        }
        int num_player_cards[10] = {0};
        int num_dealer_cards = 0;
        // dealer starting cards
        dealer_cards[num_dealer_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_dealer_cards++;
        dealer_cards[num_dealer_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_dealer_cards++;
        // player starting cards
        player_cards[0][num_player_cards[0]] = CardDecks[deck_idx];
        deck_idx++;
        num_player_cards[0]++;
        player_cards[0][num_player_cards[0]] = CardDecks[deck_idx];
        deck_idx++;
        num_player_cards[0]++;
        // player turn
        int player_done = 0;
        int split_num = 0;
        // player turn
        player_turn(CardDecks, &deck_idx, &balance, &bet, dealer_cards, player_cards, num_player_cards, 0);
        // dealer turn
        printf("\n\n-- Dealers Turn --\n");        
        printf("Dealer Cards: ");
        for (int i = 0; i < num_dealer_cards; i++) {
            print_card(dealer_cards[i]);
        }
        int dealer_done = 0;
        while (dealer_done == 0) {
            if (score_cards(dealer_cards, num_dealer_cards) > 16) {
                dealer_done++;
            } else {
                dealer_cards[num_dealer_cards] = CardDecks[deck_idx];
                print_card(dealer_cards[num_dealer_cards]);
                num_dealer_cards++;
                deck_idx++;
            }
        }
        printf("\n");
        int split_depth = 0;
        for (int k = 0; k < 10; k++)
        {
            if (num_player_cards[k] < 1) {
                split_depth = k;
                break;
            }
        }        
        for (int k = 0; k < split_depth; k++)
        {
            // payout bet
            int won = 0; // 0 = tie, 1 = win, -1 = loss
            int player_score = score_cards(player_cards[k], num_player_cards[k]);
            int dealer_score = score_cards(dealer_cards, num_dealer_cards);
            if (player_score > 21) {
                won = -1;
            } else {
                if (player_score == 21 && num_player_cards[0] == 2) {
                    // this checks for blackjacks. this should multiply current bid
                    // by 1.5x
                    bet *= 1.5;
                    won = 1;
                    printf("You Won!\n");
                } else if (dealer_score > 21 || player_score > dealer_score) {
                    won = 1;
                    printf("You Won!\n");
                } else if (dealer_score == player_score) {
                    won = 0;
                    printf("You Tied!\n");
                } else {
                    won = -1;
                    printf("You Lost!\n");
                }
            }            
            // Kevin
            if (won == 1) {
                balance = balance + (bet * 2);
                printf("\nYou won $%d!\n", bet);
            } else if (won == 0) {
                balance = balance + bet;
                printf("\nIts a tie. Your bet was returned\n");
            } else {
                printf("\nYou lost $%d.\n", bet);
            }
            printf("Updated balance : $%d\n", balance);

            if (balance <= 0) {
                printf("\nYou are out of money!\n");
                break;
            }
            // printf("Player: %d Dealer: %d\n", player_score, dealer_score);
        }
        

    // ask if want to continue, if no set inprogress to 1
    if (balance > 0) {
        char second_input;
            printf("\nDo you want to continue? Y/n\n-> ");
            scanf("%c%*c", &second_input);
            if (second_input != 'Y' && second_input != 'y') {
                inProgress++;
            }
        } else {
            inProgress++;
        }
    }   
    // print total money, and thank user for playing
    printf("Thank you for playing\n\n");
}

void player_turn(int CardDecks[], int * deck_idx, int * balance, int * bet, int dealer_cards[], int player_cards[5][11], int num_player_cards[], int depth) {
    /*
    idea for pointers came from: https://stackoverflow.com/questions/23667497/update-int-variable-in-c-inside-a-function
    */
    int player_done = 0;
    int has_split = 0;
    if (depth != 0)
    {
        printf("\n\nCurrent Split: %d\n", depth);
    }
    
    while (player_done == 0) {
        // Hit, Stand, Double Down, Split (the hard one)
        // split can be done a max of 5 times (made so my life is easier,
        // may change if I come up with something else)
        int good_guess = 0;
        char player_input;
        while (good_guess == 0) {
            printf("Dealer's Card: ");
            print_card(dealer_cards[0]); // only shows one of the dealer's cards
            printf("\n\n");
            printf("Your Cards: ");
            for (int i = 0; i < num_player_cards[depth]; i++) {
                print_card(player_cards[depth][i]);
            }
            printf("\n");             
            int current_score = score_cards(player_cards[depth], num_player_cards[depth]);
            if (current_score >= 21) {
                if (current_score == 21) {
                    if (num_player_cards[depth] == 2) {
                    printf("Nice Blackjack!\n");
                    } else {
                    printf("Nice 21!\n");
                    }
                } else if (current_score > 21) {
                    printf("Bust!\n");
                    printf("Your Score: %d. (Max 21)\n", current_score);
                }
                player_done++;
                good_guess++; // for instant blackjack OR bust
                player_input = 'Z';
                continue;
            }           
            if ((player_cards[depth][0] == player_cards[depth][1]) && (depth < 9) && (has_split == 0)) {
                printf(
                    "Select An Option\nHit (H), Stand (S), Double Down "
                    "(D), and Split (P)\n-> "
                );
                scanf("%c%*c", &player_input);
                char valid_inputs[] = "HSDP";
                for (int i = 0; i < 4;
                        i++) { // Input validation, needs further improvement
                    if (valid_inputs[i] == player_input) {
                        good_guess++;
                    }
                }
            } else {
                printf(
                    "Select An Option\nHit (H), Stand (S), or Double Down "
                    "(D)\n-> "
                );
                scanf("%c%*c", &player_input);
                char valid_inputs[] = "HSD";
                for (int i = 0; i < 3;
                        i++) { // Input validation, needs further improvement
                    if (valid_inputs[i] == player_input) {
                        good_guess++;
                    }
                }
            }
        }
        if (player_input == 'H') {
            player_cards[depth][num_player_cards[depth]] = CardDecks[*deck_idx];
            num_player_cards[depth]++;
            *deck_idx += 1;

        } else if (player_input == 'S') {
            player_done++;
        } else if (player_input == 'D') {
            if (*balance >= *bet) {
                *balance = *balance - *bet;
                *bet = *bet * 2;
                printf("Bet doubled to %d.\n", *bet);

                // bet doubles, you get one more card, then you stand
                player_cards[depth][num_player_cards[depth]] = CardDecks[*deck_idx];
                printf("Your New Card is: ");
                print_card(player_cards[depth][num_player_cards[depth]]);    
                printf("\n");            
                num_player_cards[depth]++;
                *deck_idx += 1;
                int current_score = score_cards(player_cards[depth], num_player_cards[depth]);
                if (current_score > 21) {
                    printf("Bust!\n");
                    printf("Your Score: % (Max 21).\n", current_score);    
                } else if (current_score == 21){
                    printf("Nice 21!\n");
                }
                player_done++;                           
                
            } else {
                printf("Not enough money to double down.\n");
            }
        } else if (player_input == 'P') {
            player_cards[depth+1][0] = player_cards[depth][1];
            player_cards[depth][1] = CardDecks[*deck_idx];
            *deck_idx += 1;
            player_cards[depth+1][1] = CardDecks[*deck_idx];
            *deck_idx += 1;
            num_player_cards[depth+1] = 2;
            has_split == 1;
            *balance = *balance - *bet;
            player_turn(CardDecks, deck_idx, balance, bet, dealer_cards, player_cards,num_player_cards, depth+1);
        }
    }

}

void print_card(int cardnum) {
    if (cardnum < 0 || cardnum > 51) {
        printf("cardnum out of bounds");
        exit(2);
    }
    char suits[] = "SCDH";
    char suit = suits[cardnum / 13];
    char type[3] = "  ";
    int temp;
    temp = cardnum % 13;
    temp += 1;
    if (temp == 1) {
        type[0] = 'A';
    } else if (temp < 10) {
        type[0] = temp + '0';
    } else if (temp == 10) {
        type[0] = '1';
        type[1] = '0';
    } else {
        temp -= 11;
        char convert[] = "JQK";
        type[0] = convert[temp];
    }
    for (int i = 3; i>= 0; i--) {
        if (type[i] == ' ') {
            type[i] = '\0';
        }
    }
    printf("[%c%s] ", suit, type);
}

int card_value(int cardnum) {
    if (cardnum < 0 || cardnum > 51) {
        printf("cardnum out of bounds");
        exit(1);
    }
    // 0-51
    int remainder;
    remainder = cardnum % 13;
    // A 2 3 4 5 6 7 8 9 10 J  Q  K
    // 0 1 2 3 4 5 6 7 8 9 10 11 12
    // 1 2 3 4 5 6 7 8 9 10 11 12 13 // +1
    remainder += 1;
    if (remainder >= 10) {
        remainder = 10;
    }
    return remainder;
}

int score_cards(int cards[], int card_idx) {
    int num_ace = 0;
    int total = 0;
    for (int i = 0; i < card_idx; i++) {
        int cur_val = card_value(cards[i]);
        if (cur_val == 1) {
            num_ace++;
        } else {
            total += cur_val;
        }
    }
    if (num_ace > 0) {
        total += num_ace; // adds all aces as value 1 after the first one
        if (total <= 11) {
            total +=
                10; // adds the first value as 11 (1 was added already above)
        }
    }
    return total;
}

void swap(int *a, int *b) {
    /* NOT USED - Was Jacob's Alternate solution for swapping elements; Devin came up with the version used above
    idea came from: https://stackoverflow.com/questions/25925603/swap-function-of-elements-in-array
    and https://www.youtube.com/watch?v=4KdvcQKNfbQ - uses no itermediary variables
    */
    // int temp = a;
    // a = b;
    // b = temp;

    *a = *b^*a;
    *b = *a^*b;
    *a = *b^*a;
}