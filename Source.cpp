//Brevin Mixon Demo
//October 30th, 2022
//This is a demo of a game where a ball breaks bricks, with powerups
//It lasts roughgly 1:30
//Please Enjoy

//Notes to selves:
/*
* Need to copy over things like fss simple window and gl stuff
* Upload picture of a blackjack table and get pictures of cards
*/

#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
//#include "fssimplewindow.h"
//#include "yssimplesound.h"
//#include "DrawingUtilNG.h"

using namespace std;


vector<vector<string>> MakeDeck();


int main() {

	vector<vector<string>> deck = MakeDeck(); //make the deck

	//How do I want to do the core gameplay loop?

	
}



//Makes deck at start of game
vector<vector<string>> MakeDeck() {

	vector<vector<string>> deck(52);

	string suits[4] = { "Clubs","Diamonds","Hearts","Spades" };
	string ranks[13] = { "2","3","4","5","6","7","8","9","10","J","Q","K","A" };

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 4; j++) {
			vector<string> card;
			card.push_back(ranks[i]);
			card.push_back(suits[j]);
			deck.push_back(card);
			
		}
	};

	random_device rd;
	default_random_engine rng(rd());
	shuffle(deck.begin(), deck.end(), rng);

	return deck;
}