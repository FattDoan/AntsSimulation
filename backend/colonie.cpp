#include "colonie.hpp"

// Chaque fourmi possède son propre IDFourmi qui nous permet d'accéder rapidement à n'importe quelle fourmi et à sa colonie à partir de la classe Grille.
// En effet, sans cela, pour accéder à une fourmi spécifique, il faut faire une boucle pour chaque colonie et comparer chaque fourmi (vraiment lent)
// Aussi, IDFourmi est également un moyen pour la classe Place d'identifier si une fourmi est actuellement sur cette cellule (Si IDFourmi == (-1,-1) ou non)
IDFourmi::IDFourmi(int colonieID, int idx): colonieID_(colonieID), idx_(idx) {}
	
void IDFourmi::set(int idx) {
	idx_ = idx;
}
bool IDFourmi::memeColonie(const IDFourmi& autre) const {
	return colonieID_ == autre.colonieID();
}
bool IDFourmi::operator==(const IDFourmi& autre) const {
	return idx_ == autre.idx() && colonieID_ == autre.colonieID();
}
// Classe Fourmi
Fourmi::Fourmi(Coord coord, int colonieID, int idx, Caste caste): coord_(coord), idFourmi_(colonieID, idx), caste_(caste), porteSucre_(false) {
	switch (caste_) {
		case Caste::REPRODUCTRICE:
			puissance_ = REGLE_SIMULATION["PUISSANCE_FOURMIS_REPRODUCTRICE"];
			break;

		case Caste::OUVRIERE:
			puissance_ = REGLE_SIMULATION["PUISSANCE_FOURMIS_OUVRIERES"];
			break;

		case Caste::GUERRIERE:
			puissance_ = REGLE_SIMULATION["PUISSANCE_FOURMIS_GEURRIERES"];
			break;

		default: // ALEATOIRE ?
			puissance_ = 0;
			break;
	}
}
bool Fourmi::estVivante() const {
	return puissance_ > 0;
}
bool Fourmi::estMemeColonie(Fourmi& autre) const {
	return this->idFourmi().memeColonie(autre.idFourmi());
}
bool Fourmi::chercheSucre() const {
	return porteSucre_ == false;
}
bool Fourmi::rentreNid() const {
	return porteSucre_ == true;
}
void Fourmi::metPuissance(int puissance) {
	puissance_ = puissance;
}
// En gros, la fourmi la plus faible mourra et la plus forte s'affaiblira 
// (sa nouvelle puissance sera égale à la différence de puissance entre lui et le plus faible).
void Fourmi::engageCombat(Fourmi& autre) {
	int diff = std::abs(this->puissance_ - autre.puissance());
	if (puissance_ > autre.puissance()) {
		puissance_ = diff;
		autre.metPuissance(0);
	}
	else {
		puissance_ = 0;
		autre.metPuissance(diff);
	}
}
void Fourmi::prendSucre(){
	porteSucre_ = true;
}
void Fourmi::poseSucre(){
    porteSucre_ = false;
}
void Fourmi::deplace(Coord c) {
	coord_ = c;
	
}

// class Colonie
Colonie::Colonie(int colonieID) {
	colonieID_ = colonieID;
	stockSucre_ = 0;
}

int Colonie::nbFourmis() const {
	return fourmis_.size();
}
bool Colonie::assezDeSucre() const {
	return stockSucre_ >= REGLE_SIMULATION["SEUIL_SUCRE"];
}
void Colonie::ajouteFourmi(Fourmi fourmi){
	fourmis_.push_back(fourmi);
}
void Colonie::retireFourmi(Fourmi fourmi) {
	int idx = fourmi.idFourmi().idx();
	fourmis_.erase(fourmis_.begin()+idx);
}

void Colonie::setNbFourmisMortes(int val) {
	nbFourmisMortes_ = val;
}
void Colonie::setNbFourmisNees(int val) {
	nbFourmisNees_ = val;
}
void Colonie::setNbFourmisTuees(int val) {
	nbFourmisTuees_ = val;
}

void Colonie::ajouteSucreAuNid() {
	stockSucre_++;
}
void Colonie::diminueSucreDuNid() {
	stockSucre_-= REGLE_SIMULATION["SEUIL_SUCRE"];
}
// Une manière efficace d'éliminer les fourmis mortes
// Lorsque la puissance d'une fourmi est tombée à 0, on ne doit pas la retirer immédiatement de la colonie LORS D'UNE ITERATION, parce que
// 1) retirer un élément quelconque de std::vector est lent O(n)
// 2) s'il y a plusieurs fourmis mortes en même temps, non seulement (1) s'aggrave, mais nous avons réidentifié d'autres fourmis
// (mettre à jour IDFourmi pour chaque fourmi de la colonie entière et les synchroniser avec Grille) ce qui n'est pas simple.
//
// Cependant, lors d'une iteration (tour), 
// la fourmi avec 0 ne sera tout simplement pas autorisée à prendre une décision (se déplacer, prendre du sucre, etc).
// Puis, une fois l'itération terminée (toutes les fourmis ont pris une décision), 
// il suffit de passer pour chaque colonie, toute fourmi ayant une puissance de 0 est retirée 
// et on met à jour IDFourmi des autres fourmis vivantes en conséquence
//
// On doit aussi mettre à jour et synchroniser IDFourmis avec Grille (std::vector <Place>)
// Cependant, la fonction Grille::remetFourmisEtPheroSucre() s'en charge pour nous 
void Colonie::retireFourmisMortes() {
    // Algo:
	// Faire un std::vector<Fourmi> auxilaire
    // Toute fourmi vivante est placée dans ce vecteur
    // Enfin, il suffit de copier ce vecteur
	
	std::vector<Fourmi> tmpFourmis;
	int n = 0;
    for (int i = 0; i < fourmis_.size(); i++) {
        if (fourmis_[i].estVivante()) {
			// Mise à jour IDFourmi 
			// n toujours <= i
			fourmis_[i].idFourmi().set(n++);

			tmpFourmis.push_back(fourmis_[i]);
        } 
    }
    fourmis_ = tmpFourmis;
}
void Colonie::poseNid(EnsCoord nid) {
	nid_ = nid;
}


