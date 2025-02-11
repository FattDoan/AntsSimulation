#include "window.hpp"
#include <string>
#include <vector>
#include <sstream>

enum COLORNAME {
	RED,
	BLUE,
	GREEN,
	ORANGE,
	YELLOW,
	LIGHTBLUE,
	PINK,
	WHITE,
	GRAY
};

std::vector <RGB> COLORS = {RGB(253,1,1),
						   RGB(0,51,204), 
						   RGB(12,189,24),
						   RGB(235,135,21), 	
						   RGB(136,0,253), 
						   RGB(222, 236, 0),
						   RGB(79,224,232),
                           RGB(240, 74, 174),
						   RGB(255, 255, 255),
                           RGB(151, 151, 151)};

bool simulationStarted = false; //Dirty flag
int CELL_SIZE = 16;
int SIMULATION_SIZE = 800;

// Must let last 2 colors are white and gray
int bestColID(const Place& p) {
	int nC = REGLE_SIMULATION["NB_COLONIES"];
	int bestID = 0;
	for (int colonieID = 1; colonieID < nC; colonieID++) {
		if (p.pheroSucre(colonieID) > p.pheroSucre(bestID)) {
			bestID = colonieID;
		}
	}
	return bestID;
}
RGB getRGB(const Place& p) {
	int n = COLORS.size();
	if (p.contientNid()) {
		return COLORS[n-1];	
	}
	else if (p.contientSucre()) {
		return COLORS[n-2];
	}
	else if (p.contientFourmi()) {
		IDFourmi idFourmi = p.idFourmi();
		return COLORS[idFourmi.colonieID()];
	}
	else {
		return COLORS[bestColID(p)];
	}
	return COLORS[n-1];
}
float getOpacity(Fourmi* f) {
	Caste caste = f->caste();
	int p = f->puissance();
	switch (caste) {
		case Caste::REPRODUCTRICE:
			return float(p)/REGLE_SIMULATION["PUISSANCE_FOURMIS_REPRODUCTRICE"];
			break;
		case Caste::OUVRIERE:
			return float(p)/REGLE_SIMULATION["PUISSANCE_FOURMIS_OUVRIERES"];
			break;
		case Caste::GUERRIERE:
			return float(p)/REGLE_SIMULATION["PUISSANCE_FOURMIS_GEURRIERES"];
			break;
		default:
			break;
	}
	return 1.0;
}
void generateCSS(Glib::RefPtr<Gtk::CssProvider> css_provider) {
	std::stringstream css;
	for (size_t i = 0; i < COLORS.size(); i++) {
		css << ".colored-frame-" << i << " { background-color: rgba("
			<< COLORS[i].r*255.0 << "," << COLORS[i].g*255.0 << "," << COLORS[i].b*255.0 << ",0.8); }\n";
	}
	css_provider->load_from_data(css.str());

}
SimulationArea::SimulationArea(Simulation* ptr) {
	simulationInfoArea = ptr;
	Glib::signal_timeout().connect( sigc::bind(sigc::mem_fun(*this, &SimulationArea::on_timeout), simulationInfoArea), 200 );
	set_draw_func(sigc::mem_fun(*this, &SimulationArea::on_draw));
}
SimulationArea::~SimulationArea() {}
void SimulationArea::draw_square(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a) {
	int x = c.lig(), y = c.col();
	cr->save();
	cr->set_source_rgba(color.r, color.g, color.b, a);
	cr->rectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
	cr->fill();
	cr->restore();
}
void SimulationArea::draw_circle(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a) {
	int x = c.lig(), y = c.col();
	int xc = x*CELL_SIZE + CELL_SIZE/2, yc = y*CELL_SIZE + CELL_SIZE/2;
	cr->save();
	cr->set_source_rgba(color.r, color.g, color.b, a);
	cr->arc(xc, yc, int(CELL_SIZE/2), 0.0, 2.0 * M_PI);
	cr->fill();
	cr->restore();
}
void SimulationArea::draw_triangle(const Cairo::RefPtr<Cairo::Context>& cr, const Coord& c, RGB color, float a) {
	int x = c.lig(), y = c.col();
	int tx = x * CELL_SIZE + CELL_SIZE/2, ty = y * CELL_SIZE;
	int lx = x * CELL_SIZE, ly = y * CELL_SIZE + CELL_SIZE;
	int rx = x * CELL_SIZE + CELL_SIZE, ry = y * CELL_SIZE + CELL_SIZE;
	cr->save();
	cr->set_source_rgba(color.r, color.g, color.b, a);
	
	cr->move_to(tx, ty);
	cr->line_to(lx, ly);
	cr->line_to(rx, ry);
	cr->line_to(tx, ty);
	
	cr->close_path();
	cr->fill();
	cr->restore();
}
void SimulationArea::draw_object(const Cairo::RefPtr<Cairo::Context>& cr, const Place& p) {
	if (p.contientNid()) {
	 	draw_square(cr, p.coord(), getRGB(p), 1.0);
	}
	else if (p.contientSucre()) {
		draw_square(cr, p.coord(), getRGB(p), 1.0);
	}
	else if (p.contientFourmi()) {
		IDFourmi idFourmi = p.idFourmi();
		Fourmi f = simulation.fourmi(idFourmi);
		switch (f.caste()) {
			case Caste::OUVRIERE:
				draw_square(cr, p.coord(), getRGB(p), 1.0);
				break;
			case Caste::REPRODUCTRICE:
				draw_circle(cr, p.coord(), getRGB(p), 1.0);
				break;
			case Caste::GUERRIERE:
				draw_triangle(cr, p.coord(), getRGB(p), 1.0);
				break;
			default:
				break;
		}
	}
	else {
	 	int id = bestColID(p);
		draw_square(cr, p.coord(), getRGB(p), 0.6 - ((255 - p.pheroSucre(id))*0.6)/(255.0));	
	}
	return;
}
void SimulationArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {	
	// Background

	cr->save();
	cr->set_source_rgba(0, 0, 0, 1);
	cr->rectangle(0, 0, SIMULATION_SIZE, SIMULATION_SIZE);
	cr->fill();
	cr->restore();

	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
	for (size_t i = 0; i < n; i++) {
		for (size_t j = 0; j < n; j++) {
			Coord c = Coord(i,j);
			draw_object(cr, simulation.place(c));
		}
	}
}
bool SimulationArea::on_timeout(Simulation* simulationInfoArea) {
	// force our program to redraw the entire simulation area
	queue_draw();
	if (simulationStarted && !simulationStopped && curIteration <= REGLE_SIMULATION["NB_ITERATIONS_SIMULATION"]) {
		simulation.iterationSuivante();
		simulationInfoArea->onSimulationChanged();
		curIteration++;
	}
	return true;
}
void SimulationArea::initSimulation() {
	simulation.initSimulation();
}
void SettingsMenu::updateSimulationRule(std::string rule, int val) { 
	REGLE_SIMULATION[rule] = val; 
}
const int buttonWidth = 200;
const int buttonHeight = 32;
SettingsMenu::SettingsMenu(Gtk::Orientation orientation, int spacing, MainWindow* mainWindow):
	Box(orientation, spacing),
	buttonGoToMainMenu("Back to main menu"),
	settingsMenuTitle("Settings"),
	lb_TAILLE_GRILLE("La taille de la grille"),
	lb_NB_COLONIES("Nombre de colonies"),
	lb_NB_FOURMIS_PAR_COLONIE("Nombre de fourmis par colonie"),
	lb_POURCENTAGE_FOURMIS_OUVRIERES("Pourcentage de fourmis ouvrieres"),
	lb_POURCENTAGE_FOURMIS_REPRODUCTRICE("Pourcentage de fourmis reproductrice"),
	lb_POURCENTAGE_FOURMIS_GEURRIERES("Pourcentage de fourmis geurrieres"),
	lb_PUISSANCE_FOURMIS_OUVRIERES("Puissance de fourmis ouvrieres"),
	lb_PUISSANCE_FOURMIS_REPRODUCTRICE("Puissance de fourmis reproductrice"),
	lb_PUISSANCE_FOURMIS_GEURRIERES("Puissance de fourmis geurrieres"),
	lb_SEUIL_SUCRE("Seuil de sucre pour la reproduction"),
	lb_NB_MORCEAUX_SUCRE("Nombre de morceaux de sucre"),
	lb_QUANTITE_SUCRE_CHAQUE_MORCEAU("Quantite sucre de chaque morceau"),
	lb_NB_ITERATIONS_SIMULATION("Nombre de iterations de simulation")
{

	// TODO 
	// https://gitlab.gnome.org/GNOME/gtkmm-documentation/-/issues/10
	// investigate why sigc::mem_fun doesnt work (the get_value() doens't update even though the display is correct)
	layout.set_row_spacing(32);
	layout.set_column_spacing(32);
	layout.set_margin_top(32);
    layout.set_margin_bottom(32);

	b_TAILLE_GRILLE.set_expand(false);
	b_TAILLE_GRILLE.set_size_request(buttonWidth, buttonHeight);

	b_NB_COLONIES.set_expand(false);
	b_NB_COLONIES.set_size_request(buttonWidth, buttonHeight);

	b_NB_FOURMIS_PAR_COLONIE.set_expand(false);
	b_NB_FOURMIS_PAR_COLONIE.set_size_request(buttonWidth, buttonHeight);

	b_POURCENTAGE_FOURMIS_OUVRIERES.set_expand(false);
	b_POURCENTAGE_FOURMIS_OUVRIERES.set_size_request(buttonWidth, buttonHeight);

	b_POURCENTAGE_FOURMIS_GEURRIERES.set_expand(false);
	b_POURCENTAGE_FOURMIS_GEURRIERES.set_size_request(buttonWidth, buttonHeight);

	b_POURCENTAGE_FOURMIS_REPRODUCTRICE.set_expand(false);
	b_POURCENTAGE_FOURMIS_REPRODUCTRICE.set_size_request(buttonWidth, buttonHeight);

	b_PUISSANCE_FOURMIS_OUVRIERES.set_expand(false);
	b_PUISSANCE_FOURMIS_OUVRIERES.set_size_request(buttonWidth, buttonHeight);

	b_PUISSANCE_FOURMIS_REPRODUCTRICE.set_expand(false);
	b_PUISSANCE_FOURMIS_REPRODUCTRICE.set_size_request(buttonWidth, buttonHeight);

	b_PUISSANCE_FOURMIS_GEURRIERES.set_expand(false);
	b_PUISSANCE_FOURMIS_GEURRIERES.set_size_request(buttonWidth, buttonHeight);

	b_SEUIL_SUCRE.set_expand(false);
	b_SEUIL_SUCRE.set_size_request(buttonWidth, buttonHeight);

	b_NB_MORCEAUX_SUCRE.set_expand(false);
	b_NB_MORCEAUX_SUCRE.set_size_request(buttonWidth, buttonHeight);

	b_QUANTITE_SUCRE_CHAQUE_MORCEAU.set_expand(false);
	b_QUANTITE_SUCRE_CHAQUE_MORCEAU.set_size_request(buttonWidth, buttonHeight);

	b_NB_ITERATIONS_SIMULATION.set_expand(false);
	b_NB_ITERATIONS_SIMULATION.set_size_request(buttonWidth, buttonHeight);


	b_TAILLE_GRILLE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["TAILLE_GRILLE"],10,1000), 1, 0);
	b_TAILLE_GRILLE.signal_value_changed().connect([this]() { updateSimulationRule("TAILLE_GRILLE", this->b_TAILLE_GRILLE.get_value()); });
	
	b_NB_COLONIES.configure(Gtk::Adjustment::create(REGLE_SIMULATION["NB_COLONIES"],1,8), 1, 0);
	b_NB_COLONIES.signal_value_changed().connect([this]() { updateSimulationRule("NB_COLONIES", this->b_NB_COLONIES.get_value()); });
	
	b_NB_FOURMIS_PAR_COLONIE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["NB_FOURMIS_PAR_COLONIE"],1,10000), 1, 0);
	b_NB_FOURMIS_PAR_COLONIE.signal_value_changed().connect([this]() { updateSimulationRule("NB_FOURMIS_PAR_COLONIE", this->b_NB_FOURMIS_PAR_COLONIE.get_value()); });
	
	b_POURCENTAGE_FOURMIS_OUVRIERES.configure(Gtk::Adjustment::create(REGLE_SIMULATION["POURCENTAGE_FOURMIS_OUVRIERES"],0,100), 1, 0);
	b_POURCENTAGE_FOURMIS_OUVRIERES.signal_value_changed().connect([this]() { updateSimulationRule("POURCENTAGE_FOURMIS_OUVRIERES", this->b_POURCENTAGE_FOURMIS_OUVRIERES.get_value()); });
	
	b_POURCENTAGE_FOURMIS_REPRODUCTRICE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["POURCENTAGE_FOURMIS_REPRODUCTRICE"],0,100), 1, 0);
	b_POURCENTAGE_FOURMIS_REPRODUCTRICE.signal_value_changed().connect([this]() { updateSimulationRule("POURCENTAGE_FOURMIS_REPRODUCTRICE", this->b_POURCENTAGE_FOURMIS_REPRODUCTRICE.get_value()); });
	
	b_POURCENTAGE_FOURMIS_GEURRIERES.configure(Gtk::Adjustment::create(REGLE_SIMULATION["POURCENTAGE_FOURMIS_GEURRIERES"],0,100), 1, 0);
	b_POURCENTAGE_FOURMIS_GEURRIERES.signal_value_changed().connect([this]() { updateSimulationRule("POURCENTAGE_FOURMIS_GEURRIERES", this->b_POURCENTAGE_FOURMIS_GEURRIERES.get_value()); });
	
	b_PUISSANCE_FOURMIS_OUVRIERES.configure(Gtk::Adjustment::create(REGLE_SIMULATION["PUISSANCE_FOURMIS_OUVRIERES"],1,100), 1, 0);
	b_PUISSANCE_FOURMIS_OUVRIERES.signal_value_changed().connect([this]() { updateSimulationRule("PUISSANCE_FOURMIS_OUVRIERES", this->b_PUISSANCE_FOURMIS_OUVRIERES.get_value()); });
	
	b_PUISSANCE_FOURMIS_REPRODUCTRICE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["PUISSANCE_FOURMIS_REPRODUCTRICE"],0,100), 1, 0);
	b_PUISSANCE_FOURMIS_REPRODUCTRICE.signal_value_changed().connect([this]() { updateSimulationRule("PUISSANCE_FOURMIS_REPRODUCTRICE", this->b_PUISSANCE_FOURMIS_REPRODUCTRICE.get_value()); });
	
	b_PUISSANCE_FOURMIS_GEURRIERES.configure(Gtk::Adjustment::create(REGLE_SIMULATION["PUISSANCE_FOURMIS_GEURRIERES"],0,100), 1, 0);
	b_PUISSANCE_FOURMIS_GEURRIERES.signal_value_changed().connect([this]() { updateSimulationRule("PUISSANCE_FOURMIS_GEURRIERES", this->b_PUISSANCE_FOURMIS_GEURRIERES.get_value()); });
	
	b_SEUIL_SUCRE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["SEUIL_SUCRE"],1,100), 1, 0);
	b_SEUIL_SUCRE.signal_value_changed().connect([this]() { updateSimulationRule("SEUIL_SUCRE", this->b_SEUIL_SUCRE.get_value()); });
	
	b_NB_MORCEAUX_SUCRE.configure(Gtk::Adjustment::create(REGLE_SIMULATION["NB_MORCEAUX_SUCRE"],0,10000), 1, 0);
	b_NB_MORCEAUX_SUCRE.signal_value_changed().connect([this]() { updateSimulationRule("NB_MORCEAUX_SUCRE", this->b_NB_MORCEAUX_SUCRE.get_value()); });
	
	b_QUANTITE_SUCRE_CHAQUE_MORCEAU.configure(Gtk::Adjustment::create(REGLE_SIMULATION["QUANTITE_SUCRE_CHAQUE_MORCEAU"],1,1000), 1, 0);
	b_QUANTITE_SUCRE_CHAQUE_MORCEAU.signal_value_changed().connect([this]() { updateSimulationRule("QUANTITE_SUCRE_CHAQUE_MORCEAU", this->b_QUANTITE_SUCRE_CHAQUE_MORCEAU.get_value()); });
	
	b_NB_ITERATIONS_SIMULATION.configure(Gtk::Adjustment::create(REGLE_SIMULATION["NB_ITERATIONS_SIMULATION"],1,1000000), 1, 0);
	b_NB_ITERATIONS_SIMULATION.signal_value_changed().connect([this]() { updateSimulationRule("NB_ITERATIONS_SIMULATION", this->b_NB_ITERATIONS_SIMULATION.get_value()); });

	layout.attach(settingsMenuTitle, 0, 0, 2, 1);
	
	layout.attach(lb_TAILLE_GRILLE, 0, 1, 1, 1); layout.attach(b_TAILLE_GRILLE, 1, 1, 1, 1);
	layout.attach(lb_NB_COLONIES, 0, 2, 1, 1); layout.attach(b_NB_COLONIES, 1, 2, 1, 1);
	layout.attach(lb_NB_FOURMIS_PAR_COLONIE, 0, 3, 1, 1); layout.attach(b_NB_FOURMIS_PAR_COLONIE, 1, 3, 1, 1);
	layout.attach(lb_POURCENTAGE_FOURMIS_OUVRIERES, 0, 4, 1, 1); layout.attach(b_POURCENTAGE_FOURMIS_OUVRIERES, 1, 4, 1, 1);
	layout.attach(lb_POURCENTAGE_FOURMIS_GEURRIERES, 0, 5, 1, 1); layout.attach(b_POURCENTAGE_FOURMIS_GEURRIERES, 1, 5, 1, 1);
	layout.attach(lb_POURCENTAGE_FOURMIS_REPRODUCTRICE, 0, 6, 1, 1); layout.attach(b_POURCENTAGE_FOURMIS_REPRODUCTRICE, 1, 6, 1, 1);
	layout.attach(lb_PUISSANCE_FOURMIS_OUVRIERES, 0, 7, 1, 1); layout.attach(b_PUISSANCE_FOURMIS_OUVRIERES, 1, 7, 1, 1);
	layout.attach(lb_PUISSANCE_FOURMIS_GEURRIERES, 0, 8, 1, 1); layout.attach(b_PUISSANCE_FOURMIS_GEURRIERES, 1, 8, 1, 1);
	layout.attach(lb_PUISSANCE_FOURMIS_REPRODUCTRICE, 0, 9, 1, 1); layout.attach(b_PUISSANCE_FOURMIS_REPRODUCTRICE, 1, 9, 1, 1);
	layout.attach(lb_SEUIL_SUCRE, 0, 10, 1, 1); layout.attach(b_SEUIL_SUCRE, 1, 10, 1, 1);
	layout.attach(lb_NB_MORCEAUX_SUCRE, 0, 11, 1, 1); layout.attach(b_NB_MORCEAUX_SUCRE, 1, 11, 1, 1);
	layout.attach(lb_QUANTITE_SUCRE_CHAQUE_MORCEAU, 0 ,12, 1, 1); layout.attach(b_QUANTITE_SUCRE_CHAQUE_MORCEAU, 1, 12, 1, 1);
	layout.attach(lb_NB_ITERATIONS_SIMULATION, 0, 13, 1, 1); layout.attach(b_NB_ITERATIONS_SIMULATION, 1, 13, 1, 1);
	layout.attach(buttonGoToMainMenu, 0, 14, 2, 1);
	
	buttonGoToMainMenu.signal_clicked().connect(sigc::bind(sigc::mem_fun(*mainWindow, &MainWindow::onButtonClickedChangeScene), Scene::MAIN_MENU)); 

	this->append(layout);
	this->set_halign(Gtk::Align::CENTER);
	this->set_valign(Gtk::Align::CENTER);
}
SettingsMenu::~SettingsMenu() {}
// simulation
Simulation::Simulation(Gtk::Orientation orientation, int spacing, MainWindow* mainWindow):
	Box(orientation, spacing),
	rightPanel(Gtk::Orientation::VERTICAL, 32),	
	buttonArea(Gtk::Orientation::HORIZONTAL, 16),
	infoArea(Gtk::Orientation::VERTICAL, 32),
	tourLabel("TOUR :"),
	buttonGoToMainMenu("Back to main menu"),
	buttonResumeSimulation("Pause/Resume"),
	simulationArea(this),
	coloniesInfoFrame(),
	coloniesInfoLabel()
{
	set_margin_start(24);
	set_margin_end(24);
	buttonArea.append(buttonResumeSimulation);
	buttonArea.append(buttonGoToMainMenu);
	buttonResumeSimulation.set_hexpand();
	buttonGoToMainMenu.set_hexpand();
	buttonArea.set_hexpand();

	tourLabel.set_use_markup(true);

	scrolledWindow.set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
	scrolledWindow.set_expand();
	scrolledWindow.set_child(infoArea);

	css_provider = Gtk::CssProvider::create();
	generateCSS(css_provider);
	Gtk::StyleContext::add_provider_for_display(
      Gdk::Display::get_default(), css_provider,
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	rightPanel.append(buttonArea);
	rightPanel.append(tourLabel);
	rightPanel.append(scrolledWindow);

	this->append(simulationArea);
	this->append(rightPanel);
	
	buttonGoToMainMenu.signal_clicked().connect(sigc::bind(sigc::mem_fun(*mainWindow, &MainWindow::onButtonClickedChangeScene), Scene::MAIN_MENU)); 
	buttonResumeSimulation.signal_clicked().connect(sigc::mem_fun(simulationArea, &SimulationArea::resumeSimulation));
}
Simulation::~Simulation() {}
void Simulation::initSimulation() {
	int n = REGLE_SIMULATION["TAILLE_GRILLE"];
	CELL_SIZE = int(1000 / n);
	if (n % 2) CELL_SIZE--;
	SIMULATION_SIZE = CELL_SIZE * n;	

	simulationArea.set_content_width(SIMULATION_SIZE);
	simulationArea.set_content_height(SIMULATION_SIZE);
	simulationArea.curIteration = 0;
	simulationArea.initSimulation();

	int nbColonies = REGLE_SIMULATION["NB_COLONIES"];
	
	for (int i = 0; i < coloniesInfoLabel.size(); i++) {
		coloniesInfoFrame[i].unset_label();
		coloniesInfoFrame[i].unset_child();
		infoArea.remove(coloniesInfoFrame[i]);
	}
	coloniesInfoLabel.clear();
	coloniesInfoFrame.clear();
	
	for (int i = 0; i < nbColonies; i++) {
		coloniesInfoFrame.push_back(Gtk::Frame());
		int nbFourmis = simulationArea.simulation.colonie(i).nbFourmis();
		Gtk::Label title("<span font_weight='bold' size='x-large'>Colonie " + std::to_string(i) + "  (" + std::to_string(nbFourmis) + " fourmis)</span>");
		title.set_use_markup(true);
		coloniesInfoFrame[i].set_label_widget(title);
		coloniesInfoFrame[i].set_margin(12);	
		std::string cssClassName = "colored-frame-" + std::to_string(i);	
		coloniesInfoFrame[i].get_style_context()->add_class(cssClassName);
		
		coloniesInfoLabel.push_back(Gtk::Label("\n"));
		coloniesInfoLabel[i].set_use_markup(true);

		coloniesInfoFrame[i].set_child(coloniesInfoLabel[i]);
		infoArea.append(coloniesInfoFrame[i]);
	}
}
void Simulation::onSimulationChanged() {
	int n = REGLE_SIMULATION["NB_COLONIES"];

	tourLabel.set_markup("<span font_weight='bold' size='xx-large'>TOUR : " + std::to_string(simulationArea.curIteration) + "</span>");

	for (int colonieID = 0; colonieID < n; colonieID++) {
		int nbFourmis = simulationArea.simulation.colonie(colonieID).nbFourmis();
		int stockSucre = simulationArea.simulation.colonie(colonieID).stockSucre();
		Gtk::Label title("<span font_weight='bold' size='xx-large'>Colonie " + std::to_string(colonieID) + "  (" + std::to_string(nbFourmis) + 
						 " fourmis)  ---  SUCRE: </span><span font_weight='bold' size='xx-large' color='white'>" + std::to_string(stockSucre) + "</span>");
		title.set_use_markup(true);
		coloniesInfoFrame[colonieID].set_label_widget(title);

		int nbFourmisOuvrieres = simulationArea.simulation.nbFourmis(colonieID, Caste::OUVRIERE);
		int nbFourmisGeurrieres = simulationArea.simulation.nbFourmis(colonieID, Caste::GUERRIERE);
		int nbFourmisReproductrices = simulationArea.simulation.nbFourmis(colonieID, Caste::REPRODUCTRICE);
		int nbFourmisNees = simulationArea.simulation.colonie(colonieID).nbFourmisNees();
		int nbFourmisMortes = simulationArea.simulation.colonie(colonieID).nbFourmisMortes();
		int nbFourmisTuees = simulationArea.simulation.colonie(colonieID).nbFourmisTuees();
	
		coloniesInfoLabel[colonieID].set_markup("<span size='x-large'>Nb fourmis ouvrieres : " + std::to_string(nbFourmisOuvrieres) +
			"\nNb fourmis geurrieres : " + std::to_string(nbFourmisGeurrieres) +
			"\nNb fourmis reproductrice : " + std::to_string(nbFourmisReproductrices) +
			"\nNb fourmis nées : " + std::to_string(nbFourmisNees) +
			"\nNb fourmis mortes : " + std::to_string(nbFourmisMortes) +
			"\nNb fourmis tuées : " + std::to_string(nbFourmisTuees) +
		+ "</span>");	
	}
}
// main menu
MainMenu::MainMenu(Gtk::Orientation orientation, int spacing, MainWindow* mainWindow):  
	Box(orientation, spacing),
	buttonGoToSimulation("Start simulation"),
	buttonGoToSettingsMenu("Settings"),
	simulationTitle("<span size='48pt' font_weight='bold' color='red'>Ants simulation </span>") 
{
	simulationTitle.set_use_markup(true);

	simulationTitle.set_size_request(512, 64);
	buttonGoToSimulation.set_size_request(512, 64);
	buttonGoToSettingsMenu.set_size_request(512, 64);

	this->append(simulationTitle);
	this->append(buttonGoToSimulation);
	this->append(buttonGoToSettingsMenu);

	this->set_halign(Gtk::Align::CENTER);
	this->set_valign(Gtk::Align::CENTER);

	buttonGoToSimulation.signal_clicked().connect(sigc::bind(sigc::mem_fun(*mainWindow, &MainWindow::onButtonClickedChangeScene), Scene::SIMULATION));
	buttonGoToSettingsMenu.signal_clicked().connect(sigc::bind(sigc::mem_fun(*mainWindow, &MainWindow::onButtonClickedChangeScene), Scene::SETTINGS_MENU)); 
};

MainMenu::~MainMenu() {}
// main window
MainWindow::MainWindow():  settingsMenuScene(Gtk::Orientation::VERTICAL, 64, this), 
						   simulationScene(Gtk::Orientation::HORIZONTAL, 24, this),
						   mainMenuScene(Gtk::Orientation::VERTICAL, 80, this) {
	set_title("Ants simulation");
	set_default_size(1800, 1000);
	maximize();

	stackScenesManager.add(mainMenuScene, std::to_string(Scene::MAIN_MENU));
	stackScenesManager.add(simulationScene, std::to_string(Scene::SIMULATION));
	stackScenesManager.add(settingsMenuScene, std::to_string(Scene::SETTINGS_MENU));

    set_child(stackScenesManager);
}
void MainWindow::onButtonClickedChangeScene(Scene scene) {
	stackScenesManager.set_visible_child(std::to_string(scene));
	if (scene == Scene::SIMULATION) {
		simulationStarted = true;
		simulationScene.initSimulation();//Multi threading rabbit hole.
		set_size_request(SIMULATION_SIZE + 360, SIMULATION_SIZE);	
	}
	else {
		simulationStarted = false;
	}
}

MainWindow::~MainWindow() {}


