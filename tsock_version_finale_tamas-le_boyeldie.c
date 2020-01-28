/******************************************/
/*           Boyeldieu Loic               */
/*           Tamas-leloup Aurélien        */
/******************************************/


/* FICHIERS A INCLURE */


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <limits.h>


/*******************************************************************************/
					  /* FONCTIONS POUR LES MESSAGES */
/*******************************************************************************/

/* CONSTRUCTION DU MESSAGE */
/* Répéter un motif pour créer un message de longueur donnée */


void construire_message(char *message, char motif, int lg) { 
	int i;
	for (i=0;i<lg;i++){
		message[i] = motif;
	}
} 

/* AFFICHER LE MESSAGE */
/* Afficher un message de longueur donnée */


void afficher_chaine(char *message, int lg) { 
	int i; 
	for (i=0;i<lg;i++){
		printf("%c", message[i]); 
	}
	//printf("\n");
}

/* FORMAT DU NUMERO DU MESSAGE */

/* fonction qui donne la longueur d'un entier en nombre de symbole */
int longueur_nombre(int Nombre) {
  int lg=0;
  int leNombre = Nombre;
  
  if (leNombre>=0) {
    
    while (leNombre >= 1) {
      leNombre = leNombre/10;
      lg++;
    }
  }
  else {
    printf("Le numero du message ne doit pas etre negatif\n");
    exit(1);
  }
  
  
  return lg;
}

/* Fonction pour le format de l'entete d'un message */

void entete_message(int numero_message) {
  // Message num 99999 max, soit 5 chiffres
  if (numero_message < 99999){
    int lg = 5-longueur_nombre(numero_message);
    int i;
    for (i=0;i<lg;i++){
      printf("-");
    }
    printf("%d",numero_message);
  }

  else {
    printf("Nombre de message a envoyer doit etre inferieur a 99999\n");
    exit(1);
  }

}


/* FONCTION POUR AFFICHER LE MESSAGE AU FORMAT FINAL */
void afficher_message(char *message, int lg_message, int nb_message){
	
	printf("[");
	entete_message(nb_message);
	afficher_chaine(message, lg_message);
	printf("]\n");
	

}



/* FONCTION POUR L'UTILISATION DE TSOCK */

void utilisation_tsock() {
	printf("usage puit   : cmd [-p][-u][-n ##][-l ##] port\n");
	printf("usage source : cmd [-s][-u][-n ##][-l ##] host port\n");

}

/*******************************************************************************/
						/* FONCTIONS POUR LE SOCKET */
/*******************************************************************************/


/* CREER UN SOCKET POUR UDP */

int ouvrir_socket() {
	int sock;
	// int socket(int domain, int type, int protocol);
	// Permet de créer un socket
	// int domain : domaine d'utilisation du socket
	// int type : spécifie la sémantique de la communication
	// int protocol : Protocole à utiliser (0 par défaut)
	// ===> identifiant logique du socket (représentation interne à l'OS)
  	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    		printf("échec lors de la création du socket\n");
    		exit(1);
  	}
	return sock;
}

/* CREER UN SOCKET POUR TCP */
/* On utilise le type SOCK_STREAM */

int ouvrir_socket_tcp() {
  int sock;  
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("échec lors de la création du socket\n");
    exit(1);
  }
  return sock;
  
}


/* FERMETURE D'UN SOCKET */

void fermer_socket(int sock) {
	// int close(int fd);
	// Permet la fermeture du socket. Il fait en sorte que le descripteur ne référence plus aucun fichier.
	// int fd : descripteur de notre socket
	// ===> code d'erreur : -1 s'il y a un problème, 0 sinon
  	if (close(sock) == -1) {
    		printf("échec lors de la fermeture du socket\n");
    		exit(1);
  	}
}


/*******************************************************************************/
				/* FONCTIONS POUR LA SOURCE PROTOCOLE UDP */
/*******************************************************************************/

// sourceUDP
// * Objectif : ouvre une source avec le protocole UDP
// * Entrées : adresse (de la destination), port, nb_message (à envoyer), lg_message (à envoyer)
// * Sorties :

void sourceUDP(char *adresse, int port, int nb_message, int lg_message) {
	// Création du socket :
	int sock = ouvrir_socket();
	
	// Construction de l'adresse du socket distant afin de pouvoir communiquer avec lui.

	// Déclaration des variables
	struct hostent *hp = NULL;
	struct sockaddr_in adr_distant;

	// void *memset(void *str, int c, size_t n);
	// Copie le caractere c dans les n premiers caracteres de l'espace mémoire pointé par str
	// (ici on met tout à zero la structure)
	// ===> str
	memset((char *)&adr_distant, 0, sizeof(adr_distant));
	adr_distant.sin_family = AF_INET; // Domaine Internet
	adr_distant.sin_port = port; // Numéro de port


	// struct hostent *gethostbyname(const char *name);
	// permet de trouver l'adresse IP de la station dont on connait le nom
	// const char *name : Nom de la station
	// ===> Pointeur sur une structure de type hostent qui contient entre autre une liste d'adresse IP
	// ===> NULL en cas d'erreur
	if ((hp = gethostbyname(adresse)) == NULL){
		printf("Erreur gethostbyname\n");
		exit(1);
	}


	// On met l'adresse IP trouvé à l'aide de gethostbyname dans notre structure adresse
	// void *memcpy(void *str1, const void *str2, size_t n);
	// Copie n caractères de l'espace pointé par str2 dans la mémoire pointé par str1
	// ===> str1
	memcpy((char*)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);
	// Création et envoie des messages

 	char *message = malloc(lg_message*sizeof(char));
	int i;

	for (i=0;i<nb_message;i++) {
	  construire_message(message, 65+(i%26),lg_message);


		// int sendto(int sock, const char *msg, int len, int flags, const struct sockaddr *to, int tolen);
		// int sock : repréentation interne du socket local
		// const char *msg : adresse du message à envoyer
		// int len : taille du message à envoyer en octet
		// int flags : 0
		// const struct sockaddr *to : Pointeur sur la structure contenant l'adresse du socket distant
		// int tolen : longueur de la structure pointé par to
		if (sendto(sock, message, lg_message, 0, (struct sockaddr *) &adr_distant, sizeof(adr_distant)) == -1){
			printf("echec de l'envoi\n"); 
			exit(1);
		}
		
		
		//Affichage du message dans le bon format
		
		printf("SOURCE : Envoi n° %d (%d)",i+1, lg_message);
		afficher_message(message, lg_message, i+1);

  	}
  	printf("SOURCE : Fin des envois!\n");


	// Fermeture du socket :
	fermer_socket(sock);
}


/*******************************************************************************/
				/* FONCTIONS POUR LE PUIT PROTOCOLE UDP */
/*******************************************************************************/


// puitsUDP
// * Objectif : ouvre un puits avec le protocole UDP
// * Entrées : port, nb_message (à recevoir), lg_message (maximum des tampons)
// * Sorties :

void puitsUDP (int port, int nb_message, int lg_message) {
	// Création du socket :
	int sock;
	sock = ouvrir_socket();
    
	// Déclaration des variables :
	struct sockaddr_in adr_local;

	memset((char *)&adr_local, 0, sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = port;
	adr_local.sin_addr.s_addr = INADDR_ANY; // = addresse locale


	// Association @socket avec la représentation interne :
	// On fait le bind pour lier au socket les 
	// informations d'adresse destiner à la 
	// communication vers l'extérieur.
	// int bind(int descripteur, sockaddr localaddr, int addrlen);
	// int descripteur : identifiant local du socket
	// sockaddr localaddr : identifiant exterieur du socket
	// int addrlen : taille de l'identifiant exterieur du socket
	if(bind(sock, (struct sockaddr *)&adr_local, sizeof(adr_local)) == -1){
		printf("Echec du bind\n");
		exit(1);
	}
	// Reception et affichage :
	char *message = malloc((sizeof(char))*lg_message);  //NULL
	struct sockaddr padr_em;
	unsigned int plg_adr;
		
        // int recvfrom(int sock, char *pmesg, int lg_max, int option, struct sockaddr *padr_em, int *plg_adr_em);
		// Permet de recevoir un message
		// int sock : Représentation interne du socket local
		// char *pmesg : Adresse mémoire à laquelle le message lu sera stocké en sortie de la fonction
		// int lg_max : taille de l'espace réservé pour stocker le message reçu
		// int option : 0
		// struct sockaddr *padr_em : Adresse mémoire à laquelle l'adresse du socket émetteur sera stockée en sortie de la fonction
		// int *plg_adr_em : adresse à laquelle la longeur de la structure pointée par padr_em sera stockée en sortie de la fonction
		// ===> Nombre d'octet lus
		// ===> -1 si erreur
	if (nb_message==-1) {
		while (1) {
			if (recvfrom(sock, message, (sizeof(char))*lg_message, 0, &padr_em, &plg_adr) == -1){
				printf("echec de reception\n"); 
				exit(1);
			}
			
			afficher_chaine(message,lg_message);
 		}
	} else {
		int i;
		for (i=0;i<nb_message;i++) {
			if (recvfrom(sock, message, (sizeof(char))*lg_message, 0, &padr_em, &plg_adr) == -1){
				printf("echec de reception\n");
				exit(1);
			}
			
			printf("PUIT : Reception n° %d (%d)",i+1, lg_message);
			afficher_message(message, lg_message,i+1);
		}
		printf("PUIT : Fin de la reception\n");
	}

	
	// Fermeture du socket :
	fermer_socket(sock);
}

/*******************************************************************************/
				/* FONCTIONS POUR LA SOURCE PROTOCOLE TCP */
/*******************************************************************************/

void sourceTCP(char *adresse, int port, int nb_message, int lg_message) {
 	
 	// Création du socket :
	int sock = ouvrir_socket_tcp();
	int connexion=0;
	int etat_envoi;
	int i;
	char *message;
	
	// Construction de l'adresse du socket distant afin de pouvoir communiquer avec lui.

	// Déclaration des variables
	struct hostent *hp = NULL;
	struct sockaddr_in adr_distant;

	// void *memset(void *str, int c, size_t n);
	// Copie le caractere c dans les n premiers caracteres de l'espace mémoire pointé par str
	// (ici on met tout à zero la structure)
	// ===> str
	memset((char *)&adr_distant, 0, sizeof(adr_distant));
	adr_distant.sin_family = AF_INET; // Domaine Internet
	adr_distant.sin_port = port; // Numéro de port

	/*if(bind(sock, (struct sockaddr *)&adr_distant, sizeof(adr_distant)) == -1){
		printf("Echec du bind\n");
		exit(1);
	}*/

	if ((hp = gethostbyname(adresse)) == NULL){
		printf("Erreur gethostbyname\n");
		exit(1);
	}
	
	memcpy((char*)&(adr_distant.sin_addr.s_addr), hp->h_addr, hp->h_length);

	// Phase de connexion
	connexion=connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
	if (connexion == -1)
	  {
	    printf("Pas de connexion\n");
	    exit(1);
	  }
	  
	message = malloc(sizeof(char)*lg_message);
	
	for (i=0;i<nb_message;i++) {
	  construire_message(message, 65+(i%26),lg_message);	
	  printf("SOURCE : Envoi n° %d (%d)",i+1, lg_message);
	  afficher_message(message,lg_message,i+1);	
	  etat_envoi=send(sock,message,lg_message,MSG_OOB);
 
	  if (etat_envoi==-1)
	    {
	      printf("Erreur d'envoi \n");
	      exit(1);
	    }
	}

	
	// Fermeture du socket 
	if (shutdown(sock, 1)==-1)
	  {
	    printf("Erreur avec le shutdown\n");
	  }

	fermer_socket(sock);
	free(message);




}


/*******************************************************************************/
				/* FONCTIONS POUR LE PUIT PROTOCOLE TCP */
/*******************************************************************************/


void puitsTCP (int port, int nb_message, int lg_message){
  
  int sock,sock_bis;
  int max = 10;
  int i;
  int lg_max=30;
  char *message;
  sock = ouvrir_socket_tcp();

  struct sockaddr_in adr_local;

  memset((char *)&adr_local, 0, sizeof(adr_local));
  adr_local.sin_family = AF_INET;
  adr_local.sin_port = port;
  adr_local.sin_addr.s_addr = INADDR_ANY; // = addresse locale
	
	
  // Association @socket avec la représentation interne :
  // On fait le bind pour lier au socket les 
  // informations d'adresse destiner à la 
  // communication vers l'extérieur.
  // int bind(int descripteur, sockaddr localaddr, int addrlen);
  // int descripteur : identifiant local du socket
  // sockaddr localaddr : identifiant exterieur du socket
  // int addrlen : taille de l'identifiant exterieur du socket
  
  if(bind(sock, (struct sockaddr *)&adr_local, sizeof(adr_local)) == -1){
    printf("Echec du bind\n");
    exit(1);

  }
  struct sockaddr *padr_client;
  padr_client=malloc(sizeof(struct sockaddr));
	
  listen(sock,5);
  int *plg_adr_client;
  plg_adr_client=malloc(sizeof(int));
  sock_bis=accept(sock,padr_client,plg_adr_client);

  if(sock_bis==-1)
    {
      printf("echec du accept\n");
      exit(1);
    }
  message = malloc(lg_max*sizeof(char));
  i=0;
      while((lg_message=read(sock_bis,message,lg_max))>0)
      {	
	printf("PUIT : Reception n° %d (%d)",i+1, lg_message);
	
	afficher_message(message,lg_message,i+1);
	i++;
      }
 
   
  if (shutdown(sock, 1)==-1)
    {
      printf("Erreur avec le shutdown\n");
    }
  
	
      fermer_socket(sock);
      free(message);
}



/*******************************************************************************/
						/* PROCEDURE PRINCIPALE */
/*******************************************************************************/

int main (int argc, char **argv){
	int c;
	int n = 0;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int lg_message = -1; /* Longueur des messages à envoyer ou à recevoir, par défaut : 10 en émission et en réception */
	int source = -1 ; /* 0=puits, 1=source */
	char * adresse;
	int port;
	int protocole=1; /* 1=TCP, 0=UDP */
	char * protoUtilise;
	// -----------------------------------------------------------------------------
	while ((c = getopt(argc, argv, "psun:l:")) != -1) {
		switch (c) {
		case 'p':
		  if (source == 1) {
		    utilisation_tsock();
		    exit(1);
		  }
		  source = 0;
		  n++;
		  break;

		case 's':
		  if (source == 0) {
		    utilisation_tsock();
		    exit(1) ;
		  }
		  source = 1;
		  n++;
		  break;

		case 'n':
		  nb_message = atoi(optarg);
		  n+=2;
		  break;

		case 'l':
		  lg_message = atoi(optarg);
		  n+=2;
		break;
		
		case 'u':
		  protocole=0;
		  n++;
		  break;
	
		default:
		  utilisation_tsock();
		  exit(1);
		  break;
		}
	}
	// -----------------------------------------------------------------------------
	
	// Analyse des arguments :
	if (argc-1>n) {
		port = atoi(argv[argc-1]);
		if (port == 0){
		  utilisation_tsock();
		  exit(1);
		}
	}
  	else {
    		printf("Erreur : port manquant\n");
			utilisation_tsock();
    		exit(1);
	}
	if (source == 1) {
		if (argc-2>n) 
		  adresse = argv[argc-2];
    		else {
		  printf("Erreur : adresse manquante\n");
		  utilisation_tsock();
		  exit(1);
		}
	}
	
	
	// Analyse des options :
	switch (source) {
	case 1:	// cas de la source
		
		// on regarde quel protocole on utilise
		if (protocole==0){
			protoUtilise = "UDP";
		}
		else
		{
			protoUtilise = "TCP";
		}
		
		// Option par défaut
		
		if (nb_message == -1){
			nb_message = 10 ;
		}
		
		if (lg_message == -1){
			lg_message = 10 ;
		}	
		
		 
		printf("SOURCE : lg_mesg_emis : %d, port : %d, nb_envois : %d, TP : %s, dest : %s \n",lg_message, port, nb_message, protoUtilise, adresse);
		

		break;

	case 0:// cas du puits
		
		// on regarde quel protocole on utilise
		if (protocole==0){
			protoUtilise = "UDP";
		}
		else
		{
			protoUtilise = "TCP";
		}
		
		// Option par défaut
		if (lg_message == -1){
			lg_message = 10 ;
		}
	
		if (nb_message == -1){
		  nb_message = INT_MAX;
		  printf("PUIT : lg_mesg_recu : %d, port : %d, nb_reception : infini, TP : %s\n",lg_message, port, protoUtilise);
		}
		else
		  {
		    printf("PUIT : lg_mesg_recu : %d, port : %d, nb_reception : %d, TP : %s\n",lg_message, port, nb_message, protoUtilise);

		  }
			
		break;
		
	default:
		utilisation_tsock();
		exit(1);
		break;
	}
	// -----------------------------------------------------------------------------
	
	// -----------------------------------------------------------------------------
	if (protocole)
	  {
	    if (source){
	      sourceTCP(adresse,port,nb_message,lg_message);
	    }
	    else{
	      puitsTCP(port,nb_message,lg_message);
	    }
	  }
	else
	  {
	    if (source){
	      sourceUDP(adresse, port, nb_message, lg_message);
	    }
	    else{
	      puitsUDP(port, nb_message, lg_message);
	    }
	  }

	return 0;
}

