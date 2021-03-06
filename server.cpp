#include <stdio.h>
#include <stdlib.h>
/* You will to add includes here */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>

#define MAXCLIENTS 100

using namespace std;

int nrOfPlayers;
int *pectators;
int nrOfSpectators;
int Gamequeue[50];
int qeueu = 0;

struct game
{
    int player1;
    int player2;
    int player1Score = 0;
    int player2Score = 0;
    int player1choise = 0;
    int player2choise = 0;
    int rounds = 0;
    bool startRound = false;
    bool p1Set, p2Set;
    time_t timer = time(0);
    bool active = false;
};

int nrOfGames = 0;
game games[25];

string removeWord(string str, string word)
{
    // Check if the word is present in string
    // If found, remove it using removeAll()
    if (str.find(word) != string::npos)
    {
        size_t p = -1;

        // To cover the case
        // if the word is at the
        // beginning of the string
        // or anywhere in the middle
        string tempWord = word + " ";
        while ((p = str.find(word)) != string::npos)
            str.replace(p, tempWord.length(), "");

        // To cover the edge case
        // if the word is at the
        // end of the string
        tempWord = " " + word;
        while ((p = str.find(word)) != string::npos)
            str.replace(p, tempWord.length(), "");
    }

    // Return the resultant string
    return str;
}
int Winner(int p1, int p2)
{
    int win = 0;
    //1: Rock 2: Paper 3: Scissors
    //WIN: 1: p1 winns. 2: p2 winns. 0: Draw
    if (p1 == 1)
    {
        if (p2 == 1)
        {
            //Draw
            win = 0;
        }
        else if (p2 == 2)
        {
            //p2 wins
            win = 2;
        }
        else if (p2 == 3)
        {
            //p1 wins
            win = 1;
        }
    }
    else if (p1 == 2)
    {
        if (p2 == 1)
        {
            //p1 wins
            win = 1;
        }
        else if (p2 == 2)
        {
            //Draw
            win = 0;
        }
        else if (p2 == 3)
        {
            //p2 wins
            win = 2;
        }
    }
    else if (p1 == 0)
    {
        if (p2 == 0)
        {
            //Draw
            win = 0;
        }
        else
        {
            //p2 wins
            win = 2;
        }
    }
    return win;
}
string Menu()
{
    return ("Please select:\n1:Play\n2:Watch\n3:Exit\n");
}
void removeFromQeueu(int sock)
{
    for (int i = 0; i < qeueu; i++)
    {
        if (Gamequeue[i] == sock)
        {
            //Found the client
            for (int j = i; j < qeueu; j++)
            {
                Gamequeue[j] = Gamequeue[j + 1];
            }
            qeueu--;
            break;
        }
    }
}
void sendMsg(int sock, string msg)
{
    if (send(sock, msg.c_str(), msg.length(), 0) < 0)
    {
        printf("Sending msg.\n");
    }
}
void newGame(int p1, int p2)
{
    removeFromQeueu(p1);
    removeFromQeueu(p2);
    games[nrOfGames].player1 = p1;
    games[nrOfGames].player2 = p2;
    games[nrOfGames].rounds = 0;
    games[nrOfGames].active = true;

    sendMsg(games[nrOfGames].player1, "You are in a game.\n1:Rock\n2:Paper\n3:Scissors\n");
    sendMsg(games[nrOfGames].player2, "You are in a game.\n1:Rock\n2:Paper\n3:Scissors\n");
    //Send rules
    nrOfGames++;
}
void endGame(game gameToFind){
     for (int i = 0; i < nrOfGames; i++)
    {
        if (games[i].player1 == gameToFind.player1 || games[i].player2 == gameToFind.player2)
        {
            //Found the game
            sendMsg(games[i].player1, Menu());
            sendMsg(games[i].player2, Menu());
            games[i].player1 = 0;
            games[i].player2 = 0;
            games[i].p1Set = false;
            games[i].p2Set = false;
            games[i].active = false;
            for (int j = i; j < nrOfGames; j++)
            {
                games[j] = games[j + 1];
            }
            nrOfGames--;
            
            break;
        }
    }
}
void handleQueue()
{
    if (qeueu % 2 == 0 && qeueu > 0)
    {
        //There is an equal number of players in qeueu and the is more that 0 players in the qeueu
        //Start a new Game
        newGame(Gamequeue[0], Gamequeue[1]);
    }
}
void handleGames()
{
    for (int i = 0; i < nrOfGames; i++)
    {
        if (games[i].player1Score == 3)
        {
            //p1 wins
            sendMsg(games[i].player1, "p1 wins\n");
            sendMsg(games[i].player2, "p1 wins\n");
            endGame(games[i]);
        }
        else if (games[i].player1Score == 3)
        {
            //p2 wins
            sendMsg(games[i].player1, "p2 wins\n");
            sendMsg(games[i].player2, "p2 wins\n");
            endGame(games[i]);
        }
        else if (games[i].p1Set && games[i].p2Set)
        {
            printf("Boath players have set theri choises.\n");
            printf("P1 chose %d\nP2 chose %d\n", games[i].player1choise, games[i].player2choise);
            int win = Winner(games[i].player1choise, games[i].player2choise);
            printf("Thw winner of this round is %d\n", win);
            switch (win)
            {
            case 0:
                //Draw
                sendMsg(games[i].player1, "Draw\n");
                sendMsg(games[i].player2, "Draw\n");
                break;
            case 1:
                //p1 socre
                games[i].player1Score++;
                sendMsg(games[i].player1, "p1 score\n");
                sendMsg(games[i].player2, "p1 score\n");
                break;
            case 2:
                //p2 score
                games[i].player2Score++;
                sendMsg(games[i].player1, "p2 score\n");
                sendMsg(games[i].player2, "p2 score\n");
                break;
            }
            printf("P1 score: %d\nP2 score: %d\n", games[i].player1Score, games[i].player2Score);
            games[i].p1Set = false;
            games[i].p2Set = false;
            handleGames();
        }
    }
}
void setPlayerChoise(int player, int choise)
{
    for (int i = 0; i < nrOfGames; i++)
    {
        if (player == games[i].player1)
        {
            if (!games[i].p1Set)
            {
                games[i].player1choise = choise;
                games[i].p1Set = true;
                printf("P1 set its choise: %d.\n", choise);
                break;
            }
        }
        else if (player == games[i].player2)
        {
            if (!games[i].p2Set)
            {
                games[i].player2choise = choise;
                games[i].p2Set = true;
                printf("P2 set its choise: %d.\n", choise);
                break;
            }
        }
    }
}
int checkPlayerStatus(int sock)
{
    //1 = in queue. 2 = in game
    int ret = 0;
    bool inQueue = false;
    for (int k = 0; k < qeueu; k++)
    {
        if (sock == Gamequeue[k])
        {
            inQueue = true;
            ret = 1;
            break;
        }
    }
    bool innGame = false;
    for (int k = 0; k < nrOfGames; k++)
    {
        if (sock == games[k].player1 || sock == games[k].player2)
        {
            innGame = true;
            ret = 2;
            break;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{

    //Takes ip and port in the following format:
    //ip:port
    if (argc != 2)
    {
        //We dont have the correct input. Exit the program
        printf("Invalid input.\n");
        exit(1);
    }
    char delim[] = ":";
    char *Desthost = strtok(argv[1], delim);
    char *Destport = strtok(NULL, delim);
    //Desthost should now have the ip in Desthost and port in Destport
    //Check if one of them might be NULL
    if (Desthost == NULL || Destport == NULL)
    {
        //One of the variables are null. Exit the program
        printf("Invalid input.\n");
        exit(1);
    }

    //Variables
    int listener;
    int newfd;
    int rv;
    int nrOfClient = 0;
    int yes = 1;
    int bytesRecived;
    string cmds[4] = {"OK\n", "1\n", "2\n", "3\n"};

    struct addrinfo hint, *servinfo, *p;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;     //IPv4 or IPv6
    hint.ai_socktype = SOCK_STREAM; //TCP

    rv = getaddrinfo(Desthost, Destport, &hint, &servinfo);
    if (rv != 0)
    {
        //Getaddrinfo faild
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    //Loop trough all the information and try to make a sockt
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0)
        {
            //Could not make socket try again
            continue;
        }
        //setsockoptions
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            printf("setsockopt.\n");
            exit(1);
        }
        //bind
        if (bind(listener, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(listener);
            printf("Could not bind.\n");
            continue;
        }
        //If we made it here we succesfully created a socket. Exit loop
        break;
    }
    freeaddrinfo(servinfo);
    //Check if the for loop did not make a socket
    if (p == NULL)
    {
        printf("Could not bind.\n");
        exit(1);
    }
    //Litser for connections
    if (listen(listener, MAXCLIENTS) < 0)
    {
        printf("Litse.\n");
        close(listener);
        exit(1);
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(listener, &master);
    int fdmax = listener;

    char buf[255];
    memset(buf, 0, sizeof(buf));

    while (true)
    {
        fd_set copy = master;
        handleQueue();
        //handleGames();

        if (select(fdmax + 1, &copy, NULL, NULL, NULL) == -1)
        {
            printf("select.\n");
            close(listener);
            exit(1);
        }

        for (int i = 0; i < fdmax + 1; i++)
        {
            if (FD_ISSET(i, &copy))
            {
                if (i == listener)
                {
                    //Accept a new connection
                    newfd = accept(listener, p->ai_addr, &p->ai_addrlen);
                    if (newfd < 0)
                    {
                        printf("Accept.\n");
                        continue;
                    }
                    else
                    {
                        FD_SET(newfd, &master);
                        nrOfClient++;
                        if (newfd > fdmax)
                        {
                            fdmax = newfd;
                        }
                        if (send(newfd, "1.0\n", strlen("1.0\n"), 0) < 0)
                        {
                            printf("Sending.\n");
                        }
                    }
                }
                else
                {

                    //Accept a new message
                    memset(buf, 0, sizeof(buf));
                    bytesRecived = recv(i, buf, sizeof(buf), 0);
                    if (bytesRecived <= 0)
                    {
                        printf("Remove client.\n");
                        removeFromQeueu(i);
                        close(i);
                        FD_CLR(i, &master);
                    }
                    //Did the client support the protocol?
                    //If client sent "OK\n", send back alternatives of what the client can do.
                    /*Check for client responce
                    1: Play (If no players, put in qeueu. If odd number of players, start a new game. If even numbre of players, place in qeueu)
                       Give the option to leave qeueu 
                    2:Watch (List all the active games and give the cilet a choise)
                    0: Exit
                    */

                    //Player is NOT in qeueu
                    if (strcmp(buf, cmds[0].c_str()) == 0)
                    {   //cmds[0]=="OK\n"
                        //The client supports the prorocols
                        if (checkPlayerStatus(i) == 0)//Not in qeueu and not in game
                        {
                            printf("The client supports the prorocols\n");
                            if (send(i, Menu().c_str(), Menu().length(), 0) < 0)
                            {
                                printf("Sending.\n");
                            }
                        }
                    }
                    else if (strcmp(buf, cmds[1].c_str()) == 0)
                    { //cmds[1]=="1\n"
                        //The client wanst to play
                        memset(buf, 0, sizeof(buf));
                        if (checkPlayerStatus(i) == 0) //not in game or queue
                        {
                            printf("The client wants to play\n");
                            Gamequeue[qeueu++] = i;
                            sendMsg(i, "Putting you in qeueu\n.Press 1 to exit qeueu.\n");
                            printf("Nr of people in qeueu: %d\n", qeueu);
                        }
                        else if (checkPlayerStatus(i) == 1) //In queue
                        {
                            //Client wants to exit qeueu
                            removeFromQeueu(i);
                            sendMsg(i, Menu());
                        }
                        else if (checkPlayerStatus(i) == 2) //In game
                        {
                            setPlayerChoise(i, 1);
                            handleGames();
                        }
                    }
                    else if (strcmp(buf, cmds[2].c_str()) == 0)
                    { //cmds[2]=="2\n"
                        //The client wats to watch
                        memset(buf, 0, sizeof(buf));
                        if (checkPlayerStatus(i) == 0) //Not in game or queue
                        {
                            printf("The client wants to watch\n");
                            printf("Nr of acrive games: %d\n", nrOfGames);
                            string allGames = "";
                            for (int j = 0; j < nrOfGames; j++)
                            {
                                printf("Game %d\n", j);
                            }
                        }
                        else if (checkPlayerStatus(i) == 1) //In queue
                        {
                        }
                        else if (checkPlayerStatus(i) == 2) //In game
                        {
                            setPlayerChoise(i, 2);
                            handleGames();
                        }
                    }
                    else if (strcmp(buf, cmds[3].c_str()) == 0)
                    { //cmds[3]=="3\n"
                        memset(buf, 0, sizeof(buf));
                        if (checkPlayerStatus(i) == 0) //Not in game or queue
                        {
                            //The client wats to exit
                            printf("The client wants to exit\n");
                            printf("Remove client.\n");
                            close(i);
                            FD_CLR(i, &master);
                        }
                        else if (checkPlayerStatus(i) == 1)
                        { //In queue
                        }
                        else if (checkPlayerStatus(i) == 2)
                        { //In game
                            setPlayerChoise(i, 3);
                            handleGames();
                        }
                    }
                    else
                    {
                        printf("Wrong command. You sent: %s", buf);
                    }

                    //printf("New message.\n");
                }
            }
        }
    }
    return 0;
}