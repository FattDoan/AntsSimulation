#include "grille.hpp"
#include <algorithm>

// Classe Place
Place::Place(const Coord& coord): coord_(coord), idFourmi_(-1,-1) {	
	pheroSucre_ = std::vector <int>(REGLE_SIMULATION["NB_COLONIES"], 0);
	pheroNid_ = std::vector<float>(REGLE_SIMULATION["NB_COLONIES"], 0);
	quantiteSucre_ = 0;
}
bool Place::contientSucre() const {
    return quantiteSucre_ > 0;
}
// Si pheroNid_[colonieID] == 1 -> cette cellule contient Nid de colonie avec colonieID
bool Place::contientNid(int colonieID) const {
	if (colonieID == -1) {
		int n = REGLE_SIMULATION["NB_COLONIES"];
		for (int i = 0; i < n; i++) {
			if (pheroNid_[i] == 1) return true;
		}
		return false;
	}
    return pheroNid_[colonieID] == 1;
}
bool Place::contientFourmi() const {
	return !(idFourmi_ == IDFourmi(-1,-1));
}
// Si !contientFourmi && !contientSucre && !contientNid
bool Place::estVide() const {
	if (contientSucre()	|| contientFourmi()) return false;
	int n = REGLE_SIMULATION["NB_COLONIES"];
	for (int i = 0; i < n; i++) {
		if (contientNid(i)) return false;
	}
	return true;
}
bool Place::estSurUnePiste(int colonieID) const {
	return pheroSucre_[colonieID] > 0;
}
bool Place::estSurUnePisteAutreColonie(int colonieID) const {
	int n = REGLE_SIMULATION["NB_COLONIES"];
	for (size_t i = 0; i < n; i++) {
		if (colonieID != i && pheroSucre_[i] > 0) return true;
	}
	return false;
}
bool Place::plusProcheNid(const Place& autre) const {
	//autre contient fourmi
	if (!autre.contientFourmi()) return false;
	int colonieID = autre.idFourmi().colonieID();
	return pheroNid(colonieID) > autre.pheroNid(colonieID); 
}
bool Place::plusLoinNid(const Place& autre) const {
	if (!autre.contientFourmi()) return false;
	int colonieID = autre.idFourmi().colonieID();
	return pheroNid(colonieID) <= autre.pheroNid(colonieID); 
}
void Place::poseSucre() {
	quantiteSucre_++;
}
void Place::enleveSucre() {
	quantiteSucre_--;
}
void Place::poseNid(int colonieID) {
	pheroNid_[colonieID] = 1;
}
void Place::poseFourmi(Fourmi fourmi) {
	idFourmi_ = fourmi.idFourmi();
}
void Place::enleveFourmi() {
	idFourmi_ = IDFourmi(-1,-1);
}
void Place::posePheroNid(float intensite, int colonieID_) {
	pheroNid_[colonieID_] = intensite;
}
void Place::posePheroSucre(int colonieID) {
	pheroSucre_[colonieID] = 255;
}
void Place::diminuePheroSucre(int colonieID) {
	pheroSucre_[colonieID] = std::max(pheroSucre_[colonieID] - 5, 0);
}
void Place::initSucre() {
	quantiteSucre_ = REGLE_SIMULATION["QUANTITE_SUCRE_CHAQUE_MORCEAU"];
}
void Place::misAJourFourmi(IDFourmi idFourmi) {
	idFourmi_ = idFourmi;
}
// class Grille
void Grille::deplaceFourmi(Fourmi& fourmi, Place& place1, Place& place2) {
	// safe guard
	if (fourmi.idFourmi() == place1.idFourmi()) {
		place1.enleveFourmi();
		place2.poseFourmi(fourmi);
		fourmi.deplace(place2.coord());
	}
}
// renvoie True si il y a au moins 2 fourmis dont 
// 1) caste est reproductrice
// 2) la position est la même que celle du nid de leur colonie (une des 4 cellules)
bool Grille::assezFourmisProductriceAuNid(int colonieID) const {
	EnsCoord ensC = colonies_[colonieID].nid();
	int nbFourmis = 0;
	for (size_t i = 0; i < ensC.taille(); i++) {
		Coord c = ensC.ieme(i);
		Place p = this->place(c);
		if (!p.contientFourmi()) continue;
		
		Fourmi f = this->fourmi(p.idFourmi());
		if (f.caste() == Caste::REPRODUCTRICE) {
			nbFourmis++;
		}
	}
	return nbFourmis >= 2;
}
// Regle pour fourmis reproductrice: 0 -> 8 (regles 0, 1 et 2 sont pour reproduction)
// Dès qu'il y a assez de sucre, toutes les fourmis reproductrices essaient de retourner au nid le plus vite possible.
// Les deux premières fourmis se reproduiront et les autres fourmis retourneront à leur travail normal.
//
// Regle pour fourmis ouvrieres :    3 -> 8
// Regle pour fourmis geurrieres:    9 -> 11
bool Grille::conditionsSatisfaites(int regleID, const Place& place1, const Place& place2) const {
	if (!place1.contientFourmi()) return false;
	IDFourmi idFourmi = place1.idFourmi();
	int colonieID = idFourmi.colonieID();
	Fourmi f = fourmi(idFourmi);
	switch (regleID) {
		case 0:
			return colonies_[colonieID].assezDeSucre() && assezFourmisProductriceAuNid(colonieID) && place1.contientNid(colonieID); 
			break;

		case 1:
			return colonies_[colonieID].assezDeSucre() && !assezFourmisProductriceAuNid(colonieID) && place1.contientNid(colonieID);
			break;

		case 2:
			return colonies_[colonieID].assezDeSucre() && !assezFourmisProductriceAuNid(colonieID) && place2.plusProcheNid(place1) 
				&& (place2.estVide() || (!place2.contientFourmi() && place2.contientNid()));
			break;

		case 3:
			return f.chercheSucre() && place2.contientSucre();
			break;
		
		case 4:
			return f.rentreNid() && place2.contientNid(colonieID);
			break;

		case 5:
			return f.rentreNid() && place2.estVide() && place2.plusProcheNid(place1);
			break;
		
		case 6:
			return f.chercheSucre() && place1.estSurUnePiste(colonieID) && place2.estVide() && place2.plusLoinNid(place1) && place2.estSurUnePiste(colonieID);
			break;
		
		case 7:
			return f.chercheSucre() && place2.estSurUnePiste(colonieID) && place2.estVide();
			break;

		case 8:
			return f.chercheSucre() && place2.estVide();
			break;
		
		case 9: {
			if (!place2.contientFourmi()) return false;
			Fourmi autre = fourmi(place2.idFourmi());
			return !f.estMemeColonie(autre);
			break;
		}
		case 10:
			return place2.estSurUnePisteAutreColonie(colonieID) && place2.estVide();
			break;

		case 11:
			return place2.estVide();
			break;
	}
	return false;
}

void Grille::appliqueRegle(int regleID, Place& place1, Place& place2) {
	IDFourmi idFourmi = place1.idFourmi();
	int colonieID = idFourmi.colonieID();
	Fourmi& f = fourmi(idFourmi);

	switch (regleID) {	
		case 0: {
			int rayon = getRayon(colonies_[colonieID].nid(), REGLE_SIMULATION["NB_FOURMIS_PAR_COLONIE"]);
            EnsCoord ensC = voisinesNid(colonies_[colonieID].nid(), rayon);
			ajouteFourmi(colonieID, ensC, Caste::ALEATOIRE);
			colonies_[colonieID].diminueSucreDuNid();
			colonies_[colonieID].setNbFourmisNees(colonies_[colonieID].nbFourmisNees() + 1);
			break;
		}
		case 1:
			// Ne rien faire (attendre) jusqu'à ce que suffisamment de fourmis reviennent au nid pour se reproduire
			break;
		case 2: {
			deplaceFourmi(f, place1, place2);
			break;
		}
		case 3: {
			place2.enleveSucre();
			f.prendSucre();
			place1.posePheroSucre(colonieID);
			break;
		}
		
		case 4: {
			f.poseSucre();
			colonies_[colonieID].ajouteSucreAuNid();
			break;
		}

		case 5: {
			deplaceFourmi(f, place1, place2);
			place2.posePheroSucre(colonieID);
			break;
		}
		
		case 6: case 7: case 8: {
			deplaceFourmi(f, place1, place2);
			break;
		}

		case 9: {
			Fourmi& autre = fourmi(place2.idFourmi());
			f.engageCombat(autre);
	
			int colonieID1 = f.idFourmi().colonieID();
			int colonieID2 = autre.idFourmi().colonieID();
			if (!f.estVivante()) {
				colonies_[colonieID1].setNbFourmisMortes(colonies_[colonieID1].nbFourmisMortes() + 1);
				colonies_[colonieID2].setNbFourmisTuees(colonies_[colonieID2].nbFourmisTuees() + 1);
			}
			if (!autre.estVivante()) {
				colonies_[colonieID2].setNbFourmisMortes(colonies_[colonieID2].nbFourmisMortes() + 1);
				colonies_[colonieID1].setNbFourmisTuees(colonies_[colonieID1].nbFourmisTuees() + 1);
			}
			break;
		}

		case 10: case 11: {
			deplaceFourmi(f, place1, place2);
			break;
		}
	}
}

void Grille::ajouteFourmi(int colonieID, EnsCoord& fraiEnsCoord, Caste caste) {
	EnsCoord nid = colonies_[colonieID].nid();
	if (caste == Caste::ALEATOIRE) {
		caste = static_cast<Caste>(numRand(int(Caste::OUVRIERE), int(Caste::REPRODUCTRICE)));
	}
    for (size_t i = 0; i < fraiEnsCoord.taille(); i++) {
        Coord coord = fraiEnsCoord.ieme(i);
        if (this->place(coord).estVide()) {
            Fourmi f(coord, colonieID, colonies_[colonieID].nbFourmis(), caste);
            this->place(coord).poseFourmi(f);
            colonies_[colonieID].ajouteFourmi(f);
	        return;
        }
    }
}

std::vector <int> regleReproductrice{0, 1, 2, 3, 4, 5, 6, 7, 8};
std::vector <int> regleOuvriere{3, 4, 5, 6, 7, 8};
std::vector <int> regleGeurriere{9, 10, 11};
void Grille::remetFourmisEtPheroSucre() {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
	int nC = REGLE_SIMULATION["NB_COLONIES"];
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			for (int colonieID = 0; colonieID < nC; colonieID++) {
				grille_[i][j].diminuePheroSucre(colonieID);
			}
			grille_[i][j].enleveFourmi();
		}
	}
	for (int colonieID = 0; colonieID < nC; colonieID++) {
		colonies_[colonieID].retireFourmisMortes();
		for (int i = 0; i < colonies_[colonieID].nbFourmis(); i++) {
			IDFourmi idFourmi(colonieID, i);
			Coord c = this->fourmi(idFourmi).coord();
			this->place(c).misAJourFourmi(idFourmi);
		}
	}

}

// Boucle pour chaque règle d'abord, puis pour la direction 
// -> de cette façon, la fourmi trouvera le meilleur chemin à suivre en fonction de ses priorités.
// Aussi, mélanger voisinesCoords pour randomiser leur prochaine direction.
void Grille::decisionFourmi(Fourmi& fourmi) {
	std::vector <int> regle;
	switch (fourmi.caste()) {
		case Caste::REPRODUCTRICE:
			regle = regleReproductrice;
			break;
		case Caste::OUVRIERE:
			regle = regleOuvriere;
			break;
		case Caste::GUERRIERE:
			regle = regleGeurriere;
			break;
		default:
			break;
	}
	Coord coord = fourmi.coord();
	EnsCoord voisinesCoords = voisines(coord);
	voisinesCoords.shuffle();

	Place& place1 = this->place(coord);
	for (int rID: regle) {
		for (size_t i = 0; i < voisinesCoords.taille(); i++) {
			Place& place2 = this->place(voisinesCoords.ieme(i));
			if (conditionsSatisfaites(rID, place1, place2)) {
				appliqueRegle(rID, place1, place2);
				return;
			}
		}
	}
}
// Algo général pour la simulation :

// Par itération :
// + Pour chaque colonie, pour chaque fourmi, prendre la décision en fonction des cellules environnantes
// + Réduire les pheros sucre des fourmis et retirer toutes les fourmis mortes.

void Grille::iterationSuivante() {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
	int nC = REGLE_SIMULATION["NB_COLONIES"];
	// O(nC * nbFourmis * nbRegles * 10) au pire pour chaque iteration
	remetFourmisEtPheroSucre();
	for (size_t colonieID = 0; colonieID < nC; colonieID++) {
		for (size_t i = 0; i < colonies_[colonieID].nbFourmis(); i++) {
			IDFourmi idFourmi(colonieID, i);
			Fourmi& f = this->fourmi(idFourmi);
			decisionFourmi(f);
		}
	}
}

Coord Grille::getRandCoord() {
	// placesVides_ is a vector containing Coord of empty cells
	// placesVides_ is supposed to be shuffled beforehand
	Coord coord = placesVides_.back();
	placesVides_.pop_back();
	return coord;
}

void Grille::getNidPos(int colonieID) {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
	Coord c(0,0);
	int i, j;
	while (true) {
		c = getRandCoord();
		i = c.lig(); j = c.col();
		if (i + 1 >= n || j + 1 >= n) {
			placesVides_.insert(placesVides_.begin(), c);
			continue;
		}
		if (!grille_[i][j+1].estVide() || !grille_[i+1][j].estVide() || !grille_[i+1][j+1].estVide()) {
			placesVides_.insert(placesVides_.begin(), c);
			continue;
		}
		break;
	}

	EnsCoord nid;
	nid.ajoute(c);
	grille_[i][j].poseNid(colonieID);
	
	nid.ajoute(Coord(i, j+1)); grille_[i][j+1].poseNid(colonieID); 
	enleveEle(Coord(i,j+1), placesVides_);
	
	nid.ajoute(Coord(i+1, j)); grille_[i+1][j].poseNid(colonieID);
	enleveEle(Coord(i+1,j), placesVides_);
	
	nid.ajoute(Coord(i+1, j+1)); grille_[i+1][j+1].poseNid(colonieID); 
	enleveEle(Coord(i+1,j+1), placesVides_);

	colonies_[colonieID].poseNid(nid);

}

float Grille::maxPheroNidVoisines(const Coord& coord, int colonieID) const {
	EnsCoord ensCoord = voisines(coord);
	float ans = 0;
	for (size_t i = 0; i < ensCoord.taille(); i++) {
		Coord c = ensCoord.ieme(i);
		ans = std::max(ans, grille_[c.lig()][c.col()].pheroNid(colonieID));
	}
	return ans;
}
Grille::Grille() {}

void Grille::initSimulation() {
	int nbColonies = REGLE_SIMULATION["NB_COLONIES"];
	int tailleGrille = REGLE_SIMULATION["TAILLE_GRILLE"];
	int nbFourmis = REGLE_SIMULATION["NB_FOURMIS_PAR_COLONIE"];
	int nbFourmisOuvrieres = int(nbFourmis * REGLE_SIMULATION["POURCENTAGE_FOURMIS_OUVRIERES"]/100);
	int nbFourmisReproductrices = int(nbFourmis * REGLE_SIMULATION["POURCENTAGE_FOURMIS_REPRODUCTRICE"]/100);
	int nbFourmisGeurrieres = int(nbFourmis * REGLE_SIMULATION["POURCENTAGE_FOURMIS_GEURRIERES"]/100);
	int nbMorceauxSucre = REGLE_SIMULATION["NB_MORCEAUX_SUCRE"];
	
	//init
	grille_.clear();
	colonies_.clear();
	placesVides_.clear();	
	for (size_t i = 0; i < tailleGrille; i++) {
		grille_.push_back(std::vector<Place>());
		for (size_t j = 0; j < tailleGrille; j++) {
			Place p(Coord(i,j));
			grille_[i].push_back(p);
			placesVides_.push_back(Coord(i,j));
		}
	}
	std::shuffle(placesVides_.begin(), placesVides_.end(), rng);
	for (size_t i = 0; i < nbColonies; i++) {
		Colonie c(i);
		colonies_.push_back(c);
	}

	std::cout << "Initialise nest positions\n";
	for (int i = 0; i < nbColonies; i++) {
		getNidPos(i);
	}

	std::cout << "Initialise nest phero\n";
	// La complexité temporelle pour l'initialisation du nid phero de l'algo de l'indication du projet est O(N^3 * 10 * nbColonies).
	// On crois que on peux faire mieux avec O(N^2 * 4 * nbColonies) avec des pheros de nids presque identiques.
	// Algo:
	// Pour chaque Coord(x,y) calculer la distance entre lui et le nid
	// par la formule dis = max( |nid.x - x|, |nid.y - y| )
	// Comme le nid est constitué de 4 blocs, on doit passer par ces 4 cellules pour trouver la meilleure distance.
	// Enfin, le phero du nid à cette position est 1 - dis/tailleGrille
	int maxDis = tailleGrille;
	for (int colonieID = 0; colonieID < nbColonies; colonieID++) {
		EnsCoord nid = colonies_[colonieID].nid();
		for (int i = 0; i < tailleGrille; i++) {
			for (int j = 0; j < tailleGrille; j++) {
				if (grille_[i][j].pheroNid(colonieID) == 1.0) continue;
				int bestDis = maxDis;
				for (int t = 0; t < 4; t++) {
					Coord c = nid.ieme(t);
					int dis = std::max(std::abs(i - c.lig()), std::abs(j - c.col()));
					bestDis = std::min(bestDis, dis);
				}
				grille_[i][j].posePheroNid(1.0 - float(bestDis)/tailleGrille, colonieID);
			}
		}	
	}	

	std::cout << "Spawning sugar\n";
	// randomized sugar location
	for (int i = 0; i < nbMorceauxSucre; i++) {
		Coord c = getRandCoord();
		this->place(c).initSucre();
	}

	std::cout << "Spawning ants\n";
	for (int colonieID = 0; colonieID < nbColonies; colonieID++) {
        int rayon = getRayon(colonies_[colonieID].nid(), nbFourmis);
		EnsCoord fraiEnsCoord = voisinesNid(colonies_[colonieID].nid(), rayon);
		fraiEnsCoord.shuffle();
        for (int i = 0; i < nbFourmis; i++) {
            Caste caste;
            if (i < nbFourmisReproductrices) {
                caste = Caste::REPRODUCTRICE;
            }
            else if (i < nbFourmisReproductrices + nbFourmisOuvrieres) {
                caste = Caste::OUVRIERE;
            }
            else {
                caste = Caste::GUERRIERE;
            }
            Coord coord = fraiEnsCoord.ieme(i);
            Fourmi f(coord, colonieID, colonies_[colonieID].nbFourmis(), caste);
            this->place(coord).poseFourmi(f);
            colonies_[colonieID].ajouteFourmi(f); 
        }
        std::cout << "Finished spawning ants for colony " << colonieID << "\n";
	}
}
// Binary search
int Grille::getRayon(EnsCoord nid, int nbFourmis) {
    int l = 0, r = REGLE_SIMULATION["TAILLE_GRILLE"];
    int res = r;
    while (l <= r) {
        int m = l + (r - l)/2;
        EnsCoord fraiEnsCoord = voisinesNid(nid, m);
        if (nbPlacesVides(fraiEnsCoord) < 2 * nbFourmis) {
            l = m + 1;
        } else {
            res = std::min(res, m);
            r = m - 1;
        }
    }
    return res;
}	
int Grille::nbPlacesVides(EnsCoord ensC) {
    int cnt = 0;
    for (int i = 0; i < ensC.taille(); i++) {
        if (this->place(ensC.ieme(i)).estVide()) {
            cnt++;
        }
    } 
    return cnt;
}
int Grille::nbFourmis(int colonieID, Caste caste) {
	int cnt = 0;
	for (int i = 0; i < colonies_[colonieID].nbFourmis(); i++) {
		IDFourmi idFourmi(colonieID, i);
		if (this->fourmi(idFourmi).caste() == caste) {
			cnt++;
		}
	}
	return cnt;
}
