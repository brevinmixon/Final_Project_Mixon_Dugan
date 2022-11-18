//Brevin Mixon, Madison Dugan
//December 2022
//Blackjack game
//Please Enjoy

//Notes to selves:
/*
* Need to copy over things like fs simple window and gl and yssimple window
* Upload picture of a blackjack table and get pictures of cards
* I think we can keep it all buttons, no clicking needed
* We need the libary that adds text to the gameboard
* I'm gonna make the game run in the terminal for a proof of concept
* Could use glrotate to have five players cards on the table at once with the cards slightly rotated
* If Phase is a global variable, does that mean paint() can use it?
* 
* To do:
* I need to add a feature that automatically checks for blackjack;
* Create some marker for whichever player is going, maybe the other cards dim and the players are bright
* Change the input()s to FSKEY inputs once the graphics get going
*/

#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "fssimplewindow.h"
//#include "yssimplesound.h"
#include "DrawingUtilNG.h"

using namespace std;

void paint();
int GetScore(vector<vector<string>> Hand);
vector<vector<string>> MakeDeck();

int NumPlayers = 0;
int Phase = 0;

vector<string> Card;
vector<vector<string>> deck;
vector<vector<vector<string>>> Players;
vector<vector<string>> Dealer;



int main(void) {

	FsOpenWindow(0, 0, 800, 600, 1, "Cool Window");
	bool terminate = false;
	paint();

	//Background Music Maybe
	//YsSoundPlayer SoundPlayer;
	//YsSoundPlayer::SoundData BackgroundMusic;
	//if (YSOK != popSound.LoadWav("pop2.wav"))
		//cout << "Can't find popSound data" << endl;
	//SoundPlayer.Start();

	/////////////How do I want to do the core gameplay loop?

	//game will be broken into different phases marked by this flag
	// Phase 0 game setup and home screen, click to start the game
	//Phase 1 is Player Phase, all players
	//Phase 2 is Dealer Phase
	// Phase 3 Scores are compared, money is awarded, click to play again (set to Phase 0)
	//If we want to keep track of chips through multiple rounds, this will have to change slighty

	while (!terminate) { //what a fun, easy loop

		if (Phase == 0) {
			//Display "Welcome to the game" How many Players will be playing today?
			//Enter Number 1 - 5 to select number of Players
			while (NumPlayers < 1 || NumPlayers>5) {
				cout << "How many Players? Pick 1-5" << endl;
				cin >> NumPlayers;
			};
			deck = MakeDeck(); //make the deck


			//Make a Vector of len 1-5 to hold the players cards;
			for (int i = 0; i < NumPlayers; i++) { //Make a hand for each Player
				vector<vector<string>> Hand;
				for (int j = 0; j < 2; j++) { //Give two cards to each hand
					vector<string> Card = deck.back();// take top card of deck
					deck.pop_back(); // erase top card of take
					Hand.push_back(Card); //give top card to the Hand
				}
				Players.push_back(Hand); //Append each hand to the vector Players
			}

			//Add cards to the dealers hand

			for (int j = 0; j < 2; j++) { //Give two cards to the dealer
				Card = deck.back();// take top card of deck
				deck.pop_back(); // erase top card of take
				Dealer.push_back(Card); //give top card to the Hand
			}

			Phase = 1; //Moves along to the next phase so the deck is not made again


		}
		else if (Phase == 1) { //Player Phase
			//Asks each player if they would like to hit or stand, 1 for hit, 2 for stand
			//If we have time, we can implement other features like Double down or Split
			for (int i = 0; i < NumPlayers; i++) {
				int choice = 0;
				while (choice != 2) {
					cout << "Player " << i + 1 << " " << "has:  ";
					for (int j = 0; j < Players[i].size(); j++) {
						cout << Players[i][j][0] << "  ";
					}
					cout << endl;
					cout << "Press 1 to hit, 2 to stand" << endl;
					cin >> choice;
					if (choice == 1) {
						Card = deck.back();// take top card of deck
						deck.pop_back(); // erase top card of take
						Players[i].push_back(Card); //give top card to the Hand
						if (GetScore(Players[i]) > 21) {
							choice = 2; //If player busts, their turn ends, no questions asked
							cout << "Player " << i + 1 << " " << "busted with " << GetScore(Players[i]) << endl << endl;
						}
					}
					else if (choice == 2) {
						cout << "Player " << i + 1 << " " << "stood at " << GetScore(Players[i]) << endl << endl;
					}

				}
			}
			Phase = 2; //Player pahse ends, dealer phase starts
		}
		else if (Phase == 2) { //Dealer Phase
			cout << endl << "Dealer" << endl;
			while (GetScore(Dealer) <= 17) {
				cout << "Dealer Hits" << endl;
				Card = deck.back();// take top card of deck
				deck.pop_back(); // erase top card of take
				Dealer.push_back(Card); //give top card to the Hand
				//Maybe add a sleep function so that the dealer adds each card slowly

			}
			int DealerScore = GetScore(Dealer);
			if (DealerScore<=21) cout << "Dealer stood at " << GetScore(Dealer) << endl;
			else cout << "Dealer Busted with " << GetScore(Dealer) << endl;

			Phase = 3;
			
		}

		else if (Phase == 3) { //Count scores and show winners or give chips to winners
			cout << endl;
			bool DidDealerBust;
			int DealerScore = GetScore(Dealer);
			if (DealerScore > 21) DidDealerBust = true;
			else DidDealerBust = false;

			if (DidDealerBust == true) {
				for (int i = 0; i < NumPlayers; i++) {
					if (GetScore(Players[i]) <= 21) cout << "Player " << i + 1 << " Wins!" << endl;
					else cout << "Player " << i + 1 << " busted" << endl;
				}
			}
			else {
				for (int i = 0; i < NumPlayers; i++) {
					int PlayerScore = GetScore(Players[i]);
					if (PlayerScore>DealerScore && PlayerScore<22) cout << "Player " << i + 1 << " Wins!" << endl;
					else if (PlayerScore == DealerScore) cout << "Player " << i + 1 << " tied the dealer" << endl;
					else cout << "Player " << i + 1 << " loses" << endl;
				}
			}


			//Asks if the player wants to play again
			int choice = -1;
			while (choice < 0 || choice>1) {
				cout << "Would you like to play again? Press 1 for yes, 0 for no";
				cin >> choice;
				if (choice == 1) Phase = 0;
				else if (choice == 0) terminate = true;
			}
		}


		paint();
	}

	return 0;
}



void paint() {
	using namespace DrawingUtilNG;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.1, 1, 0.1);
	drawRectangle(0, 0, 800, 600, true); //background color

	

	glFlush();
	FsSwapBuffers();
	FsSleep(1); //quick sleep to try to speed up sound
};

//Makes deck at start of game
vector<vector<string>> MakeDeck() {

	vector<vector<string>> deck;

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
	shuffle(begin(deck), end(deck), rng);

	random_shuffle(deck.begin(), deck.end());
	
	//test deck

	//for (int i = 0; i < 52; i++) {
		//cout << deck[i][0] << " of " << deck[i][1] << endl;
	//}

	return deck;
}

int GetScore(vector<vector<string>> Hand) {
	int Score = 0;
	int Len = Hand.size();
	int Aces = 0; //Count Number of Aces, Aces start at 11, drop down to 1 if the player goes over 21
	string Rank;
	

	for (int i = 0; i < Len; i++) {
		Rank = Hand[i][0];
		if (Rank == "J" || Rank == "Q" || Rank == "K") {
			Score += 10;
		}
		else if (Rank == "A") {
			Score += 11;
			Aces += 1;
		}
		else {
			Score += stoi(Rank); //If card is integer
		}
	}

	while (Score > 21) { //This part comes into play if the player is over 21 but has Aces. 
		if (Aces > 0) {
			Score -= 10;
			Aces -= 1;
		}
		else {
			break;
		} // If over 21 and no aces, too bad
	}

	return Score;
}