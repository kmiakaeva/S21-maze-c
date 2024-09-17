#include "backend_maze.h"

// Функция для генерации случайного числа от 0 до n-1
int rand_range(int n) {
  static int time_count = 0;
  srand(time(NULL) + time_count++);

  return rand() % n;
}

int create_matrix(int ***matrix, int rows, int cols) {
  int error = 0;
  if ((*matrix = (int **)calloc(
           1, rows * cols * sizeof(int) + rows * sizeof(int *))) == NULL) {
    error = 1;
  } else {
    (*matrix)[0] = (int *)(*matrix + rows);
    for (int i = 1; i < rows; i++) {
      (*matrix)[i] = (*matrix)[0] + (i * cols);
    }
  }
  return error;
}

int check_allocate(int error) {
  int err = error == 1 ? 1 : 0;

  return err;
}

void free_matrix(int **maze, int **maze_vert, int **maze_hor) {
  if (maze != NULL) {
    free(maze);
    maze = NULL;
  }
  if (maze_vert != NULL) {
    free(maze_vert);
    maze_vert = NULL;
  }
  if (maze_hor != NULL) {
    free(maze_hor);
    maze_hor = NULL;
  }
}

void create_vert_line(int **maze, int **maze_vert, int cols, int rows_ind) {
  for (int i = 0; i < cols - 1; i++) {
    // если принадлежат одному множеству - создать границу
    if (maze[rows_ind][i] == maze[rows_ind][i + 1]) {
      maze_vert[rows_ind][i] = 1;
    } else {
      int random = rand_range(2);
      if (random == 1) {  // создать границу
        maze_vert[rows_ind][i] = 1;
      } else {
        merge_cells(maze, rows_ind, cols, i);
      }
    }
  }
  maze_vert[rows_ind][cols - 1] = 1;  // добавление правой границы
}

// Функция для объединения множеств двух соседних ячеек
void merge_cells(int **maze, int row, int cols, int col1) {
  int temp = maze[row][col1 + 1];
  for (int j = 0; j < cols; j++) {
    if (maze[row][j] == temp) {
      maze[row][j] = maze[row][col1];
    }
  }
}

void create_hor_line(int **maze, int **maze_hor, int cols, int rows,
                     int rows_ind) {
  int count_walls = 1;  // количество границ
  int count_set = 1;
  if (rows_ind == rows - 1) {  // заполнить нижнюю границу
    for (int i = 0; i < cols; i++) {
      maze_hor[rows_ind][i] = 1;
    }
  } else {
    for (int i = 0; i < cols; i++) {
      if (maze[rows_ind][i] == maze[rows_ind][i + 1] && i != 0) {
        count_set++;
      }
      int random = rand_range(2);
      if (random == 1) {
        maze_hor[rows_ind][i] = 1;
        if (maze[rows_ind][i] == maze[rows_ind][i + 1] && i != 0) count_walls++;
      }
      if (count_walls > 1 && count_set > 1 && count_walls == count_set &&
          (maze[rows_ind][i] != maze[rows_ind][i + 1])) {
        maze_hor[rows_ind][i - rand_range(count_set)] = 0;
        count_set = 1;
        count_walls = 1;
      }
      if (count_set > 1 && count_walls > 1 && count_set != count_walls &&
          (maze[rows_ind][i] != maze[rows_ind][i + 1])) {
        count_set = 1;
        count_walls = 1;
      }
      // проверка одиночных множеств
      if (maze[rows_ind][i] != maze[rows_ind][i + 1]) {
        maze_hor[rows_ind][i] = 0;
      }
      if (i == cols - 1 && maze[rows_ind][i] == maze[rows_ind][i - 1] &&
          ((count_set - count_walls) >= 1)) {
        maze_hor[rows_ind][i] = 1;
      }
    }
  }
}

void create_new_set(int **maze, int cols, int rows_ind, int *set) {
  // присваиваем новые множества вместо удаленных
  for (int j = 0; j < cols; j++) {
    if (maze[rows_ind + 1][j] == 0) {
      maze[rows_ind + 1][j] = *set;
      *set += 1;
    }
  }
}

void check_last_row(int **maze, int **maze_vert, int rows_ind, int cols) {
  rows_ind--;
  for (int i = 0; i < cols - 1; i++) {
    if ((maze[rows_ind][i] != maze[rows_ind][i + 1]) &&
        maze_vert[rows_ind][i]) {
      maze_vert[rows_ind][i] = 0;
      int temp = maze[rows_ind][i + 1];
      for (int j = 0; j < cols; j++) {
        if (maze[rows_ind][j] == temp) maze[rows_ind][j] = maze[rows_ind][i];
      }
    }
  }
}

// Функция для генерации лабиринта по алгоритму Эллера
void generate_maze(int **maze, int **maze_vert, int **maze_hor, int rows,
                   int cols) {
  int set = 2;
  int rows_ind = 0;
  for (int i = 0; i < cols; i++) {
    maze[rows_ind][i] = set;
    set++;
  }
  while (rows > rows_ind) {
    create_vert_line(maze, maze_vert, cols, rows_ind);
    create_hor_line(maze, maze_hor, cols, rows, rows_ind);

    // копируем предыдущую строку в следующую и зануляем множества с гор.
    // стенками
    if (rows_ind + 1 < rows) {
      copy_and_reset_row(maze, maze_hor, cols, rows_ind);
      create_new_set(maze, cols, rows_ind, &set);
    }
    rows_ind++;
  }
  check_last_row(maze, maze_vert, rows_ind, cols);
}

void copy_and_reset_row(int **maze, int **maze_hor, int cols, int rows_ind) {
  for (int j = 0; j < cols; j++) {
    maze[rows_ind + 1][j] = maze[rows_ind][j];
    if (maze_hor[rows_ind][j] == 1) {
      maze[rows_ind + 1][j] = 0;
    }
  }
}

int open_file(int argc, const char *argv, int *rows, int *cols,
              int ***maze_vert, int ***maze_hor, int ***maze) {
  int error = 0;
  if (argc == 2) {
    FILE *file = fopen(argv, "r");
    if (file) {
      int number;
      fscanf(file, "%d", &number);
      *rows = number;
      fscanf(file, "%d", &number);
      *cols = number;
      if (check_allocate(create_matrix(maze_vert, *rows, *cols)) == 0 &&
          check_allocate(create_matrix(maze_hor, *rows, *cols)) == 0) {
        fill_file(*maze_vert, *maze_hor, rows, cols, error, file, number);
        fclose(file);
      }
    } else {
      printf("Memory was not allocated");
      error = 1;
    }
  } else if (argc == 1) {
    *rows = 1 + rand_range(49);
    *cols = 1 + rand_range(49);
    if (check_allocate(create_matrix(maze, *rows, *cols)) == 0 &&
        check_allocate(create_matrix(maze_vert, *rows, *cols)) == 0 &&
        check_allocate(create_matrix(maze_hor, *rows, *cols)) == 0) {
      generate_maze(*maze, *maze_vert, *maze_hor, *rows, *cols);
    } else {
      printf("Memory was not allocated");
      error = 1;
    }
  }
  return error;
}

void fill_file(int **maze_vert, int **maze_hor, int *rows, int *cols, int error,
               void *file, int number) {
  int temp = *rows;
  while (temp > 0 && error == 0) {
    for (int i = 0; i < *rows; i++) {
      for (int j = 0; j < *cols; j++) {
        fscanf(file, "%d", &number);
        maze_vert[i][j] = number;
        temp--;
      }
    }
    for (int i = 0; i < *rows; i++) {
      for (int j = 0; j < *cols; j++) {
        fscanf(file, "%d", &number);
        maze_hor[i][j] = number;
      }
    }
  }
}

void save_in_file(int **maze_vert, int **maze_hor, int *rows, int *cols) {
  FILE *file = fopen("files/out_maze", "w");
  if (file) {
    fprintf(file, "%d %d\n", *rows, *cols);
    for (int i = 0; i < *rows; i++) {
      for (int j = 0; j < *cols; j++) {
        fprintf(file, "%d", maze_vert[i][j]);
        if (j + 1 != *cols) fprintf(file, " ");
      }
      fprintf(file, "\n");
    }
    fprintf(file, "\n");
    for (int i = 0; i < *rows; i++) {
      for (int j = 0; j < *cols; j++) {
        fprintf(file, "%d", maze_hor[i][j]);
        if (j + 1 != *cols) fprintf(file, " ");
      }
      if (i + 1 != *rows) fprintf(file, "\n");
    }

    fclose(file);
  }
}

bool is_valid_move(int x, int y, int n, int m, bool visited[n][m]) {
  return x >= 0 && y >= 0 && x < n && y < m && !visited[x][y];
}

bool dfs(int n, int m, int **right_walls, int **bottom_walls,
         bool visited[n][m], Point current, Point end, Point path[],
         int *path_length) {
  bool error = false;
  if (current.x == end.x && current.y == end.y) {
    path[(*path_length)++] = current;
    error = true;
  }

  if (error != true) {
    visited[current.x][current.y] = true;
    path[(*path_length)++] = current;

    // Направления: вправо, вниз, влево, вверх
    int dx[] = {0, 1, 0, -1};
    int dy[] = {1, 0, -1, 0};

    for (int i = 0; i < 4; i++) {
      int nx = current.x + dx[i];
      int ny = current.y + dy[i];

      // Проверка на правые стены
      if (i == 0 &&
          (current.y == m - 1 || right_walls[current.x][current.y] == 1))
        continue;

      // Проверка на нижние стены
      if (i == 1 &&
          (current.x == n - 1 || bottom_walls[current.x][current.y] == 1))
        continue;

      // Проверка на левые стены
      if (i == 2 &&
          (current.y == 0 || right_walls[current.x][current.y - 1] == 1))
        continue;

      // Проверка на верхние стены
      if (i == 3 &&
          (current.x == 0 || bottom_walls[current.x - 1][current.y] == 1))
        continue;

      if (is_valid_move(nx, ny, n, m, visited)) {
        if (dfs(n, m, right_walls, bottom_walls, visited, (Point){nx, ny}, end,
                path, path_length)) {
          error = true;
        }
      }
    }
    // Убираем из пути, если этот путь не привел к цели
    if (error != true) {
      (*path_length)--;
    }
  }
  return error;
}