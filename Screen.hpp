#pragma once
#include <ncurses.h>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

// ==========================================
// CLASSE SCREEN (Wrapper Objet pour ncurses)
// ==========================================
class Screen {
	public:
		// 1. Constructeur : Initialise ncurses automatiquement
		Screen() {
			initscr();             // Démarre ncurses
			noecho();              // Cache les touches tapées
			cbreak();              // Pas de buffer de ligne
			curs_set(0);           // Cache le curseur clignotant
			nodelay(stdscr, TRUE); // Rend getch() non-bloquant pour la Game Loop
			keypad(stdscr, TRUE);  // Active les flèches directionnelles
		}

		// 2. Destructeur : Restaure le terminal quand l'objet est détruit (VITAL !)
		~Screen() {
			endwin(); 
		}

		// 3. Effacer l'écran
		void clear() {
			::clear(); // Appelle la fonction clear() globale de ncurses
		}

		// 4. Dessiner un Pixel (Notez qu'on utilise X puis Y, comme sur ESP32 !)
		void drawPixel(int x, int y, char c) {
			mvaddch(y, x, c); 
		}

		// 5. Écrire du texte
		void drawText(int x, int y, const string& text) {
			mvprintw(y, x, "%s", text.c_str());
		}

		// 6. Dessiner un joli cadre avec les caractères spéciaux de ncurses
		void drawRect(int x, int y, int w, int h) {
			// Coins
			mvaddch(y, x, ACS_ULCORNER);                 // Haut-Gauche
			mvaddch(y, x + w - 1, ACS_URCORNER);         // Haut-Droite
			mvaddch(y + h - 1, x, ACS_LLCORNER);         // Bas-Gauche
			mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER); // Bas-Droite

			// Lignes horizontales
			mvhline(y, x + 1, ACS_HLINE, w - 2);         // Ligne du haut
			mvhline(y + h - 1, x + 1, ACS_HLINE, w - 2); // Ligne du bas

			// Lignes verticales
			mvvline(y + 1, x, ACS_VLINE, h - 2);         // Ligne de gauche
			mvvline(y + 1, x + w - 1, ACS_VLINE, h - 2); // Ligne de droite
		}

		// 7. Envoyer à l'écran
		void render() {
			refresh();
		}

		// 8. Fonction bonus : Lire le clavier facilement
		int getInput() {
			return getch();
		}
};
