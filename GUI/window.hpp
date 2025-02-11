#ifndef WINDOW_H
#define WINDOW_H

#include <gtkmm.h>
#include "../backend/grille.hpp"
#include <string>

enum Scene { SETTINGS_MENU, SIMULATION, MAIN_MENU };

struct RGB {
	float r, g, b;
	RGB(float r_, float g_, float b_) {
		r = r_ / 255.0;
		g = g_ / 255.0;
		b = b_ / 255.0;
	}
};
RGB getRGB(const Place& p);
float getOpacity(Fourmi* f);
int bestColID(const Place& p);
void generateCSS();

class MainWindow;
class Simulation;
class SimulationArea: public Gtk::DrawingArea {
	public:
		SimulationArea(Simulation* simulationInfoArea);
		virtual ~SimulationArea();
		void initSimulation();

		Grille simulation;
		int curIteration;
		void resumeSimulation() { simulationStopped = !simulationStopped; }
	
	protected:
		void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);	
		bool on_timeout(Simulation* simulationInfoArea);

		void draw_square(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a = 1.0);
		void draw_circle(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a = 1.0);
		void draw_triangle(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a = 1.0);

		void draw_object(const Cairo::RefPtr<Cairo::Context>& cr, const Place& p);

	private:
		bool simulationStopped = false;
		Simulation* simulationInfoArea;	
};

class SettingsMenu: public Gtk::Box {
	public:
		SettingsMenu(Gtk::Orientation orientation, int spacing, MainWindow* mainWindow);
		virtual ~SettingsMenu();
	protected:
		Gtk::Grid layout;

		Gtk::Button buttonGoToMainMenu;
		Gtk::Label settingsMenuTitle;

		Gtk::Label lb_TAILLE_GRILLE;
		Gtk::Label lb_NB_COLONIES;
		Gtk::Label lb_NB_FOURMIS_PAR_COLONIE;
		Gtk::Label lb_POURCENTAGE_FOURMIS_OUVRIERES;
		Gtk::Label lb_POURCENTAGE_FOURMIS_REPRODUCTRICE;
		Gtk::Label lb_POURCENTAGE_FOURMIS_GEURRIERES;
		Gtk::Label lb_PUISSANCE_FOURMIS_OUVRIERES;
		Gtk::Label lb_PUISSANCE_FOURMIS_REPRODUCTRICE;
		Gtk::Label lb_PUISSANCE_FOURMIS_GEURRIERES;
		Gtk::Label lb_SEUIL_SUCRE;
		Gtk::Label lb_NB_MORCEAUX_SUCRE;
		Gtk::Label lb_QUANTITE_SUCRE_CHAQUE_MORCEAU;
		Gtk::Label lb_NB_ITERATIONS_SIMULATION;

		Gtk::SpinButton b_TAILLE_GRILLE;
		Gtk::SpinButton b_NB_COLONIES;
		Gtk::SpinButton b_NB_FOURMIS_PAR_COLONIE;
		Gtk::SpinButton b_POURCENTAGE_FOURMIS_OUVRIERES;
		Gtk::SpinButton b_POURCENTAGE_FOURMIS_REPRODUCTRICE;
		Gtk::SpinButton b_POURCENTAGE_FOURMIS_GEURRIERES;
		Gtk::SpinButton b_PUISSANCE_FOURMIS_OUVRIERES;
		Gtk::SpinButton b_PUISSANCE_FOURMIS_REPRODUCTRICE;
		Gtk::SpinButton b_PUISSANCE_FOURMIS_GEURRIERES;
		Gtk::SpinButton b_SEUIL_SUCRE;
		Gtk::SpinButton b_NB_MORCEAUX_SUCRE;
		Gtk::SpinButton b_QUANTITE_SUCRE_CHAQUE_MORCEAU;
		Gtk::SpinButton b_NB_ITERATIONS_SIMULATION;

		Gtk::SpinButton test;
	
		void updateSimulationRule(std::string rule, int val);	
};
class Simulation: public Gtk::Box {
	public:
		Simulation(Gtk::Orientation orientation, int spacing, MainWindow* mainWindow);
		virtual ~Simulation();
	
		void initSimulation();
		void onSimulationChanged();

	protected:
		Gtk::Box rightPanel;
		Gtk::Box buttonArea;
		Gtk::Box infoArea;
		Gtk::Label tourLabel;
		Gtk::Button buttonGoToMainMenu;
		Gtk::Button buttonResumeSimulation;
		Gtk::ScrolledWindow scrolledWindow;
		SimulationArea simulationArea;
		std::vector <Gtk::Frame> coloniesInfoFrame;
		std::vector <Gtk::Label> coloniesInfoLabel;

		Glib::RefPtr<Gtk::CssProvider> css_provider;
};
class MainMenu: public Gtk::Box {
	public:
		MainMenu(Gtk::Orientation orientation, int spacing,  MainWindow* mainWindow);
		virtual ~MainMenu();

	protected:
		Gtk::Button buttonGoToSimulation, buttonGoToSettingsMenu;
		Gtk::Label simulationTitle;
};
class MainWindow : public Gtk::Window {
	public:
  		MainWindow();
  		~MainWindow() override;

		void onButtonClickedChangeScene(Scene scene);
		void showLoadingDialog();
		void hideLoadingDialog();
	protected:
		
		Gtk::Stack stackScenesManager;
		SettingsMenu settingsMenuScene;
		Simulation simulationScene;
		MainMenu mainMenuScene;

};


#endif 
