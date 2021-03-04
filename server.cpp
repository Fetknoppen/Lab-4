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
int Gameqeueu[50];
int qeueu = 0;

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
    switch (p1)
    {
    case 1:
        if (p2 == 1)
            win = 0;
        else if (p2 == 2)
            win = 2;
        else if (p2 == 3)
            win = 1;
        else
            win = 1;

        break;
    case 2:
        if (p2 == 1)
            win = 1;
        else if (p2 == 2)
            win = 0;
        else if (p2 == 3)
            win = 2;
        else
            win = 1;
        break;
    case 3:
        if (p2 == 1)
            win = 2;
        else if (p2 == 2)
            win = 1;
        else if (p2 == 3)
            win = 0;
        else
            win = 1;
        break;
    default:
        if (p2 != 1 || p2 != 2 || p2 != 3)
        {
            win = 0;
        }
        else
        {
            win = 2;
        }
        break;
    }
}
string Menu()
{
    return ("Please select:\n1:Play\n2:Watch\n3:Exit\n");
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
                    if (strcmp(buf, cmds[0].c_str()) == 0)
                    { //cmds[0]=="OK\n"
                        //The client supports the prorocols
                        printf("The client supports the prorocols\n");
                        if (send(i, Menu().c_str(), Menu().length(), 0) < 0)
                        {
                            printf("Sending.\n");
                        }
                    }
                    else if (strcmp(buf, cmds[1].c_str()) == 0)
                    { //cmds[1]=="1\n"
                        //The client wanst to play
                        printf("The client wants to play\n");
                        nrOfPlayers++;
                        qeueu++;
                        if (qeueu % 2 == 0)
                        {
                            //Game is ready
                            if (send(i, "Game is ready\n", strlen("Game is ready\n"), 0) < 0)
                            {
                                printf("Sending.\n");
                            }
                        }
                        else
                        {
                            //Not enough players, put in qeueu
                            Gameqeueu[qeueu] = i;
                            if (send(i, "Not enouth players, putting you in qeueu\n.", strlen("Not enouth players, putting you in qeueu\n."), 0) < 0)
                            {
                                printf("Sending.\n");
                            }
                        }
                    }
                    else if (strcmp(buf, cmds[2].c_str()) == 0)
                    { //cmds[2]=="2\n"
                        //The client wats to watch
                        printf("The client wants to watch\n");
                    }
                    else if (strcmp(buf, cmds[3].c_str()) == 0)
                    { //cmds[3]=="3\n"
                        //The client wats to exit
                        printf("The client wants to exit\n");
                        printf("Remove client.\n");
                        close(i);
                        FD_CLR(i, &master);
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