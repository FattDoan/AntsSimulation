#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../backend/coord.hpp"

#include <stdexcept>
#include <sstream>

//================================================================//
TEST_SUITE_BEGIN("Class Coord");

TEST_CASE("Constructeur et opérations") {
    int tailleGrille = REGLE_SIMULATION["TAILLE_GRILLE"];
	// Test opérateur ==
    CHECK(Coord(3,2) == Coord(3,2));
    CHECK_THROWS_AS(Coord(-1,2), std::runtime_error);
    
    // Test opérateur !=
    CHECK(Coord(3,0) != Coord{0,0});
    CHECK_THROWS_AS(Coord(4,tailleGrille), std::runtime_error);
}

TEST_CASE("Operateur <<") {
    std::ostringstream ch; 
    // ch.str() retourne la chaîne.
    // ch.str(s) remplace la chaîne par s.
    ch << Coord(1,2);
    CHECK(ch.str() == "(1,2)");
    ch.str(""); // remet ch à zéro (chaîne vide)
    ch << Coord(4,3);
    CHECK(ch.str() == "(4,3)");
    ch.str(""); // remet ch à zéro (chaîne vide)
    ch << Coord(10,1);
    CHECK(ch.str() == "(10,1)");
}

TEST_SUITE_END();
//================================================================//
TEST_SUITE_BEGIN("Class EnsCoord");

TEST_CASE("Constructeur par défaut") {
    EnsCoord ec1;
    CHECK(ec1.estVide());
    CHECK(ec1.taille() == 0); 
}
TEST_CASE("Constructeur avec paramètres") {
    Coord c1 = {1, 2};
    Coord c2 = {3, 4};
    std::vector<Coord> coords = {c1, c2};
    EnsCoord ec2(coords);
    CHECK(!ec2.estVide()); 
    CHECK(ec2.taille() == 2); 
}

TEST_CASE("Méthode contient") {
    Coord c1 = {1, 2};
    Coord c2 = {3, 4};
    std::vector<Coord> coords = {c1, c2};
    EnsCoord ec2(coords);
    CHECK(ec2.contient(c1)); 
    CHECK(!ec2.contient({5, 6})); 
}

TEST_CASE("Méthode ajoute") {
    Coord c1 = {1, 2};
    Coord c2 = {3, 4};
    std::vector<Coord> coords = {c1, c2};
    EnsCoord ec2(coords);
	Coord c3 = {5, 6};
    ec2.ajoute(c3);
    CHECK(ec2.taille() == 3); 
    CHECK(ec2.contient(c3)); 
}

TEST_CASE("Méthode supprime") {
    Coord c1 = {1, 2};
    Coord c2 = {3, 4};
    std::vector<Coord> coords = {c1, c2};
    EnsCoord ec2(coords);
    ec2.supprime(c1);
    CHECK(ec2.taille() == 1); 
    CHECK(!ec2.contient(c1)); 
}
TEST_CASE("Méthode ieme") {
    Coord c1 = {1, 2};
    Coord c2 = {3, 4};
    std::vector<Coord> coords = {c1, c2};
    EnsCoord ec2(coords);
    Coord c = ec2.ieme(1);
    CHECK(c == c2); 
}

TEST_SUITE_END();
//================================================================//
TEST_SUITE_BEGIN("Fonctions utilitaires dans coord.hpp");

TEST_CASE("voisines") {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
    // les voisines de (2,1) sont (1,0), (1,1), (1,2), (2,2), (3,2), (3,1), (3,0), (2,0).
    EnsCoord ev = voisines(Coord(2,1));
    CHECK(ev.taille() == 8);
    CHECK(ev.contient(Coord(1,0)));
    CHECK(ev.contient(Coord(1,1)));
    CHECK(ev.contient(Coord(1,2)));
    CHECK(ev.contient(Coord(2,2)));
    CHECK(ev.contient(Coord(3,2)));
    CHECK(ev.contient(Coord(3,1)));
    CHECK(ev.contient(Coord(3,0)));
    CHECK(ev.contient(Coord(2,0)));
    CHECK_FALSE(ev.contient(Coord(2,1)));
    CHECK_FALSE(ev.contient(Coord(0,0)));
    
    // les voisines de (0, 0) sont (0, 1), (1, 0), (1, 1)
    ev = voisines(Coord(0,0));
    CHECK(ev.taille() == 3);
    CHECK(ev.contient(Coord(0,1)));
    CHECK(ev.contient(Coord(1,0)));
    CHECK(ev.contient(Coord(1,1)));
    CHECK_FALSE(ev.contient(Coord(0,0)));
    CHECK_FALSE(ev.contient(Coord(2,0)));

    // les voisines de (n-2, n-1) sont (n-3, n-2), (n-3, n-1), (n-2, n-2), (n-1, n-2), (n-1, n-1).
    ev = voisines(Coord(n-2,n-1));
    CHECK(ev.taille() == 5);
    CHECK(ev.contient(Coord(n-3,n-2)));
    CHECK(ev.contient(Coord(n-3,n-1)));
    CHECK(ev.contient(Coord(n-2,n-2)));
    CHECK(ev.contient(Coord(n-1,n-2)));
    CHECK(ev.contient(Coord(n-1,n-1)));
    CHECK_FALSE(ev.contient(Coord(n-1,n-3)));
    CHECK_FALSE(ev.contient(Coord(n-2,n-3)));
    CHECK_FALSE(ev.contient(Coord(0,0)));
    CHECK_FALSE(ev.contient(Coord(n-3,n-3)));
}


TEST_CASE("voisinesNid") {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
    // les voisines du nid (1,1),(1,2),(2,1),(2,2) avec rayon == 1 sont (0,0), (0,1), (0,2), (0,3), (1,0), (1,3), (2,0), (2,3), (3,0), (3,1), (3,2), (3,3).
    EnsCoord nid(std::vector<Coord>{Coord(1,1),Coord(1,2),Coord(2,1),Coord(2,2)});
	EnsCoord eV = voisinesNid(nid);
	EnsCoord ans(std::vector<Coord>{Coord(0,0), Coord(0,1), Coord(0,2), Coord(0,3),
								    Coord(1,0), Coord(1,3),
									Coord(2,0), Coord(2,3),
									Coord(3,0), Coord(3,1), Coord(3,2), Coord(3,3)});
	CHECK(eV.taille() == ans.taille());
	for (int i = 0; i < eV.taille(); i++) {
		CHECK(ans.contient(eV.ieme(i)));
	}
}


TEST_CASE("choixHasard") {
    EnsCoord ev;
    Coord c = Coord(0,0);
    ev = voisines(Coord(2,1));
    c = choixHasard(ev);
    CHECK(ev.contient(c));
    CHECK_FALSE(ev.contient(Coord(4,4)));
    
    ev = voisines(Coord(0,0));
    c = choixHasard(ev);
    CHECK(ev.contient(c));
    CHECK_FALSE(ev.contient(Coord(3,3)));

    ev = voisines(Coord(3,4));
    c = choixHasard(ev);
    CHECK(ev.contient(c));
    CHECK_FALSE(ev.contient(Coord(0,0)));
}

TEST_SUITE_END();
//================================================================//

// On n'a pas besoin main ici car on a deja DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
