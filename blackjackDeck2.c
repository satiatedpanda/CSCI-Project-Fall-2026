#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    int CardDeck[260]; // Array to store 260 total cards
    int index = 0; // Counter to track the current position in the CardDeck array

    srand(time(NULL)); // Seeds the random number generator using clock

    for (int deck = 0; deck < 5; deck++) { // Loop 5 times for 5 decks
        for (int card = 1; card <= 13; card++) { // Loop through card values 1 to 13
            for (int count = 0; count < 4; count++) {
                CardDeck[index] = card; // Store the card value at current index
                index++;                // Move to the next array spot
            }
        }
    }

    for (int i = 0; i < 260; i++) { // Swap each card with a card at a random position
        int random_spot = rand() % 260; // Pick a random array index from 0 to 259
