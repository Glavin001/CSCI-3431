/*
 *  botm.c
 *  Lab 7 Server Side Component
 *  by Tami Meredith
 *  Edited by: Arsum Iqbal - A00351433
 *  Permission given for students to modify and use this in assignments
 */

/*
 *  The server should be run as a background process. That is, start it
 *  from the command line with an "&" after the name (not in quotes).
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG 0 /* Set to 0 to turn off debugging */
#define PROTECTION 1 /* Set to 0 to turn off Protection */
#define PORT 	0x2604 /* Port number 2604 = 9732 in decimal */
#define BUFSIZE 128
#define loop    while(1)

#define NUMACCTS 101
#define SHMSIZE  (NUMACCTS*sizeof(t_account))

typedef struct
{
    int balance;
} t_account;
t_account *acct;
char      *shm;

int childmain(char *msg, int sd_current)
{
    char buf[BUFSIZE];
    char *token;
    const char space[2] = " ";
    int account, amount;
    /* display the incoming request */
    fprintf(stderr, "process %d handling: <%s>\n", getpid(), msg);

    //Get the first token so we can break up the "msg" into pieces using
    //spaces as a delimiter
    token = strtok(msg, space);
    
    //If the request is a deposit
    if(strcmp(token,"dep") == 0)
    {
        //Knowing that after dep, the next 2 arguments are the account
        //number and the amount
        token = strtok(NULL, space);
        //Change "account" into integer
        account = atoi(token);
        //Proper error checking to see if "account" is between 1-99 
        if(account < 1 || account > 99)
        {
            fprintf(stderr,"Account number must be between 1-99.\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, space);
        //Change "amount" into integer
        amount = atoi(token);
#if PROTECTION
        if(amount < 0)
        {
            fprintf(stderr,"Invalid Deposit.\n");
            exit(EXIT_FAILURE);
        }
#endif       
        //Add "amount" to the "account"
        acct[account].balance += amount;
        sprintf(buf,"OK");
    }
    //If the request is a withdrawal
    else if(strcmp(token,"wd") == 0)
    {
        //Knowing that after wd, the next 2 arguments are the account
        //number and the amount
        token = strtok(NULL, space);
        //Change "account" into integer
        account = atoi(token);
        //Proper error checking to see if "account" is between 1-99 
        if(account < 1 || account > 99)
        {
            fprintf(stderr,"Account number must be between 1-99.\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, space);
        //Change "amount" into integer
        amount = atoi(token);
#if PROTECTION
        if(amount < 0)
        {
            fprintf(stderr,"Invalid Withdrawal.\n");
            exit(EXIT_FAILURE);
        }
        if((acct[account].balance - amount) < 0)
        {
            fprintf(stderr,"Invalid Withdrawal.\n");
            exit(EXIT_FAILURE);
        }
#endif
        //Take away the "amount" from the "account"
        acct[account].balance -= amount;
        sprintf(buf,"OK");
    }
    //If the request is a transfer
    else if(strcmp(token,"trans") == 0)
    {
        //Knowing that after trans, the next 2 arguments are the acccount number
        //followed by the last argument which is the amount
        token = strtok(NULL, space);
        //Change sourceAcc number into integer
        int sourceAcc = atoi(token);
        token = strtok(NULL, space);
        //Change destinationAcc account number into integer
        int destinationAcc = atoi(token);
        //Proper error checking to see if both "account" is between 1-99 
        if((sourceAcc < 1 || sourceAcc > 99) || (destinationAcc < 1 || destinationAcc > 99))
        {
            fprintf(stderr,"Account number must be between 1-99.\n");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, space);
        //Convert "amount" into integer
        amount = atoi(token);
#if PROTECTION
        if((acct[sourceAcc].balance - amount) < 0)
        {
            fprintf(stderr,"Invalid Transfer.\n");
            exit(EXIT_FAILURE);
        }
#endif
        //Add "amount" to "destination" account
        acct[destinationAcc].balance += amount;
        //Subtract "amount" from "source" account
        acct[sourceAcc].balance =  acct[sourceAcc].balance - amount;
        sprintf(buf,"OK");
    }
    //If the request is a balance check
    else if(strcmp(token,"bal") == 0)
    {
        //Knowing that after bal, the next argument is the acccount number
        token = strtok(NULL, space);
        //Change "account" number to integer
        account = atoi(token);
        //Proper error checking to see if "account" is between 1-99
        if(account < 1 || account > 99)
        {
            fprintf(stderr,"Account number must be between 1-99.\n");
            exit(EXIT_FAILURE);
        }
        //Return the balance in the "account"
        sprintf(buf,"Balance: %d", acct[account].balance);
    }

    /* acknowledge the message */
    if (send(sd_current, buf, strlen(buf)+1, 0) == -1)
        perror("send");
    close (sd_current);
}

int main(int argc, char **argv)
{
    char     buf[BUFSIZE];  /* used for incoming and outgoing data */
    char    *msg;
    int      sd, sd_current, cc, fromlen, tolen;
    int      addrlen;
    struct   sockaddr_in sin;
    struct   sockaddr_in pin;
    int      pid;
    key_t    key = 96438;
    int      shmid;

    /* setup the accounts */
    if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0600)) < 0)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    if ((shm = shmat (shmid, NULL, 0)) == (char *) -1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    acct = (t_account *) shm;
    memset (acct, 0, NUMACCTS * sizeof(t_account));

    /* get an internet domain socket */
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* complete the socket structure */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    /* bind the socket to the port number */
    if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "%s: status: listening on port %d\n", argv[0], PORT);

    /* show that we are willing to listen */
    if (listen(sd, 5) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    loop
    {
        /* wait for a client to talk to us */
        addrlen = sizeof(pin);
        if ((sd_current = accept(sd, (struct sockaddr *)  &pin, &addrlen)) == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

#if DEBUG
        /* show the ip address and port of the client */
        fprintf(stderr, "Server received connection from %s\n", inet_ntoa(pin.sin_addr));
        fprintf(stderr, "Coming from port %d\n", ntohs(pin.sin_port));
#endif

        /* get a message from the client */
        if (recv(sd_current, buf, sizeof(buf), 0) == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        msg = strdup(buf);

#if DEBUG
        /* show the data received from the client */
        fprintf(stderr, "Server received: <%s>\n", msg);
#endif

        /* Is this a shutdown request? */
        if (strcmp(msg,"shutdown") == 0)
        {
            fprintf(stderr, "%s: status: shutting down\n", argv[0]);
            break;
        }

        /* Handle this transaction */
        if ((pid = fork()) == -1)
        {
            fprintf(stderr, "Fork Failed.\n");
            exit(EXIT_FAILURE);
        }

        /*If its the child */
        if (pid == 0)
             return childmain(msg,sd_current);

        close(sd_current);
    } /* end loop */

    close(sd);

} /* end main() */
