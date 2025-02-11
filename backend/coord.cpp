#include "coord.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>
#include <algorithm>

// Toutes les règles de simulation sont ici
std::map <std::string, size_t> REGLE_SIMULATION = {{ "TAILLE_GRILLE", 40 },
												{ "NB_COLONIES", 3 },
												{ "NB_FOURMIS_PAR_COLONIE", 10 },
												{ "POURCENTAGE_FOURMIS_OUVRIERES", 70 },
												{ "POURCENTAGE_FOURMIS_REPRODUCTRICE", 20 },
												{ "POURCENTAGE_FOURMIS_GEURRIERES", 10},
												{ "PUISSANCE_FOURMIS_OUVRIERES", 2},
												{ "PUISSANCE_FOURMIS_REPRODUCTRICE", 1},
												{ "PUISSANCE_FOURMIS_GEURRIERES", 5},
												{ "SEUIL_SUCRE", 5},
												{ "NB_MORCEAUX_SUCRE", 80},
												{ "QUANTITE_SUCRE_CHAQUE_MORCEAU", 4},
												{ "NB_ITERATIONS_SIMULATION", 1000 }};

// Moteur de randomisation en C++11. Il est meilleur pour les valeurs de randomisation que rand() en C.
// il est également utilisé pour mélanger std::vector
// Voir numRand(int minVal, int maxVal) et EnsCoord::shuffle() pour son utilisation.
std::random_device rd;
std::mt19937 rng(rd());

// Classe Coord
Coord::Coord(int lig, int col) {
	int TAILLE_GRILLE = REGLE_SIMULATION["TAILLE_GRILLE"];
    if (lig < 0 || lig >= TAILLE_GRILLE || col < 0 || col >= TAILLE_GRILLE)
        throw std::runtime_error("Coordonnee invalide");
    lig_ = lig;
    col_ = col;
}
Coord::Coord(const Coord& c) {
	lig_ = c.lig();
	col_ = c.col();
}
bool Coord::operator==(const Coord& autre) const {
    return (lig_ == autre.lig() && col_ == autre.col());
}
bool Coord::operator!=(const Coord& autre) const {
    return !(*this == autre);
}
Coord& Coord::operator=(const Coord& coord) {
	lig_ = coord.lig();
	col_ = coord.col();

	return *this;
}
std::ostream& operator<<(std::ostream& out, const Coord& coord) {
    out << '(' << coord.lig() << ',' << coord.col() << ')';
    return out;
}

// Classe EnsCoord
std::ostream& operator<<(std::ostream& out, const EnsCoord& ensCoord) {
    for (size_t i = 0; i < ensCoord.taille(); i++) {
        out << ensCoord.ieme(i) << ' ';
    }
    return out;
}
EnsCoord::EnsCoord(const std::vector <Coord>& ensCoord) {
	ensCoord_ = ensCoord;
}
int EnsCoord::position(const Coord& coord) const{    
    for(int i = 0 ; i <= ensCoord_.size() ; ++i) {
        if(ensCoord_[i] == coord){
            return i;
        }
    }
    return -1;
}

bool EnsCoord::contient(const Coord& coord) const {
    for (Coord c: ensCoord_) {
        if (c == coord) return true;
    }
    return false;
}
void EnsCoord::ajoute(const Coord& coord) {
	if (!contient(coord)) ensCoord_.push_back(coord);
}
void EnsCoord::ajouteEnsCoord(const EnsCoord& autre) {
	for (size_t i = 0; i < autre.taille(); i++) {
		ajoute(autre.ieme(i));
	}
}
void EnsCoord::supprime(const Coord& coord) {
    for (size_t i = 0; i < ensCoord_.size(); i++) {
        if (ensCoord_[i] == coord) {
            ensCoord_.erase(ensCoord_.begin()+i);
            return;
        }
    }
}
void EnsCoord::supprimeEnsCoord(const EnsCoord& autre) {
	for (size_t i = 0; i < autre.taille(); i++) {
		supprime(autre.ieme(i));
	}
}
int EnsCoord::taille() const {
    return ensCoord_.size();
}
bool EnsCoord::estVide() const {
    return ensCoord_.size() == 0;
}
Coord EnsCoord::ieme(int n) const {
    if (n >= taille()) {
        throw std::runtime_error("Position invalide");
    }
    return ensCoord_[n];
}

EnsCoord& EnsCoord::operator=(const EnsCoord& ensCoord) {
	// D'abord vidons ensCoord_
    // il faut noter que on doit supprimer l'élément à la fin (this->taille() - 1) du std::vector<Coord> afin que std::erase() prenne un temps constant.
	while (this->taille() > 0) {
		this->supprime(this->ieme(this->taille() - 1));
	}
	// Ajoutons tous les nouveaux éléments à ensCoord_
	for (size_t i = 0; i < ensCoord.taille(); i++) {
		this->ajoute(ensCoord.ieme(i));
	}
	// en gros &ensCoord_
	return *this;
}
bool EnsCoord::operator==(const EnsCoord& autre) const {
	// Si la taille de deux EnsCoord n'est pas égale, il n'est pas nécessaire de comparer chaque élément.
    if (this->taille() != autre.taille()) {
		return false;
	}
	// Dès qu'un élément n'est pas égal à un autre à la même position i, il renvoie immédiatement false.
	int n = this->taille();
	for (int i = 0; i < n; i++) {
		if (this->ieme(i) != autre.ieme(i)) {
			return false;
		}
	}
	// Sinon renvoie true
	return true;
}
// Fonctions utilitaires
EnsCoord voisines(const Coord& coord) {
	// l'algo pour cette fonction est dans le TP
	int tailleGrille = REGLE_SIMULATION["TAILLE_GRILLE"];
    int iMin = std::max(coord.lig() - 1, 0);
    int iMax = std::min(coord.lig() + 1, tailleGrille - 1);
    int jMin = std::max(coord.col() - 1, 0);
    int jMax = std::min(coord.col() + 1, tailleGrille - 1);

    EnsCoord ensCoord;
    for (int i = iMin; i <= iMax; i++) {
        for (int j = jMin; j <= jMax; j++) {
            Coord tmpCoord = Coord(i,j);
            if (tmpCoord != coord) {
                ensCoord.ajoute(tmpCoord);
            }
        }
    }
    return ensCoord;
}

EnsCoord voisinesNid(const EnsCoord& ensCoord, int rayon) {
	// Essentiellement le même algo que voisines()
	// Cependant, ceci est pour le nid (qui est de 4 blocs) et on peut spécifier le rayon des cellules voisines. Ex:
	// o o o o o o
	// o o o o o o
	// o n n o o o
    // o n n o o o 
	// o o o o o o
	//
	// Donc les voisines du Nid (les 'n') si rayon == 1 sont les 'x':
	// o o o o o o
	// x x x x o o
	// x n n x o o
    // x n n x o o 
	// x x x x o o
	// 
	// Si rayon == 2 donc les voisines sont:
	// x x x x x o
	// x x x x x o
	// x n n x x o
    // x n n x x o 
	// x x x x x o

	int tailleGrille = REGLE_SIMULATION["TAILLE_GRILLE"];
	Coord topLeft = ensCoord.ieme(0);
	int iMin = std::max(topLeft.lig() - rayon, 0);
	int iMax = std::min(topLeft.lig() + rayon + 1, tailleGrille - 1);
	int jMin = std::max(topLeft.col() - rayon, 0);
	int jMax = std::min(topLeft.col() + rayon + 1, tailleGrille - 1);

	EnsCoord ans;
	for (int i = iMin; i <= iMax; i++) {
		for (int j = jMin; j <= jMax; j++) {
			Coord tmpCoord = Coord(i,j);
			if (!ensCoord.contient(tmpCoord)) {
				ans.ajoute(tmpCoord);
			}
		}
	}
	return ans;
}

int numRand(int minVal, int maxVal) {
	std::uniform_int_distribution<int> distribution(minVal, maxVal);
    return distribution(rng);
}
Coord choixHasard(const EnsCoord& ensCoord) {
    return ensCoord.ensCoord()[numRand(0,ensCoord.taille()-1)];
}
void EnsCoord::shuffle() {
	// Mélange EnsCoord en utilisant std::shuffle
    std::shuffle(ensCoord_.begin(), ensCoord_.end(), rng);
}
// Supprime un Coord spécifique de std::vector<Coord> (pas EnsCoord)
void enleveEle(Coord c, std::vector<Coord>& vec) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] == c) {
			vec.erase(vec.begin()+i);
			return;
		}
	}
}
