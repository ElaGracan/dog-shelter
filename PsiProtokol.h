
#ifndef __PSIPROTOCOL_H_
#define __PSIPROTOCOL_H_

#define POPIS          1
#define KRITERIJI      2
#define IZBOR          3
#define BOK            4
#define ODGOVOR        5
#define PONUDA         6

// ovo ispod koriste i klijent i server, pa moze biti tu...
#define OK      1
#define NIJEOK  0

int primiPoruku( int sock, int *vrstaPoruke, char **poruka );
int posaljiPoruku( int sock, int vrstaPoruke, const char *poruka );

#define error1( s ) { printf( s ); exit( 0 ); }
#define error2( s1, s2 ) { printf( s1, s2 ); exit( 0 ); }
#define myperror( s ) { perror( s ); exit( 0 ); }

#endif
