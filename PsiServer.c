/*
# kriterij
0 musko
1 zensko
2 mladi od 2 godine
3 izmedu 2 i 5 godina
4 stariji od 5 godina
5 labrador
6 beagle
7 rottweiler
*/

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

#define BROJKRITERIJA 8

void komunicirajSaKlijentom(int sock);
void posaljiKriterije(int sock);
void posaljiPonude(int sock, char *poruka);
void pretraziPrihvatiliste(int kriterij, char** ponuda);
void inicijalizirajPrihvatiliste();
void inicijalizirajPsa(char* ime, int brojPsa, int k1, int k2, int k3);

int prihvatiliste[BROJKRITERIJA][20]={0};
char* imenaPasa[20]={""};

int main( int argc, char **argv )
{
    //inicijalizacija prihvatilista
    inicijalizirajPrihvatiliste();


	if( argc != 2 )
		error2( "Upotreba: %s port\n", argv[0] );

	int port; sscanf( argv[1], "%d", &port );

	// socket...
    int listenerSocket = socket( PF_INET, SOCK_STREAM, 0 );
	if( listenerSocket == -1 )
		myperror( "socket" );

	// bind...
	struct sockaddr_in mojaAdresa;

	mojaAdresa.sin_family = AF_INET;
	mojaAdresa.sin_port = htons( port );
	mojaAdresa.sin_addr.s_addr = inet_addr("127.0.0.1"); //161.
	memset( mojaAdresa.sin_zero, '\0', 8 );

	if( bind( listenerSocket, (struct sockaddr *) &mojaAdresa, sizeof( mojaAdresa ) ) == -1 )
		myperror( "bind" );

	// listen
	if( listen( listenerSocket, 10 ) == -1 )
		myperror( "listen" );

	// accept
	while( 1 )
	{
        struct sockaddr_in klijentAdresa;
		int lenAddr = sizeof( klijentAdresa );

		int commSocket = accept( listenerSocket, (struct sockaddr *) &klijentAdresa, &lenAddr );

		if( commSocket == -1 )
			myperror( "accept" );

		char *dekadskiIP = inet_ntoa( klijentAdresa.sin_addr );
		printf( "Prihvatio konekciju od %s [socket=%d]\n", dekadskiIP, commSocket );

		komunicirajSaKlijentom( commSocket );

		close( commSocket );
		printf( "Zavrsio komunikaciju sa %s [socket=%d]\n", dekadskiIP, commSocket );
	}

	return 0;
}

void komunicirajSaKlijentom(int sock)
{
	int vrstaPoruke, gotovo = 0;
	char *poruka;

	while(!gotovo)
	{
		if(primiPoruku(sock, &vrstaPoruke, &poruka) != OK)
		{
			printf("Greska u komunikaciji sa klijentom [socket=%d]...\n", sock);
			gotovo = 1;
			continue;
		}

		switch(vrstaPoruke)
		{
			case POPIS: posaljiKriterije(sock); break;
			case IZBOR: posaljiPonude(sock, poruka); gotovo=1; break;
			case BOK: posaljiPoruku(sock, ODGOVOR, "OK"); gotovo = 1; break;
			default: posaljiPoruku(sock, ODGOVOR, "Nepostojeci kod poruke!\n");
		}

		free(poruka);
	}
}

void posaljiKriterije(int sock)
{

	char poruka[200]="";
	strcat(poruka, "1 musko\n2 zesko\n3 mladji od dvije godine\n4 dvije do pet godina\n");
	strcat(poruka, "5 stariji od pet godina\n6 labrador\n7 beagle\n8 rottweiler\n\n");



    posaljiPoruku(sock, KRITERIJI, poruka);


    return;
}

void posaljiPonude(int sock, char *poruka)
{
	int broj;

	if(sscanf(poruka, "%d\n", &broj) != 1 || broj <= 0 || broj > BROJKRITERIJA)
	{
		posaljiPoruku(sock, ODGOVOR, "Pogresan oblik naredbe\n");
		return;
	}
	broj--;


    char* ponuda="";
    pretraziPrihvatiliste(broj, &ponuda);

	posaljiPoruku(sock, PONUDA, ponuda);

	return;
}

void pretraziPrihvatiliste(int kriterij, char** p)
{

    int i=0, j=0;
    char poruka[100]="";

    while(i<5 && j<20)
    {
        //trazim u polju prihvatiliste, redak kriterij
        if(prihvatiliste[kriterij][j])
        {
            i++;
            strcat(poruka, imenaPasa[j]);
            strcat(poruka, "\n");
        }
        j++;
    }
    *p=poruka;
    return;

}
void inicijalizirajPrihvatiliste()
{
    inicijalizirajPsa("Binka", 0, 1, 2, 7);
    inicijalizirajPsa("Alfa", 1, 1, 3, 6);
    inicijalizirajPsa("Dona", 4, 1, 4, 5);
    inicijalizirajPsa("Bela", 2, 1, 2, 5);
    inicijalizirajPsa("Nera", 3, 1, 3, 6);
    inicijalizirajPsa("Max", 5, 0, 4, 7);
    inicijalizirajPsa("Rex", 6, 0, 3, 7);
    inicijalizirajPsa("Fido", 7, 0, 4, 5);
    inicijalizirajPsa("Ben", 8, 0, 2, 6);
    inicijalizirajPsa("Alf", 9, 0, 4, 6);
    inicijalizirajPsa("Siljo", 10, 3, 4, 7);
    inicijalizirajPsa("Miki", 11, 0, 2, 5);
    inicijalizirajPsa("Maza", 12, 1, 4, 6);
    inicijalizirajPsa("Dama", 13, 1, 3, 5);
    inicijalizirajPsa("Crta", 15, 1, 2, 7);
    return;
}
void inicijalizirajPsa(char* ime, int brojPsa, int k1, int k2, int k3)
{
    imenaPasa[brojPsa]=ime;
    prihvatiliste[k1][brojPsa]=1;
    prihvatiliste[k2][brojPsa]=1;
    prihvatiliste[k3][brojPsa]=1;
    return;
}
