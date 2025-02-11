#ifndef GRILLE_HPP
#define GRILLE_HPP

#include "colonie.hpp"

class Place {
    // Attributs
    private:
        Coord coord_;
        IDFourmi idFourmi_; 
		std::vector <int> pheroSucre_;
		std::vector <float> pheroNid_;
		int quantiteSucre_;
	// Methodes
    public:
        // Constructeur
        Place(const Coord& coord_);
   
		// getter fonctions 
        const Coord& coord() const          	{ return coord_; }
		const IDFourmi& idFourmi() const    	{ return idFourmi_; }
        int pheroSucre(int colonieID) const 	{ return pheroSucre_[colonieID]; }
        float pheroNid(int colonieID) const 	{ return pheroNid_[colonieID]; }
		int quantiteSucre() const           	{ return quantiteSucre_; }
		
		// Accès

		// Prédicats
		bool contientSucre() const;
        bool contientNid(int colonieID = -1) const;
		bool contientFourmi() const;
        bool estVide() const;
		bool estSurUnePiste(int colonieID) const;
		bool estSurUnePisteAutreColonie(int colonieID) const;
		bool plusProcheNid(const Place& autre) const;
		bool plusLoinNid(const Place& autre) const;


		// Modifications
        void poseSucre();
        void enleveSucre();
        void poseNid(int colonieID);
        void poseFourmi(Fourmi fourmi);
        void enleveFourmi();
        void posePheroNid(float intensite, int colonieID_);
        void posePheroSucre(int colonieID_);
        void diminuePheroSucre(int colonieID_);
		void initSucre();
		void misAJourFourmi(IDFourmi idFourmi);

};


// THIS CLASS REPRESENTS THE GAME STATE
class Grille {
	// Attributs
	private:
		std::vector <std::vector <Place>> grille_;
		std::vector <Colonie> colonies_;
		
		// seulement pour init
		std::vector <Coord> placesVides_;  
	// Methodes
	public:
		// Constructeurs
		Grille(); // Initialiser le jeu selon la règle de simulation

		// getter
		const Place& place(Coord c) const                 { return grille_[c.lig()][c.col()]; }
		const Fourmi& fourmi(IDFourmi idFourmi) const     { return colonies_[idFourmi.colonieID()].fourmi(idFourmi.idx()); }
		const Colonie& colonie(int colonieID) const       { return colonies_[colonieID]; }

		// setters
		Fourmi& fourmi(IDFourmi idFourmi)                 { return colonies_[idFourmi.colonieID()].fourmi(idFourmi.idx()); }	
		Place& place(Coord c)                             { return grille_[c.lig()][c.col()]; }

		// Accès
		float maxPheroNidVoisines(const Coord& coord, int colonieID) const;
		int nbFourmis(int colonieID, Caste caste); // pour GUI

		// Prédicats
		bool assezFourmisProductriceAuNid(int colonieID) const;
		bool conditionsSatisfaites(int regleID, const Place& place1, const Place& place2) const;
		bool estVide(const Coord& coord) const;

		// Modifications
		void deplaceFourmi(Fourmi& fourmi, Place& place1, Place& place2); // On deplace fourmi sur place1 vers place2
		void appliqueRegle(int regleID, Place& place1, Place& place2);
		void ajouteFourmi(int colonieID, EnsCoord& fraiEnsCoord, Caste caste = Caste::ALEATOIRE);	
		void getNidPos(int colonieID);
		void decisionFourmi(Fourmi& fourmi);
		void remetFourmisEtPheroSucre();
		void iterationSuivante();
		void initSimulation();
		void retireFourmisMortes(int colonieID);

		Coord getRandCoord();
        int getRayon(EnsCoord nid, int nbFourmis);
        int nbPlacesVides(EnsCoord ensC);
};
#endif
