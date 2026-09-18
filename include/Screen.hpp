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
  private:
    int width;
    int height;

  public:
    Screen() {
        initscr();
        noecho();
        cbreak();
        curs_set(0);
        nodelay(stdscr, TRUE);
        keypad(stdscr, TRUE);
		getmaxyx(stdscr, height, width);
    }
    ~Screen() { endwin(); }

    // --- LES NOUVELLES FONCTIONS ---
    int getWidth() { return width; }
    int getHeight() { return height; }

    void clear() {
        ::clear();
        // ncurses met à jour 'height' et 'width' avec la taille réelle du terminal
        getmaxyx(stdscr, height, width); 
    }
    // -------------------------------

    void drawPixel(int x, int y, char c) { mvaddch(y, x, c); }
    void drawText(int x, int y, const string& text) { mvprintw(y, x, "%s", text.c_str()); }

    // La fameuse fonction qui dessine le cadre
    void drawRect(int x, int y, int w, int h) {
        mvaddch(y, x, ACS_ULCORNER);                 
        mvaddch(y, x + w - 1, ACS_URCORNER);         
        mvaddch(y + h - 1, x, ACS_LLCORNER);         
        mvaddch(y + h - 1, x + w - 1, ACS_LRCORNER); 

        mvhline(y, x + 1, ACS_HLINE, w - 2);         
        mvhline(y + h - 1, x + 1, ACS_HLINE, w - 2); 

        mvvline(y + 1, x, ACS_VLINE, h - 2);         
        mvvline(y + 1, x + w - 1, ACS_VLINE, h - 2); 
    }

    void render() { refresh(); }
    int getInput() { return getch(); }
};
