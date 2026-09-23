#include <math.h>
#include <stdio.h>

int main(void) {

    // Variables be utilized in the if-else statements (Global)
    // https://gist.github.com/justneedtxt/fea5f1d9e58468a62bb6

    int balance; // the initial amount of cents 10000 = 100$
    int bet;     // holds bet value
    int result;  // results from win/losses

    printf("Enter starting balance: \n"); // User enters betting amount
    if (scanf("%d", &balance) != 1) { // !=1 assures 1 integer value is entered
        return 1; // reports error / stops program due to user not entering an
                  // integer
    }

    while (balance > 0) {
        printf(
            "Your balance: $%d\n", balance
        ); // reports balance as long as balance > 0

        printf(
            "Enter bet (type 0 to quit): \n"
        ); // User will enter bet or enter 0 to quit
        if (scanf("%d", &bet) !=
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

            printf(
                "Enter result: 1 = win, 2 = loss, 3 = tie: "
            ); // User will input end results based off game decisions

            if (scanf("%d", &result) !=
                1) { // Results gets assigned to int result

                return 1; // Stops programs when fails to read input
            }
            if (result ==
                1) { // If result = 1 the following statement is processed
                balance = balance + bet * 2;
                printf("You won!\n");
            }

            else if (result == 2) { // if the result = 2 the following statement
                                    // is processed
                printf("You lost!\n");
            }

            else if (result == 3) { // if the result = 3 the following state is
                                    // processed
                balance = balance + bet;
                printf("It's a tie. Your bet was returned\n");
            } else {
                balance =
                    balance + bet; // if input is anything else besides the
                                   // options. Invalid message will prompt.
                printf("invalid result. Your bet has been returned.\n");
            }

            printf(
                "Your updated balance: $%d\n", balance
            ); // Updated balance after wins/losses
        }
    }

    printf(
        "Your final balance is $%d.\n", balance
    ); // Final balance after game exits

    return 0;
}
