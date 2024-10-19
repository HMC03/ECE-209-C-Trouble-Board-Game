// ECE 209 - Fall 2023 - Program 1
// This is an implementation of the Trouble board game.
// Allows for up to four players.

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
// add other #include statements as needed

// symbols for the various colors for players
// using global constants instead of #define
const int Red = 0;
const int Green = 1;
const int Yellow = 2;
const int Blue = 3;
const char * const gPlayerNames[] = {"Red", "Green", "Yellow", "Blue"};

// NOTE: We are using global variable to store the state of the game.
// This is appropriate because there is only one game, and different functions
// need access to all of the board information.

// A player's pieces are represented by an integer. There are four pieces per player (0-3),
// and a piece is represented by an integer p = 4 * color + piece.
// So, red's pieces will be 0, 1, 2, 3, green's pieces will be 4, 5, 6, 7, etc.
int gPieces[16];   // the current position of each piece

// board information
int gHome[4];        // number of players in the home area for each player
int gRing[28];       // which piece is in each spot on the ring
int gFinish[4][4];   // which pieces are in each player's finish line

// Each board position is represented by an integer.
// Position -1 is the home area for any player.
//
// Positions 0 through 27 are positions along the ring.
// The ring is divided into four regions, one for each color. Each region has seven positions.
// Red is region 0, Green is region 1, Yellow is region 2, Blue is region 3.
// A "region" begins with the position that transitions from one color to the next.
//
// Each player's starting point (where it enters the ring)
// is position 5 within that player's region.
//
// The finish line positions are represented by 100 + player number * 10 + position.
// Example: Position 132 is the third position in the finish line for player 3 (Blue).

// LOCATION OF EACH PIECE
// The location (position) of each piece is stored in the gPieces array.
// The Ring array contains the identifier of the piece that occupies each ring position,
// or -1 if the position is empty.
// The Finish array contains the identifier of the piece that occupies each finish line position,
// or -1 if the position is empty.

// STRING REPRESENTATION OF A BOARD POSITION
//
// NOTE: Since we are communicating with humans who are not used to counting from zero,
// our printed position values will start counting from 1.
//
// Print "H" for any player's home area.
//
// When a ring position is printed, we specify the color of the region and the
// position within that region. (See the picture in the spec.)
// For a given position on the ring, the color of a region is given by position / 7, and the
// number within the region is given by (index % 7)+1.
// Example: Position 0 is written as "R1" (meaning "red region, position 1")
// because 0 / 7 == 0 (Red) and (0 % 7)+1 == 1.
// Example: Position 19 is written as "Y6" (meaning "yellow region, position 6")
// because 19 / 7 == 2 (Yellow) and (19 % 7)+1 == 6.
//
// The finish line positions are written as "xF1" through "xF4", where x is the color
// (e.g., "RF2" for red's finish line position 2).

// MANDATORY FUNCTIONS ================================================================
// functions that you must implement -- you can add others if you want

void initializeBoard();    // set up the board for a new game (see the spec)

void printStatus();        // print the current position of each player's pieces (see the spec)

int playerTurn(int player);   // do everything necessary for this player's turn
// return 1 if player wins, otherwise return 0;

int printMoves(int player, int roll);   // print the legal moves for the player
// return the number of legal moves

int movePiece(int player, int from, int spaces);    // move a piece forward
// player is requesting a move from a specified place on the board
// return 1 if the move is legal, 0 otherwise

int checkWin(int player);   // check if the player has won the game

// OPTIONAL FUNCTIONS =================================================================
// Please declare all functions that you have created for this program. Do not define
// them here. Put the code **after** the main() function, in the designated area.

int CheckDup(int i,char *NewName,char *NewLocation[4]);

void PrintMovesInfo(int player,int roll, char *UserInput, int *Info);

int NameToNumber(char *UserInput);

void AltPostRollInfo(int roll, int location, int player, int *PostRollArray2);

int PlayersPieceSelector(int player, int location);

// HELPER FUNCTIONS ===================================================================
// These functions are defined for you. (See code below main.)
// DO NOT delete these declarations or alter that code in any way.
void seedRandom(int);   // seed the random number generator
int pop_o_matic();  // "pop" to roll the dice, returns a number between 1 and 6

// MAIN FUNCTION ======================================================================

#ifndef TEST_MAIN   // DO NOT REMOVE this line -- it is needed for grading purposes.
int main() {
    int i,j;  // generic loop counters
    // initialize the board
    initializeBoard();
    // checking the board
    for (i=0; i<4; i++) assert(gHome[i] == 4);
    for (i=0; i<28; i++) assert(gRing[i] == -1);
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) assert(gFinish[i][j] == -1);
    }
    for (i=0; i<16; i++) assert(gPieces[i] == -1);

    // ask user for a random seed, and for the number of players
    int seed = 0;
    int numPlayers = 0;
    printf("Enter a random seed: ");
    fflush(stdout);
    scanf("%d", &seed);
    if (seed == -1) return 0;   // for testing only -- don't enter -1 in the real game
    seedRandom(seed);
    while (!numPlayers) {
        printf("Enter the number of players (2, 3, 4): ");
        fflush(stdout);
        scanf("%d", &numPlayers);
        if (numPlayers < 2 || numPlayers > 4) numPlayers = 0;
    }

    // play the game
    int player = Red;     // we'll start with Red
    int gameOver = 0;

    while (!gameOver) {
        // perform this player's turn
        gameOver = playerTurn(player);
        if (numPlayers == 2) player = (player + 2) % 4;
        else player = (player + 1) % numPlayers;
    }
    return 0;
}
#endif  // TEST_MAIN: DO NOT REMOVE this line -- it is needed for grading purposes.

// FUNCTION DEFINITIONS ===============================================================
// Please put your function definitions here. Do not put a main() function here.

void initializeBoard() {
    int i, j;

    for (i = 0; i < 4; i++) {       //sets 4 pieces in each players home
        gHome[i] = 4;
    }
    for (i=0; i<28; i++){           //makes all locations in ring empty
        gRing[i] = -1;
    }
    for (i=0; i<4; i++) {           //makes all location in finish lines empty
        for (j=0; j<4; j++)
            gFinish[i][j] = -1;
    }
    for (i=0; i<16; i++){           //sets each pieces' location home
        gPieces[i] = -1;
    }
}

void printStatus() {
    int i,j;
    const char *RingName[28] = {"R1", "R2", "R3", "R4", "R5", "R6", "R7",      //"array of strings" corresponding to names
                          "G1", "G2", "G3", "G4", "G5", "G6", "G7",
                          "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7",
                          "B1", "B2", "B3", "B4", "B5", "B6", "B7"};
    const char *FinishName[16] = {"RF1","RF2","RF3","RF4",
                                  "GF1","GF2", "GF3", "GF4",
                                  "YF1","YF2", "YF3", "YF4",
                                  "BF1", "BF2", "BF3", "BF4"};
    const char *HomeName = "H";
    const char *PieceName[16];                          //Made to contain string value of all players piece locations

    for(i=0;i<16;i++){                                  //Fills "PieceName" with appropriate string location of every piece
        if(gPieces[i] == -1) PieceName[i] = HomeName;       //Fills "H" for pieces that are home
        else if(gPieces[i] >= 100) {                        //Fills appropriate finish string(ie. RF1) for pieces that are on finish line
            j = i;
            j /= 4;
            j *=4;
            j += (gPieces[i]-100) - ((i/4)*10);
            PieceName[i] = FinishName[j];
        }
        else {                                              //Fills appropriate ring string(ie. R1) for pieces that are around the ring
            PieceName[i] = RingName[gPieces[i]];
        }
    }
    printf("%6s: %s,%s,%s,%s\n",gPlayerNames[0],PieceName[0],PieceName[1],PieceName[2],PieceName[3]);
    printf("%6s: %s,%s,%s,%s\n",gPlayerNames[1],PieceName[4],PieceName[5],PieceName[6],PieceName[7]);
    printf("%6s: %s,%s,%s,%s\n",gPlayerNames[2],PieceName[8],PieceName[9],PieceName[10],PieceName[11]);
    printf("%6s: %s,%s,%s,%s\n",gPlayerNames[3],PieceName[12],PieceName[13],PieceName[14],PieceName[15]);
}

int printMoves(int player, int roll) {
    int i, j, k, l;
    int counter = 0;
    k = player * 4;
    l = player * 7;

    char *RingName[28] = {"R1", "R2", "R3", "R4", "R5", "R6","R7",      //"array of strings" corresponding to names
                                "G1", "G2", "G3", "G4", "G5", "G6", "G7",
                                "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7",
                                "B1", "B2", "B3", "B4", "B5", "B6", "B7"};
    char *FinishName[16] = {"RF1", "RF2", "RF3", "RF4",
                                  "GF1", "GF2", "GF3", "GF4",
                                  "YF1", "YF2", "YF3", "YF4",
                                  "BF1", "BF2", "BF3", "BF4"};
    char *HomeName = "H";

    char *CurrentLocation[4];                             //Contains pieces location b4 roll in string form
    char *NewLocation[4] = {NULL,NULL,NULL,NULL};  //Contains pieces location after roll in string form
    int PostRollArray[2];                                       //Contains piece location in [0] & remaining moves in [1] after roll from a PostRollInfo() call
    char *NewName;                                        //placeholder name for CheckDup()

    for (i = 0, j = player * 4; i < 4; i++, j++) {                 //Fills CurrentLocation[] with string name of location before roll
        if (gPieces[j] == -1) CurrentLocation[i] = HomeName;         //Fills "H" for pieces that are home
        else if (gPieces[j] >= 100) {                                //Fills appropriate string for pieces that are on finish line
            int z = k;
            z += (gPieces[j] - 100) - (player * 10);
            CurrentLocation[i] = FinishName[z];
        } else {                                                      //Fills appropriate string for pieces that are around the ring
            CurrentLocation[i] = RingName[gPieces[j]];
        }
    }

    for (i = 0, j = k; i < 4; i++, j++) {              //Fills NewLocation[] with correct string names & updates # of possible moves

        int location;
        location = NameToNumber(CurrentLocation[i]);
        AltPostRollInfo(roll, location, player, PostRollArray); //****** was Calls PostRollInfo(j,player,PostRollArray)

        if (location == -1) {     //Executes if piece is home
            if ((roll != 6) || ((k <= gRing[l + 4]) && (gRing[l + 4] < k + 4 ))) continue; //Goes next piece if piece cant leave home
            else {
                NewName = RingName[l + 4];
                if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                else {                                                      //Prints move & next piece
                    counter += 1;
                    NewLocation[i] = RingName[l + 4];
                    printf("%s to %s\n", CurrentLocation[i], NewLocation[i]);
                    continue;
                }
            }
        }
        else if ((PostRollArray[1] == l + 3) && (PostRollArray[0] > 0)) {       //Executes if piece is right before finish line w/ moves remaining
            if ((PostRollArray[0] > 4) || (gFinish[player][PostRollArray[0] - 1] != -1)) continue; //Goes to next piece if piece can't or doesn't land in finish line
            else {
                NewName = FinishName[k + PostRollArray[0] - 1];             //Placeholder for name
                if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                else {                                                      //Prints move & next piece
                    counter += 1;
                    NewLocation[i] = FinishName[k + PostRollArray[0] - 1];
                    printf("%s to %s\n", CurrentLocation[i], NewLocation[i]);
                    continue;
                }
            }
        }
        else if(PostRollArray[1] >= 100) {      //Executes if piece is on finish line
            if(PostRollArray[1] == 103 + (10 * player)) {       //Executes if piece is at end of finish line
                if ((PostRollArray[0] > 0) || (gFinish[player][3] != -1)) continue; //Goes to next piece if piece goes past finish or end of finish is full
                else {                                              //Prints move if not duplicate**** and goes next piece
                    NewName = FinishName[k + 3];                                  //Placeholder for name
                    if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                    else {
                        counter += 1;
                        NewLocation[i] = FinishName[k + 3];
                        printf("%s to %s\n", CurrentLocation[i], NewLocation[i]);
                        continue;
                    }
                }
            }
            else {                                              //Prints move if not duplicate and goes next piece
                if(gFinish[player][PostRollArray[1] - (100 + (player *10))]) continue;
                else {
                    NewName = FinishName[k + (PostRollArray[1] - (100 + (player * 10)))]; //Placeholder for name
                    if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                    else {
                        counter += 1;
                        NewLocation[i] = FinishName[k + (PostRollArray[1] - (100 + (player * 10)))];
                        printf("%s to %s\n", CurrentLocation[i], NewLocation[i]);
                        continue;
                    }
                }
            }
        }
        else {                                  //Executes if piece is on ring but not right before finish line
            NewName = RingName[PostRollArray[1]];                       //Placeholder for name
            if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
            else {
                counter += 1;
                NewLocation[i] = RingName[PostRollArray[1]];
                printf("%s to %s\n", CurrentLocation[i], NewLocation[i]);
                continue;
            }
        }
    }
    return counter;
}

int movePiece(int player, int from, int spaces) {
    int PostRollArray2[2];
    int k = player * 4;
    int l = player * 7;
    int PlayersPieceNum;
    int PlayerReplaced;

    if (from >= 100) {                                               //returns 0 if player tries to move another players piece on finish line
        if (gFinish[player][from - (100 + (10 * player))] < k ||
            k + 4 <= gFinish[player][from - (100 + (10 * player))]) return 0; //Executes if piece starts in finish line
    } else if (from == -1) {                                            //returns 0 if player tries to move another players piece in home
        if (gHome[player] == 0) return 0;
    } else if (from < 28) {                                           //returns 0 if player tries to move another players piece around ring
        if ((gRing[from] < k || k + 4 <= gRing[from])) return 0;
    } else return 0;        //returns 0 if player tries to move a piece from non-existent location

    AltPostRollInfo(spaces, from, player, PostRollArray2);

    if (PostRollArray2[1] == -1) {     //Executes if piece is home
        if ((spaces != 6) || ((k <= gRing[l + 4]) && (gRing[l + 4] < k + 4))) return 0; //Goes next piece if piece cant leave home
        else {
            PlayersPieceNum = PlayersPieceSelector(player, from); //Gets the exact piece from gPieces that will be moved
            if(gRing[l + 4] > -1){                      //Executes if the piece will replace another
                PlayerReplaced = gRing[l + 4] / 4;      //Gets exact piece of player getting replaced
                gPieces[PlayerReplaced] = -1;           //Change value of replaced piece
                gHome[PlayerReplaced] += 1;             //Change value of replaced pieces new location
            }
            gPieces[PlayersPieceNum] = l + 4;           //Change value of piece
            gRing[l + 4] = PlayersPieceNum;             //Change value of new location
            gHome[player] -= 1;                         //Change value of old location
        return 1;
        }
    }
    else if ((PostRollArray2[1] == l + 3) && (PostRollArray2[0] > 0)) {       //Executes if piece is right before finish line w/ moves remaining
        if ((PostRollArray2[0] > 4) || (gFinish[player][PostRollArray2[0] - 1] != -1)) return 0; //Goes to next piece if piece can't or doesn't land in finish line
        else {
            PlayersPieceNum = PlayersPieceSelector(player, from);           //Gets the exact piece from gPieces that will be moved
            gPieces[PlayersPieceNum] = (100 + (player * 10) + (PostRollArray2[0] - 1));       //Change value of piece
            gFinish[player][PostRollArray2[0] - 1] = PlayersPieceNum;               //Chang value in new location
            gRing[from] = -1;                                                       //Change value in old location
        return 1;
        }
    }
    else if (PostRollArray2[1] >= 100) {      //Executes if piece is on finish line
        if (PostRollArray2[1] == 103 + (10 * player)) {       //Executes if piece is at end of finish line
            if ((PostRollArray2[0] > 0) || (gFinish[player][3] != -1)) return 0; //Goes to next piece if piece goes past finish or end of finish is full
            else {
                PlayersPieceNum = PlayersPieceSelector(player, from);           //Gets the exact piece from gPieces that will be moved
                gPieces[PlayersPieceNum] = PostRollArray2[1];                           //Change value of piece
                gFinish[player][3] = PlayersPieceNum;                                   //Change value in new location
                gFinish[player][from -(100 + (player * 10))] = -1;                      //Change value in old location
            return 1;
            }

        }
        else {   //Finish line but not end (no moves left)
            if (gFinish[player][PostRollArray2[1]] != -1) return 0; //Goes to next piece if piece goes past finish or end of finish is full
            else {
                PlayersPieceNum = PlayersPieceSelector(player,from);           //Gets the exact piece from gPieces that will be moved
                gPieces[PlayersPieceNum] = PostRollArray2[1];                           //Change value of piece
                gFinish[player][PostRollArray2[1] - (100 + (player * 10))] = PlayersPieceNum; //Change value in new location
                gFinish[player][from - (100 + (player * 10))] = -1;                      //Change value in old location
            return 1;
            }
        }
    }
    else {                                  //Executes if piece is on ring, but not right before finish line with moves left
        if((k <= gRing[PostRollArray2[1]]) && (gRing[PostRollArray2[1]] < k + 4)) return 0;  //return 0 if move lands on one of your own pieces
        else {
            PlayersPieceNum = PlayersPieceSelector(player,
                                                   from);        //Gets the exact piece from gPieces that will be moved
            if (gRing[PostRollArray2[1]] > -1) {          //Executes if the piece will replace another
                PlayerReplaced = PostRollArray2[1] / 4;      //Gets exact piece of player getting replaced
                gPieces[PlayerReplaced] = -1;           //Change value of replaced piece
                gHome[PlayerReplaced] += 1;             //Change value of replaced pieces new location
            } else {
                gPieces[PlayersPieceNum] = PostRollArray2[1];    //Change value of piece
                gRing[PostRollArray2[1]] = PlayersPieceNum;      //Change value of new location
                gRing[from] = -1;                             //Change value of old location
            }
            return 1;
        }
    }
}


int checkWin(int player){
    if(gFinish[player][0] != -1 && gFinish[player][1] != -1 && gFinish[player][2] != -1 && gFinish[player][3] != -1) return 1;
    else return 0;
}

int playerTurn(int player) {        //goes through process of a players turn
    char UserInput[4] = "0";

    int Info[2] = {0,0};

    int location;

    printf("---- %s's turn\n", gPlayerNames[player]);

    printStatus();

    int roll = pop_o_matic();

    printf("You rolled %d.\n", roll);

    printMoves(player,roll);



    while(roll == 6) {                                      //Executes if roll is 6

        PrintMovesInfo(player,roll,UserInput,Info);             //Fills Info[0] & Info[1] ****also moves the piece through call of postrollarray

        if(Info[0] == 0) {                                      //Executes if there are no legal moves
            printf("You have no legal moves.\n");
            printf("You rolled 6, so you get to roll again.\n");
            printStatus();
            roll = pop_o_matic();
            printf("You rolled %d.\n", roll);
            printMoves(player,roll);
            continue;
        }
        else {                                                  //Executes if there is a legal move
            printf("Enter position of piece to move:");
            scanf("%3s", UserInput);

            PrintMovesInfo(player,roll,UserInput,Info);         //If User input matches a possible move then Info[1] = 1

            while(Info[1] == 0){                    //Executes while user input is not a legal move
                if(*UserInput == 'Q') return 1;
                else {
                    printf("Illegal move, try again.\n");
                    printf("Enter position of piece to move:");
                    scanf("%3s", UserInput);
                    PrintMovesInfo(player,roll,UserInput,Info);
                    continue;
                }
            }
            location = NameToNumber(UserInput);
            movePiece(player,location,roll);
            if(checkWin(player) == 1) return 1;
            else {
                printf("You rolled 6, so you get to roll again.\n");
                printStatus();
                roll = pop_o_matic();
                printf("You rolled %d.\n", roll);
                printMoves(player,roll);
                continue;
            }
        }
    }

    PrintMovesInfo(player,roll,UserInput,Info);             //Fills Info[0] & Info[1]

    if(Info[0] == 0) {                                      //Executes if there are no legal moves
        printf("You have no legal moves.\n\n");
        return 0;
    }
    else{                                                   //Executes if there is a legal move
        printf("Enter position of piece to move:");
        scanf("%3s", UserInput);

        PrintMovesInfo(player,roll,UserInput,Info);         //If User input matches a possible move then Info[1] = 1

        while(Info[1] == 0){                                //Executes while user input is not a legal move
            if(*UserInput == 'Q') return 1;
            else {
                printf("Illegal move, try again.\n");
                printf("Enter position of piece to move:");
                scanf("%3s", UserInput);
                PrintMovesInfo(player,roll,UserInput,Info);
            }
        }
        location = NameToNumber(UserInput);
        movePiece(player,location,roll);
        if(checkWin(player) == 1) return 1;
        else return 0;
    }
}

int CheckDup(int i,char *NewName,char *NewLocation[4]) {       //Returns 1 if name is duplicate & 0 if not

    int p = i;

    while (p >= 0) {        //checks each existing name
            if(NewLocation[p] == NULL){                         //goes to next name if established name is null b/c it crashes strcmp
                p -=1;
                continue;
            }
            else if(strcmp(NewName, NewLocation[p]) == 0) {     //compares names & returns 1 if duplicate
                return 1;
            }
            else p -= 1;                                        //goes next name
        }
    return 0;                                                //returns 0 if no duplicates detected
}

void PrintMovesInfo(int player, int roll, char *UserInput, int *Info){           //Info[0] holds count of valid moves after roll & Info[1] has a 1 if the userinput matches a possible move
    int i, j, k, l;
    int counter = 0;
    k = player * 4;
    l = player * 7;

    char *RingName[28] = {"R1", "R2", "R3", "R4", "R5", "R6","R7",      //"array of strings" corresponding to names
                                "G1", "G2", "G3", "G4", "G5", "G6", "G7",
                                "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7",
                                "B1", "B2", "B3", "B4", "B5", "B6", "B7"};
    char *FinishName[16] = {"RF1", "RF2", "RF3", "RF4",
                                  "GF1", "GF2", "GF3", "GF4",
                                  "YF1", "YF2", "YF3", "YF4",
                                  "BF1", "BF2", "BF3", "BF4"};
    char *HomeName = "H";

    char *CurrentLocation[4];                             //Contains pieces location b4 roll in string form
    char *NewLocation[4] = {NULL,NULL,NULL,NULL};  //Contains pieces location after roll in string form
    char *CompareLocation[4] = {NULL,NULL,NULL,NULL}; //location of piece that can move from roll
    int PostRollArray[2];                                       //Contains piece location in [0] & remaining moves in [1] after roll from a PostRollInfo() call
    char *NewName;                                        //placeholder name for CheckDup()

    for (i = 0, j = player * 4; i < 4; i++, j++) {                 //Fills CurrentLocation[] with string name of location before roll
        if (gPieces[j] == -1) CurrentLocation[i] = HomeName;         //Fills "H" for pieces that are home
        else if (gPieces[j] >= 100) {                                //Fills appropriate string for pieces that are on finish line
            int z = k;
            z += (gPieces[j] - 100) - (player * 10);
            CurrentLocation[i] = FinishName[z];
        } else {                                                      //Fills appropriate string for pieces that are around the ring
            CurrentLocation[i] = RingName[gPieces[j]];
        }
    }

    for (i = 0, j = k; i < 4; i++, j++) {              //Fills NewLocation[] with correct string names & updates # of possible moves
        int location;
        location = NameToNumber(CurrentLocation[i]);
        AltPostRollInfo(roll, location, player, PostRollArray); //****** was Calls PostRollInfo(j,player,PostRollArray)

        if (location == -1) {     //Executes if piece is home
            if ((roll != 6) || ((k <= gRing[l + 4]) && (gRing[l + 4] < k + 4 ))) continue; //Goes next piece if piece cant leave home
            else {
                NewName = RingName[l + 4];
                if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                else {                                                      //Prints move & next piece
                    counter += 1;
                    NewLocation[i] = RingName[l + 4];
                    CompareLocation[i] = CurrentLocation[i];
                    continue;
                }
            }
        }
        else if ((PostRollArray[1] == l + 3) && (PostRollArray[0] > 0)) {       //Executes if piece is right before finish line w/ moves remaining
            if ((PostRollArray[0] > 4) || (gFinish[player][PostRollArray[0] - 1] != -1)) continue; //Goes to next piece if piece can't or doesn't land in finish line
            else {
                NewName = FinishName[k + PostRollArray[0] - 1];             //Placeholder for name
                if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                else {                                                      //Prints move & next piece
                    counter += 1;
                    NewLocation[i] = FinishName[k + PostRollArray[0] - 1];
                    CompareLocation[i] = CurrentLocation[i];
                    continue;
                }
            }
        }
        else if(PostRollArray[1] >= 100) {      //Executes if piece is on finish line
            if(PostRollArray[1] == 103 + (10 * player)) {       //Executes if piece is at end of finish line
                if ((PostRollArray[0] > 0) || (gFinish[player][3] != -1)) continue; //Goes to next piece if piece goes past finish or end of finish is full
                else {                                              //Prints move if not duplicate**** and goes next piece
                    NewName = FinishName[k + 3];                                  //Placeholder for name
                    if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                    else {
                        counter += 1;
                        NewLocation[i] = FinishName[k + 3];
                        CompareLocation[i] = CurrentLocation[i];
                        continue;
                    }
                }
            }
            else {                                              //Prints move if not duplicate and goes next piece
                NewName = FinishName[k + (PostRollArray[1] - (100 + (player * 10)))]; //Placeholder for name
                if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
                else {
                    counter += 1;
                    NewLocation[i] = FinishName[k + (PostRollArray[1] - (100 + (player * 10)))];
                    CompareLocation[i] = CurrentLocation[i];
                    continue;
                }
            }
        }
        else {                                  //Executes if piece is on ring but not right before finish line
            NewName = RingName[PostRollArray[1]];                       //Placeholder for name
            if (CheckDup(i, NewName, NewLocation)) continue;            //Goes next piece if duplicate move
            else {
                counter += 1;
                NewLocation[i] = RingName[PostRollArray[1]];
                CompareLocation[i] = CurrentLocation[i];
                continue;
            }
        }
    }
    Info[0] = counter;
    Info[1] = CheckDup(3, UserInput, CompareLocation);
}

int NameToNumber(char *UserInput){
    size_t k = strlen(UserInput);
    int i;
    int total = 0;
    for(i = 0; i < k; i++){
        if(k == 3){     //Execute when name is on finish line
            total += 100;
            if(UserInput[i] == 'R') total += 0;
            else if(UserInput[i] == 'G') total += 10;
            else if(UserInput[i] == 'Y') total += 20;
            else if(UserInput[i] == 'B') total += 30;
            else if(UserInput[i] == '1') total += 1;
            else if(UserInput[i] == '2') total += 2;
            else if(UserInput[i] == '3') total += 3;
        }
        else if(k == 2){
            if(UserInput[i] == 'R') total += 0;
            else if(UserInput[i] == 'G') total += 7;
            else if(UserInput[i] == 'Y') total += 14;
            else if(UserInput[i] == 'B') total += 21;
            else if(UserInput[i] == '1') total += 0;
            else if(UserInput[i] == '2') total += 1;
            else if(UserInput[i] == '3') total += 2;
            else if(UserInput[i] == '4') total += 3;
            else if(UserInput[i] == '5') total += 4;
            else if(UserInput[i] == '6') total += 5;
            else if(UserInput[i] == '7') total += 6;
        }
        else if(k == 1){
            if(UserInput[i] == 'H') total = -1;
        }
    }
    return total;
}

void AltPostRollInfo(int roll, int location, int player, int *PostRollArray2) {       //Puts location of piece on ring & moves remaining into array
    int MovesLeft = roll;

    while (MovesLeft > 0) {     //Updates piece location and "moves left" until there are no more moves left or condition is met
        if(location == (103 + (10 * player)) || location == (player * 7) + 3 || location == -1) break; //stops if piece is right before finish line or end of finish line or home
        else if (location == 27) location -= 28;        //Wraps piece around ring if hits ring edge
        location += 1;      //Increments location
        MovesLeft -= 1;                 //decrements "moves left"
    }
    PostRollArray2[0] = MovesLeft;
    PostRollArray2[1] = location;
}

int PlayersPieceSelector(int player, int location){
    int k,K;
    for(K = (player * 4) + 4,k = player * 4; k < K; k++){
        if(gPieces[k] == location) return k;
    }
    return -1;
}

//-----------------------------------------------------------------------------------------
// Pseudorandom number generator -- DO NOT edit or remove this variable or these functions

// Using 16-bit linear feedback shift register (LFSR) for pseudorandom number generator
int lfsr = 0;  // global variable -- state of random number generator

// lfsrNext -- function to advance Xorshift LFSR for pseudorandom number generation
// uses global variable (lfsr)
// code from: https://en.wikipedia.org/wiki/Linear-feedback_shift_register
int lfsrNext(void) {
    lfsr ^= lfsr >> 7;
    lfsr ^= lfsr << 9;
    lfsr ^= lfsr >> 13;
    return (int) (lfsr & INT_MAX);  // return a positive int
}

// set starting value for LFSR-based random number generator
void seedRandom(int seed) {
    if (seed) lfsr = seed;
}
// return a random integer between 0 and limit-1
int getRandom(int limit) {
    return lfsrNext() % limit;
}

int pop_o_matic() {
    return getRandom(6) + 1;
}
//-----------------------------------------------------------------------------------------
