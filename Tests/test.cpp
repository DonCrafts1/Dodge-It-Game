#include <iostream>
#include <ncurses/ncurses.h>

int main() {
    // std::cout << "hi" << std::endl;
    
    initscr();
    noecho();
    cbreak(); 
    

    WINDOW* win = newwin(25, 50, 0, 0);
    nodelay(win, false);
    box(win, 0, 0);
    wrefresh(win);
    char c = wgetch(win);
    while (c != 'a') {
        c = wgetch(win);
    }

    endwin();
}