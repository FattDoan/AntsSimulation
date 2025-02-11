#ifndef COORD_HPP
#define COORD_HPP

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <random>

// Variable globale indiquant les règles de simulation
extern std::map<std::string, size_t> REGLE_SIMULATION;

// Variable globale pour générateur de hasard (C++11)
extern std::random_device rd;
extern std::mt19937 rng;

class Coord {
    // Attributs
    private:
        int lig_, col_;
    // Methodes
    public:

		// Constructeurs
        Coord(int lig, int col);
		Coord(const Coord& c);
        // getter
        int lig() const { return lig_; }
        int col() const { return col_; }
        
		// Surcharge des opérateurs
        bool operator==(const Coord& autre) const;
        bool operator!=(const Coord& autre) const;
		Coord& operator=(const Coord& coord);
		friend std::ostream& operator<<(std::ostream& out, const Coord& coord);
};


class EnsCoord {
    // Attributs
    private:
        std::vector <Coord> ensCoord_;

        // Methode privee
        int position(const Coord& coord) const;
    // Methodes
    public:
        
		//Constructeurs
        EnsCoord() {};
        EnsCoord(const std::vector <Coord>& ensCoord);
        
		// getter
        const std::vector <Coord>& ensCoord() const { return ensCoord_; }
        
		// Accès
		int taille() const;
		Coord ieme(int n) const;
		
		// Prédicats 
        bool contient(const Coord& coord) const;
       	bool estVide() const;
 
		// Modifications
		void ajoute(const Coord& coord);
        void supprime(const Coord& coord);
        void ajouteEnsCoord(const EnsCoord& autre);
		void supprimeEnsCoord(const EnsCoord& autre);
		void shuffle();

		// Surcharge de l'opérateur
		bool operator==(const EnsCoord& autre) const;
		EnsCoord& operator=(const EnsCoord& coord);
		friend std::ostream& operator<<(std::ostream& out, const EnsCoord& ensCoord);
};


// Fonctions utilitaires
EnsCoord voisines(const Coord& coord);
EnsCoord voisinesNid(const EnsCoord& ensCoord, int rayon = 1);
int numRand(int minVal, int maxVal);
Coord choixHasard(const EnsCoord& ensCoord);
void enleveEle(Coord c, std::vector<Coord>& vec);

#endif
