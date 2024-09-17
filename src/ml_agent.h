#ifndef MLAGENT_H
#define MLAGENT_H
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_STATES \
  10000  // Максимальное количество состояний (возможные позиции в лабиринте)
#define MAX_ACTIONS \
  4  // Максимальное количество действий (вверх, вправо, вниз, влево)
#define NUM_EPISODES 1000  // Количество эпизодов для обучения агента
#define MAX_STEPS 100  // Максимальное количество шагов в одном эпизоде
#define ALPHA 0.1  // Коэффициент обучения (скорость обновления Q-значений)
#define GAMMA 0.9  // Коэффициент дисконтирования (учет будущих наград)
#define EPSILON 0.1  // Вероятность случайного действия в ε-greedy стратегии

// Структура для представления лабиринта
typedef struct {
  int rows;  // Количество строк в лабиринте
  int cols;  // Количество столбцов в лабиринте
  int **right_wall;  // Стенки справа
  int **down_wall;   // Стенки снизу
} Maze;
typedef struct {
  double q_table[MAX_STATES]
                [MAX_ACTIONS];  // Q-таблица для хранения знаний агента
  int num_states;   // Количество состояний
  int num_actions;  // Количество действий
} Agent;

/// @brief Функция загрузки лабиринта из файла
/// @param filename
/// @return maze
Maze *load_ml_maze(const char *filename);

/// @brief Функция освобождения памяти, занятой лабиринтом
/// @param maze
void free_ml_maze(Maze *maze);

/// @brief Функция инициализации агента
/// @param agent
/// @param num_states
/// @param num_actions
void initialize_agent(Agent *agent, int num_states, int num_actions);

/// @brief Функция вычисления евклидова расстояния до цели (используется для
/// коррекции награды)
/// @param row
/// @param col
/// @param goal_row
/// @param goal_col
/// @return
double calculate_distance(int row, int col, int goal_row, int goal_col);

/// @brief Функция выбора действия агента с использованием ε-greedy (epsilon
/// жадной) стратегии
/// @param agent
/// @param state
/// @param epsilon
/// @param maze
/// @return
int select_action(Agent *agent, int state, double epsilon, Maze *maze);

/// @brief Функция получения следующего состояния на основе текущего состояния и
/// действия
/// @param state
/// @param action
/// @param maze
/// @param next_row
/// @param next_col
/// @return
int get_next_state(int state, int action, Maze *maze, int *next_row,
                   int *next_col);

/// @brief Функция получения вознаграждения за переход в следующее состояние
/// @param state
/// @param next_state
/// @param maze
/// @param end_row
/// @param end_col
/// @param visited
/// @return
double get_reward(int next_state, Maze *maze, int end_row, int end_col,
                  int *visited);

/// @brief Обновление Q-значений с учетом приближения к цели
/// @param agent
/// @param state
/// @param action
/// @param next_state
/// @param reward
/// @param distance_to_goal
void update_q_value(Agent *agent, int state, int action, int next_state,
                    double reward, double distance_to_goal);

/// @brief Функция обучения агента
/// @param agent
/// @param maze
/// @param start_row
/// @param start_col
/// @param end_row
/// @param end_col
void train_agent(Agent *agent, Maze *maze, int start_row, int start_col,
                 int end_row, int end_col);

/// @brief Функция для печати найденного пути Для дебага, заменить прорисовкой
/// @param start_row
/// @param start_col
/// @param end_row
/// @param end_col
/// @param maze
/// @param agent

void print_ml_path(int start_row, int start_col, int end_row, int end_col,
                   Maze *maze, Agent *agent);

/// @brief Функция для освобождения памяти матрицы пути
/// @param path_matrix
/// @param rows
void free_path_matrix(int **path_matrix, int rows);

/// @brief Функция для выделения памяти под матрицу пути
/// @param rows
/// @param cols
/// @return path_matrix
int **create_path_matrix(int rows, int cols);

/// @brief Функция для сохранения пути в матрицу
/// @param start_row
/// @param start_col
/// @param end_row
/// @param end_col
/// @param maze
/// @param agent
/// @param path_matrix
void save_path(int start_row, int start_col, int end_row, int end_col,
               Maze *maze, Agent *agent, int **path_matrix);

/// @brief Функция для печати матрицы пути
/// @param path_matrix
/// @param rows
/// @param cols
void print_path_matrix(int **path_matrix, int rows, int cols);

/// @brief Функция для чтения данных стен
/// @param file
/// @param wall
/// @param rows
/// @param cols
/// @param wall_type
/// @return
int read_wall_data(FILE *file, int **wall, int rows, int cols,
                   const char *wall_type);
#endif  // MLAGENT