extern "C" {

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <direct.h>
#include <time.h>
#include <Windows.h>
#include "cards_globals.h"
#include "cards_structs.h"
#include "cards_suits.h"
#include "cards_values.h"
#include "cards.h"
#include "decks.h"
#include "cards_test.h"
#include "blackjack_globals.h"
#include "blackjack_structs.h"
#include "blackjack_hands.h"
#include "blackjack_players.h"
#include "blackjack_table.h"
#include "blackjack.h"
#include "blackjack_UIX.h"
#if ( UIX == 0 )
#include "blackjack_CLI.h"
#elif ( UIX == 1 )
#include "blackjack_GUI.h"
#endif
#include "blackjack_run.h"



	__declspec(dllexport) table *DLLcreateTable(int NO_OF_PLAYERS, int NO_OF_COMPS) {

		table *newTable;
		newTable = (table *)malloc(sizeof(table));
		_initTable(newTable, NO_OF_PLAYERS, NO_OF_COMPS);
		return newTable;
	}

	__declspec(dllexport) void DLL_initTable(table *t, int NO_OF_PLAYERS, int NO_OF_COMPS) {

		int i, j = 0;
		char comp[10], playerNo[2], playerNoPtr[2];
		assert(t);

		t->dealer = createPlayer("Dealer", 0, 1, 1);
		for (i = 0; i < NO_OF_PLAYERS - NO_OF_COMPS; i++) {
			t->players[i] = createPlayer(playerArr[i], (i + 1), 0, 0);
			j = i + 1;
		}
		t->NO_OF_PLAYERS = NO_OF_PLAYERS;

		for (i = 0; i< NO_OF_COMPS; i++) {
			playerNo[0] = (char)(((int)'0') + i + 1);/*convert int to char*/
			playerNo[1] = '\0';/*make string from char by adding endline character*/
			strcpy_s(playerNoPtr, sizeof playerNoPtr, playerNo);
			strcpy_s(comp, sizeof comp, "Computer");
			strcat_s(comp, sizeof comp, playerNoPtr);
			t->players[j] = createPlayer(comp, (j + 1), 0, 1);
			j++;
			//fgfdfdfdg
		}
		t->NO_OF_COMPS = NO_OF_COMPS;

		/*create a deck to be pointed to by discardPile*/
		t->discardPile = (deck *)malloc(sizeof(deck));

		/*create array of pointers to struct card for discard pile*/
		for (i = 0; i < NO_OF_CARDS; i++) {
			t->discardPile->cards[i] = NULL;
		}
		t->discardPile->cards_left = 0;
		t->discardPile->top = NULL;

		t->currPlayer = 0;
		t->spacing = 0;
		t->margin = (int)(10 - (t->NO_OF_PLAYERS * 2.5));
		t->handsAreDealt = 0;
		t->hasSplits = 0;
	}

	__declspec(dllexport) table *DLLSetTable(int NO_OF_PLAYERS, int NO_OF_COMPS, char *names[]) {

		int no_Total_Players, i;

		for (i = 0; i < NO_OF_PLAYERS; i++) {
			strcpy_s(playerArr[i], sizeof playerArr[i], names[i]);
		}

		no_Total_Players = NO_OF_PLAYERS + NO_OF_COMPS;

		return createTable(no_Total_Players, NO_OF_COMPS);
	}

	__declspec(dllexport) deck *DLLcreateDeck() {

		struct deck *newDeck = (deck *)malloc(sizeof(deck));
		_initDeck(newDeck);
		return newDeck;
	}

	__declspec(dllexport) deck *DLLshuffle(deck *d) {

		time_t t;
		int i, n, cardsLeft;
		deck *shuffled;
		assert(d && !isEmpty(d));

		/*create, initialize temporary deck to all cards NULL*/
		cardsLeft = d->cards_left;
		shuffled = (deck *)malloc(sizeof(deck));
		clearDeck(shuffled);

		/* Intializes random number generator */
		srand((unsigned)time(&t));

		/* puts each of the cards from d into a random slot in shuffled */
		for (i = 0; i < cardsLeft; i++) {
			n = rand() % cardsLeft;
			while (shuffled->cards[n] != NULL) {
				n++;/*find empty slot*/
				if (n == cardsLeft) { n = 0; }/*loop back to beginning of deck*/
			}
			shuffled->cards[n] = d->cards[i];/*puts card in empty slot*/
		}

		/*assign shuffled cards back to original deck with rest of cards NULL*/
		for (i = 0; i < NO_OF_CARDS; i++) {
			d->cards[i] = shuffled->cards[i];
		}

		d->top = d->cards[cardsLeft - 1];/*sets last card to top card*/

		free(shuffled);
		return d;

	}

	__declspec(dllexport) void DLLcleanUp(table *t, deck *d) {
		int i, h, players;
		assert(t);
		assert(d);

		players = t->NO_OF_PLAYERS;
		_deletePlayer(t->dealer);

		for (i = 0; i < players; i++) {
			_deletePlayer(t->players[i]);
		}
		free(t->discardPile);
		//free(t);
			

		_deleteCardSuits(suitArr);
		_deleteCardValues(valueArr);
		if (HI_LO_ACES == 1) {
			for (h = 0; h < NO_OF_SUITS; h++) {
				_deleteCardValue(hi_loAceArr[h]);
			}
		}
#if (NO_OF_EXTRA_CARDS > 0)
		_deleteExtraCards(extraCardsArr);
#endif

		for (i = 0; i < NO_OF_CARDS; i++) {
			_deleteCard(d->cards[i]);
		}
		//free(d);
	}

	__declspec(dllexport) void DLLsetAllHands(table *t) {

		int i;

		/*create hand structs for each player*/
		for (i = 0; i < (t->NO_OF_PLAYERS); i++) {
			setNewHand(t->players[i]->playerHand);
		}

		/*create hand struct for dealer*/
		setNewHand(t->dealer->playerHand);

		/*reset handsAreDealt attribute back to false*/
		t->handsAreDealt = 0;
	}

#if ( 1 )


	__declspec(dllexport) void runBlackjack(table *BJTable) {

		int cont;/* boolean 1 or 0 (True/False) variable used only for determining whether
				 user has chosen to continue (deal again) or quit the program*/
		//table *BJTable;/* pointer to main BlackJack Table object created and used in the program */
		deck *BJdeck;/*   pointer to main BlackJack Deck object created and used in the program */
		//FILE *fp;

		/*for communication between a DLL made from this program and another program*/
		//fp = freopen("newIn.txt", "r", stdin);

		/*get the # of players, players' names, # of comps, create the table,
		return a pointer to it*/
		//BJTable = setTable();

		/*create the blackjack deck, return a pointer to it*/
		BJdeck = createDeck();

		/*shuffle the deck*/
		BJdeck = shuffle(BJdeck);

		/*Main game driver loops through playing blackjack hands against the dealer until loop is
		broken when user chooses not to deal again, but to exit and quit program*/
		do {

			/*(re)create and (re)initialize all new hand objects for each player before each turn*/
			setAllHands(BJTable);

			/*Display the table before bets taken*/
			displayTable(BJTable);

			/*Take bets from all players*/
			getBets_ALL(BJTable);

			/*Deal starting hand to each player and dealer*/
			dealStartingHands(BJTable, BJdeck);

			/*Show players their dealt hands and feedback if they have blackjack*/
			displayTable(BJTable);
			//handFeedBack_Display_ALL(BJTable);

			/*Each player plays their hand*/
			playerTurn_ALL(BJTable, BJdeck);

			/*Dealer plays their hand*/
			dealerTurn(BJTable, BJdeck);

			/*Player scores are computed, compared against dealer's*/
			takeScores(BJTable);

			/*Show final results of the hand with feedback of win/loss/push*/
			displayTable(BJTable);
			//handFeedBack_Display_ALL(BJTable);

			/*Prompt to deal again or quit the game/program*/
			cont = continueGamePrompt(BJTable);

		} while (cont == 1);/*Driver loop continues until user quits*/

		/*After quitting the game, clean up: delete and free up all allocated memory
		Then, exit program*/
		cleanUp(BJTable, BJdeck);

		//for C# integration cleanup
		//fclose(fp);

	}
#endif
}