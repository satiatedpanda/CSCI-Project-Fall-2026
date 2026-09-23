#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*  Figure out how to randomize an array
    How Combine and shuffle the array
    How to store bets
    https://bicyclecards.com/how-to-play/blackjack
*/

int card_value(int cardnum);
int print_card(int cardnum);
int score_cards(int cards[], int card_idx);

int main(void) {
    int Cards[] = {0}; // not what im making
    int inProgress = 0;
    int player_cards[11] = {0};
    int dealer_cards[11] = {0};
    int current_card_idx = 0; // current idx in deck - random card out of 52*5 cards
    srand(time(NULL));
    int CardDecks[260] = {0,14,45,23,27,23,45,44,33,42,21,12}; 
    int deck_idx = 0;
    
    // You get two cards -> dealer gets 1 face up 1 face down card -> you chose your stuff -> dealer does stuff -> reward -> repeat

    // Devin : figure out how to generate a card and randomize a deck to get 5 decks
    // Kevin : figure out how to bet and store bids, while having a set amount to gamble // reward system
    // Jacob : figure out the logic for the dealer and to show the cards delt

    while (inProgress == 0) { // Main game loop
        for (int i = 0; i<11; i++) { // Resets cards to no value
            player_cards[i] = -1;
            dealer_cards[i] = -1;
        }
        int num_player_cards = 0;
        int num_dealer_cards = 0;
        //player starting cards
        player_cards[num_player_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_player_cards++;
        player_cards[num_player_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_player_cards++;
        // check for immediate win
        // stuff
        //dealer starting cards
        dealer_cards[num_dealer_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_dealer_cards++;
        dealer_cards[num_dealer_cards] = CardDecks[deck_idx];
        deck_idx++;
        num_dealer_cards++;
        //player turn
        int player_done = 0;
        int split_num = 0;
        while (player_done == 0) {
            // Hit, Stand, Double Down, Split (the hard one)
            // split can be done a max of 5 times (made so my life is easier, may change if I come up with something else)
            printf("Dealer's Card: ");
            print_card(dealer_cards[0]); // only shows one of the dealer's cards
            printf("\n"); 
            printf("Your Cards: ");            
            for (int i = 0; i < num_player_cards; i++) {
                print_card(player_cards[i]);
            }
            printf("\n");            
            int good_guess = 0;
            char player_input;            
            while (good_guess == 0) {
                int current_score = score_cards(player_cards, num_player_cards);
                if (current_score >= 21) {
                    if (current_score == 21) {
                        printf("Nice Blackjack!\n");
                    } else {
                        printf("Bust!\n");
                    }
                    player_done++;
                    good_guess++; // for instant blackjack OR bust                    
                    continue;
                } 
                if (player_cards[0] == player_cards[1]) {
                    printf("Select An Option\nHit (H), Stand (S), Double Down (D), and Split (P)\n-> ");
                    scanf("%c%*c", &player_input);
                    char valid_inputs[] = "HSDP";
                    for (int i = 0; i<4; i++) { // Input validation, needs further improvement
                        if (valid_inputs[i] == player_input) {
                            good_guess++;
                        }
                    }
                } else {
                    printf("Select An Option\nHit (H), Stand (S), or Double Down (D)\n-> ");
                    scanf("%c%*c", &player_input);
                    char valid_inputs[] = "HSD";
                    for (int i = 0; i<3; i++) { // Input validation, needs further improvement
                        if (valid_inputs[i] == player_input) {
                            good_guess++;
                        }
                    }
                }                

         
            }
            if (player_input == 'H') {
                player_cards[num_player_cards] = CardDecks[deck_idx];
                num_player_cards++;
                deck_idx++;

            } else if (player_input == 'S') {
                player_done++;
            } else if (player_input == 'D') {
                // bet doubles, you get one more card, then you stand
                player_cards[num_player_cards] = CardDecks[deck_idx];
                num_player_cards++;
                deck_idx++;          
                player_done++;
            } else if (player_input == 'P') {
                // skip for now, this one is more complicated
            }            
        }
        // dealer turn
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
        //payout if won, lose if lost
        int won = 0; // 0 = tie, 1 = win, -1 = loss
        int player_score = score_cards(player_cards, num_player_cards);
        int dealer_score = score_cards(dealer_cards, num_dealer_cards);
        if (player_score > 21) {
            won = -1;
        } else {
            if (player_score == 21 && num_player_cards == 2) {
                // this checks for blackjacks. this should multiply current bid by 1.5x // @todo
                won = 1;
            } else if (dealer_score > 21 || player_score > dealer_score) {
                won = 1;
            } else if (dealer_score == player_score) {
                won = 0;
            } else {
                won = -1;
            }
        }
        // money += bet * won
        // if (money == 0) {
        //     inprogress++;   
        // }
        printf("%d\n", won);
        //ask if want to continue, if no set inprogress to 1
        char second_input;
        printf("\nDo you want to continue? Y/n\n-> ");
        scanf("%c%*c", &second_input);
        if (second_input != 'Y' && second_input != 'y') {
            inProgress++;
        } 
    }
    // print total money, and thank user for playing
    printf("Thank you for playing\n");

}

int print_card(int cardnum) {
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
    printf("%c%s", suit, type);
    return 0;
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
        total+= num_ace; // adds all aces as value 1 after the first one
        if (total <= 11) {
            total += 10; // adds the first value as 11 (1 was added already above)
        }
    }
    return total;
}