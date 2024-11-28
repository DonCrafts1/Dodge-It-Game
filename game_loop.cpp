#include <iostream>
#include <chrono>
#include <thread>
#include <ncurses.h>
#include <cstring>
#include <sstream>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include "game_space.h"
#include "game_text.h"

using namespace std;

constexpr int FRAME_RATE = 60;
constexpr double FRAME_TIME = 1000000.0/FRAME_RATE; // in microseconds
GameSpace game_space;

// Gets current time in milliseconds
long long get_current_time() {
    chrono::milliseconds time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
    return time.count();
}

// Get current time in microsecond
long long get_current_time_micro() {
    chrono::microseconds time = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch());
    return time.count();
}

enum class GameStage {
    SelectDifficulty,
    Ready,
    Game,
    End
};

void process_input(WINDOW* window, bool& paused) {
    vector<int> keys_pressed;
    int key_pressed = wgetch(window);
    while (key_pressed != ERR) {
        // mvwaddch(window, y_coor, x_coor, key_pressed);
        keys_pressed.push_back(key_pressed);
        key_pressed = wgetch(window);
    }
    vector<Direction> input_directions;
    Player* player = game_space.get_player();
    
    // Check if the key pressed was a special key, such as an wasd key
    for (char key : keys_pressed) {
        // Keys allowed during only unpause
        if (!paused) {
            switch (key) {
                case 'a':       // left
                    input_directions.push_back(Direction::Left);
                    break; 
                case 's':       // down
                    input_directions.push_back(Direction::Down);
                    break; 
                case 'd':       // right
                    input_directions.push_back(Direction::Right);
                    break; 
                case 'w':       // up
                    input_directions.push_back(Direction::Up);
                    break; 
                case 't':
                    game_space.spawn_falling_obj_random();
                    break;
            }
        }
        
        // For any case
        switch (key) { 
            case 'p': // pause. just for testing
                paused = !paused;
                break;
            case 'x': 
                endwin();
                exit(0);
                break;
            default:
                break;
        }
    }
    player->move(input_directions);
    
    // cout << player->get_position() << endl;
}

bool process_menu_input(WINDOW* window, Difficulty& difficulty) {
    int key_pressed = wgetch(window);
    if (key_pressed == ERR) {
        return false;
    }
    switch (key_pressed) {
        case '1':
            difficulty = Difficulty::Easy;
            return true;
        case '2':
            difficulty = Difficulty::Medium;
            return true;
        case '3':
            difficulty = Difficulty::Hard;
            return true;
        case 'x':
            endwin();
            exit(0);
        default:
            break;
    }
    return false;
}

bool process_ready_input(WINDOW* window, bool& proceed) {
    int key_pressed = wgetch(window);
    if (key_pressed == ERR) {
        return false;
    }
    switch (key_pressed) {
        case 'q':
            proceed = false;
            break;
        case 'e':
            proceed = true;
            break;
        case 'x':
            endwin();
            exit(0);
        default:
            return false;
    }
    return true;
}

// If game over (player dies, or some other objective), returns true 
bool update(const long& frame_time) {
    return game_space.update(frame_time);
}

void render(WINDOW* window) {
    // cout << "Hi!" << endl;
    wclear(window);
    box(window, 0, 0);
    game_space.print(window);
}

// int main() {
//     initscr();
//     printw("Hello!");
//     refresh();
//     char c = getch();
//     addch(c);

//     getch();
//     endwin();
// }

void display_main_menu(WINDOW* window, const bool& test_mode) {
    box(window, 0, 0);
    if (test_mode) {
        mvwaddstr(window, MAX_Y/2 - 6, MAX_X / 2 - test_mode_text.length() / 2, test_mode_text.c_str());
    }
    
    mvwaddstr(window, MAX_Y/2 - 2, MAX_X/2 - welcome_text.length() / 2, welcome_text.c_str());
    mvwaddstr(window, MAX_Y/2 - 1, MAX_X/2 - made_by_text.length() / 2, made_by_text.c_str());
    mvwaddstr(window, MAX_Y/2 + 2, MAX_X/2 - difficulty_text.length() / 2, difficulty_text.c_str());
    mvwaddstr(window, MAX_Y/2 + 5, MAX_X/2 - exit_text.length() / 2, exit_text.c_str());
    
    wrefresh(window);
}

void display_ready_screen(WINDOW* window, const Difficulty& difficulty) {
    box(window, 0, 0);
    mvwaddstr(window, MAX_Y/2 - 5, MAX_X/2 - ready_text.length() / 2, ready_text.c_str());

    string difficulty_text = "Your difficulty: ";
    stringstream ss; ss << difficulty;
    difficulty_text += ss.str();
    mvwaddstr(window, MAX_Y/2, MAX_X/2 - difficulty_text.length() / 2, difficulty_text.c_str());

    mvwaddstr(window, MAX_Y/2 + 5, MAX_X/2 - proceed_option_text.length() / 2, proceed_option_text.c_str());

    wrefresh(window);
}

void display_end_results(WINDOW* window, const GameResults& results) {
    box(window, 0, 0);

    if (results.won) {
        mvwaddstr(window, MAX_Y/2 - 5, MAX_X/2 - you_win_text.length()/2, you_win_text.c_str());
    } else {
        mvwaddstr(window, MAX_Y/2 - 5, MAX_X/2 - game_over_text.length()/2, game_over_text.c_str());
    }
    
    string data_str = "You survived: " + to_string(results.time_elapsed / MILLION).substr(0, 5) + "s, on Difficulty: ";
    stringstream ss; ss << results.difficulty;
    data_str += ss.str() + ". Lives remaining: " + to_string(results.lives_remaining);
    mvwaddstr(window, MAX_Y/2, MAX_X/2 - data_str.length()/2, data_str.c_str());

    wrefresh(window);
}

void display_game_stage(WINDOW* window, const GameStage& stage) {
    switch (stage) {
        case GameStage::SelectDifficulty:
            mvwaddstr(window, 5, MAX_X/2 - main_menu_stage_text.length()/2, main_menu_stage_text.c_str());
            break;
        case GameStage::Ready:
            mvwaddstr(window, 5, MAX_X/2 - ready_stage_text.length()/2, ready_stage_text.c_str());
            break;
        case GameStage::Game:
            return;
        case GameStage::End:
            mvwaddstr(window, 5, MAX_X/2 - end_stage_text.length()/2, end_stage_text.c_str());
            break;
    }
    
    wrefresh(window);
}

// Found on the internet :)
void handler(int sig) {
    clear();
    endwin();
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char* argv[]) {
    signal(SIGSEGV, handler);
    signal(SIGBUS, handler);
    srand(time(0));
    
    initscr();              // Start curses mode
    cbreak();               // Line buffering disabled
    noecho();               // Don't echo() while we do getch
    // nodelay(stdscr, TRUE);  // Non-blocking input
    // keypad(stdscr, TRUE);
    // int yMax, xMax;
    // getmaxyx(stdscr, yMax, xMax);

    WINDOW* play_win = newwin(MAX_Y, MAX_X, 0, 0);
    nodelay(play_win, FALSE);  // Non-blocking input
    keypad(play_win, TRUE);
    box(play_win, 0, 0);
    // refresh();
    wrefresh(play_win);
    // nodelay(play_win, TRUE);

    bool test_mode = argc >= 2 && strcmp(argv[1], "test") == 0;
    bool playing = true;
    GameStage game_stage = GameStage::SelectDifficulty;
    Difficulty difficulty = Difficulty::NotSet;
    
    while (playing) {
        werase(play_win);

        nodelay(play_win, TRUE);
        switch (game_stage) {
            case GameStage::SelectDifficulty: {
                bool selected = false;
                while (!selected) {
                    werase(play_win);
                    display_main_menu(play_win, test_mode);
                    selected = process_menu_input(play_win, difficulty);
                    display_game_stage(play_win, game_stage);
                    this_thread::sleep_for(chrono::microseconds((long)FRAME_TIME));
                }
                game_stage = GameStage::Ready;
                break;
            }
            case GameStage::Ready: {
                werase(play_win);
                bool proceed, selected = false;
                while (!selected) {
                    werase(play_win);
                    display_ready_screen(play_win, difficulty);
                    selected = process_ready_input(play_win, proceed);
                    display_game_stage(play_win, game_stage);
                    this_thread::sleep_for(chrono::microseconds((long)FRAME_TIME));
                }
                if (proceed) {
                    game_stage = GameStage::Game;     
                } else {
                    game_stage = GameStage::SelectDifficulty;
                }
                break;
            }
            case GameStage::Game: {
                game_space.reset(difficulty, test_mode);
                bool paused = false;
                nodelay(play_win, FALSE);
                
                mvwaddstr(play_win, MAX_Y/2, MAX_X/2 - instruction_text.length()/2, instruction_text.c_str());
                wgetch(play_win);

                nodelay(play_win, TRUE);
                long prev_time = get_current_time_micro();
                bool game_over = false;
                while (!game_over) {
                    long current_time = get_current_time_micro();
                    long elapsed = current_time - prev_time;
                    prev_time = current_time;

                    werase(play_win);
                    process_input(play_win, paused);

                    if (!paused) {
                        game_over = update(elapsed);
                        render(play_win);
                        display_game_stage(play_win, game_stage);
                    } else {
                        mvwaddstr(play_win, MAX_Y / 2 - 1, MAX_X / 2 - paused_str.length() / 2, paused_str.c_str());
                        mvwaddstr(play_win, MAX_Y / 2 + 1, MAX_X / 2 - paused_instruction_str.length() / 2, paused_instruction_str.c_str());
                    }
                    
                    // string paused_str = "Paused: " + to_string(paused);
                    // mvwaddstr(play_win, 2, 0, paused_str.c_str());

                    
                    long process_elapsed = get_current_time_micro() - prev_time;
                    long time_until_next_frame = (long)(FRAME_TIME) - process_elapsed;
                    
                    if (test_mode) {
                        string debug_frame_time_str = "FRAME_TIME: " + to_string(FRAME_TIME) + ". TIME_TO_NEXT_FRAME: " + to_string(time_until_next_frame);
                        mvwaddstr(play_win, 5, MAX_X - debug_frame_time_str.length() - 1, debug_frame_time_str.c_str());
                    }
                    
                    wrefresh(play_win);
                    if (time_until_next_frame <= 0) {
                        continue;
                    }
                    
                    this_thread::sleep_for(chrono::microseconds(time_until_next_frame));
                }
                wclear(play_win);
                game_stage = GameStage::End;
                break;
            }
            case GameStage::End: {
                display_end_results(play_win, game_space.get_game_results());
                
                mvwaddstr(play_win, MAX_Y/2 + 5, MAX_X/2 - end_message.length() / 2, end_message.c_str());
                display_game_stage(play_win, game_stage);
                wrefresh(play_win);
                this_thread::sleep_for(chrono::milliseconds(1000));

                char input = ERR;
                while (input == ERR) {
                    input = wgetch(play_win);
                    if (input == 'q') {
                        game_stage = GameStage::SelectDifficulty;
                        break;
                    } else if (input == 'x') {
                        playing = false;
                        break;
                    }
                    input = ERR;
                }
                break;
            }
        }
    }
    
    endwin();
    return 0;
}

