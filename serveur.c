//Author: PETIT Alloys

#include <stdio.h>
#include <netinet/in.h> /* pour protocole dans cas AF_INET */
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h> 
#include <unistd.h> /* close */
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  


int main(int argc, char* argv[]){

	int soc, res, socLog, resLog;

    ////////////////////////////////////////////////////// soc
	soc = socket(AF_INET,SOCK_STREAM,0);
	if( soc == -1 )
		printf("erreur socket() 1\n");

	struct sockaddr_in MyAddr = { 0 };
	MyAddr.sin_family = AF_INET;
	MyAddr.sin_port = htons(atoi(argv[1]));
	MyAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	res = bind(soc, ( struct sockaddr *) &MyAddr, sizeof(MyAddr) );
	if( res == -1 )
		printf("erreur bind()\n");

	if(listen(soc, 5) == -1)
	{
		printf("erreur listen()\n");
	}

    fd_set readfds;   

    ///////////////////////////////////////////////////// socLog

    socLog = socket(AF_INET,SOCK_STREAM,0);
	if( socLog == -1 )
		printf("erreur socket() 1\n");

	struct sockaddr_in MyAddrLog = { 0 };
	MyAddrLog.sin_family = AF_INET;
	MyAddrLog.sin_port = htons(atoi(argv[2]));
	MyAddrLog.sin_addr.s_addr = htonl(INADDR_ANY);

	resLog = bind(socLog, ( struct sockaddr *) &MyAddrLog, sizeof(MyAddrLog) );
	if( resLog == -1 )
		printf("erreur bind()\n");

	if(listen(socLog, 5) == -1)
	{
		printf("erreur listen()\n");
	}


	int max_sd;
	if ( soc > socLog )
		max_sd = soc;
		else
		max_sd = socLog;

	while(1)
	{

		FD_ZERO(&readfds);   
		FD_SET(soc, &readfds);
		FD_SET(socLog, &readfds);

		if ( select( max_sd + 1, &readfds , NULL , NULL , NULL) < 0)
        {
          printf ("erreur select\n");
        } 

        printf ("select OK\n");

		if (FD_ISSET(soc, &readfds)) ////////////////////////// Socket pour les requetes de fichiers
        {   
        	printf ("soc\n");

        	struct sockaddr_in sinClient = { 0 };
			unsigned int sinsize = sizeof( sinClient );

			int sock_client = accept(soc, (struct sockaddr *)&sinClient, &sinsize);
			if( sock_client == -1 )
			{
				printf("erreur accept()\n");
			}


			int n = 0;
			char vMsgClient[5000];

			if((n = recv(sock_client, vMsgClient, sizeof(vMsgClient) - 1, 0)) < 0)
			{
				printf("erreur recv()\n");
			}

			vMsgClient[n] = 0;




			char* first_line = strtok(vMsgClient, "\r\n");

			strtok(first_line, " ");
			char* fileRequested = strtok(NULL, " ");
			memmove(fileRequested, fileRequested+1, strlen(fileRequested));



			char adresse_client[INET_ADDRSTRLEN];
			inet_ntop( AF_INET, &sinClient.sin_addr, adresse_client, sizeof( adresse_client ));

			time_t t = time(NULL);
			struct tm vTm = *localtime(&t);

			FILE *fp = fopen("log_file.txt", "a");
			fprintf (fp, "%s %d-%02d-%02d_%02d:%02d:%02d %s\n", adresse_client, vTm.tm_year + 1900, vTm.tm_mon + 1, vTm.tm_mday, vTm.tm_hour, vTm.tm_min, vTm.tm_sec ,fileRequested);
			fclose(fp);



			FILE *fileToSend = fopen(fileRequested, "rb");
			if( fileToSend != NULL)
			{
				fseek(fileToSend, 0, SEEK_END);
				long fsize = ftell(fileToSend);
				fseek(fileToSend, 0, SEEK_SET); 
				char *string = malloc(fsize + 1);
				fread(string, 1, fsize, fileToSend);
				fclose(fileToSend);

				if(send(sock_client, string, strlen(string), 0) < 0)
				{
					printf("erreur send()\n");
				}
				free(string);
			}
			else
			{
				char * msg = "Ce fichier n'existe pas";
				if(send(sock_client, msg, strlen(msg), 0) < 0)
				{
					printf("erreur send()\n");
				}
			}

			close(sock_client);

		}

        else ////////////////////////// Pour envoyer le fichier de logs 
        {   
        	printf ("socLog\n");

        	struct sockaddr_in sinClient = { 0 };
			unsigned int sinsize = sizeof( sinClient );

			int sock_client = accept(socLog, (struct sockaddr *)&sinClient, &sinsize);
			if( sock_client == -1 )
			{
				printf("erreur accept()\n");
			}


			int n = 0;
			char vMsgClient[5000];

			if((n = recv(sock_client, vMsgClient, sizeof(vMsgClient) - 1, 0)) < 0)
			{
				printf("erreur recv()\n");
			}

			vMsgClient[n] = 0;

			FILE *fileToSend = fopen("log_file.txt", "rb");
			if( fileToSend != NULL)
			{
				fseek(fileToSend, 0, SEEK_END);
				long fsize = ftell(fileToSend);
				fseek(fileToSend, 0, SEEK_SET); 
				char *string = malloc(fsize + 1);
				fread(string, 1, fsize, fileToSend);
				fclose(fileToSend);

				if(send(sock_client, string, strlen(string), 0) < 0)
				{
					printf("erreur send()\n");
				}
				free(string);
			}
			else
			{
				char * msg = "Le fichier de logs n'a pas encore ete cree";
				if(send(sock_client, msg, strlen(msg), 0) < 0)
				{
					printf("erreur send()\n");
				}
			}
			close(sock_client);
		}
	} // while
	close(soc);
	close(socLog);
} 
