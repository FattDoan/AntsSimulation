#ifndef FOURMI_HPP
#define FOURMI_HPP

#include "coord.hpp"


enum Caste { OUVRIERE, GUERRIERE, REPRODUCTRICE, ALEATOIRE }; // ALEATOIRE POUR METHODE DE FRAI

class IDFourmi {
	private:
		int colonieID_;
		int idx_;

	public:
		// Constructueur
		IDFourmi(int colonieID, int idx);
		
		// Accès
		int idx() const                   { return idx_; }
		int colonieID() const             { return colonieID_; }

		// Prédicats
		bool memeColonie(const IDFourmi& autre) const;
		
		// Modifications
		void set(int idx);
		
		// Surcharge l'opérateur
		bool operator==(const IDFourmi& autre) const;
};
class Fourmi {
    // Atributs
    private:
		Coord coord_;
        bool porteSucre_;
		int puissance_; 
    	Caste caste_;
		IDFourmi idFourmi_;

	// Methodes
    public:
        // Constructeur
        Fourmi(Coord coord, int colonieID, int idx, Caste caste); 
        
		// getters
		Coord coord() const         { return coord_; }
		bool porteSucre() const     { return porteSucre_; }
		int puissance() const       { return puissance_; }
		Caste caste() const         { return caste_; }
		IDFourmi idFourmi() const   { return idFourmi_; }
		
		// setters
		IDFourmi& idFourmi()        { return idFourmi_; }
		
		// Prédicats
		bool chercheSucre() const;
		bool rentreNid() const;	
		bool estVivante() const;                        
    	bool estMemeColonie(Fourmi& autre) const;

		// Modifications
        void metPuissance(int puissance);
		void engageCombat(Fourmi& autre);
		void prendSucre();
        void poseSucre();
		void deplace(Coord c);
};

class Colonie {
	// Atributs
    private:
        std::vector <Fourmi> fourmis_;
		EnsCoord nid_;
		int colonieID_;
		int stockSucre_;
	
		// pour GUI
		int nbFourmisMortes_ = 0;
		int nbFourmisNees_ = 0;
		int nbFourmisTuees_ = 0; 
	// Methodes
    public:
        // Constructeurs
		Colonie(int colonieID);	
		
		// getters
		const EnsCoord& nid() const				{ return nid_; }
		int colonieID() const					{ return colonieID_; }
		int stockSucre() const		   			{ return stockSucre_; }
		const Fourmi& fourmi(int idx) const		{ return fourmis_[idx]; } 
		int nbFourmisMortes() const             { return nbFourmisMortes_; }
		int nbFourmisNees() const               { return nbFourmisNees_; }
		int nbFourmisTuees() const              { return nbFourmisTuees_; }

		// setters	
		Fourmi& fourmi(int idx)					{ return fourmis_[idx]; } 

		// Accès
		int nbFourmis() const; 

		// Prédicats	
		bool assezDeSucre() const; 
		
		// Modifications
		void ajouteFourmi(Fourmi fourmi);	
		void retireFourmi(Fourmi fourmi);

		void setNbFourmisMortes(int val);
		void setNbFourmisNees(int val);
		void setNbFourmisTuees(int val);


		void retireFourmisMortes();
		void ajouteSucreAuNid();
		void diminueSucreDuNid();
		void poseNid(EnsCoord nid);
};

#endif
