#include "window.hpp"

int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.ants-simulation");

	return app->make_window_and_run<MainWindow>(argc, argv);
}
