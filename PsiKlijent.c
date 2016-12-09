

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "PsiProtokol.h"

void pretraziPopis(int sock);
void izaberiKriterij(int sock ,char* popisKriterija);
void obradiBOK(int sock);

int main(int argc, char **argv)
{
	if(argc != 3)
		error2("Upotreba: %s IP-adresa port\n", argv[0]);

	char dekadskiIP[20]; strcpy(dekadskiIP, argv[1]);
	int port;          sscanf(argv[2], "%d", &port);

	// socket...
	int mojSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(mojSocket == -1) myperror("socket");

	// connect...
	struct sockaddr_in adresaServera;

	adresaServera.sin_family = AF_INET;
	adresaServera.sin_port = htons( port );
    adresaServera.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset( adresaServera.sin_zero, '\0', 8 );

	if( connect(mojSocket, (struct sockaddr *) &adresaServera, sizeof( adresaServera )) == -1)
		myperror("connect");

	// ispisi menu
	int gotovo = 0;
	while(!gotovo)
	{
		printf( "\n\nOdaberi opciju...\n"
				"   1. pretraga popisa pasa\n"
				"   2. izlaz iz programa\n"
				"   \n: " );

		int opcija;
		scanf("%d", &opcija);

		switch(opcija)
		{
			case 1: pretraziPopis(mojSocket); gotovo=1; break;
			case 2: obradiBOK(mojSocket); gotovo = 1; break;
			default: printf("Pogresna opcija...\n"); break;
		}
	}

	close(mojSocket);

	return 0;
}

void pretraziPopis(int sock)
{


    int vrstaPoruke;
    char* popisKriterija;
    char* praznaPoruka="";
    int brojKriterija;



    if(posaljiPoruku(sock, POPIS, praznaPoruka) != OK)
        error1("Doslo je do pogreske u komunikaciji sa serverom...\n");


    if(primiPoruku(sock, &vrstaPoruke, &popisKriterija) != OK)
		error1("Doslo je do pogreske u komunikaciji sa serverom...\n");
    //dodaj kontrolu

	izaberiKriterij(sock, popisKriterija);
	return;

}

void izaberiKriterij(int sock , char* popisKriterija)
{
    printf("Upisite redni broj ispred kriterija po kojem zelite pretrazivati:\n\n");
	printf("%s", popisKriterija);
    int brojKriterija;

	scanf("%d", &brojKriterija);

	char poruka[10];
	sprintf(poruka, "%d", brojKriterija);

	posaljiPoruku(sock, IZBOR, poruka);

	int vrstaPoruke; char *odgovor;
	if(primiPoruku(sock, &vrstaPoruke, &odgovor) != OK)
		error1("Doslo je do pogreske u komunikaciji sa serverom...\n");

	if(vrstaPoruke != PONUDA)
		error2("Doslo je do pogreske u komunikaciji sa serverom (nije poslao PONUDU)...\n%s", odgovor);

    printf("\n%s\n",odgovor);

    printf("OK\n");

	free(odgovor);
}


void obradiBOK( int sock )
{
	posaljiPoruku( sock, BOK, "" );

	int vrstaPoruke;
	char *odgovor;
	if( primiPoruku( sock, &vrstaPoruke, &odgovor ) != OK )
		error1( "Doslo je do pogreske u komunikaciji sa serverom...\n" );

	if( vrstaPoruke != ODGOVOR )
		error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao ODGOVOR)...\n" );

	if( strcmp( odgovor, "OK" ) != 0 )
		printf( "Greska: %s\n", odgovor );
	else
	    printf( "OK\n" );
}
