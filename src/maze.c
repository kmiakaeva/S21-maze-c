#include "maze.h"

int main() {
  init_ncurses();
  init_colors();
  maze_loop();

  endwin();
  return 0;
}

void maze_loop() {
  int choice = 0, highlight = 1;

  while (1) {
    clear();
    draw_menu(highlight);
    int ch = getch();

    switch (ch) {
      case KEY_UP:
        highlight = highlight == 1 ? MAIN_MENU_ITEMS : highlight - 1;
        break;
      case KEY_DOWN:
        highlight = highlight == MAIN_MENU_ITEMS ? 1 : highlight + 1;
        break;
      case ENTER_KEY:
        choice = highlight;
        break;
    }

    process_choice(choice);
    choice = 0;  // Сброс выбора
  }
}

void process_choice(int choice) {
  if (choice == 1) {
    load_maze();
  } else if (choice == 2) {
    generate_maze_ui();
  } else if (choice == 3) {
    find_way();
  } else if (choice == 4) {
    load_cave();
  } else if (choice == 5) {
    generate_cave_ui();
  } else if (choice == 6) {
    ml_agent_ui();
  } else if (choice == 7) {
    endwin();
    exit(0);
  }
}

void load_maze() {
  int rows = 0, cols = 0;
  int **maze = NULL, **maze_vert = NULL, **maze_hor = NULL;

  while (1) {  // Цикл для возвращения в меню выбора
    int example_choice = select_example();

    const char *filename = NULL;
    if (example_choice == 1) {
      filename = "files/maze_1";
    } else if (example_choice == 2) {
      filename = "files/maze_2";
    } else if (example_choice == 3) {
      filename = "files/maze_3";
    } else if (example_choice == 4) {
      return;  // Возврат в главное меню
    }

    if (!open_file(2, filename, &rows, &cols, &maze_vert, &maze_hor, &maze)) {
      draw_maze(NULL, rows, cols, maze_vert, maze_hor);
      wait_for_enter();
    } else {
      draw_error_message("Error loading file\n");
      wait_for_enter();
    }

    free_matrix(maze, maze_vert, maze_hor);
  }
}

void generate_maze_ui() {
  int rows = 0, cols = 0;
  int input_y = 2, input_x = 2;

  clear();
  echo();
  input_with_validation("Enter the size of maze (rows, columns) (max 50): ",
                        "Invalid size! Please try again.", &rows, &cols,
                        MAZE_SIZE, 1, MAZE_SIZE, 1, input_y, input_x, 1);
  noecho();

  int **maze_vert = NULL, **maze_hor = NULL, **maze = NULL;

  create_matrix(&maze_vert, rows, cols);
  create_matrix(&maze_hor, rows, cols);
  create_matrix(&maze, rows, cols);

  generate_maze(maze, maze_vert, maze_hor, rows, cols);
  save_in_file(maze_vert, maze_hor, &rows, &cols);
  draw_maze(NULL, rows, cols, maze_vert, maze_hor);

  wait_for_enter();

  free_matrix(maze, maze_vert, maze_hor);
}

void find_way() {
  int rows = 0, cols = 0;
  int **maze_vert = NULL, **maze_hor = NULL, **maze = NULL;

  if (!open_file(1, "#", &rows, &cols, &maze_vert, &maze_hor, &maze)) {
    save_in_file(maze_vert, maze_hor, &rows, &cols);
  }

  draw_maze(NULL, rows, cols, maze_vert, maze_hor);

  int input_y = rows + 8, input_x = 2;
  Point start, end;

  echo();
  input_with_validation(
      "Enter start point (row col): ", "Invalid start point! Please try again.",
      &start.x, &start.y, rows, 1, cols, 1, input_y, input_x, 1);
  start.x -= 1;  // Корректируем на индексацию массива
  start.y -= 1;
  input_with_validation(
      "Enter end point (row col): ", "Invalid end point! Please try again.",
      &end.x, &end.y, rows, 1, cols, 1, input_y + 2, input_x, 1);
  end.x -= 1;
  end.y -= 1;
  noecho();

  find_and_draw_path(rows, cols, maze_vert, maze_hor, start, end);  // Рисуем

  wait_for_enter();

  free_matrix(maze, maze_vert, maze_hor);
}

void load_cave() {
  int example_choice;
  t_cave *cave = NULL;

  while (1) {
    example_choice = select_example();

    const char *filename = NULL;
    if (example_choice == 1) {
      filename = "caves/ex_1.txt";
    } else if (example_choice == 2) {
      filename = "caves/ex_2.txt";
    } else if (example_choice == 3) {
      filename = "caves/ex_3.txt";
    } else if (example_choice == 4) {
      return;  // Возврат в главное меню
    }

    int birth_limit = 0, death_limit = 0, chance = -1;
    input_simulation_params(&birth_limit, &death_limit, &chance, 10);

    cave = initialize_cave(birth_limit, death_limit, 1, filename, 0, 0, chance);
    if (cave == NULL) return;

    cave_simulation_mode(cave);

    free_cave_matrix(cave->matrix, cave->n);
    free_cave_matrix(cave->prev_matrix, cave->n);
    free(cave);
  }
}

void generate_cave_ui() {
  t_cave *cave = NULL;

  clear();

  int birth_limit = 0, death_limit = 0, chance = 0;
  input_simulation_params(&birth_limit, &death_limit, &chance, 2);

  srand(time(NULL));
  int n = rand() % 50 + 1;
  int m = rand() % 50 + 1;

  cave = initialize_cave(birth_limit, death_limit, 2, NULL, n, m, chance);
  if (cave == NULL) return;

  cave_simulation_mode(cave);

  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}

void ml_agent_ui() {
  int keep_running = 1;

  while (keep_running) {
    const char *filename = select_maze_file();

    if (filename == NULL) {
      keep_running = 0;  // Завершаем цикл и возвращаемся в главное меню
    } else {
      process_ml_agent_for_maze(filename);
    }
  }
}

void process_ml_agent_for_maze(const char *filename) {
  Maze *maze = load_ml_maze(filename);
  if (!maze) {
    draw_error_message("Error loading maze file\n");
    wait_for_enter();
    return;
  }

  clear();
  draw_ml(maze, NULL);

  Agent agent;
  initialize_agent(&agent, maze->rows * maze->cols, MAX_ACTIONS);

  int input_y = maze->rows + 8, input_x = 2;
  int start_row, start_col, end_row = maze->rows - 1, end_col = maze->cols - 1;
  echo();
  input_with_validation(
      "Enter start point (row col): ", "Invalid start point! Please try again.",
      &start_row, &start_col, maze->rows, 1, maze->cols, 1, input_y, input_x,
      1);
  noecho();

  train_agent(&agent, maze, start_row - 1, start_col - 1, end_row, end_col);

  int **path_matrix = create_path_matrix(maze->rows, maze->cols);
  save_path(start_row - 1, start_col - 1, end_row, end_col, maze, &agent,
            path_matrix);

  clear();
  draw_ml(maze, path_matrix);
  wait_for_enter();

  free_path_matrix(path_matrix, maze->rows);
  free_ml_maze(maze);
}

int select_example() {
  int choice = 0, highlight = 1;

  while (1) {
    clear();
    draw_loading_menu(highlight);
    int ch = getch();

    switch (ch) {
      case KEY_UP:
        highlight = highlight == 1 ? LOADING_MENU_ITEMS : highlight - 1;
        break;
      case KEY_DOWN:
        highlight = highlight == LOADING_MENU_ITEMS ? 1 : highlight + 1;
        break;
      case ENTER_KEY:
        choice = highlight;
        return choice;
    }
  }
}

void cave_simulation_mode(t_cave *cave) {
  int choice = 0, highlight = 1;

  while (1) {
    clear();
    draw_mode_menu(highlight);
    int ch = getch();

    switch (ch) {
      case KEY_UP:
        highlight = highlight == 1 ? MODE_MENU_ITEMS : highlight - 1;
        break;
      case KEY_DOWN:
        highlight = highlight == MODE_MENU_ITEMS ? 1 : highlight + 1;
        break;
      case ENTER_KEY:
        choice = highlight;
        break;
    }

    if (choice > 0) {
      break;
    }
  }

  if (choice == 1) {
    step_by_step_cave_simulation(cave);
  } else if (choice == 2) {
    automatic_cave_simulation(cave);
  }
}

void step_by_step_cave_simulation(t_cave *cave) {
  int ch = 0;

  do {
    clear();
    draw_cave(cave);
    printw("\n\n  Press Space to move to the next step -->");

    do {
      ch = getch();
    } while (ch != ' ');
  } while (simulate(cave));

  wait_for_enter();
}

void automatic_cave_simulation(t_cave *cave) {
  int delay = 0;

  clear();
  draw_cave(cave);

  echo();
  input_with_validation("Enter the number of milliseconds for delay (1-1000): ",
                        "Invalid input! Please try again.", &delay, NULL, 1000,
                        1, 0, 0, 2, 2, 0);
  noecho();

  while (simulate(cave)) {
    clear();
    draw_cave(cave);
    napms(delay);
  }

  wait_for_enter();
}

void input_with_validation(const char *message_success,
                           const char *message_failure, int *x, int *y,
                           int max_x, int min_x, int max_y, int min_y,
                           int input_y, int input_x, int expect_two_values) {
  int valid_input = 0;

  do {
    attron(COLOR_PAIR(3));
    move(input_y, input_x);
    clrtoeol();  // Очищаем строку перед вводом
    printw("%s", message_success);
    refresh();

    if (expect_two_values) {
      valid_input = scanw("%d %d", x, y);
    } else {
      valid_input = scanw("%d", x);
    }

    attroff(COLOR_PAIR(3));

    int invalid_input =
        (expect_two_values && (valid_input != 2 || *x < min_x || *x > max_x ||
                               *y < min_y || *y > max_y)) ||
        (!expect_two_values && (valid_input != 1 || *x < min_x || *x > max_x));

    if (invalid_input) {
      move(input_y + 1, input_x);
      attron(COLOR_PAIR(4));
      printw("%s", message_failure);
      attroff(COLOR_PAIR(4));
      refresh();
      napms(1000);  // Задержка
      move(input_y + 1, input_x);
      clrtoeol();  // Очищаем сообщение об ошибке
    }
  } while (
      (expect_two_values && (valid_input != 2 || *x < min_x || *x > max_x ||
                             *y < min_y || *y > max_y)) ||
      (!expect_two_values && (valid_input != 1 || *x < min_x || *x > max_x)));
}

void find_and_draw_path(int rows, int cols, int **maze_vert, int **maze_hor,
                        Point start, Point end) {
  bool visited[MAZE_SIZE][MAZE_SIZE] = {false};  // Массив пути
  Point path[rows * cols];  // Максимальная длина пути = rows * cols
  int path_length = 0;

  // Поиск пути
  if (dfs(rows, cols, maze_vert, maze_hor, visited, start, end, path,
          &path_length)) {
    int temp_maze[rows * cols];  // Одномерная матрица для хранения пути
    memset(temp_maze, 0, sizeof(int) * rows * cols);

    for (int i = 0; i < path_length; i++) {
      temp_maze[path[i].x * cols + path[i].y] = 1;  // Путь отмечаем 1
    }
    draw_maze(temp_maze, rows, cols, maze_vert, maze_hor);
  } else {
    draw_error_message("No valid path found.\n");
  }
}

const char *select_maze_file() {
  int example_choice = select_example();

  const char *filename = NULL;
  if (example_choice == 1) {
    filename = "files/maze_1";
  } else if (example_choice == 2) {
    filename = "files/maze_2";
  } else if (example_choice == 3) {
    filename = "files/maze_3";
  }
  return filename;
}

void wait_for_enter() {
  attron(COLOR_PAIR(2));
  printw("\n\n  Press Enter to return to menu -->");
  attroff(COLOR_PAIR(2));

  while (getch() != '\n') {
  }
}

void init_ncurses() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  srand(time(NULL));
}

void init_colors() {
  start_color();

  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  init_pair(3, COLOR_BLUE, COLOR_BLACK);
  init_pair(4, COLOR_RED, COLOR_BLACK);

  wbkgd(stdscr, COLOR_PAIR(1));
}

void draw_menu(int highlight) {
  int start_x = 2, start_y = 2;
  char *title[] = {" _____  _____  __   ___  ___  ___   ______ _____ ",
                   "/  ___|/ __  \\/  |  |  \\/  | / _ \\ |___  /|  ___|",
                   "\\ `--. `' / /'`| |  | .  . |/ /_\\ \\   / / | |__  ",
                   " `--. \\  / /   | |  | |\\/| ||  _  |  / /  |  __| ",
                   "/\\__/ /./ /____| |_ | |  | || | | |./ /___| |___ ",
                   "\\____/ \\_____/\\___/ \\_|  |_/\\_| |_/\\_____/\\____/"};
  char *subtitle = "developed by lorindab, harkonex, directpi";
  char *choices[] = {"Load maze",     "Generate maze", "Find way", "Load cave",
                     "Generate cave", "ML agent",      "Exit"};

  int title_len = strlen(title[0]);
  int subtitle_len = strlen(subtitle);
  int subtitle_offset_x = (title_len - subtitle_len) / 2;

  attron(COLOR_PAIR(1) | A_BOLD);
  for (int i = 0; i < 6; i++) {
    mvprintw(start_y + i, start_x, "%s", title[i]);
  }
  attroff(COLOR_PAIR(1) | A_BOLD);

  attron(COLOR_PAIR(3) | A_BOLD);
  mvprintw(start_y + 7, start_x + subtitle_offset_x, "%s", subtitle);
  attroff(COLOR_PAIR(3) | A_BOLD);

  for (int i = 0; i < MAIN_MENU_ITEMS; i++) {
    if (highlight == i + 1) {
      attron(COLOR_PAIR(2) | A_REVERSE);
      mvprintw(start_y + 11 + i * 2, start_x, " > %s", choices[i]);
      attroff(COLOR_PAIR(2) | A_REVERSE);
    } else {
      attron(COLOR_PAIR(1));
      mvprintw(start_y + 11 + i * 2, start_x, "   %s", choices[i]);
      attroff(COLOR_PAIR(1));
    }
  }
  refresh();
}

void draw_loading_menu(int highlight) {
  int start_x = 2, start_y = 2;
  char *text = "Choose a maze example to load:";
  char *examples[] = {"Example 1", "Example 2", "Example 3",
                      "Return to main menu"};

  attron(COLOR_PAIR(3));
  mvprintw(start_y, start_x, "%s", text);
  attroff(COLOR_PAIR(3));

  for (int i = 0; i < LOADING_MENU_ITEMS; i++) {
    if (highlight == i + 1) {
      attron(COLOR_PAIR(2) | A_REVERSE);
      mvprintw(start_y + 2 + i * 2, start_x, " > %s", examples[i]);
      attroff(COLOR_PAIR(2) | A_REVERSE);
    } else {
      attron(COLOR_PAIR(1));
      mvprintw(start_y + 2 + i * 2, start_x, "   %s", examples[i]);
      attroff(COLOR_PAIR(1));
    }
  }
  refresh();
}

void draw_mode_menu(int highlight) {
  int start_x = 2, start_y = 2;
  char *text = "Choose a mode of work:";
  char *modes[] = {"Step-by-step work", "Automatic work"};

  attron(COLOR_PAIR(3));
  mvprintw(start_y, start_x, "%s", text);
  attroff(COLOR_PAIR(3));

  for (int i = 0; i < MODE_MENU_ITEMS; i++) {
    if (highlight == i + 1) {
      attron(COLOR_PAIR(2) | A_REVERSE);
      mvprintw(start_y + 2 + i * 2, start_x, " > %s", modes[i]);
      attroff(COLOR_PAIR(2) | A_REVERSE);
    } else {
      attron(COLOR_PAIR(1));
      mvprintw(start_y + 2 + i * 2, start_x, "   %s", modes[i]);
      attroff(COLOR_PAIR(1));
    }
  }
  refresh();
}

void draw_maze(int temp_maze[], int rows, int cols, int **maze_vert,
               int **maze_hor) {
  int start_x = 2, start_y = 1;

  clear();

  attron(COLOR_PAIR(3));
  move(start_y, start_x + 2);
  for (int j = 0; j < cols; j++) {
    printw(" %2d", j + 1);  // Нумерация столбцов
  }
  attroff(COLOR_PAIR(3));
  printw("\n");

  move(start_y + 1, start_x + 3);  // Верхняя часть лабиринта
  for (int j = 0; j < cols; j++) {
    printw(" _ ");
  }
  printw("\n");

  for (int i = 0; i < rows; i++) {
    move(start_y + i + 2, start_x);
    attron(COLOR_PAIR(3));
    printw("%2d", i + 1);  // Нумерация строк
    attroff(COLOR_PAIR(3));

    printw(" |");

    for (int j = 0; j < cols; j++) {
      if (temp_maze != NULL && temp_maze[i * cols + j] == 1) {
        attron(COLOR_PAIR(2));
        printw("+");  // Путь
        attroff(COLOR_PAIR(2));
      } else {
        printw(" ");  // Иначе пустое пространство
      }

      if (maze_vert[i][j] == 1 && maze_hor[i][j] == 1) {
        printw("_|");  // Нижняя и правая стенки
      } else if (maze_vert[i][j] == 1 && maze_hor[i][j] == 0) {
        printw(" |");  // Правая стенка
      } else if (maze_vert[i][j] == 0 && maze_hor[i][j] == 0) {
        printw("  ");  // Пустая ячейка
      } else if (maze_vert[i][j] == 0 && maze_hor[i][j] == 1) {
        printw("_ ");  // Нижняя стенка
      }
    }
    printw("\n");
  }
  refresh();
}

void draw_cave(t_cave *cave) {
  int start_x = 2, start_y = 4;

  clear();

  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 1) {
        mvprintw(start_y + i, start_x + j * 2, "██");  // Живая клетка
      } else {
        mvprintw(start_y + i, start_x + j * 2, "  ");  // Мертвая клетка
      }
    }
  }
  refresh();
}

void draw_ml(Maze *maze, int **path_matrix) {
  int start_x = 2, start_y = 1;

  clear();

  attron(COLOR_PAIR(3));
  move(start_y, start_x + 2);
  for (int j = 0; j < maze->cols; j++) {
    printw(" %2d", j + 1);  // Нумерация столбцов
  }
  attroff(COLOR_PAIR(3));
  printw("\n");

  move(start_y + 1, start_x + 3);  // Верхняя часть лабиринта
  for (int j = 0; j < maze->cols; j++) {
    printw(" _ ");
  }
  printw("\n");

  for (int i = 0; i < maze->rows; i++) {
    move(start_y + i + 2, start_x);
    attron(COLOR_PAIR(3));
    printw("%2d", i + 1);  // Нумерация строк
    attroff(COLOR_PAIR(3));

    printw(" |");

    for (int j = 0; j < maze->cols; j++) {
      if (path_matrix != NULL && path_matrix[i][j] == 1) {
        attron(COLOR_PAIR(2));
        printw("+");  // Путь
        attroff(COLOR_PAIR(2));
      } else {
        printw(" ");  // Обычная клетка
      }

      if (maze->right_wall[i][j] == 1 && maze->down_wall[i][j] == 1) {
        printw("_|");  // Нижняя и правая стенки
      } else if (maze->right_wall[i][j] == 1 && maze->down_wall[i][j] == 0) {
        printw(" |");  // Правая стенка
      } else if (maze->right_wall[i][j] == 0 && maze->down_wall[i][j] == 0) {
        printw("  ");  // Пустое пространство
      } else if (maze->right_wall[i][j] == 0 && maze->down_wall[i][j] == 1) {
        printw("_ ");  // Нижняя стенка
      }
    }
    printw("\n");
  }
  refresh();
}

void input_simulation_params(int *birth_limit, int *death_limit, int *chance,
                             int input_y) {
  echo();
  input_with_validation(
      "Enter birth limit (0-7): ", "Invalid limit! Please try again.",
      birth_limit, NULL, 7, 0, 0, 0, input_y++, 2, 0);

  input_with_validation(
      "Enter death limit (0-7): ", "Invalid limit! Please try again.",
      death_limit, NULL, 7, 0, 0, 0, input_y++, 2, 0);

  if ((*chance) != -1) {
    input_with_validation(
        "Enter chance of life (0-100%): ", "Invalid chance! Please try again.",
        chance, NULL, 100, 0, 0, 0, input_y++, 2, 0);
  }
  noecho();
}

void draw_error_message(const char *message) {
  attron(COLOR_PAIR(4));
  printw("%s", message);
  attroff(COLOR_PAIR(4));
}