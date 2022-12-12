#include <iostream>
#include <CL\\cl.h>

using namespace std;

unsigned int* InitMatrice(const unsigned int& taille);
void AffichageMatrice(const unsigned int* matrice, const unsigned int& taille);

int main() {

	unsigned int taille;
	cin >> taille;

	if (taille < 2){
		cout << "La taille doit être supérieure ou égale à 2." << endl;
		return -1;
	}

	unsigned int* matrice = InitMatrice(taille);
	AffichageMatrice(matrice, taille);

	delete matrice;

	return 0;
}

unsigned int* InitMatrice(const unsigned int& taille) {
	unsigned int* nouvelleMatrice = new unsigned int[taille*taille];

	// Initialisation de la matrice
	unsigned int i;
	for (i = 0; i < (taille * taille) - 1; i++) { // Boucle d'initialisation des valeurs de la table
		nouvelleMatrice[i] = taille + 1;
	}
	nouvelleMatrice[taille * (taille - 1)] = 1; // Définit le lien entre l'élément n-1 et l'élément 0
	nouvelleMatrice[(taille * taille) - 1] = 0;
	for (i = 0; i < taille - 1; i++) {
		nouvelleMatrice[(taille * i) + i] = 0;
		nouvelleMatrice[(taille * i) + i + 1] = 1;
	}

	return nouvelleMatrice;
}

void AffichageMatrice(const unsigned int* matrice, const unsigned int& taille) {
	for (unsigned int i = 0; i < taille; i++) {
		for (unsigned int j = 0; j < taille; j++) {
			cout << matrice[(taille * i) + j] << " ";
		}
		cout << endl;
	}
}