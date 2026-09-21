#include <math.h>
#include <stdio.h>

int main(void) {

    // Variables be utilized in the if-else statements (Global)
    // https://gist.github.com/justneedtxt/fea5f1d9e58468a62bb6

    int balance; // the initial amount of cents 10000 = 100$
    int bet;     // holds bet value

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
        // if bet is false. The following statements are considered
        else if (bet < 0) {
            printf("You do not have enough mone\n");
        } else if (bet > balance) {
            printf("You can't place a negative bet\n");
        }
        // Once the previous statements have been verified.
        // The following statement continues
        else {
            balance = balance - bet;
            printf("You placed a bet of %d.\n", bet);
        }
    }

    printf("Your final balance is $%d.\n", balance);

    return 0;
}
