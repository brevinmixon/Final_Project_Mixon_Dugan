//Brevin Mixon, Madison Dugan
//December 2022
//Blackjack game
//Please Enjoy

//Notes to selves:
/*
* yssimplewindow currently not working
* We need the libary that adds text to the gameboard
* Could angle the cards
* 
* To do:
* I need to add a feature that automatically checks for blackjack;
* Create some marker for whichever player is going, maybe the other cards dim and the players are bright
* Upload PNG of the back of a playing card for the dealer
*/

#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctype.h>
#include "fssimplewindow.h"
//#include "yssimplesound.h"
#include "DrawingUtilNG.h"
#include "Cards.h"
#include "yspng.h"
//#include <unistd.h> //I might need to be connected to the internet to update this

using namespace std;

void paint();
int GetScore(vector<vector<string>> Hand);
vector<vector<string>> MakeDeck();
void ReadPNGs();
int FindPNG(string rank, string suit);
void PaintCards();
void reset();

int NumPlayers = 0;
int Phase = 0;
int key = FSKEY_NULL;
int curPlayer = 0;
int curCard = 0;
YsRawPngDecoder png[52];

vector<string> Card;
vector<vector<string>> deck; //a collection of cards (a hand is the same)
vector<vector<vector<string>>> Players; // a collection of hands
vector<vector<string>> Dealer;

vector<vector< int>> PlayerTextIDs; //once a card is drawn, its PNG is appneded here
vector<int> DealerTextIDs; //Learning textures might be better than this

GLuint texId[52];


int main(void) {

	FsOpenWindow(0, 0, 800, 600, 1, "Cool Window");
	if (true) {

	}
	bool terminate = false;
	paint();
	ReadPNGs();

	//Background Music Maybe
	//YsSoundPlayer SoundPlayer;
	//YsSoundPlayer::SoundData BackgroundMusic;
	//if (YSOK != popSound.LoadWav("pop2.wav"))
		//cout << "Can't find popSound data" << endl;
	//SoundPlayer.Start();

	
	//gameplay loop
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
			//cin >> NumPlayers;
			FsPollDevice();
			key = FsInkey();
			switch (key) { //Does a switch statement stop the gameplay loop. Maybe it needs to be able to run thorough
				case FSKEY_1:
					NumPlayers = 1;
					break;
				case FSKEY_2:
					NumPlayers = 2;
					break;
				case FSKEY_3:
					NumPlayers = 3;
					break;
				case FSKEY_4:
					NumPlayers = 4;
					break;
				case FSKEY_5:
					NumPlayers = 5;
					break;
			}

			if (NumPlayers !=0) { //Once NumPlayers has been entered
				deck = MakeDeck(); //make the deck
				//Make a Vector of len 1-5 to hold the players cards;
				for (int i = 0; i < NumPlayers; i++) { //Make a hand for each Player
					vector<vector<string>> Hand;
					vector< int > TextIDs;
					for (int j = 0; j < 2; j++) { //Give two cards to each hand
						vector<string> Card = deck.back();// take top card of deck
						deck.pop_back(); // erase top card of take
						Hand.push_back(Card); //give top card to the Hand
						FindPNG(Card[0], Card[1]);
						TextIDs.push_back(texId[FindPNG(Card[0], Card[1])]);

					}
					Players.push_back(Hand); //Append each hand to the vector Players
					PlayerTextIDs.push_back(TextIDs);
				}

				//Add cards to the dealers hand

				for (int j = 0; j < 2; j++) { //Give two cards to the dealer
					Card = deck.back();// take top card of deck
					deck.pop_back(); // erase top card of take
					Dealer.push_back(Card); //give top card to the Hand
					DealerTextIDs.push_back(texId[FindPNG(Card[0], Card[1])]);
				}

				Phase = 1; //Moves along to the next phase so the deck is not made again
			}

		}
		else if (Phase == 1) { //Player Phase
			//Asks each player if they would like to hit or stand, 1 for hit, 2 for stand
			//This is gonna need a slightly more complicated locking system where it keeps track of which pleyer is up
			// and which card is up
			//If I want to play test before figuring out the cards, I'll need to return cout cards but lock after one use
					
			FsPollDevice();
			key = FsInkey();
			if (GetScore(Players[curPlayer]) <= 21){
				switch (key) {
				case FSKEY_1:
					Card = deck.back();// take top card of deck
					deck.pop_back(); // erase top card of take
					Players[curPlayer].push_back(Card); //give top card to the Hand
					PlayerTextIDs[curPlayer].push_back(texId[FindPNG(Card[0], Card[1])]);
					paint();
					glFlush();
					key = FSKEY_NULL;
					break;
				case FSKEY_2:
					curCard = 0;
					curPlayer++;
					key = FSKEY_NULL;
					break;
				}
			}
			else {
				curCard = 0;
				curPlayer++;
			}

			if (curPlayer==Players.size())
				Phase = 2; //All Players finished, dealer phase starts

		}
		else if (Phase == 2) { //Dealer Phase
			cout << endl << "Dealer" << endl;
			while (GetScore(Dealer) <= 17) { //This shouldn't be a problem, it should go through once and lock
				cout << "Dealer Hits" << endl;
				Card = deck.back();// take top card of deck
				deck.pop_back(); // erase top card of take
				Dealer.push_back(Card); //give top card to the Hand
				DealerTextIDs.push_back(texId[FindPNG(Card[0], Card[1])]);
				paint();
				glFlush();
				//Maybe add a sleep function so that the dealer adds each card slowly
				//FsSleep(30);
				

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


			//Asks if the player wants to play again, 0 for no, 1 for yes
			FsPollDevice();
			key = FsInkey();
			switch (key) {
			case FSKEY_1:
				reset();
				break;
			case FSKEY_0:
				terminate = true;
				break;
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

	PaintCards();

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


void ReadPNGs() {

	//This might not be the best way to do this

	string suits[4] = { "Clubs","Diamonds","Hearts","Spades" };
	string ranks[13] = { "2","3","4","5","6","7","8","9","10","jack","queen","king","ace" };
	string filename;
	int count = 0;

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 4; j++) {
			filename = ranks[i] + "_of_" + suits[j] + ".png";
			if (YSOK == png[count].Decode(filename.c_str())) {
				;
			}
			else cout << "Card Read error" << endl;
			count++;
		}
	}
	
	
	for (int i = 0; i < 52; i++) {
		glGenTextures(i, &texId[i]);
		glBindTexture(GL_TEXTURE_2D, texId[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D
		(GL_TEXTURE_2D, 0, GL_RGBA, png[i].wid, png[i].hei, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, png[i].rgba);
	}
	double angle = 0.0;

	cout << "Read in All PNGs" << endl;
	
}
int FindPNG(string rank, string suit) {
	 //searches throught the list until the card matches the rank and suit, then you know which card in png is it
	//This is not very fast but maybe it will work. Maybe a hash table would be better

	string suits[4] = { "Clubs","Diamonds","Hearts","Spades" };
	string ranks[13] = { "2","3","4","5","6","7","8","9","10","J","Q","K","A" };
	int count = 0;

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 4; j++) {
			if (rank == ranks[i] && suit == suits[j])
				return count;
			else count++;
			
		}
	}
	if (count >= 51) cout << "FindPNG error";

}

void PaintCards() {

	YsRawPngDecoder curCard;
	int index;

	int wid, hei;
	FsGetWindowSize(wid, hei);

	int angle = 0;  // change thisf
	double PI = 3.14;

	//Card Size
	int width = 80;
	int height = 124; //Height: Width = 14/9
	//Card locations
	int DealerX = 200;
	int DealerY = 50;
	int PlayerX = 50;
	int PlayerY = 400;

	//Players
	if (Phase < 1) return;

	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, wid, hei);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)wid - 1, (float)hei - 1, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Texture value will be “multiplied” into the current color
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4d(1.0, 1.0, 1.0, 1.0); // Current color is solid white

	for (int i = 0; i < NumPlayers; i++) {
		for (int j = 0; j < Players[i].size(); j++) {

			glEnable(GL_TEXTURE_2D); // Turn on the texture mapping
			glBindTexture(GL_TEXTURE_2D, PlayerTextIDs[i][j]);
			glBegin(GL_QUADS);
			// For each vertex, assign texture coordinate before vertex coordinate.

			//Idk how this works but I will try the left corner as the reference point
			glTexCoord2d(0.0, 0.0);
			glVertex2d(PlayerX + 150 * i + 20 * j, PlayerY - 20 * j);
			glTexCoord2d(1.0, 0.0);
			glVertex2d(PlayerX + width + 150 * i + 20 * j, PlayerY - 20 * j);  //top right? //100 wide?
			glTexCoord2d(1.0, 1.0);
			glVertex2d(PlayerX + width + 150 * i + 20 * j, PlayerY + height - 20 * j); //bottom right?
			glTexCoord2d(0.0, 1.0);
			glVertex2d(PlayerX + 150 * i + 20 * j, PlayerY + height - 20 * j); //bottom left?
			glEnd();

			//glVertex2d(400.0 + 200.0 * cos(angle - PI / 2.0), 300.0 - 200.0 * sin(angle - PI / 2.0)); //angles!!

		}

	}

	//Dealer
	for (int j = 0; j < Dealer.size(); j++) {

		//glEnable(GL_TEXTURE_2D); // Turn on the texture mapping
		//glBindTexture(GL_TEXTURE_2D, PlayerTextIDs[i][j]); // testing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		glBindTexture(GL_TEXTURE_2D, DealerTextIDs[j]);
		glBegin(GL_QUADS);
		// For each vertex, assign texture coordinate before vertex coordinate.

		//Idk how this works but I will try the left corner as the reference point
		glTexCoord2d(0.0, 0.0);
		glVertex2d(DealerX + 120 * j, DealerY);
		glTexCoord2d(1.0, 0.0);
		glVertex2d(DealerX + width + 120 * j, DealerY);  //top right? //100 wide?
		glTexCoord2d(1.0, 1.0);
		glVertex2d(DealerX + width + 120 * j, DealerY + height); //bottom right?
		glTexCoord2d(0.0, 1.0);
		glVertex2d(DealerX + 120 * j, DealerY + height); //bottom left?
		glEnd();

	}
	for (int k=0 ; k < Dealer.size(); k++)
	{
		cout << Dealer[k][0] << "  " << Dealer[k][1] << endl;
	}
}

void reset() {
	NumPlayers = 0;
	Phase = 0;
	key = FSKEY_NULL;
	curPlayer = 0;
	curCard = 0;

	Card.clear();
	deck.clear();
	Players.clear();
	Dealer.clear();
	PlayerTextIDs.clear();
	DealerTextIDs.clear();

	
}