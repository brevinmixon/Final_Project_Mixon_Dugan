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
* Change the input()s to FSKEY inputs once the graphics get going, switch statements
* Upload PNG of the back of a playing card
* 
* Maybe I should just have a vector that matches Players that stores all the cards so it doesnt have to run
* FindPNG every loop. Only when a card is appended, will I use FingPNG to add the file
* 
* Make sure that deck is truly random, when playing multiple rounds
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

int NumPlayers = 0;
int Phase = 0;
int key = FSKEY_NULL;
int curPlayer = 0;
int curCard = 0;
int choice = -1;
YsRawPngDecoder png[52];

vector<string> Card;
vector<vector<string>> deck; //a collection of cards (a hand is the same)
vector<vector<vector<string>>> Players; // a collection of hands
vector<vector<string>> Dealer;




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
			int choice = -1;
			FsPollDevice();
			key = FsInkey();
			switch (key) {
			case FSKEY_1:
				Phase = 0;
				//create true reset function that resets global variables
				curCard = 0;
				curPlayer = 0;
				
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
	string ranks[13] = { "2","3","4","5","6","7","8","9","10","J","Q","K","A" };
	string filename;
	int count = 0;

	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 4; j++) {
			filename = ranks[i] + "_of_" + suits[j] + ".png";
			if (YSOK == png[count].Decode("2_of_clubs.png")) {
				png[count].Flip();
			}
			else cout << "Card Read error" << endl;
			count++;
		}
	}
	

	//another possible way is to just append this string, the filename in the vecotr of the card
	//Well then it would jsut be a string and not a YSrawdecode so it would have to decode everytime
	
}
int FindPNG(string rank, string suit) {
	 //searches throught the list until the card matches the rank and suit, then you know which card in png is it
	//This is not very fast but maybe it will work. Maybe a hash table would be better

	string suits[4] = { "Clubs","Diamonds","Hearts","Spades" };
	string ranks[13] = { "2","3","4","5","6","7","8","9","10","J","Q","K","A" };
	int count = 0;

	/*
	for (int i = 0; i < 13; i++) {
		if (rank == ranks[i]) {
			for (int j = 0; j < 4; j++) {
				if (suit == suits[j]) {
					return png[count];
				}
				else count++;
			}
		}

		//does c++ have this feature
		else count += 4;
	}*/

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
	double height = png[0].hei/10;
	double width = png[0].wid/10;
	int index;

	//Players
	if (Phase < 1) return;

	for (int i = 0; i < NumPlayers; i++) {
		for (int j = 0; j < Players[i].size(); j++) {
			//index= FindPNG(Players[i][j][0], Players[i][j][1]); //find PNG using rank and suit
			//curCard = png[index];

			glRasterPos2d(200+100*i+40*j, (double)(500-40*j));   //(x,y)
			glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, png[0].rgba); //some error with rgba function
			FsSleep(50);
			//usleep(1);

		}
		
	}


	//Dealer
	for (int j = 0; j < Dealer.size(); j++) {
		if (YSOK == png[0].Decode("2_of_clubs.png")) {
			png[0].Flip();
		}
		//must be card specific, i guess
		glRasterPos2d(400 + 80 * j, (double)(100));
		glDrawPixels(png[0].wid / 10, png[0].hei / 10, GL_RGBA, GL_UNSIGNED_BYTE, png[0].rgba);
		//Open gl how to scale down an image
		//add an angle function
		FsSleep(1);
		// if phase==1, only show one of the dealer's cards

	}
	//Learn to use textures then I can change the size and angle
}

