#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "backend_maze.h"
#include "cave.h"
#include "ml_agent.h"

#define MAZE_SIZE 50
#define MAIN_MENU_ITEMS 7
#define LOADING_MENU_ITEMS 4
#define MODE_MENU_ITEMS 2
#define ENTER_KEY 10

// Main functions
void maze_loop();
void process_choice(int choice);
void load_maze();
void generate_maze_ui();
void find_way();
void load_cave();
void generate_cave_ui();
void ml_agent_ui();
void process_ml_agent_for_maze(const char *filename);

// Utility functions
int select_example();
void cave_simulation_mode(t_cave *cave);
void step_by_step_cave_simulation(t_cave *cave);
void automatic_cave_simulation(t_cave *cave);
void input_with_validation(const char *message_success,
                           const char *message_failure, int *x, int *y,
                           int max_x, int min_x, int max_y, int min_y,
                           int input_y, int input_x, int expect_two_values);
void find_and_draw_path(int rows, int cols, int **maze_vert, int **maze_hor,
                        Point start, Point end);
const char *select_maze_file();
void wait_for_enter();

// Drawing functions
void init_ncurses();
void init_colors();
void draw_menu(int highlight);
void draw_loading_menu(int highlight);
void draw_mode_menu(int highlight);
void draw_maze(int temp_maze[], int rows, int cols, int **maze_vert,
               int **maze_hor);
void draw_cave(t_cave *cave);
void draw_ml(Maze *maze, int **path_matrix);
void input_simulation_params(int *birth_limit, int *death_limit, int *chance,
                             int input_y);
void draw_error_message(const char *message);