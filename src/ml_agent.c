#include "ml_agent.h"

// Основная функция загрузки лабиринта
Maze *load_ml_maze(const char *filename) {
  FILE *file = fopen(filename, "r");
  Maze *maze = NULL;
  int error_code = 0;
  if (!file) {
    perror("Error opening file");
    return NULL;
  }
  maze = malloc(sizeof(Maze));
  if (!maze) {
    perror("Error allocating memory for maze");
    error_code = 1;
  }
  if (!error_code && fscanf(file, "%d %d", &(maze->rows), &(maze->cols)) != 2) {
    perror("Error reading maze dimensions");
    error_code = 1;
  }
  if (!error_code) {
    maze->right_wall = malloc(maze->rows * sizeof(int *));
    maze->down_wall = malloc(maze->rows * sizeof(int *));
    if (!maze->right_wall || !maze->down_wall) {
      perror("Error allocating memory for walls");
      error_code = 1;
    }
  }
  for (int i = 0; !error_code && i < maze->rows; ++i) {
    maze->right_wall[i] = malloc(maze->cols * sizeof(int));
    maze->down_wall[i] = malloc(maze->cols * sizeof(int));
    if (!maze->right_wall[i] || !maze->down_wall[i]) {
      perror("Error allocating memory for wall rows");
      error_code = 1;
    }
  }
  if (!error_code) {
    error_code =
        read_wall_data(file, maze->right_wall, maze->rows, maze->cols, "right");
  }
  if (!error_code) {
    error_code =
        read_wall_data(file, maze->down_wall, maze->rows, maze->cols, "down");
  }
  fclose(file);
  if (error_code) {
    free_ml_maze(maze);
    maze = NULL;
  }
  return maze;
}

int read_wall_data(FILE *file, int **wall, int rows, int cols,
                   const char *wall_type) {
  int error_code = 0;
  for (int i = 0; i < rows && !error_code; ++i) {
    for (int j = 0; j < cols; ++j) {
      if (fscanf(file, "%d", &(wall[i][j])) != 1) {
        fprintf(stderr, "Error reading %s wall data\n", wall_type);
        error_code = 1;
        // Принудительно завершаем оба цикла
        i = rows;
        j = cols;
      }
    }
  }
  return error_code;
}

void free_ml_maze(Maze *maze) {
  if (maze) {
    if (maze->right_wall && maze->down_wall) {
      for (int i = 0; i < maze->rows; ++i) {
        free(maze->right_wall[i]);
        free(maze->down_wall[i]);
      }
    }
    free(maze->right_wall);
    free(maze->down_wall);
    free(maze);
  }
}

void initialize_agent(Agent *agent, int num_states, int num_actions) {
  agent->num_states = num_states;
  agent->num_actions = num_actions;

  for (int i = 0; i < num_states; ++i) {
    for (int j = 0; j < num_actions; ++j) {
      agent->q_table[i][j] = 0.0;
    }
  }
}

double calculate_distance(int row, int col, int goal_row, int goal_col) {
  return sqrt((goal_row - row) * (goal_row - row) +
              (goal_col - col) * (goal_col - col));
}

int select_action(Agent *agent, int state, double epsilon, Maze *maze) {
  int error_code = 0;
  int best_action = -1;
  if (state < 0 || state >= agent->num_states) {
    fprintf(stderr, "Invalid state: %d\n", state);
    error_code = 1;
  } else if ((rand() / (double)RAND_MAX) <
             epsilon) {  // Вероятность случайного выбора действия (с
                         // вероятностью epsilon)
    best_action = rand() % agent->num_actions;
  } else {  // Выбор действия с максимальным значением Q
    double max_value = -DBL_MAX;  // Используем -DBL_MAX для инициализации
    for (int action = 0; action < agent->num_actions; ++action) {
      int next_row, next_col;
      int next_state =
          get_next_state(state, action, maze, &next_row, &next_col);
      // Если соседнее состояние допустимо
      if (next_state != -1) {
        if (agent->q_table[state][action] > max_value) {
          max_value = agent->q_table[state][action];
          best_action = action;
        }
      }
    }
  }
  // Если все доступные действия приводят к посещенным ячейкам, выбираем
  // случайное действие
  if (best_action == -1 && !error_code) {
    best_action = rand() % agent->num_actions;
  }
  return best_action;
}

int get_next_state(int state, int action, Maze *maze, int *next_row,
                   int *next_col) {
  int row = state / maze->cols;
  int col = state % maze->cols;
  int result = 0;
  switch (action) {
    case 0:  // Вверх
      if (row > 0 && !maze->down_wall[row - 1][col]) {
        --row;
      }
      break;
    case 1:  // Вправо
      if (col < maze->cols - 1 && !maze->right_wall[row][col]) {
        ++col;
      }
      break;
    case 2:  // Вниз
      if (row < maze->rows - 1 && !maze->down_wall[row][col]) {
        ++row;
      }
      break;
    case 3:  // Влево
      if (col > 0 && !maze->right_wall[row][col - 1]) {
        --col;
      }
      break;
    default:
      result = -1;  // Некорректное действие
  }
  // Обновляем координаты следующего состояния
  if (result != -1) {
    *next_row = row;
    *next_col = col;
    result = row * maze->cols + col;
  }
  return result;  // Возвращаем новое состояние в виде индекса
}

double get_reward(int next_state, Maze *maze, int end_row, int end_col,
                  int *visited) {
  int next_row = next_state / maze->cols;
  int next_col = next_state % maze->cols;
  int reward = 0;
  if (next_row == end_row && next_col == end_col) {
    reward = 100.0;  // Большое вознаграждение за достижение цели
  } else if (visited[next_state] > 0) {
    reward = -10.0 * visited[next_state];  // Штраф за повторное посещение
  } else
    reward = -1.0;  // Штраф за обычное движение
  return reward;
}

void update_q_value(Agent *agent, int state, int action, int next_state,
                    double reward, double distance_to_goal) {
  double max_next_q_value = agent->q_table[next_state][0];
  for (int a = 1; a < agent->num_actions; ++a) {
    if (agent->q_table[next_state][a] > max_next_q_value) {
      max_next_q_value = agent->q_table[next_state][a];
    }
  }
  // Добавляем дополнительную награду за приближение к цели
  double adjusted_reward = reward - distance_to_goal;
  // Используем уравнение Беллмана
  agent->q_table[state][action] +=
      ALPHA * (adjusted_reward + GAMMA * max_next_q_value -
               agent->q_table[state][action]);
}

void train_agent(Agent *agent, Maze *maze, int start_row, int start_col,
                 int end_row, int end_col) {
  int num_states = maze->rows * maze->cols;
  for (int episode = 0; episode < NUM_EPISODES; ++episode) {
    int state = start_row * maze->cols + start_col;
    // Массив для отслеживания посещенных состояний
    int visited[num_states];
    for (int i = 0; i < num_states; ++i) visited[i] = 0;
    for (int step = 0; step < MAX_STEPS; ++step) {
      // Увеличиваем счетчик посещений для текущего состояния
      visited[state] += 1;
      int action = select_action(agent, state, EPSILON, maze);
      if (action < 0 || action >= MAX_ACTIONS) {
        continue;
      }
      int next_row = -1, next_col = -1;
      int next_state =
          get_next_state(state, action, maze, &next_row, &next_col);
      if (next_state == -1) {
        continue;
      }
      double reward = get_reward(next_state, maze, end_row, end_col, visited);
      double distance_to_goal =
          calculate_distance(next_row, next_col, end_row, end_col);
      // Обновление Q-значения с учетом награды за приближение к цели
      update_q_value(agent, state, action, next_state, reward,
                     distance_to_goal);
      state = next_state;
      // Проверка достижения цели
      if (next_row == end_row && next_col == end_col) {
        break;
      }
    }
  }
}

void save_path(int start_row, int start_col, int end_row, int end_col,
               Maze *maze, Agent *agent, int **path_matrix) {
  // Инициализация состояния агента на начальной позиции
  int state = start_row * maze->cols + start_col;
  path_matrix[start_row][start_col] = 1;  // Стартовую клетку отмечаем в матрице
  // Проход по шагам для поиска кратчайшего пути
  for (int step = 0; step < MAX_STEPS; ++step) {
    // Выбираем действие с максимальным Q-значением
    int action = select_action(agent, state, 0.0, maze);
    int next_row, next_col;
    int next_state = get_next_state(state, action, maze, &next_row, &next_col);
    if (next_state != -1) {
      // Отмечаем путь в матрице
      path_matrix[next_row][next_col] = 1;
      // Переход в следующее состояние
      state = next_state;
      // Проверка достижения цели
      if (next_row == end_row && next_col == end_col) {
        break;
      }
    }
  }
}

int **create_path_matrix(int rows, int cols) {
  int **path_matrix = malloc(rows * sizeof(int *));
  for (int i = 0; i < rows; ++i) {
    path_matrix[i] = malloc(cols * sizeof(int));
    for (int j = 0; j < cols; ++j) {
      path_matrix[i][j] = 0;  // Инициализация нулями (нет пути)
    }
  }
  return path_matrix;
}

void free_path_matrix(int **path_matrix, int rows) {
  for (int i = 0; i < rows; ++i) {
    free(path_matrix[i]);
  }
  free(path_matrix);
}