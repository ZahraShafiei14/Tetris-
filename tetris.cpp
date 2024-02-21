#include <iostream>
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
using namespace std;
using namespace chrono;
// Terminal color codes------------------------
const string RED_COLOR = "\033[31m";
const string GREEN_COLOR = "\033[32m";
const string YELLOW_COLOR = "\033[33m";
const string PURPLE_COLOR = "\033[36m";
const string BLUE_COLOR = "\033[34m";
const string WHITE_COLOR = "\033[37m";
const string MAGENTA_COLOR = "\033[35m";
const string BLACK_COLOR = "\033[30m";
//---------------------------------------------
struct Player{
    string name;
    int score;
    int timer;
};

struct tetromino{
    int shape;
    int x , y;
    int cntRotate; //counter of amount of rotate
};
//------------------------
void menu();
void NewGame();
void Game(int**);
void validMove(int**);
void right(int**);
void left(int**);
void isFullRow(int**);
void isGameOver(int**);
void randShape(int**);
void rotate(int**);
void down(int**);
void pause(int**);
void saveGame(int**);
void saveLoad();
void MoveCursor(int , int);
void cursorRemover(bool , DWORD);
void shadowShape(int**);
void shadowRemove(int**);
void speedUp();
void newGame2p();
void save();
//------------------------
Player player;
Player player1 , player2;
tetromino block;
tetromino blockp1 , blockp2;
int row, col;
double S_PER_UPDATE = 1;
double TempFps;
time_point<system_clock> first , last;
int* list;
int nextBlock;
int currentBlock;
bool firstShape = true; 
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
int ceil_points = 200;
double downTime = 0.96;
int timeSaved = 0;
string difficulty;
int numPlayers; 
Player* players;
//------------------------
int main(){
    SetConsoleOutputCP(CP_UTF8);
    cursorRemover(0, 0);
    menu();
    return 0;
} 
void boardDeleter(int** board){
    for(int i = 0 ; i < row ; i++)
        delete[] board[i];
    delete[] board;
}
//------leaderboard-------
int playersCnt(){
    ifstream readfile("leaderboard" + difficulty + ".txt");
    string line;
    numPlayers = 0;
    while (getline(readfile, line)){
        numPlayers++;
    }
    return numPlayers;
}
void read() {
    ifstream readfile("leaderboard" + difficulty + ".txt");
    string line;
    int i = 0;
    players = new Player[playersCnt()];
    while (getline(readfile, line)){
        players[i].name = line.substr(0, line.find(","));
        players[i].score = stoi(line.substr(line.find(",") + 1, line.find(".")));
        players[i].timer = stoi(line.substr(line.find(".") + 1));
        i++;
    }
    numPlayers = i; 
}
void save(){
    bool highScore = true;
    read();
    for(int i = 0 ; i < numPlayers; i++){
        if(players[i].name == player.name){
            highScore = false;
            if(player.score > players[i].score){
                players[i].score = player.score;
                players[i].timer = player.timer;
                ofstream file("leaderboard" + difficulty + ".txt");
                for(int i = 0 ; i < numPlayers ;i++)
                    file << players[i].name << "," << players[i].score << "." << players[i].timer << '\n';
                break;
            }
        }
    }
    if(highScore){
        ofstream file("leaderboard" + difficulty + ".txt" , ios::app);
        file << player.name << "," << player.score << "." << player.timer << '\n';
    }
}
void TimeSort(){
    bool swapped;
    for (int i = 0; i < numPlayers - 1; i++) {
        swapped = false;
        for (int j = 0; j < numPlayers - i - 1; j++) {
            if (players[j].score == players[j + 1].score) {
                if(players[j].timer > players[j + 1].timer){
                    swap(players[j], players[j + 1]);
                    swapped = true;
                }
            }
        }
        if (swapped == false)
           break;
    }
}
void bubbleSort(){
    bool swapped;
    for (int i = 0; i < numPlayers - 1; i++) {
        swapped = false;
        for (int j = 0; j < numPlayers - i - 1; j++) {
            if (players[j].score < players[j + 1].score) {
                swap(players[j], players[j + 1]);
                swapped = true;
            
            }
        }
        if (swapped == false)
           break;
    }
    TimeSort();
}
void showLeaderboard() {
    cout << MAGENTA_COLOR <<"╔═════════════╗     "<< YELLOW_COLOR<<"╔═════════════╗     "<< GREEN_COLOR<<"╔═════════════╗\n";
    cout << MAGENTA_COLOR <<"║   1. EASY   ║     "<< YELLOW_COLOR<<"║  2. MEDIUM  ║     "<< GREEN_COLOR<<"║   3. HARD   ║\n";
    cout << MAGENTA_COLOR <<"╚═════════════╝     "<< YELLOW_COLOR<<"╚═════════════╝     "<< GREEN_COLOR<<"╚═════════════╝\n";
    while(true){
        char diff = getch();
        system("cls");
        switch(diff){
            case '1':
            difficulty = "easy";
            cout << MAGENTA_COLOR <<"╔═════════════╗     "<< BLACK_COLOR<<"╔═════════════╗     "<< BLACK_COLOR<<"╔═════════════╗\n";
            cout << MAGENTA_COLOR <<"║   1. EASY   ║     "<< BLACK_COLOR<<"║  2. MEDIUM  ║     "<< BLACK_COLOR<<"║   3. HARD   ║\n";
            cout << MAGENTA_COLOR <<"╚═════════════╝     "<< BLACK_COLOR<<"╚═════════════╝     "<< BLACK_COLOR<<"╚═════════════╝\n";
            break;
            case '2':
            difficulty = "medium";
            cout << BLACK_COLOR <<"╔═════════════╗     "<< YELLOW_COLOR<<"╔═════════════╗     "<< BLACK_COLOR<<"╔═════════════╗\n";
            cout << BLACK_COLOR <<"║   1. EASY   ║     "<< YELLOW_COLOR<<"║  2. MEDIUM  ║     "<< BLACK_COLOR<<"║   3. HARD   ║\n";
            cout << BLACK_COLOR <<"╚═════════════╝     "<< YELLOW_COLOR<<"╚═════════════╝     "<< BLACK_COLOR<<"╚═════════════╝\n";
            break;
            case '3':
            difficulty = "hard";
            cout << BLACK_COLOR <<"╔═════════════╗     "<< BLACK_COLOR<<"╔═════════════╗     "<< GREEN_COLOR<<"╔═════════════╗\n";
            cout << BLACK_COLOR <<"║   1. EASY   ║     "<< BLACK_COLOR<<"║  2. MEDIUM  ║     "<< GREEN_COLOR<<"║   3. HARD   ║\n";
            cout << BLACK_COLOR <<"╚═════════════╝     "<< BLACK_COLOR<<"╚═════════════╝     "<< GREEN_COLOR<<"╚═════════════╝\n";
            break;
            case 'e':
            delete [] players;
            menu();
            break;
            default:
            break;
        }
        cout <<"\n\n";
        read();
        bubbleSort();
        for(int i = 0 ; i < 40; i++)
            cout << RED_COLOR << "\u2501";
        cout << "\n\n";
                    cout <<PURPLE_COLOR<<"╔═══════════════════════════╗\n";
        cout <<PURPLE_COLOR << setw(10) <<"║Rank   Name" << setw(6) << "   Score" << setw(6) << "  Time  ║" << "\n";
                    cout <<PURPLE_COLOR <<"╚═══════════════════════════╝\n";
        for (int i = 0; i < numPlayers; ++i) {
            cout << i + 1 << "."
                << setw(10) << players[i].name
                << setw(6) << players[i].score
                << setw(6) << players[i].timer / 60 << ':';
                if(players[i].timer % 60 < 10)
                    cout << '0' << players[i].timer % 60  << "\n";
                else
                    cout << players[i].timer % 60  << "\n";
        }
        cout <<"\n\n Press E to Return to the Main Menu.";
    }
}
//----------help------------
void Help(){
    system("cls");
    cout << RED_COLOR<<"╔════════════════════════════════════════════════════════════╗\n";
    cout << RED_COLOR<<"║Tetris was made in 1984.It's a simple game here is the keys:║\n";
    cout << RED_COLOR<<"╚════════════════════════════════════════════════════════════╝\n\n";
    cout << BLUE_COLOR <<"        "<<"╔═════════════╗                "<< YELLOW_COLOR<<"╔═════════════╗\n";
    cout << BLUE_COLOR <<"        "<<"║ A: LEFT <=  ║                "<< YELLOW_COLOR<<"║  R: ROTATE  ║\n";
    cout << BLUE_COLOR <<"        "<<"╚═════════════╝                "<< YELLOW_COLOR<<"╚═════════════╝\n";
    cout << PURPLE_COLOR <<"        "<<"╔═════════════╗                "<< MAGENTA_COLOR<<"╔═════════════╗\n";
    cout << PURPLE_COLOR <<"        "<<"║ D: RIGHT => ║                "<< MAGENTA_COLOR<<"║ Q: PAUSE || ║\n";
    cout << PURPLE_COLOR <<"        "<<"╚═════════════╝                "<< MAGENTA_COLOR<<"╚═════════════╝\n";
    cout << GREEN_COLOR<<"        "<<"╔═════════════╗                "<< RED_COLOR<<"╔═════════════╗\n";
    cout << GREEN_COLOR<<"        "<<"║  S: DOWN ↓  ║                "<< RED_COLOR<<"║SPASE: PLACE ║\n";
    cout << GREEN_COLOR<<"        "<<"╚═════════════╝                "<< RED_COLOR<<"╚═════════════╝\n";
    system("pause");
    system("cls");
    menu();
}
//--------------------------------
void menu(){
    cout << endl;
    cout << BLUE_COLOR   << "████████╗███████╗████████╗██████╗ ██╗███████╗\n";
    cout << PURPLE_COLOR << "╚══██╔══╝██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝\n";
    cout << GREEN_COLOR  << "   ██║   █████╗     ██║   ██████╔╝██║███████╗\n";
    cout << YELLOW_COLOR << "   ██║   ██╔══╝     ██║   ██╔══██╗██║╚════██║\n"; 
    cout << MAGENTA_COLOR<< "   ██║   ███████╗   ██║   ██║  ██║██║███████║\n"; 
    cout << RED_COLOR    << "   ╚═╝   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝\n";
    cout << RED_COLOR    << "       A Game Made By Zahra And Soroush       \n\n";
    cout <<"                "<< BLUE_COLOR <<"╔═════════════╗\n";
    cout <<"                "<< BLUE_COLOR <<"║ 1. NEW GAME ║\n";
    cout <<"                "<< BLUE_COLOR <<"╚═════════════╝\n";
    cout <<"                "<< PURPLE_COLOR <<"╔═════════════╗\n";
    cout <<"                "<< PURPLE_COLOR <<"║ 2. CONTINUE ║\n";
    cout <<"                "<< PURPLE_COLOR <<"╚═════════════╝\n";
    cout <<"                "<< GREEN_COLOR<<"╔═════════════╗\n";
    cout <<"                "<< GREEN_COLOR<<"║3.LEADERBOARD║\n";
    cout <<"                "<< GREEN_COLOR<<"╚═════════════╝\n";
    cout <<"                "<< YELLOW_COLOR<<"╔═════════════╗\n";
    cout <<"                "<< YELLOW_COLOR<<"║4.HOW TO PLAY║\n";
    cout <<"                "<< YELLOW_COLOR<<"╚═════════════╝\n";
    cout <<"                "<< MAGENTA_COLOR<<"╔═════════════╗\n";
    cout <<"                "<< MAGENTA_COLOR<<"║    5.EXIT   ║\n";
    cout <<"                "<< MAGENTA_COLOR<<"╚═════════════╝\n\n";
    char option = getch();
    system("cls");
    switch(option){
        case '1':
        NewGame();
        break;
        case '2':
        saveLoad();
        break;
        case '3':
        showLeaderboard();
        break;
        case '4':
        Help();
        break;
        case '5':
        exit(0);
        break;
        //case '6':
        //newGame2p();
        //break;
        default:
        menu();
        break;
    }
}

void printNext(){
    cout <<"\n\n";
    switch(nextBlock){
        case 1:
        cout <<"  \033[103m    \033[0m  \n  \033[103m    \033[0m  "; //square shape
        break;
        case 2:
        cout <<"\033[102m        \n\033[0m        "; //line shape
        break;
        case 3:
        cout << "    \033[106m  \033[0m  \n  \033[106m      \033[0m  "; // T shape
        break;
        case 4:
        cout <<"  \033[104m    \033[0m  \n    \033[104m    \033[0m"; // S shape
        break;
        case 5:
        cout << "    \033[45m    \033[0m\n  \033[45m    \033[0m  "; // Z shape
        break;
        case 6:
        cout <<"      \033[46m  \033[0m\n  \033[46m      \033[0m";// L shape
        break;
        case 7:
        cout << "  \033[101m  \033[0m    \n  \033[101m      \033[0m"; // J shape
        break;
    }
    cout << "\n\nNext Shape\n\n";
}

void print(int** board){
    MoveCursor(0 , 0);
    last =  system_clock::now();
    duration<double> elapsed_time = (last - first);
    player.timer = timeSaved + elapsed_time.count();
    printNext();
    cout << "\u2554";
       for(int i = 0 ; i < col*3 - 1 ; i++){
            if((i + 1) % 3 == 0){
                cout <<"\u2566";
            }
            else
            cout <<  "\u2550";
       }
       cout << "\u2557";
       cout << endl;

    for(int i = 3 ; i < row - 3  ; i++){
        cout << "\u2503";
            for(int j = 0 ; j < col ; j++){
                switch(abs(board[i][j])){
                    case 0:
                        cout <<"  ";
                        break;
                    case 1:
                        cout << "\033[103m  \033[0m";
                        break;
                    case 2:
                        cout <<"\033[102m  \033[0m";
                        break;
                    case 3:
                        cout << "\033[106m  \033[0m";
                        break;
                    case 4:
                        cout <<"\033[104m  \033[0m";
                        break;
                    case 5:
                        cout << "\033[45m  \033[0m";
                        break;
                    case 6:
                        cout <<"\033[46m  \033[0m";
                        break;
                    case 7:
                        cout << "\033[101m  \033[0m";
                        break;
                    case 10:
                        cout << "\033[100m  \033[0m";
                        break;
                }
                if(j < col - 1)
                    cout << "\u2551";
                else
                    cout << "\u2503";
            }
            
            cout << endl;
            if(i != row - 4){
            cout << "\u2523";
            for(int i = 0 ; i < col * 3 - 1 ;i++){
                if((i + 1) %  3== 0){
                    cout << "\u256C";
                }
                else
                cout << "\u2550";
            }
            cout << "\u252B";
            }
            else{
        cout << "\u2517";
        for(int i = 0 ; i <col * 3 - 1 ; i++){
            if((i + 1) % 3 == 0){
                cout <<"\u253B";
            }
            else
            cout <<  "\u2501";
       }
       cout << "\u251B";
       cout << endl;
            }
            cout << endl;
        }
    
    cout << "\nScore: " << player.score;
    cout << "\nTime: " <<  player.timer;
}

void cursorRemover(bool visible, DWORD size){
    if(size == 0)
        size = 20;
    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

void MoveCursor(int x , int y){
    COORD position = {x , y};
    SetConsoleCursorPosition(console , position);
}

void diff(){
    system("cls");
    cout <<"       "<< BLUE_COLOR <<"╔══════════════════════════╗\n";
    cout <<"       "<< BLUE_COLOR <<"║   CHOOSE THE DIFFICULTY  ║\n";
    cout <<"       "<< BLUE_COLOR <<"╚══════════════════════════╝\n";
    cout <<"            "<< GREEN_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< GREEN_COLOR <<"║   1. EASY   ║\n";
    cout <<"            "<< GREEN_COLOR <<"╚═════════════╝\n";
    cout <<"            "<< YELLOW_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< YELLOW_COLOR <<"║  2. MEDIUM  ║\n";
    cout <<"            "<< YELLOW_COLOR <<"╚═════════════╝\n";
    cout <<"            "<< MAGENTA_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< MAGENTA_COLOR <<"║   3. HARD   ║\n";
    cout <<"            "<< MAGENTA_COLOR <<"╚═════════════╝\n\n";
    char option = getch();
    switch(option){
        case '1':
        S_PER_UPDATE = 1.5;
        downTime = 0.96;
        difficulty = "easy";
        break;
        case '2':
        S_PER_UPDATE = 1;
        downTime = 0.94;
        difficulty = "medium";
        break;
        case '3':
        S_PER_UPDATE = 0.5;
        downTime = 0.92;
        difficulty = "hard";
        break;
    }
    system("cls");
}

void NewGame(){
    system("cls");  
    cout << BLUE_COLOR   << "███    ██ ███████ ██     ██      ██████   █████  ███    ███ ███████ \n";
    cout << PURPLE_COLOR << "████   ██ ██      ██     ██     ██       ██   ██ ████  ████ ██      \n";
    cout << GREEN_COLOR  << "██ ██  ██ █████   ██  █  ██     ██   ███ ███████ ██ ████ ██ █████   \n";
    cout << YELLOW_COLOR << "██  ██ ██ ██      ██ ███ ██     ██    ██ ██   ██ ██  ██  ██ ██      \n";
    cout << MAGENTA_COLOR<< "██   ████ ███████  ███ ███       ██████  ██   ██ ██      ██ ███████ \n\n";
                    cout <<"                     ══════════════════════\n";
    cout << BLACK_COLOR  <<"                    ENTER YOUR NAME:  ";
    cin >> player.name;
        cout << RED_COLOR<<"                     ══════════════════════\n";
    cout << BLACK_COLOR  <<"                    ENTER THE ROWS OF BOARD: ";
    cin >> row;
    cout << YELLOW_COLOR <<"                     ══════════════════════\n";
    cout << BLACK_COLOR  <<"                    ENTER THE COLOUMN OF BOARD: ";
    cin >> col;
    row += 6;    //making a bigger board for top and end of it
    int** board = new int*[row];
    for(int i = 0 ; i < row ; i++){
        board[i] = new int[col];
        for(int j = 0 ; j < col ; j++) 
            board[i][j] = 0;
    }
    for(int i = row - 1 ; i > row - 4 ; i--)
        for(int j = 0 ; j < col ; j++)
            board[i][j] = -10;
    first = system_clock::now();
    player.score = 0;
    ceil_points = 200;
    firstShape = true;
    diff();
    randShape(board);
    print(board);
    Game(board);
}

void Game(int** board){
    time_point<system_clock> previous, current;
    double lag = 0;
    TempFps = S_PER_UPDATE;
    shadowShape(board);
    while(true){
        current = system_clock::now();
        if (lag >= S_PER_UPDATE) {
            down(board);
            print(board);
            lag = 0;
            }
        if(_kbhit()){
            switch(getch()){
                case 'd':
                shadowRemove(board);
                right(board);
                shadowShape(board);
                print(board);
                break;
                case 'a':
                shadowRemove(board);
                left(board);
                shadowShape(board);
                print(board);
                break;
                case 's':             
                down(board);
                print(board);
                break;
                case 'r':
                shadowRemove(board);
                rotate(board);
                shadowShape(board);
                break;
                case 'q':
                pause(board);
                break;
                case ' ':
                S_PER_UPDATE = 0.005;
                break;
            }
        }
        previous =  system_clock::now();
        duration<double> elapsed = (previous - current);
        lag += elapsed.count();
        }
}

void saveGame(int** board){
    ofstream saveApp("Save.txt");
    saveApp << row << ',' << col << '.' << player.score << '/' << player.timer << ';' << player.name << ':' << difficulty << '\n';
    for(int i = 0 ; i < row ; i++){
        for(int j = 0 ; j < col ; j++){
            saveApp << board[i][j] << '\n';
        }
    }
    pause(board);
}
void saveLoad(){
    ifstream saveLoad("Save.txt");
    string line;
    int l = 0;
    int k = 0;
    while(getline(saveLoad, line)){
        if(l == 0){
            row = stoi(line.substr(0,line.find(",")));
            col = stoi(line.substr(line.find(",")+1 ,line.find(".")));
            player.score = stoi(line.substr(line.find(".")+1 ,line.find("/")));
            timeSaved = stoi(line.substr(line.find("/")+1 ,line.find(";")));
            player.name = line.substr(line.find(";")+1 , line.find(":"));
            difficulty = line.substr(line.find(":")+1);
            list = new int[row*col];
        }
        else{
            if(stoi(line.substr(0)) > 0)
                list[k] = 0;
            else
                list[k] = stoi(line.substr(0));
            k++;
        }
        l++;
    }
    k = 0;
    int** board = new int*[row];
    for(int i = 0 ; i < row ; i++){
        board[i] = new int[col];
        for(int j = 0 ; j < col ; j++){ 
            board[i][j] = list[k];
            k++;
        }
    }
    if(difficulty == "easy")
        downTime = 0.96;
    else if(difficulty == "medium")
        downTime = 0.94;
    else
        downTime = 0.92;
    firstShape = true;
    first = system_clock::now();
    randShape(board);
    Game(board);   
}

void pause(int** board){
    system("cls");
    cout <<"            "<< GREEN_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< GREEN_COLOR <<"║ 1.CONTINUE  ║\n";
    cout <<"            "<< GREEN_COLOR <<"╚═════════════╝\n";
    cout <<"            "<< YELLOW_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< YELLOW_COLOR <<"║ 2.SAVE GAME ║\n";
    cout <<"            "<< YELLOW_COLOR <<"╚═════════════╝\n";
    cout <<"            "<< MAGENTA_COLOR <<"╔═════════════╗\n";
    cout <<"            "<< MAGENTA_COLOR <<"║ 3.MAIN MENU ║\n";
    cout <<"            "<< MAGENTA_COLOR <<"╚═════════════╝\n\n";
    char option = getch();
    system("cls");
    switch(option){
        case '1':
        print(board);
        Game(board);
        break;
        case '2':
        saveGame(board);
        break;
        case '3':
        menu();
        break;
        default:
        pause(board);
        break;
    }
}

void validMove(int** board){
    bool flag = false;
    for(int i = row - 4 ; i >= 0 ; i--){
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == block.shape)
                if(board[i + 1][j] < 0)  
                    flag = true;
        }
    }
    if(flag){
        for(int i = 0 ; i < row ; i++){
            for(int j = 0 ; j < col ; j++){
                if(board[i][j] == block.shape)
                    board[i][j] = -1 * block.shape;
            }
        }
        S_PER_UPDATE = TempFps;
        player.score += 20;
        speedUp();
        isGameOver(board);
        isFullRow(board);
    }
}

void down(int**board){
    validMove(board);
        for(int i = row - 1 ; i >= 0 ; i--)
            for(int j = 0 ; j < col ; j++){
                if(board[i][j] == block.shape){
                    board[i][j] = 0;
                    board[i + 1][j] = block.shape;
                }
            }
    //print(board);
    block.y++;
}

void right(int** board){
    bool flag = true;
    for(int i = 0 ; i < row ; i++){
        for(int j = col - 1 ; j >= 0 ; j--){
            if(board[i][j] == block.shape)            // to check if it's a valid move or not
                if(j + 1 == col || board[i][j + 1] < 0)
                    flag = false;
        }
    }
    if(flag){
        for(int i = 0 ; i < row ; i++){
            for(int j = col - 1 ; j >= 0 ; j--){
                if(board[i][j] == block.shape){
                    board[i][j] = 0;
                    board[i][j + 1] = block.shape;
                }   
            }
        }
        block.x++;
    }
}

void left(int** board){
    bool flag = true;
    for(int i = 0 ; i < row ; i++){
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == block.shape)            // to check if it's a valid move or not
                if(j -1 == - 1 || board[i][j - 1] < 0)
                    flag = false;
        }
    }
    if(flag){
        for(int i = 0 ; i < row ; i++){
            for(int j = 0 ; j < col ; j++){
                if(board[i][j] == block.shape){
                    board[i][j] = 0;
                    board[i][j - 1] = block.shape;
                }   
            }
        }
        block.x--;
    }
}

void isFullRow(int** board){
    for(int i = row - 4 ; i >=0 ; i--){
        bool flag = true;
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == 0){
                flag = false;
                break;
            }
        }
        if(flag){
            for(int k = i ; k > 0 ; k--){
                for(int j = 0 ; j < col ; j++){
                    board[k][j] = board[k - 1][j];
                }
            }
            for(int j = 0 ; j < col ; j++)
                board[0][j] = 0;
            i = row - 3;
            player.score += 20 * col;
        }
    }
    speedUp();
    print(board);
    randShape(board);
    Game(board);
}

void isGameOver(int** board){
    for(int i = 0; i < 3 ; i++){
        for(int j = 0; j < col ; j++){
            if(board[i][j] < 0){
                cout << endl;
                cout <<RED_COLOR<< " ██████╗  █████╗ ███╗   ███╗███████╗     ██████╗ ██╗   ██╗███████╗██████╗ \n", Sleep(200);
                cout <<RED_COLOR<< "██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ██╔═══██╗██║   ██║██╔════╝██╔══██╗\n", Sleep(200);
                cout <<RED_COLOR<< "██║  ███╗███████║██╔████╔██║█████╗      ██║   ██║██║   ██║█████╗  ██████╔╝\n", Sleep(200);
                cout <<RED_COLOR<< "██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║   ██║╚██╗ ██╔╝██╔══╝  ██╔══██╗\n", Sleep(200);
                cout <<RED_COLOR<< "╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ╚██████╔╝ ╚████╔╝ ███████╗██║  ██║\n", Sleep(200);
                cout <<RED_COLOR<< " ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝     ╚═════╝   ╚═══╝  ╚══════╝╚═╝  ╚═╝\n";
                cout << "                                                                          \n";
                timeSaved = 0;
                save();
                boardDeleter(board);
                system("pause");
                system("cls");
                menu();
            }
        }
    }
}
void speedUp(){
    while(player.score > ceil_points){
        S_PER_UPDATE *= downTime;
        ceil_points += 500;
    }
}

void shadowRemove(int** board){
    for(int i = 0 ; i < row ; i++){
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == 10)
                board[i][j] = 0;
        }
    }
}
bool validShadow(int** board){
    bool flag = true;
    for(int i = row - 1 ; i >= 0 ; i--){
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == block.shape)
                if(board[i + 1][j] < 0)  
                    flag = false;
        }
    }
    return flag;
}

void shadowShape(int** board){
    while(validShadow(board)){
        for(int i = row - 1 ; i >= 0 ; i--)
            for(int j = 0 ; j < col ; j++){
                if(board[i][j] == block.shape){
                    board[i][j] = 0;
                    board[i + 1][j] = block.shape;
                }
            }
    }
    for(int i = 0 ; i < row ; i++){
        for(int j = 0 ; j < col ; j++){
            if(board[i][j] == block.shape)
                board[i][j] = 10;
        }
    }
    block.cntRotate += 3;
    rotate(board);
    block.cntRotate -= 4;
}

void randShape(int** board){
    srand(static_cast<unsigned int>(time(nullptr)));
    if(firstShape){
        currentBlock =  rand() % 7 + 1;
        nextBlock =  rand() % 7 + 1;
        firstShape = false;
    }
    else{
        currentBlock = nextBlock;
        nextBlock =  rand() % 7 + 1;
    }
    int block_O[2][2] ={{1 , 1},
                        {1 , 1}};

    int block_I[4][4] = {{0 , 0 , 0 , 0},
                         {2 , 2 , 2 , 2},
                         {0 , 0 , 0 , 0}, 
                         {0 , 0 , 0 , 0}};

    int block_T[3][3] = {{0 , 3 , 0},
                         {3 , 3 , 3},
                         {0 , 0 , 0}};

    int block_Z[3][3] = {{4 , 4 , 0},
                         {0 , 4 , 4},
                         {0 , 0 , 0}};

    int block_S[3][3] = {{0 , 5 , 5},
                         {5 , 5 , 0},
                         {0 , 0 , 0}};

    int block_L[3][3] = {{0 , 0 , 6},
                         {6 , 6 , 6},
                         {0 , 0 , 0}};

    int block_J[3][3] = {{7 , 0 , 0},
                         {7 , 7 , 7},
                         {0 , 0 , 0}};                              
    switch(currentBlock){
        case 1:
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < 2 ;j++){
            if(block_O[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_O[i][j];
        }
    }
    block.shape = 1;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 2:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 4 ;j++){
            if(block_I[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_I[i][j];
        }
    }
    block.shape = 2;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 3:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ;j++){
            if(block_T[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_T[i][j];
        }
    }
    block.shape = 3;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 4:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ;j++){
            if(block_Z[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2]  = block_Z[i][j];
        }
    }
    block.shape = 4;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 5:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ;j++){
            if(block_S[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_S[i][j];
        }
    }
    block.shape = 5;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 6:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ;j++){
            if(block_L[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_L[i][j];
        }
    }
    block.shape = 6;
    block.x = col / 2 - 2 , block.y = 1  , block.cntRotate = 0;
        break;
        case 7:
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ;j++){
            if(block_J[i][j] == currentBlock)
                board[i + 1][j + col / 2 - 2] = block_J[i][j];
        }
    }
    block.shape = 7;
    block.x = col / 2 - 2 , block.y = 1 , block.cntRotate = 0;
        break;
    }

}

bool validRotate3(int** board , int temp[3][3]){
    bool valid = true;
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0 ; j < 3 ; j++){
            if(temp[i][j] == block.shape){
                if(board[i + block.y][j + block.x] < 0)
                    valid = false;
                else if(i + block.y > row - 4 || j + block.x < 0 || j + block.x > col - 1)
                    valid = false;
            }
        }
    }
    return valid;
}

bool validRotate4(int** board , int temp[4][4]){
    bool valid = true;
    for(int i = 0 ; i < 4 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            if(temp[i][j] == 2){
                if(board[i + block.y][j + block.x] < 0)
                    valid = false;
                else if(i + block.y > row - 4 || j + block.x < 0 || j + block.x > col - 1)
                   valid = false;
            }
        }
    }
    return valid;
}

void printShape(int** board , int temp[3][3]){
    if(validRotate3(board , temp)){
        for(int i = 0 ; i < 3 ; i++){
                for(int j = 0 ; j < 3 ; j++){
                    if(board[i + block.y][j + block.x] == block.shape){
                        board[i + block.y][j + block.x] = 0;
                    }
                    if(temp[i][j] == block.shape)
                    board[i + block.y][j + block.x] = temp[i][j];
                }
            }
        block.cntRotate++;
        print(board);
    }
}

void printShapelin(int** board , int temp[4][4]){
    if(validRotate4(board, temp)){
        for(int i = 0 ; i < 4 ; i++){
                for(int j = 0 ; j < 4 ; j++){
                    if(board[i + block.y][j + block.x] == 2){
                        board[i + block.y][j + block.x] = 0;
                    }
                    if(temp[i][j] == 2)
                    board[i + block.y][j + block.x] = temp[i][j];
                }
            }
        block.cntRotate++;
        print(board);
    }
}

void rotate(int** board){
    switch(block.shape){
        case 1:
        if(true){ 
        int temp[3][3] =   {{1 , 1 , 0},
                            {1 , 1 , 0},
                            {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
        case 2:
        if(block.cntRotate % 4 == 0){
        int temp[4][4] = {{0 , 0 , 2 , 0},
                          {0 , 0 , 2 , 0},
                          {0 , 0 , 2 , 0},
                          {0 , 0 , 2 , 0}};
            printShapelin(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[4][4] = {{0 , 0 , 0 , 0},
                          {0 , 0 , 0 , 0},
                          {2 , 2 , 2 , 2},
                          {0 , 0 , 0 , 0}};
            printShapelin(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[4][4] = {{0 , 2 , 0 , 0},
                          {0 , 2 , 0 , 0},
                          {0 , 2 , 0 , 0},
                          {0 , 2 , 0 , 0}};
            printShapelin(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[4][4] = {{0 , 0 , 0 , 0},
                          {2 , 2 , 2 , 2},
                          {0 , 0 , 0 , 0},
                          {0 , 0 , 0 , 0}};
            printShapelin(board , temp);
        }
        break;
        case 3:
        if(block.cntRotate % 4 == 0){
        int temp[3][3] = {{0 , 3 , 0},
                          {0 , 3 , 3},
                          {0 , 3 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[3][3] = {{0 , 0 , 0},
                          {3 , 3 , 3},
                          {0 , 3 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[3][3] = {{0 , 3 , 0},
                          {3 , 3 , 0},
                          {0 , 3 , 0}};
        printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[3][3] = {{0 , 3 , 0},
                          {3 , 3 , 3},
                          {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
        //------------------------------
        case 4:
        if(block.cntRotate % 4 == 0){
        int temp[3][3] = {{0 , 0 , 4},
                          {0 , 4 , 4},
                          {0 , 4 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[3][3] = {{0 , 0 , 0},
                          {4 , 4 , 0},
                          {0 , 4 , 4}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[3][3] = {{0 , 4 , 0},
                          {4 , 4 , 0},
                          {4 , 0 , 0}};
        printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[3][3] = {{4 , 4 , 0},
                          {0 , 4 , 4},
                          {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
        //------------------------------
        case 5:
        if(block.cntRotate % 4 == 0){
        int temp[3][3] = {{0 , 5 , 0},
                          {0 , 5 , 5},
                          {0 , 0 , 5}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[3][3] = {{0 , 0 , 0},
                          {0 , 5 , 5},
                          {5 , 5 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[3][3] = {{5 , 0 , 0},
                          {5 , 5 , 0},
                          {0 , 5 , 0}};
        printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[3][3] = {{0 , 5 , 5},
                          {5 , 5 , 0},
                          {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
         //------------------------------
        case 6:
        if(block.cntRotate % 4 == 0){
        int temp[3][3] = {{0 , 6 , 0},
                          {0 , 6 , 0},
                          {0 , 6 , 6}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[3][3] = {{0 , 0 , 0},
                          {6 , 6 , 6},
                          {6 , 0 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[3][3] = {{6 , 6 , 0},
                          {0 , 6 , 0},
                          {0 , 6 , 0}};
        printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[3][3] = {{0 , 0 , 6},
                          {6 , 6 , 6},
                          {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
        //----------------------------
        case 7:
        if(block.cntRotate % 4 == 0){
        int temp[3][3] = {{0 , 7 , 7},
                          {0 , 7 , 0},
                          {0 , 7 , 0}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 1){
        int temp[3][3] = {{0 , 0 , 0},
                          {7 , 7 , 7},
                          {0 , 0 , 7}};
            printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 2){
        int temp[3][3] = {{0 , 7 , 0},
                          {0 , 7 , 0},
                          {7 , 7 , 0}};
        printShape(board , temp);
        }
        else if(block.cntRotate % 4 == 3){
        int temp[3][3] = {{7 , 0 , 0},
                          {7 , 7 , 7},
                          {0 , 0 , 0}};
        printShape(board , temp);
        }
        break;
    }  
}