#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <conio.h>
#include <windows.h>

//definition des structures necessaires
typedef struct produit{
    int code_produit;
    char nom_produit[64];
    char description[256];
    int quantitee;
    float prix;
    char date[10];
}produit;

//liste chainée pour stocker les données du stock
typedef struct stock{
    produit valeur;
    struct stock* suivant;
}stock;

//fonctions pour effacer l'ecran
enum ClearCodes {
    CLEAR_ALL
};
static inline void deplacerVers(int ligne, int colonne) {
    printf("\x1b[%d;%df", ligne, colonne);
}
static inline void effacerEcran(void) {
    //printf("\x1b[%dJ", CLEAR_ALL);deplacerVers(0,0);
	system("cls");
}
void ecran1(stock *tete);
void ecran2(stock* tete);
//ecriture des produits d'une maniere propre
void afficherProduit(produit prod){
	printf("%-5d, %-20s, %-5.3f,%-3d,%s,%s",prod.code_produit,prod.nom_produit,prod.prix,prod.quantitee,prod.description, prod.date);
}

//fonction pour afficher le stock
void afficherStock(stock * st){
	
	stock * aux;
    aux = st;
	if (st->valeur.code_produit != -1){
		aux = st;
        // parcourir la liste chainée en affichant chaque produit
		do
		{
			afficherProduit(aux->valeur);
			aux = aux->suivant;
			printf("\n");
		}while (aux!= NULL);	
	}
}

// vérifier si un produit existe dans le stock(0 si il existe, 1 sinon)
int verif(stock * st, int id){
	stock *aux = st;
	while(aux != NULL){
		if(aux->valeur.code_produit == id){
			return 0;
		}
		aux = aux->suivant;
	}
	return 1;
}

int verifier_format_date(char* date) {
  int jour, mois, annee;
  char separateur1, separateur2;
  int nb_elements_scannes = sscanf(date, "%d%c%d%c%d", &jour, &separateur1, &mois, &separateur2, &annee);
  if (nb_elements_scannes != 5) {
    // pas assez d'éléments ont été scannés, donc la date n'est pas au bon format
    return 0;
  }
  if (separateur1 != '/' || separateur2 != '/') {
    // les séparateurs ne sont pas '/', donc la date n'est pas au bon format
    return 0;
  }
  if (jour < 1 || jour > 31 || mois < 1 || mois > 12 || annee < 1) {
    // le jour, le mois ou l'année est en dehors de la plage valide, donc la date n'est pas au bon format
    return 0;
  }
  return 1;
}

//fonction pour saisir le code du produit (peut etre utiliser pour la modification ou la supression des produits)
int saisirCP(stock * st){
	int temp_id;
	while(1){
			printf("Entre le code du produit: ");
			scanf("%d", &temp_id);
			if(verif(st, temp_id) == 0) break;
			else printf("Pas de prduits avec le code %d, veuillez réessayer\n");
		}
	return temp_id;
}

// fonction pour faciliter l'affectation d'un produit a un autre
void copierProduit(produit *prod1,produit *prod2){
	prod1->code_produit = prod2->code_produit; strcpy(prod1->nom_produit,prod2->nom_produit);
	prod1->prix = prod2->prix; prod1->quantitee = prod2->quantitee;
	strcpy(prod1->description,prod2->description);	strcpy(prod1->date,prod2->date);
}

//ajouter un produit au stock
void ajouterProduit(stock * st, produit p){
	// Adds a new produit to the stock
	stock * ptr,*aux;	
	
	if (st->valeur.code_produit == -1){
		// si le stock est vide alors modifier la valeur du premier noeud 
		copierProduit(&(st->valeur),&p);
		
	} else {
		// sinon on parcoure la liste et on ajoute le produit au fin
		ptr = (stock*)malloc(sizeof(stock));	
		ptr->suivant = NULL;
		aux = st;
		copierProduit(&(ptr->valeur),&p);
		while(aux->suivant != NULL){
			aux = aux->suivant;
		}
		aux->suivant = ptr;
	}
    printf("Produit à été ajouté avec succès\n");
    printf("Appuyez sur entrée pour continuer\n");	
}

// modifier les données d'un un produit du stock selon le code_produit
void modifier(stock *st, int code){
    int toInput; //variable muette
    stock *aux;
    aux = st;
    while(aux->valeur.code_produit != code){
        aux = aux->suivant;
    }
    if(aux == NULL){
        printf("Produit inexistant\n");
    }
    else{
        char c;
        printf("Produit n°%d :\n", code);
        afficherProduit(aux->valeur);
        printf("\n");
        printf("Nouveau prix = ");
        scanf("%d", &toInput);
        printf("\n");
        aux->valeur.prix = toInput;
        printf("Nouvelle quantité = \n");
        scanf("%d", &toInput);
        printf("\n");
        aux->valeur.quantitee = toInput;
        printf("Le produit n°%d à été modifé avec succès\n");
        printf("Appuyez sur entrée pour continuer\n");
    }
}

// supprimer un produit du stock selon le code_produit
void supprimerProduit(stock * st, int id){
	stock *ptr, *aux;
	aux = st;
	ptr = (stock*)malloc(sizeof(stock));
	if (st->valeur.code_produit == -1){
		// si le stock est vide alors il n'y a rien à supprimer
		printf("Le stock est vide");
	}
	else{
		if (st->valeur.code_produit == id){
			// supprimer le premier noeud
			st->valeur = st->suivant->valeur;
            st->suivant = st->suivant->suivant;
			free(ptr);
		}
		else{
			// sinon on parcoure la liste pour trouver le produit à effacer
			while(aux->suivant->valeur.code_produit != id){
				aux = aux->suivant;
			}
			ptr = aux->suivant;
			aux->suivant = ptr->suivant;
			free(ptr);
		}
		printf("Le produit avec le code %d a été supprimé avec succès\n", id);
        printf("Appuyez sur entrée pour continuer\n");
	}
}

//importer le stock
void importer(stock * st){
	FILE* ptr; char ch; produit temp_prod;
	ptr = fopen("stock.csv", "r"); //ouvrir le fichier en mode lecture
	
	char temp_str1[400],temp_str2[400],temp_str5[400],temp_str3[400],temp_str4[400],temp_str6[400];
	int i=0; int scan_result;
	do {
		scan_result = fscanf(ptr, "%400[^;];%400[^;];%400[^;];%400[^;];%400[^;];%400[^\n]", 
								temp_str1, temp_str2, temp_str5,temp_str3, temp_str4, temp_str6);
		ch = fgetc(ptr);
		if (i>0 && scan_result == 6){
			temp_prod.code_produit = atoi(temp_str1);
			strcpy(temp_prod.nom_produit,temp_str2);
			strcpy(temp_prod.description,temp_str5);
			temp_prod.prix = atof(temp_str3);
			temp_prod.quantitee = atoi(temp_str4);
			strcpy(temp_prod.date,temp_str6);
			ajouterProduit(st,temp_prod);
		}
		i++;
	}while(ch != EOF);
	fclose(ptr);
}

//exporter le stock au fichier
void exporter(stock * st){
	FILE* ptr;
	ptr = fopen("stock.csv","w"); // ouvrir le fichier en mode ecriture
	stock *temp = st;
    fprintf(ptr, "code_produit;nom_produit;description;pric;quantitée;date\n");
	while(temp != NULL) {
	fprintf(ptr,"%d;%s;%s;%.3f;%d;%s\n",temp->valeur.code_produit,temp->valeur.nom_produit,temp->valeur.description,
	temp->valeur.prix,temp->valeur.quantitee,temp->valeur.date);
	temp = temp->suivant;
	}
	fclose(ptr);
}

// ecran d'accueil
void ecran1(stock *tete){
    printf("bienvenue dans stock-manager\n");
    char character;
    character = getch();
    // bouton 'enter' de la clavier == 13
    printf("Appuyez sur entrée pour continuer\n");
    if ((int)character == 13){
		effacerEcran();
        ecran2(tete);
    }
}

//autre ecrans
void ecran2(stock* tete){
	char character;
	char choice;
	int c;
	printf("1 - ajouter un produit\n"
			"2 - supprimer un produit\n"
			"3 - afficher tous les produits\n"
			"4 - modifier un produit\n"
			"5 - exit\n");
	scanf("%d", &choice);
	switch (choice) {
        // ajouter un prduit
		case 1:
			effacerEcran();
			produit *p;
            do{
                printf("Donnez le code du produit: ");
                scanf("%d",&(p->code_produit));
                if(verif(tete, p->code_produit) == 0){
                    printf("il y a déjà un produit avec le code %d, veuillez réessayer\n", p->code_produit);
                }
            }while(verif(tete, p->code_produit) == 0);
            printf("Donnez le nom du produit: ");
            scanf("%s",&(p->nom_produit));
            printf("Donnez la description du produit: ");
            scanf("%s",&(p->description));
            printf("Donnez la quantite du produit: ");
            scanf("%d",&(p->quantitee));
            printf("Donnez la prix unit du produit: ");
            scanf("%f",&(p->prix));
            do{
                printf("Donnez la date d'exp du produit en respectant ce modele -'JJ/MM/AAAA': ");
                scanf("%s",&(p->date));
                if(verifier_format_date(p->date) == 0){
                    printf("date erronée, veuillez réessayer\n", p->code_produit);
                }
            }while(verifier_format_date(p->date) == 0);
			ajouterProduit(tete, *p);
    		character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete);
			break;
        // supprimer un produit
		case 2:
			effacerEcran();
			c= saisirCP(tete);
			verif(tete, c);
			supprimerProduit(tete, c);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete);
			break;
        // afficher tous les produits
		case 3:
			effacerEcran();
			afficherStock(tete);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete);
			break;
        // modifier un produit
		case 4:
			effacerEcran();
			c= saisirCP(tete);
			verif(tete, c);
			modifier(tete, c);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			break;
        // quitter
		case 5:
            exporter(tete); // exportation du stock
            exit(0);
			break;
		default:
			printf("choix invalide, veuillez réessayer.\n");
			break;
    }
}

// programme principale
int main(){
    SetConsoleOutputCP(65001); // définir l'encodage de la console sur utf8
	system("color");
	stock *tete;
	tete = (stock*)malloc(sizeof(stock));
	// creation du liste chainée pour le stock
	tete->valeur.code_produit = -1; //pas de produits
	tete->suivant = NULL;
	importer(tete); // importation du stock dans la listee chainnée
	effacerEcran() ;
    // boucle infini
	while(1){
		ecran1(tete);
	}
	return 0;
}

