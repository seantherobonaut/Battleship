/*
	Homework 4: Battleship
		This program simulates the game of battle ship between human and computer players, all moves are logged to a file
	Sean Leapley, CISP 400
	December 19th 2019
*/

#include <iostream>
#include <string> 
#include <cstdlib> 
#include <ctime> 
#include <memory>
#include <fstream>
#include <iomanip>
#include <sstream>

//Function Prototype for getRandom
int getRandom(int, int);

//Function Prototype for isInt
bool isInt(std::string, bool);

//Class Prototype for dyn_array
//Dynamically resizing arrays that accept any data type
//Works with pointers, but it's the programmer's responsibility to delete memory
template <class T>
class dyn_array
{
private:
	unsigned size;
	T* array1;
	T* array2;
public:
	unsigned getSize();
	dyn_array();
	dyn_array(const dyn_array&);
	dyn_array& operator=(const dyn_array&);
	void push(T);
	void deleteItem(int);
	void clear();
    T& operator[](int);
	~dyn_array();
};

//Function Prototype for swapper
template <class T>
void swapper(T&, T&);

//Function Prototype for the str_explode
dyn_array<std::string> str_explode(std::string, std::string, bool);

//Function Prototype for removeWhiteSpace
std::string removeWhiteSpace(std::string);

//Function Prototype for allLowerCase
std::string allLowerCase(std::string);

//Function Prototype for allUpperCase
std::string allUpperCase(std::string);

//Function Prototype for capFirstLetter
std::string capFirstLetter(std::string);

//Function Prototype for str_replace
std::string str_replace(std::string, std::string, std::string, bool);

//Function Prototype for formatText
std::string formatText(std::string, std::string);

using namespace std;

//Function Prototype for setCursor
void setCursor(int, int);

//Function Prototype for getTimeStamp
string getTimeStamp(time_t);

//Function Prototype for getTimeStampFull
string getTimeStampFull(time_t);

//Class Prototype for TerminalEraser
//Stores input and erases the contents that's converted into whitespace at specific (row,column) coords
class TerminalEraser
{
private:
	string eraser;
	unsigned _row, _column;
public:
	TerminalEraser(unsigned, unsigned);
	void resetPos(unsigned, unsigned);
	string capture(string);
	void erase();
	unsigned getSize();
};

//Enum for Ship spots
enum Spot {Carrier, Battleship, Cruiser, Submarine, Destroyer, Water};

//Structure for points on the board
struct Point
{
	int row=0,column=0;
	Spot contents = Water;
	bool hit=false, miss=false;
};

//Specification C1 - OOP
//Class Prototype for Ship
class Ship
{
private:
	static unsigned lengths[5];
	static string names[5];
	static string colors[5];
	Spot type;
	bool sunk=false;	
public:
	unsigned HP=0;
	static unsigned getLength(Spot);
	static string getName(Spot);
	static string getColor(Spot);
	void setup(Spot, unsigned);
	Spot getType();
	bool isSunk();
	void operator--(int);	
};

//Class declaration for LogicBase so it can exist inside PlayerData
class LogicBase;

//Structure declaration for PlayerData
struct PlayerData
{
	dyn_array<dyn_array<Point>> board_player;
	dyn_array<dyn_array<Point>> board_enemy;
	dyn_array<Ship> ships_player;
	dyn_array<Ship> ships_enemy;
	string playerName;
	string playerType;
	shared_ptr<LogicBase> logic;
	unsigned numMoves=0;
	unsigned numHits=0;
};

//Class Prototype for LogicBase
class LogicBase
{
public:
	Point lastHit;
	virtual Point getPoint(PlayerData& player){return Point();};
};

//Class Prototype for LogicAI
class LogicAI : public LogicBase
{
private:
	dyn_array<dyn_array<Point>> hitShips;
public:	
	Point getPoint(PlayerData&);
};

//Function Prototype for boardToString
string boardToString(dyn_array<dyn_array<Point>>);

//Class Prototype for Game (Singleton pattern)
class Game
{
private:
	//Lock down constructor
	Game(){}; 
	static Game* _instance;
	unsigned numPlayers = 0;
	int active=1;
	dyn_array<string> messages;
	dyn_array<dyn_array<Point>> cheat_board;
	void setupPlayerHuman(PlayerData&);
	void setupPlayerComputer(PlayerData&);	
	Point makeMoveHuman(PlayerData&);
	Point makeMoveComputer(PlayerData&);
public:
	static Game* getInstance();
	ofstream file;
	int gameActive();
	unsigned numShipsAlive(PlayerData&);
	void cheatBoard(PlayerData&);
	void setupPlayer(PlayerData&);	
	void makeMove(PlayerData&,PlayerData&);
	void pushMessage(string);
	string pullMessage();
};

/*
    TODO 
    	Revert design to human and player classes
        Each class will have common functions, but also must have unique functionality.
    	The AI needs multiple member functions where huamans just don't and the humans need member functions the AI just doesn't. 
    	Store interfaces inside the players. Sure, AI won't have a human interface, that is fine. 
*/

int main()
{
	system("clear");

	//Specification B2 - Log file to Disk
	Game::getInstance()->file.open("BattleshipGame - "+to_string(time(0))+".log", ios::app);	

	TerminalEraser mainOutput(1,1);

	//Function greeting
	cout << mainOutput.capture("Welcome to the game of Battleship!\n");
	//Specification A3 - Date Class in Program Greeting
	cout << mainOutput.capture("Today's date is " + getTimeStamp(time(0)) + ".\n");

	dyn_array<PlayerData> players;
	string input;
	bool multiFire;

	//Specification A2 - MultiFire Game
	cout << mainOutput.capture("\nMultifire game? (yes/no)\nChoice: ");
	do
	{			
		getline(cin, input);
		mainOutput.capture(input+"\n");

		input = removeWhiteSpace(input);
		input = allLowerCase(input);

		if(input != "yes" && input!="no")
			cout << mainOutput.capture("\nThis is really simple... either 'yes' or 'no'. Try again!\nChoice: ");
		else
		{
			if(input=="yes")
				multiFire = true;
			else
				multiFire = false;
		}
	}
	while(input != "yes" && input!="no");		

	for(unsigned i=0; i<2; i++)
	{
		cout << mainOutput.capture("\nWill Player" + to_string(i+1) + " be human? (yes/no)\nChoice: ");

		do
		{			
			getline(cin, input);
			mainOutput.capture(input+"\n");

			input = removeWhiteSpace(input);
			input = allLowerCase(input);

			if(input != "yes" && input!="no")
				cout << mainOutput.capture("\nThis is really simple... either 'yes' or 'no'. Try again!\nChoice: ");
			else
			{
				if(input=="yes")
				{									
					players.push(PlayerData());
					players[i].playerType = "human";
				}				
				else
				{
					players.push(PlayerData());
					players[i].playerType = "computer";
					players[i].logic = make_shared<LogicAI>();
				}
			}
		}
		while(input != "yes" && input!="no");	
	}

	mainOutput.erase();

	//Get players ready
	for(unsigned i=0; i<2; i++)
		Game::getInstance()->setupPlayer(players[i]);

	PlayerData& currentPlayer = players[0];
	PlayerData& otherPlayer = players[1];	
	int keepPlaying = 1;
	
	while(keepPlaying==1)
	{			
		if(multiFire)
		{
			unsigned moves = Game::getInstance()->numShipsAlive(currentPlayer);
			while(moves>0 && keepPlaying==1)
			{
				if(currentPlayer.playerType == "human")		
					Game::getInstance()->pushMessage("You have "+to_string(moves)+" moves remaining in your turn.");	

				//Grab the other player's board
				Game::getInstance()->cheatBoard(otherPlayer);
				Game::getInstance()->makeMove(currentPlayer, otherPlayer);
				keepPlaying = Game::getInstance()->gameActive();
				mainOutput.erase();

				moves--;
			}
		}
		else
		{
			Game::getInstance()->cheatBoard(otherPlayer);
			Game::getInstance()->makeMove(currentPlayer, otherPlayer);
			keepPlaying = Game::getInstance()->gameActive();
			mainOutput.erase();
		}	

		swapper(currentPlayer, otherPlayer);
	}

	if(keepPlaying==-1)
		cout << "Sorry to see you leave " << otherPlayer.playerName << "!" << endl;

	if(keepPlaying==0)
	{
		cout << otherPlayer.playerName << " has won the game in " << otherPlayer.numMoves << " moves!" << endl;
		cout << "Thanks for playing!" << endl;
	}

	Game::getInstance()->file.close();

	return 0;
}

//Return a random number between min and max
int getRandom(int min, int max)
{
	//Set initial state of seeded for rand()
	static bool seeded = false;
	if(!seeded)
	{
		srand(time(0));
		seeded = true;
	}

	return (rand()%(max-min+1)) + min;
}

//Check to see if a string could be an integer
bool isInt(std::string number, bool requireUnsigned=false)
{
	bool result = false;

	if(number.size() > 0)
	{
		result = true;
		unsigned startPos = 0;
		
		//If the number starts with a hyphen, start the loop counter @[1]
		if(number[0] == '-')
			startPos = 1;

		//Iterate through the string and set to false if input isn't an integer
		for(unsigned i = startPos; i<number.size(); i++)
			if(!isdigit(number[i]))
				result = false;

		//Check for above zero when required
		if(requireUnsigned && number[0] == '-')
			result = false;
	}

	return result;
}

//Return the current size of the array
template <class T>
unsigned dyn_array<T>::getSize()
{
	return this->size;
}
//Constructor that sets the initial array size and dynamically allocates an internal array
template <class T>
dyn_array<T>::dyn_array()
{
	this->size = 0;
	this->array1 = new T[this->size+1];
	this->array2 = nullptr;
} 
//Copy constructor to copy size, create a new array and copy its contents
template <class T>
dyn_array<T>::dyn_array(const dyn_array& obj)
{
	this->size = obj.size;
	this->array1 = new T[this->size+1];
	for(unsigned i=0; i<this->size; i++)
		this->array1[i] = obj.array1[i];
	
	this->array2 = nullptr;
}
//Overloaded = operator for copying
template <class T>
dyn_array<T>& dyn_array<T>::operator=(const dyn_array& obj)
{
	//Check if it tries to copy itself
	if(this != &obj)
	{
		//Delete old array
		delete [] this->array1;

		this->size = obj.size;
		this->array1 = new T[this->size+1];
		for(unsigned i=0; i<this->size; i++)
			this->array1[i] = obj.array1[i];

		this->array2 = nullptr;
	}

	return *this;
}
//Allow pushing new elements to the array
template <class T>
void dyn_array<T>::push(T data)
{
	//Insert new element into the empty spot
	this->array1[this->size] = data;
	this->size++;
	this->array2 = new T[this->size+1];

	//copy to array2 + 1
	for(unsigned i=0; i<this->size; i++)
		this->array2[i] = this->array1[i];

	//Delete old array and switch
	delete [] this->array1;
	this->array1 = this->array2;
	this->array2 = nullptr;
}
//Delete item from internal array, shrink array size, copy old array to new array(size-1), switch pointer
template <class T>
void dyn_array<T>::deleteItem(int index)
{
	//Check to make sure array isn't empty
	if(this->getSize() > 0)
	{
		if(index >= 0 && index <= static_cast<int>(this->size)-1)
		{
			//If array has more than 1 element perform normal operation, otherwise, just delete and recreate internal array
			if(this->getSize() > 1)
			{
				this->array2 = new T[this->size];				

				//copy array1 to array2 skipping "index"
				for(unsigned i=0; i<this->size; i++)
				{
					if(i!=static_cast<unsigned>(index))
					{
						if(i>static_cast<unsigned>(index))
							this->array2[i-1] = this->array1[i];
						else
							this->array2[i] = this->array1[i];						
					}
				}				

				//Delete old array and switch
				delete [] this->array1;
				this->array1 = this->array2;
				this->array2 = nullptr;								
			}
			else
			{
				delete [] this->array1;
				this->array1 = new T[1];
			}

			//Make arraysize 1 smaller
			this->size--;
		}
		else
		{
			std::cout << "Error: index (" << index << ") out of bounds of array!" << std::endl;
			exit(0);  
		}
	}
	else
	{
		std::cout << "Error: you can't delete items from an empty array!" << std::endl;
		exit(0);
	}
}
//Delete all elements in the array and set size back to 0
template <class T>
void dyn_array<T>::clear()
{
	delete [] this->array1;
	this->size = 0;
	this->array1 = new T[this->size+1];
}
//Overloaded [] operator to allow normal array syntax
template <class T>
T& dyn_array<T>::operator[](int index)
{
	if(index >= 0 && index <= static_cast<int>(this->size)-1)
		return this->array1[index];
	else
	{
		std::cout << "Error: index (" << index << ") out of bounds of array!" << std::endl;
		exit(0);  
	}
}  
//Destructor to delete all unused memory
template <class T>
dyn_array<T>::~dyn_array()
{
	delete [] this->array1;
	this->array1 = nullptr;	
}

//Swaps variables (pointers too)
template <class T>
void swapper(T& item1, T& item2)
{
	T temp = item1;
	item1 = item2;
	item2 = temp;
}

//Splits string into a dynamic string array based on a given delimiter 
dyn_array<std::string> str_explode(std::string str, std::string delimiter, bool printErrors=false)
{
	//Default result
	dyn_array<std::string> result;

	//Make sure string is bigger than delimiter and it isn't empty
	if(str.size() > delimiter.size() && delimiter.size()>0)
	{		
		std::string str_begin = str.substr(0, delimiter.size());
		std::string str_end = str.substr((str.size()-delimiter.size()), delimiter.size());								

		//Make sure there aren't any delimiters surrounding the string
		if(str_begin!=delimiter && str_end!=delimiter)
		{
			int duplicatePos = str.find(delimiter+delimiter);
			if(duplicatePos > 0)
			{
				if(printErrors)
					std::cout << "Error: double delimiter instance detected" << std::endl;
			}
			else
			{
				int matchPos = -1;
				unsigned startPos=0;
				//Iterate through string at least once
				do
				{	
					//Change startPos to end of last found delimiter and push the string in between delimiters into result array
					matchPos = str.find(delimiter, startPos);
					if(matchPos!=-1)
					{
						result.push(str.substr(startPos, (matchPos-startPos)));
						startPos = matchPos+delimiter.size();
					}
					else
						result.push(str.substr(startPos, (str.size()-startPos)));
				}
				while(matchPos!=-1);
			}
		}
		else
		{
			if(printErrors)
				std::cout << "Error: string cannot start or end with delimiters" << std::endl;
		}
	}
	else
	{
		if(printErrors)
			std::cout << "Error: string length less than delimiter length and/or delimiter length less than 1" << std::endl;
	}

	return result;
}

//Returns a string free of spaces, newlines, and tabs
std::string removeWhiteSpace(std::string text)
{
	std::string newString;
	for(unsigned i=0; i<text.size(); i++)
		if(text[i] != ' ' && text[i] != '\n' && text[i] != '\t')
			newString += text[i];

	return newString;
}

//Returns string with all chars in lowercase
std::string allLowerCase(std::string text)
{
	for(unsigned i=0; i<text.size(); i++)
		text[i] = tolower(text[i]);

	return text;
}

//Returns string with all chars in uppercase
std::string allUpperCase(std::string text)
{
	for(unsigned i=0; i<text.size(); i++)
		text[i] = toupper(text[i]);

	return text;
}

//Returns a string with the first letter capitalized
std::string capFirstLetter(std::string text)
{
	text[0] = toupper(text[0]);
	return text;
}

//Returns haystackl string with replacements instead of needles
std::string str_replace(std::string haystack, std::string needle, std::string replacement, bool printErrors=false)
{
	//Default result
	std::string result;

	//haystack must be >= needle
	if(haystack.size() >= needle.size())
	{
		int matchPos = -1;
		unsigned startPos=0;
		do
		{	
			//Change startPos to end of last found needle and concatinate the string in between needles plus the replacement to the result string
			matchPos = haystack.find(needle, startPos);
			if(matchPos!=-1)
			{
				result += haystack.substr(startPos, (matchPos-startPos)) + replacement;
				startPos = matchPos+needle.size();
			}
			else
				result += haystack.substr(startPos, (haystack.size()-startPos));
		}
		while(matchPos!=-1);
	}
	else
	{
		if(printErrors)
			std::cout << "Error: Haystack string size is less than Needle string size" << std::endl;

		result = haystack;
	}						

	return result;
}

//Returns a colored string based on color choice
std::string formatText(std::string text, std::string color)
{
	/*
		reset             0  (everything back to normal)
		bold/bright       1  (often a brighter shade of the same colour)
		underline         4
		inverse           7  (swap foreground and background colours)
		bold/bright off  21
		underline off    24
		inverse off      27
	*/	

	//Convert color string to lowercase
	//No checking for spaces, if that is the other programmer's job
	for(unsigned i=0; i<color.size(); i++)
		color[i] = std::tolower(color[i]);

	//Choose formatCode - 0 default
	//background starts at 30, foreground starts at 40
	//add 60 to either for bright color variant
	int formatCode = 0;
	if(color == "bold")
		formatCode = 1;
	if(color == "underline")
		formatCode = 4;
	if(color == "inverse")
		formatCode = 7;
	if(color == "black")
		formatCode = 30;
	if(color == "red")
		formatCode = 31;
	if(color == "green")
		formatCode = 32;
	if(color == "yellow")
		formatCode = 33;
	if(color == "blue")
		formatCode = 34;
	if(color == "magenta")
		formatCode = 35;
	if(color == "cyan")
		formatCode = 36;
	if(color == "white")
		formatCode = 37;

	return "\033["+std::to_string(formatCode)+"m"+text+"\033[0m";
}

//Sets the cursor by row and column on the screen
void setCursor(int row, int column)
{
	cout << "\033["+to_string(row)+";"+to_string(column)+"H";
}

//Take in EPOCH time and return string of a formatted timestamp
string getTimeStamp(time_t time)
{
	tm* ltm = localtime(&time);
	
	ostringstream out;
	out << setw(2) << setfill ('0') << to_string(ltm->tm_mday) << "-" << to_string(ltm->tm_mon) << "-" << to_string(ltm->tm_year+1900);

	return out.str();
}

//Take in EPOCH time and return string of a formatted timestamp
string getTimeStampFull(time_t time)
{
	tm* ltm = localtime(&time);

	ostringstream out;
	out << setw(2) << setfill ('0') << to_string(ltm->tm_mday) << "-" << to_string(ltm->tm_mon) << "-" << to_string(ltm->tm_year+1900) << " - " << to_string(ltm->tm_hour) << ":" << to_string(ltm->tm_min) << ":" << setw(2) << setfill ('0') << to_string(ltm->tm_sec);	

	return out.str();
}

TerminalEraser::TerminalEraser(unsigned row, unsigned column) : _row(row), _column(column){}
//Set cursor position to another spot in the terminal
void TerminalEraser::resetPos(unsigned row, unsigned column)
{
	this->_row = row;
	this->_column = column;
}
//Capture input and repeat it back out
string TerminalEraser::capture(string input)
{
	string copy = input;

	//Strip away possible text format codes
	copy = str_replace(copy,"\033[0m","");
	copy = str_replace(copy,"\033[1m","");
	copy = str_replace(copy,"\033[4m","");
	copy = str_replace(copy,"\033[7m","");
	copy = str_replace(copy,"\033[30m","");
	copy = str_replace(copy,"\033[31m","");
	copy = str_replace(copy,"\033[32m","");
	copy = str_replace(copy,"\033[33m","");
	copy = str_replace(copy,"\033[34m","");
	copy = str_replace(copy,"\033[35m","");
	copy = str_replace(copy,"\033[36m","");
	copy = str_replace(copy,"\033[37m","");
	
	//Turn all chars into spaces
	for(unsigned i=0; i<copy.size(); i++)		
		if(copy[i]!='\n' && copy[i]!='\t')
			copy[i] = ' ';

	this->eraser += copy;	

	return input;
}
//Print the eraser
void TerminalEraser::erase()
{
	setCursor(this->_row,this->_column);
	cout << this->eraser;
	setCursor(this->_row,this->_column);
	this->eraser = "";
}
//Get size of buffer
unsigned TerminalEraser::getSize()
{
	return eraser.size();
}

//Define static members for Ship class
unsigned Ship::lengths[] = {5, 4, 3, 3, 2};
string Ship::names[] = {"carrier","battleship","cruiser","submarine","destroyer"};
string Ship::colors[] = {"red","green","yellow","blue","magenta"};
//Get a ship's length
unsigned Ship::getLength(Spot shipType)
{
	return Ship::lengths[shipType];
}
//Get a ship's name
string Ship::getName(Spot shipType)
{
	return Ship::names[shipType];
}
//Get a ship's color
string Ship::getColor(Spot shipType)
{
	return Ship::colors[shipType];
}
//Pseudo constructor
void Ship::setup(Spot shipType, unsigned hitPoints)
{
	this->type = shipType;
	this->HP = hitPoints; 
}
//Get the stored type of shipe
Spot Ship::getType()
{
	return this->type;
}
//Has the ship been sunk?
bool Ship::isSunk()
{
	return this->sunk;
}
//Change sunk to true if HP drops to 0
void Ship::operator--(int n)
{
	this->HP--;
	if(HP==0)
		this->sunk = true;
}

//Get point to fire on other player from computer AI
Point LogicAI::getPoint(PlayerData& player)
{
	//BUG sometimes it will choose the same point multiple times over
	//BUG sometimes  it doesn't pick just 1 spot of the edge of either side of the ship or the next middle point
	//TODO better sorting algorithm, if ships have same HP, bubble up one that has a direction... or just more points that have been hit

	Point chosenTarget;

	/* Process the last point */
	//ONLY insert lastHit into the hitShips array if this is NOT the first successful shot
	if(this->lastHit.contents != Water)
	{
		//Proceed if the enemy ship isn't sunk, otherwise remove it from the hitShips array		
		if(!player.ships_enemy[this->lastHit.contents].isSunk())
		{
			//Add point to hitShips in a new row if hitShips array is empty, otherwise add it to hitShips and THEN sort it
			if(this->hitShips.getSize()==0)
			{				
				this->hitShips.push(dyn_array<Point>());		
				this->hitShips[0].push(this->lastHit);
			}
			else
			{
				int foundIndex=-1;

				//Check if the lastHit's ship exists in the hitShips array
				for(unsigned i=0; i<this->hitShips.getSize(); i++)		
					if(this->hitShips[i][0].contents == this->lastHit.contents)
						foundIndex = i;
				
				//Proceeed if the ship is found, otherwise add it to the end of the hitShips array and bubble sort it up					
				if(foundIndex!=-1)
				{						
					this->hitShips[foundIndex].push(this->lastHit);							
					
					//Sort each hitShip's points in ascending order by either row or column
					unsigned start = this->hitShips[foundIndex].getSize()-1;
					do
					{					
						//Try to sort points by row
						if(this->hitShips[foundIndex][start-1].row > this->hitShips[foundIndex][start].row)
							swapper(this->hitShips[foundIndex][start-1], this->hitShips[foundIndex][start]);

						//Try to sort points by column
						if(this->hitShips[foundIndex][start-1].column > this->hitShips[foundIndex][start].column)
							swapper(this->hitShips[foundIndex][start-1], this->hitShips[foundIndex][start]);						

						start--;							
					}
					while(start!=0);

					//If the ship is NOT at the top of the list, compare and swap it with the ship to the left if needed
					if(foundIndex!=0)
					{
						if(player.ships_enemy[this->hitShips[foundIndex-1][0].contents].HP > player.ships_enemy[this->hitShips[foundIndex][0].contents].HP)
							swapper(this->hitShips[foundIndex-1], this->hitShips[foundIndex]);					
					}	
				}
				else
				{									
					//Insert lastHit as a new row in the end of the array
					this->hitShips.push(dyn_array<Point>());		
					this->hitShips[this->hitShips.getSize()-1].push(this->lastHit);

					//Sort array starting from the end
					unsigned start = this->hitShips.getSize()-1;
					do
					{					
						//Bubble ship up the array if it has less health than the one on the left
						if(player.ships_enemy[this->hitShips[start-1][0].contents].HP > player.ships_enemy[this->hitShips[start][0].contents].HP)
							swapper(this->hitShips[start-1], this->hitShips[start]);					

						start--;							
					}
					while(start!=0);
				}				
			}
		}
		else
		{			
			int foundIndex=-1;
			for(unsigned i=0; i<this->hitShips.getSize(); i++)		
				if(this->hitShips[i][0].contents == this->lastHit.contents)
					foundIndex = i;			
			
			//BUG (this happens because when it picks the same point multiple times, shipsHP is removed by the Game::makeMove function) without this if statement, the entire game sometimes crashes because it tries to delete a ship that has been sunk, that doesn't exist in the hitShips array... how can a ship that was hit multiple times and sunk not exist in the hitShips array ever???
			//Delete the ship and all its points from hitShips
			if(foundIndex!=-1)
				this->hitShips.deleteItem(foundIndex);
		}
	}

	/* Do something with the lastPoint */
	//Check if there are hit ships that still need to be sunk
	if(this->hitShips.getSize() > 0)
	{
		dyn_array<Point> targetShip = hitShips[0];
		int direction = -1;
		int middlePoints = 0;

		//Determine if ship is verticle or horizontal and find any free points between ends of the ship
		if(targetShip.getSize()>1)
		{
			//Verticle ship
			if(targetShip[0].row < targetShip[1].row)
			{
				direction = 1;

				int position = static_cast<int>(targetShip.getSize())-1;
				middlePoints = targetShip[position].row;
				do
				{					
					position--;							
					middlePoints-=targetShip[position].row;
				}
				while(position!=0);				
				middlePoints--;
			}

			//Horizontal ship
			if(targetShip[0].column < targetShip[1].column)
			{
				direction = 0;

				int position = static_cast<int>(targetShip.getSize())-1;
				middlePoints = targetShip[position].column;
				do
				{					
					position--;							
					middlePoints-=targetShip[position].column;
				}
				while(position!=0);

				middlePoints--;				
			}
		}		

		//Choose a guarenteed hit in the middle of a ship's points if there's a gap
		if(middlePoints>0)
		{			
			if(direction==0)
			{
				chosenTarget.row = targetShip[0].row;
				chosenTarget.column = targetShip[0].column+1;
			}
			else
			{
				chosenTarget.row = targetShip[0].row+1;
				chosenTarget.column = targetShip[0].column;
			}			
		}
		else
		{
			//If there a clear direction the ship is facing...
			if(direction!=-1)
			{				
				//Ship is horizontal
				if(direction==0)
				{
					bool leftFree=false, rightFree=false;

					//Check point to left of ship (make sure it's within bounds of the board)
					if(targetShip[0].column-1 >= 0)
						if(!player.board_enemy[targetShip[0].row][targetShip[0].column-1].miss && !player.board_enemy[targetShip[0].row][targetShip[0].column-1].hit)
							leftFree = true;

					//Check point to right of ship (make sure it's within bounds of the board)
					if(targetShip[targetShip.getSize()-1].column+1 < 10)
						if(!player.board_enemy[targetShip[targetShip.getSize()-1].row][targetShip[targetShip.getSize()-1].column+1].miss && !player.board_enemy[targetShip[targetShip.getSize()-1].row][targetShip[targetShip.getSize()-1].column+1].hit)
							rightFree = true;

					chosenTarget.row = targetShip[0].row;

					//Choose point to left of ship
					if(leftFree && !rightFree)
						chosenTarget.column = targetShip[0].column-1;

					//Choose point to right of ship
					if(!leftFree && rightFree)
						chosenTarget.column = targetShip[targetShip.getSize()-1].column+1;

					//Randomly choose point to left or right of ship
					if(leftFree && rightFree)
						chosenTarget.column = getRandom(0,1)==1 ? (targetShip[0].column-1) : (targetShip[targetShip.getSize()-1].column+1);					
				}
				//Ship is vertical
				else
				{
					bool topFree=false, bottomFree=false;

					//Check point above ship (make sure it's within bounds of the board)
					if(targetShip[0].row-1 >= 0)
						if(!player.board_enemy[targetShip[0].row-1][targetShip[0].column].miss && !player.board_enemy[targetShip[0].row-1][targetShip[0].column].hit)
							topFree = true;

					//Check point below ship (make sure it's within bounds of the board)
					if(targetShip[targetShip.getSize()-1].row+1 < 10)
						if(!player.board_enemy[targetShip[targetShip.getSize()-1].row+1][targetShip[targetShip.getSize()-1].column].miss && !player.board_enemy[targetShip[targetShip.getSize()-1].row+1][targetShip[targetShip.getSize()-1].column].hit)
							bottomFree = true;

					chosenTarget.column = targetShip[0].column;

					//Choose point above ship
					if(topFree && !bottomFree)
						chosenTarget.row = targetShip[0].row-1;

					//Choose point below ship
					if(!topFree && bottomFree)
						chosenTarget.row = targetShip[targetShip.getSize()-1].row+1;

					//Randomly choose point above or below ship
					if(topFree && bottomFree)
						chosenTarget.row = getRandom(0,1)==1 ? (targetShip[0].row-1) : (targetShip[targetShip.getSize()-1].column+1);					
				}
			}	
			else
			{
				//Get random point on any free 4 sides of the ship since it's only 1 spot in length
				bool leftFree=false, rightFree=false, topFree=false, bottomFree=false;

				//Preset points for choosing each side
				Point left = targetShip[0];left.column--;				
				Point right = targetShip[0];right.column++;				
				Point top = targetShip[0];top.row--;				
				Point bottom = targetShip[0];bottom.row++;

				//Check point to left of ship (make sure it's within bounds of the board)
				if(targetShip[0].column-1 >= 0)
					if(!player.board_enemy[targetShip[0].row][targetShip[0].column-1].miss && !player.board_enemy[targetShip[0].row][targetShip[0].column-1].hit)
						leftFree = true;

				//Check point to right of ship (make sure it's within bounds of the board)
				if(targetShip[targetShip.getSize()-1].column+1 < 10)
					if(!player.board_enemy[targetShip[0].row][targetShip[0].column+1].miss && !player.board_enemy[targetShip[0].row][targetShip[0].column+1].hit)
						rightFree = true;

				//Check point above ship (make sure it's within bounds of the board)
				if(targetShip[0].row-1 >= 0)
					if(!player.board_enemy[targetShip[0].row-1][targetShip[0].column].miss && !player.board_enemy[targetShip[0].row-1][targetShip[0].column].hit)
						topFree = true;

				//Check point below ship (make sure it's within bounds of the board)
				if(targetShip[targetShip.getSize()-1].row+1 < 10)
					if(!player.board_enemy[targetShip[0].row+1][targetShip[0].column].miss && !player.board_enemy[targetShip[0].row+1][targetShip[0].column].hit)
						bottomFree = true;
				
				//Randomly pick a point if all sides are free
				if(leftFree && rightFree && topFree && bottomFree)
				{
					switch(getRandom(1,4))
					{
						case 1:
							chosenTarget = left;
							break;
						case 2:
							chosenTarget = right;
							break;
						case 3:
							chosenTarget = top;
							break;
						default:
							chosenTarget = bottom;
							break;
					}					
				}
				else
				{
					//Check for parallel top and bottom free spots
					if(!leftFree && !rightFree)
					{						
						//Randomly pick a top or bottom point
						if(topFree && bottomFree)						
							chosenTarget = getRandom(0,1)==1 ? bottom: top;						
						else						
							chosenTarget = topFree==true ? top: bottom;						
					}					
					else
					{
						//Check for parallel left and right free spots
						if(!topFree && !bottomFree)
						{
							//Randomly pick a left or right point
							if(leftFree && rightFree)							
								chosenTarget = getRandom(0,1)==1 ? right: left;							
							else							
								chosenTarget = leftFree==true ? left: right;							
						}
						else
						{
							dyn_array<Point> freePoints;

							if(leftFree)
								freePoints.push(left);
							if(rightFree)
								freePoints.push(right);
							if(topFree)
								freePoints.push(top);
							if(bottomFree)
								freePoints.push(bottom);

							//If only 1 point, or 2 perpendicular points are present, pick through the 3 or 2 free points randomly
							if(freePoints.getSize() < 3)
								chosenTarget = freePoints[static_cast<unsigned>(getRandom(0,1))];
							else
								chosenTarget = freePoints[static_cast<unsigned>(getRandom(0,2))];
						}
					}
				}
			}
		}		
	}
	else
	{
		//Get a random free point from the enemy board
		Point testPoint;
		do
		{			
			testPoint = player.board_enemy[static_cast<unsigned>(getRandom(0,9))][static_cast<unsigned>(getRandom(0,9))];
		}
		while(testPoint.hit || testPoint.miss);
		chosenTarget = testPoint;
	}

	//When calling last point multiple times without a new point, it just keeps duplicating ships (reset point)
	this->lastHit = Point();

	//Maker sure it is generic when coming out
	chosenTarget.contents = Water;	
	
	return chosenTarget;
}

//Convert the board into a string
string boardToString(dyn_array<dyn_array<Point>> board)
{
	string output=""; 

	output += "   A B C D E F G H I J\n";
	for(unsigned row=0; row<10; row++)
	{
		if(row<9)
			output += " " + to_string(row+1);
		else
			output += to_string(row+1);
		
		for(unsigned column=0; column<10; column++)
		{
			if(board[row][column].contents != Water)
			{
				if(board[row][column].hit)
					output += formatText(" *", Ship::getColor(board[row][column].contents));
				else
					output += formatText(" @", Ship::getColor(board[row][column].contents));
			}
			else
			{
				if(board[row][column].miss)
					output += " #";
				else
					output += "  ";
			}
		}
		output += "\n";
	}
	
	return output;	
}

//Store private static instance of Game
Game* Game::_instance = nullptr; 
//Get the private static instance of Game so there's only 1 instance possible
Game* Game::getInstance()
{
	//Ensure only 1 active instance
	if (_instance == nullptr)
		_instance = new Game();

	return _instance;
}
//Check if the game is active
int Game::gameActive()
{
	return this->active;
}
//Check how many ships are still alive
unsigned Game::numShipsAlive(PlayerData& player)
{
	unsigned result = 0;
	for(unsigned i=0; i<player.ships_player.getSize(); i++)
		if(!player.ships_player[i].isSunk())
			result++;

	return result;
}
//Get the board of the other player to cheat
void Game::cheatBoard(PlayerData& enemyPlayer)
{
	this->cheat_board = enemyPlayer.board_player;
}
//Setup the human player
void Game::setupPlayerHuman(PlayerData& player)
{
	//Controllers that handle printing, and erasing of the screen
	TerminalEraser instructionPrinter(1,1), inputPrinter(1,1), boardPrinter(6,1), shipListPrinter(22,1), errorPrinter(18,1), shipTitle(21,1), commandTitle(19,1);
	
	string playerInput; 
	dyn_array<Ship> boardList, shipList;
	shipList = player.ships_player;
	
	//Get player's name
	cout << inputPrinter.capture("Alright Player"+to_string(this->numPlayers)+", let's get your name: ");
	getline(cin, playerInput);
	inputPrinter.capture(playerInput+"\n");
	player.playerName = playerInput;

	//Make sure player is ready
	cout << inputPrinter.capture("Howdy " + player.playerName + "! Let's setup your board!\n");
	cout << inputPrinter.capture("    (make sure no one is looking and press enter to continue) ");
	getline(cin, playerInput);
	inputPrinter.capture(playerInput+"\n");
	inputPrinter.erase();

	//Print out instructions
	cout << instructionPrinter.capture("Place EACH ship on the board. (ships can be replaced multiple times)\n");
	cout << instructionPrinter.capture("Commands: \"confirm\", \"random\", \"clear\"\n");
	cout << instructionPrinter.capture("Format: place getName x(A-J),y(1-10),rotation(0-1)\n");
	cout << instructionPrinter.capture("Ex: place Battleship A,3,1\n");

	//Print out the titles
	setCursor(19,1);
	cout << commandTitle.capture("Command: ");
	setCursor(21,1);
	cout << shipTitle.capture("Ships:");

	setCursor(18,1);
	cout << errorPrinter.capture("Enter a command...");															
					
	//Reset the inputPrinter to just after "Command: "
	inputPrinter.resetPos(19,10);

	//Main loop for placing ship logic
	while(playerInput!="confirm")
	{		
		//Print current board layout
		boardPrinter.erase();
		cout << boardPrinter.capture(boardToString(player.board_player)+"\n\n");

		//Print ships that have yet to be placed
		shipListPrinter.erase();
		for(unsigned i=0; i<shipList.getSize(); i++)
			cout << shipListPrinter.capture(formatText(capFirstLetter(Ship::getName(shipList[i].getType())), Ship::getColor(shipList[i].getType()))+"\n");
		if(shipList.getSize() == 0)
			shipTitle.erase();

		//Accept player commands (erase last command)
		setCursor(19,10);
		getline(cin, playerInput);
		inputPrinter.capture(playerInput+"\n");
		inputPrinter.erase();

		//Main loop for trying to place ships + input validation
		playerInput = allLowerCase(playerInput);
		if(playerInput!="confirm")
		{
			if(playerInput=="clear")
			{
				shipTitle.erase();
				cout << shipTitle.capture("Ships:");		

				//Clear board
				for(unsigned _i=0; _i<10; _i++)				
					for(unsigned _j=0; _j<10; _j++)				
						player.board_player[_i][_j].contents = Water;															

				//clear both lists and reset shipList
				boardList.clear();
				shipList.clear();
				shipList = player.ships_player;		
			}
			else
			{
				if(playerInput=="random")
				{
					//Clear the board each time you randomize it
						shipTitle.erase();
						cout << shipTitle.capture("Ships:");								
						for(unsigned _i=0; _i<10; _i++)				
							for(unsigned _j=0; _j<10; _j++)				
								player.board_player[_i][_j].contents = Water;																					
						boardList.clear();
						shipList.clear();
						shipList = player.ships_player;	

					//Randomize ships on boad
					string tempName = player.playerName;
					this->setupPlayerComputer(player);
					player.playerName = tempName;
				
					//Copy shipList to boardList and empty shipList
					for(unsigned i=0; i<shipList.getSize(); i++)				
						boardList.push(shipList[i]);
					shipList.clear();
				}
				else
				{
					dyn_array<string> args = str_explode(playerInput, " ");
					if(args.getSize() == 3)
					{
						if(args[0] == "place")
						{
							int shipListMatch=-1, boardListMatch=-1;
							Ship currentShip;

							//Search shipList for matching getName
							for(unsigned i=0; i<shipList.getSize(); i++)
							{
								if(args[1] == Ship::getName(shipList[i].getType()))
								{
									shipListMatch = i;
									currentShip = shipList[i];																				
								}				
							}
							//Search boardList for matching getName
							for(unsigned i=0; i<boardList.getSize(); i++)
							{
								if(args[1] == Ship::getName(boardList[i].getType()))
								{
									boardListMatch = i;
									currentShip = boardList[i];						
								}				
							}						

							if(shipListMatch>=0 || boardListMatch>=0)
							{
								int inputColumn=-1, InputRow=-1, inputRotation=-1;

								//Gather coordinates and rotation
								dyn_array<string> coords = str_explode(args[2], ",");
								if(coords.getSize() == 3)
								{
									string arg1 = coords[0];
									string arg2 = coords[1];
									string arg3 = coords[2];

									//Input validation for coordintes
									if(arg1.size() == 1 && static_cast<int>(arg1[0])>=97 && static_cast<int>(arg1[0])<=106) 
									{
										inputColumn = static_cast<int>(arg1[0])-97;

										if(isInt(arg2, true))
										{
											int num = stoi(arg2);
											if(num>= 1 && num <=10)
											{
												InputRow = num-1;
												if(arg3.size() == 1 && (arg3[0] == '1' || arg3[0] == '0'))
												{
													inputRotation = stoi(arg3);
													
													unsigned shipLength = Ship::getLength(currentShip.getType());
													bool placement = true, shipFit=true; 

													if(inputRotation==0)												
														shipFit = (shipLength+inputColumn)<=10;
													else
														shipFit = (shipLength+InputRow)<=10;

													if(shipFit)
													{
														for(unsigned i=0; i<shipLength; i++)
														{
															if(inputRotation==0)
															{
																if(player.board_player[InputRow][inputColumn+i].contents != Water)
																	placement = false;
															}
															else
															{
																if(player.board_player[InputRow+i][inputColumn].contents != Water)
																	placement = false;
															}
														}
														
														//Place the ship on the board once a valid spot is found
														if(placement)
														{
															if(shipListMatch>=0)
															{
																boardList.push(currentShip);
																shipList.deleteItem(shipListMatch);	
															}
															//Erase the ship from the board if it exists already
															if(boardListMatch>=0)														
															{
																for(unsigned row=0; row<10; row++)
																{
																	for(unsigned column=0; column<10; column++)
																	{
																		if(player.board_player[row][column].contents == boardList[boardListMatch].getType())
																			player.board_player[row][column].contents = Water;
																	}
																}
															}
															//Place the ship on the board spot by spot
															for(unsigned i=0; i<shipLength; i++)
															{
																if(inputRotation==0)
																	player.board_player[InputRow][inputColumn+i].contents = currentShip.getType();
																else
																	player.board_player[InputRow+i][inputColumn].contents = currentShip.getType();
															}																		

															errorPrinter.erase();							
															cout << errorPrinter.capture("Placed: "+formatText(capFirstLetter(Ship::getName(currentShip.getType())), Ship::getColor(currentShip.getType())) + " @ ("+string(1,char(inputColumn+65))+","+to_string(InputRow+1)+","+to_string(inputRotation)+")");		
														}
														else
														{
															errorPrinter.erase();
															cout << errorPrinter.capture("There seem's to be another ship in the way of your placement.");															
														}
													}
													else
													{
														errorPrinter.erase();
														cout << errorPrinter.capture("You ship won't fit on the board using the coordinates: ("+to_string(inputColumn)+","+to_string(InputRow+1)+")");	
													}														
												}
												else
												{
													errorPrinter.erase();
													cout << errorPrinter.capture("Ship coordinate \"rotation\" must be either 0 or 1.");													
												}
											}
											else
											{
												errorPrinter.erase();
												cout << errorPrinter.capture("Ship coordinate \"row\" must be 1-10.");	
											}
										}
										else
										{
											errorPrinter.erase();
											cout << errorPrinter.capture("Ship coordinate \"row\" must be a number.");	
										}								
									}
									else
									{
										errorPrinter.erase();
										cout << errorPrinter.capture("Ship coordinate \"column\" must be A-J.");				
									}
								}
								else
								{
									errorPrinter.erase();
									cout << errorPrinter.capture("Ship coordinate scheme requires 3 comma seperated arguments.");												
								}
							}
							else
							{
								errorPrinter.erase();
								cout << errorPrinter.capture("That ship doesn't exist.");				
							}
						}
						else
						{
							errorPrinter.erase();
							cout << errorPrinter.capture("Invalid first argument. 1st arg must either be \"place\" or \"confirm\".");						
						}
					}
					else
					{
						errorPrinter.erase();
						cout << errorPrinter.capture("Invalid command.");							
					}
				}
			}
		}
		else
		{
			if(shipList.getSize()>0)
			{
				playerInput = "";
				errorPrinter.erase();
				cout << errorPrinter.capture("You still have ships left to place!");		
			}
		}
	}

	//Clear the screen of clutter
	instructionPrinter.erase();
	commandTitle.erase();
	shipTitle.erase();
	boardPrinter.erase();
	errorPrinter.erase();
	shipListPrinter.erase();
	setCursor(1,1);
}
//Setup the computer player
void Game::setupPlayerComputer(PlayerData& player)
{
	//Set the computer's name
	player.playerName = "Computer"+to_string(this->numPlayers);

	//Specification B3 - Random Start
	for(int i=0; i<5; i++)
	{
		bool placement=false;
		Spot currentShip = static_cast<Spot>(i);
		unsigned shipLength = Ship::getLength(currentShip);
		int tryRow, tryColumn, rotation;

		//Loop until valid ship spot is found (pick point, rotate vert/horiz, pick new point, repeat)
		while(!placement)
		{	
			//make placement true unless proven false
			placement = true; 
			//randomly choose rotation and coords (max board size - length of ship)
			tryRow = getRandom(0, (10-shipLength));
			tryColumn = getRandom(0, (10-shipLength));
			for(unsigned j=0; j<shipLength; j++)
			{
				if(rotation==0)
				{
					if(player.board_player[tryRow][tryColumn+j].contents != Water)
						placement = false;
				}
				else
				{
					if(player.board_player[tryRow+j][tryColumn].contents != Water)
						placement = false;
				}
			}
			if(!placement)
			{
				//reset placement and invert rotation
				placement = true;
				if(rotation==0)
					rotation = 1;
				else
					rotation = 0;

				for(unsigned j=0; j<shipLength; j++)
				{
					if(rotation==0)
					{
						if(player.board_player[tryRow][tryColumn+j].contents != Water)
							placement = false;
					}
					else
					{
						if(player.board_player[tryRow+j][tryColumn].contents != Water)
							placement = false;
					}
				}
			}
		}

		//Place the ship on the board once a valid spot is found
		for(unsigned j=0; j<shipLength; j++)
		{
			if(rotation==0)
				player.board_player[tryRow][tryColumn+j].contents = currentShip;
			else
				player.board_player[tryRow+j][tryColumn].contents = currentShip;
		}
	}
}
//Start setting up either type of player
void Game::setupPlayer(PlayerData& player)
{
	this->numPlayers++;

	//Fill both boards with a 10x10 grid of Point objects
	for(unsigned row=0; row<10; row++)
	{
		player.board_player.push(dyn_array<Point>());
		player.board_enemy.push(dyn_array<Point>());
		for(unsigned column=0; column<10; column++)
		{
			player.board_player[row].push(Point());
			player.board_player[row][column].row=row;
			player.board_player[row][column].column=column;

			player.board_enemy[row].push(Point());
			player.board_enemy[row][column].row=row;
			player.board_enemy[row][column].column=column;
		}
	}

	//Instantiate and setup ship objects (type, hitpoints)
	for(unsigned i=0; i<5; i++)
		player.ships_player.push(Ship());
	player.ships_player[Carrier].setup(Carrier, 5);
	player.ships_player[Battleship].setup(Battleship, 4);
	player.ships_player[Cruiser].setup(Cruiser, 3);
	player.ships_player[Submarine].setup(Submarine, 3);
	player.ships_player[Destroyer].setup(Destroyer, 2);
	player.ships_enemy = player.ships_player;

	if(player.playerType == "human")
	{
		this->setupPlayerHuman(player);
	}
	if(player.playerType == "computer")
		this->setupPlayerComputer(player);
}
//Get a point for the game from a human
Point Game::makeMoveHuman(PlayerData& p1)
{
	//TODO add more documentation

	TerminalEraser inputPrinter(1,1), instructionPrinter(27,1), boardPlayerTitle(1,1), boardPlayerPrinter(2,1), boardEnemyTitle(14,1), boardEnemyPrinter(15,1), errorPrinter(31,1);
	string playerInput; 

	//Print out stored messages
	string message = this->pullMessage();
	if(message!="")
	{
		while(message!="")
		{
			cout << inputPrinter.capture(message+"\n");
			message = this->pullMessage();
		}	
		cout << inputPrinter.capture("\n");
	}

	//Make sure player is ready
	cout << inputPrinter.capture("Alright " + p1.playerName + ", it's your turn to take a shot!\n    (make sure no one is looking and press enter to continue) ");		
	getline(cin, playerInput);
	inputPrinter.capture(playerInput+"\n");
	inputPrinter.erase();		

	instructionPrinter.erase();
	cout << instructionPrinter.capture("Look at the enemy's board, and select a point to shoot.("+formatText(formatText("+", "cyan"), "bold")+")\n");
	cout << instructionPrinter.capture("Format: letter,number (Ex: A,3) ...if type retype coords\n");
	cout << instructionPrinter.capture("You can enter coords, \"confirm\" or \"quit\".");

	boardPlayerTitle.erase();
	cout << boardPlayerTitle.capture(formatText(formatText("====== Your Board =======\n", "green"), "bold"));
	boardEnemyTitle.erase();
	cout << boardEnemyTitle.capture(formatText(formatText("===== Enemy's Board =====\n", "red"), "bold"));
	errorPrinter.erase();
	cout << errorPrinter.capture("Eneter a command...");
	inputPrinter.resetPos(32,1);

	int shotRow=-1, shotColumn=-1, testRow, testColumn;
	dyn_array<string> args;
	while(playerInput!="confirm" && playerInput!="quit")
	{
		if(playerInput!="quit")
		{
			boardPlayerPrinter.erase();
			cout << boardPlayerPrinter.capture(boardToString(p1.board_player));

			boardEnemyPrinter.erase();
			//Specification C3 - Secret Option
			if(playerInput=="cheat")
			{
				cout << boardEnemyPrinter.capture(boardToString(this->cheat_board));
				errorPrinter.erase();
				cout << errorPrinter.capture("You cheated!!!");
			}
			else
				cout << boardEnemyPrinter.capture(boardToString(p1.board_enemy));

			if(shotRow!=-1 && shotColumn!=-1)
			{
				setCursor((shotRow+1)+15,((shotColumn+1)*2)+2);
				cout << formatText(formatText("+", "cyan"), "bold");
			}

			inputPrinter.erase();
			cout << inputPrinter.capture("Command: ");
			getline(cin, playerInput);
			inputPrinter.capture(playerInput+"\n");

			playerInput = removeWhiteSpace(playerInput);
			playerInput = allLowerCase(playerInput);

			testColumn=-1;
			testRow=-1;
			if(playerInput!="confirm")
			{
				if(playerInput!="cheat")
				{
					//Specification B1 - Validate Input
					args = str_explode(playerInput, ",");
					if(args.getSize() == 2)
					{
						if(args[0].size() == 1 && static_cast<int>(args[0][0])>=97 && static_cast<int>(args[0][0])<=106)
						{
							testColumn = static_cast<int>(args[0][0])-97;
							if(isInt(args[1], true))
							{
								testRow = stoi(args[1]);
								if(testRow>=1 && testRow<=10)
								{
									//Specification A1 - Adv Input Validation
									if(!p1.board_enemy[testRow-1][testColumn].hit && !p1.board_enemy[testRow-1][testColumn].miss)
									{
										shotColumn = testColumn;
										shotRow = testRow-1;
										errorPrinter.erase();
										cout << errorPrinter.capture("Nice shot! You chose: "+string(1,char(shotColumn+65))+","+to_string(shotRow+1));
									}
									else
									{
										errorPrinter.erase();
										cout << errorPrinter.capture("That spot has already been fired upon.");
									}			
								}
								else
								{
									errorPrinter.erase();
									cout << errorPrinter.capture("The second coordinate must be 1-10.");	
								}
							}
							else
							{
								errorPrinter.erase();
								cout << errorPrinter.capture("The second coordinate must be a non-zero integer.");						
							}
						}
						else
						{
							errorPrinter.erase();
							cout << errorPrinter.capture("The first coordinate must be A-J.");						
						}
					}
					else
					{
						errorPrinter.erase();
						cout << errorPrinter.capture("Invalid command. Either comma seperated coordinates or \"confirm\".");						
					}				
				}
			}
			else
			{
				if(shotRow==-1 && shotColumn==-1)
				{
					playerInput="";
					errorPrinter.erase();
					cout << errorPrinter.capture("You haven't selected a valid point yet!");	
				}
			}
		}	
	}

	//Clear the terminal
	inputPrinter.erase();
	instructionPrinter.erase();
	boardEnemyTitle.erase();
	boardEnemyPrinter.erase();
	boardPlayerTitle.erase();
	boardPlayerPrinter.erase();
	errorPrinter.erase();
	setCursor(1,1);

	//Return selected point by value
	if(playerInput != "quit")
		return p1.board_enemy[shotRow][shotColumn];
	else
	{
		this->active = -1;
		return Point();
	}
}
//Get a point from the game from a computer
Point Game::makeMoveComputer(PlayerData& p1)
{
	return p1.logic->getPoint(p1);
}
//Get a point for the game from either type of player
void Game::makeMove(PlayerData& p1, PlayerData& p2)
{
	Point shotAttempt, shotResult;

	if(p1.playerType == "human")
		shotAttempt = this->makeMoveHuman(p1);
	if(p1.playerType == "computer")
	{		
		// int count = 0;
		//Specification C2 - Prohibit AI wasted shots
		do
		{						
			shotAttempt = this->makeMoveComputer(p1);		

			//Diagnostic to see how many times the computer selects the same point over and over
			/*count++;
			cout << "loop" << count << endl;
			file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " loop" << count << endl;*/
		}
		while(p2.board_player[shotAttempt.row][shotAttempt.column].hit || p2.board_player[shotAttempt.row][shotAttempt.column].hit);			
	}
	

	//Increase the number of moves they've made
	p1.numMoves++;

	//If player resigns
	if(this->active==-1)
		this->file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " resigned.\n";	

	if(this->active==1)
	{
		//Grab targeted point
		shotResult = p2.board_player[shotAttempt.row][shotAttempt.column];

		if(shotResult.contents != Water)
		{
			shotResult.hit = true;
			p2.ships_player[shotResult.contents]--;
			p1.ships_enemy[shotResult.contents]--;
		}
		else		
		{
			shotResult.miss = true;		
		}

		p1.board_enemy[shotResult.row][shotResult.column] = shotResult;
		p2.board_player[shotResult.row][shotResult.column] = shotResult;		

		//Save moves to file
		if(shotResult.hit)
		{
			p1.numHits++;
			if(p2.ships_player[shotResult.contents].isSunk())
				this->file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " sunk " << p2.playerName << "'s " << capFirstLetter(Ship::getName(shotResult.contents)) << " ("+string(1,char(shotResult.column+65))+","+to_string(shotResult.row+1)+")\n";				
			else
				this->file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " hit " << p2.playerName << "'s " << capFirstLetter(Ship::getName(shotResult.contents)) <<" ("+string(1,char(shotResult.column+65))+","+to_string(shotResult.row+1)+")\n";			
		}
		else	
			this->file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " missed ("+string(1,char(shotResult.column+65))+","+to_string(shotResult.row+1)+")\n";	

		//check if all ships are sunk to decide the winner of the game
		if(p2.ships_player[Carrier].isSunk() && p2.ships_player[Battleship].isSunk() && p2.ships_player[Cruiser].isSunk() && p2.ships_player[Submarine].isSunk() && p2.ships_player[Destroyer].isSunk())
		{
			this->file << "[" << getTimeStampFull(time(0)) << "] - " << p1.playerName << " won the game.\n";	
			this->active = 0;
		}

		this->file.flush();

		if(p1.playerType=="human")
		{
			if(shotResult.hit)
			{
				if(p2.ships_player[shotResult.contents].isSunk())
					this->pushMessage("You just sunk " + p2.playerName + "'s " + capFirstLetter(Ship::getName(shotResult.contents)) + "!!!");
				else
					this->pushMessage("You hit " + p2.playerName + "'s " + capFirstLetter(Ship::getName(shotResult.contents)) +"!");
			}
			else	
				this->pushMessage("You missed!");			
			
			//Print out stored messages
			TerminalEraser screenPrint(1,1);	
			string output = this->pullMessage();
			while(output!="")
			{
				cout << screenPrint.capture(output+"\n");
				output = this->pullMessage();
			}

			//Wait for player to press enter
			cout << screenPrint.capture("\nPress enter to proceeed... ");
			getline(cin, output);
			screenPrint.capture(output+"\n");
			screenPrint.erase();			
		}
		else
		{
			//Send hit computer's AI logic
			if(shotResult.hit)
				p1.logic->lastHit = shotResult;
		}		
	}
}
//Push a message for the next player to see
void Game::pushMessage(string text)
{
	this->messages.push(text);
}
//Get the last message for the player to see
string Game::pullMessage()
{
	string text="";

	if(this->messages.getSize() > 0)
	{
		text = messages[0];
		messages.deleteItem(0);
	}

	return text;
}