//
//  blackjack_GUI.c
//  Console_Cards
//
//  Created by Charles Hardes on 3/28/15.
//  Copyright (c) 2015 Charles Hardes. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
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
#include "blackjack_run.h"
#include "blackjack_UIX.h"
#include "blackjack_CLI.h"

#pragma once
#if (UIX == 0)

void CL_spaces(int s) {
    
    int i;
    for(i = 0; i < s; i++) {
        printf(" ");
    }
}

void CL_newlines(int n) {
    
    int i;
    assert(n >= 0);
    
    for (i = 0; i < n; i++) {
        printf("\n");
    }
}

void CL_tabs(int t) {
    
    int i;
    assert(t >= 0);
    
    for (i = 0; i < t; i++) {
        printf("\t");
    }
}

void CL_setPrompt(table *t) {

	assert(t);

	/* set spacing to appropriate spaces */
	t->spacing = t->currPlayer * 36;

	CL_tabs(t->margin);
	((t->players[t->currPlayer]->playerHand->hasEnded) && 
		(t->players[t->currPlayer]->playerHand->splitHand == NULL) ? 
		CL_spaces(t->spacing + 36) : CL_spaces(t->spacing));
}

void displayBet(int bet) {printf("Bet:    $%d", bet);}

void prompt_noHumanPlayers() {printf("Enter a number of human players between 0 - 4\n");}

int input_noHumanPlayers() {

	int noPlayers;

	scanf_s("%d", &noPlayers);
	return noPlayers;
}  

void prompt_playerName(int playerNo) {printf("Enter player %d's name:\n", playerNo);}

char *input_playerName() {

	static char buffer[BUFFERSIZE];

	scanf_s(" %63s", buffer, 64);
    CL_newlines(1);

	return buffer;
}

void prompt_noCompPlayers(int maxComps) {printf("Enter number of computer players between 0 - %d\n", maxComps);}

int input_noCompPlayers() {
    
	int no_Comps;
	scanf_s(" %d", &no_Comps);

	return no_Comps;
}

void prompt_playerTurn(table *t, hand *h) {

	assert(t);

	/* ask the user to hit or stay */
    prompt_HitStay();

	/*Allow player the option of doubling down if this is their first hit of the hand*/
	if (h->cardCount == 2) {
		CL_setPrompt(t);
		prompt_DoubleDown();
	}
    CL_newlines(1);

	/* Set CL prompt for next player */
	CL_setPrompt(t);
}

char input_playerTurn() {

	char ans;
	ans = 'd';

	scanf_s(" %c", &ans);
	return ans;
}

/*
 Print a string representing the instance of struct card
 -----------------------------------------------------------------------------------------
 Parameters: struct card *c
 Preconditions: c is not null
 Postconditions: a string representing the card is printed to the console
 */
void printCard(card *c) {
    
    char s[25];
    assert(c);
    strcpy_s(s, sizeof s, c->name);
    printf("%s\tWeight: %d\t%s\n", c->abbr, c->value->weight, c->name);
}

void displayCard(card *c) {
    
    assert(c);
    
    if (c->shown == 1) {
        printf("%s ", c->abbr);
    }
    else printf("XX ");
}

/*
 Loops the printCard function for every struct card in the struct deck.
 -----------------------------------------------------------------------------------------
 Parameters: deck *d
 Preconditions: d is not null
 Postconditions: the printCard function is called on every struct card in the struct deck.
 */
void printDeck(deck *d) {
    
    int i;
    assert(d);
    
    for (i = 0; i < NO_OF_CARDS; i++){
        printCard(d->cards[i]);
    }
}

void busted_Display(table *t, int player) {
        printf("BUSTED! ($%d-)", t->players[player]->playerHand->bet);
        CL_spaces(23 - (int)log10((double)t->players[player]->playerHand->bet));
}

void blackjack_Display(table *t, int player) {
    printf("BLACKJACK! ($%d+)", (int)((t->players[player]->playerHand->bet * 3) / 2));
    CL_spaces(20 - (int)log10((double)((t->players[player]->playerHand->bet * 3) / 2)));
}

void win_Display(table *t, int player) {
    printf("WIN ($%d+)", t->players[player]->playerHand->bet * 2);
    CL_spaces(27 - (int)log10((double)(t->players[player]->playerHand->bet * 2)));
}

void lose_Display(table *t, int player) {
    printf("LOSE ($%d-)", t->players[player]->playerHand->bet);
    CL_spaces(26 - (int)log10((double)t->players[player]->playerHand->bet));
}

void push_Display(table *t, int player) {
    printf("PUSH ($0+)");
    CL_spaces(26);
}

void prompt_Continue() {printf("Deal: d; Quit: q\n");}

char input_Continue() {

	char ans;

    ans = getchar();
    ans = getchar();

	return ans;
}

void error_ContinueGame() {printf("ERROR");}

void prompt_betAmount() {printf("Enter bet amount: ");}

int input_betAmount() {

	int bet;
	scanf_s(" %d", &bet);
	return bet;
}

void error_inputBet() {printf("\nERROR: Improper bet amount\nEnter bet amount: ");}

void diplayDealerName(player *p) {
	assert(p);
	printf("%s", p->name);
}

void displayPlayerName(player *p) {
	assert(p);
	printf("%s     $%d", p->name, p->chips);
}

void prompt_HitStay() {printf("Hit: h\tStay: s\n");}

void prompt_DoubleDown() {printf("Double Down: d\n");}

void prompt_Split(hand *h) {
	if (h->hasSplit) displayHandIndex(h);
	printf("Split?: (y/n) ");
}

char input_Split() {return input_Continue();}/* reused input_Continue() because def. is same */

void displayHandIndex(hand *h) {

	assert(h);
	printf("(%d) ", h->handIndex);
}

#endif