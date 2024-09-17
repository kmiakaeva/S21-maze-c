#include "cave.h"

int **create_cave_matrix(int n, int m) {
  int **matrix = NULL;
  int success = 1;
  matrix = malloc(n * sizeof(int *));
  if (matrix == NULL) {
    perror("Ошибка выделения памяти для матрицы");
    success = 0;
  }
  if (success) {
    for (int i = 0; i < n && success; i++) {
      matrix[i] = malloc(m * sizeof(int));
      if (matrix[i] == NULL) {
        perror("Ошибка выделения памяти для строки матрицы");
        success = 0;
      }
    }
  }
  if (!success) {
    if (matrix != NULL) {
      for (int i = 0; i < n; i++) {
        if (matrix[i] != NULL) {
          free(matrix[i]);
        }
      }
      free(matrix);
    }
    matrix = NULL;
  }
  return matrix;
}

void fill_matrix_random(t_cave *cave, int chance) {
  srand(time(NULL));  // Инициализация генератора случайных чисел

  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      cave->matrix[i][j] =
          rand() % 100 < chance ? 1 : 0;  // 0 или 1 в зависимости от шанса на
                                          // начальную инициализацию клетки
    }
  }
}

t_cave *read_cave_from_file(const char *filename) {
  FILE *file = NULL;
  t_cave *cave = NULL;
  int value = 0, n = 0, m = 0, success = 1;
  if (filename == NULL) {
    success = 0;
  }
  if (success) {
    file = fopen(filename, "r");
    if (file == NULL || fscanf(file, "%d %d", &n, &m) != 2) {
      success = 0;
    }
  }
  if (success) {
    cave = malloc(sizeof(t_cave));
    if (cave == NULL) success = 0;
  }
  if (success) {
    cave->matrix = create_cave_matrix(n, m);
    cave->prev_matrix = create_cave_matrix(n, m);
    if (cave->matrix == NULL || cave->prev_matrix == NULL) {
      success = 0;
    } else {
      cave->n = n;
      cave->m = m;
      cave->iteration = 0;
    }
  }
  if (success) {
    for (int i = 0; i < n && success; i++) {
      for (int j = 0; j < m && success; j++) {
        if (fscanf(file, "%d", &value) != 1) {
          success = 0;
        } else
          cave->matrix[i][j] = value;
      }
    }
  }
  if (!success) {
    if (cave != NULL) {
      free_cave_matrix(cave->matrix, cave->n);
      free_cave_matrix(cave->prev_matrix, cave->n);
      free(cave);
      cave = NULL;
    }
  }
  if (file != NULL) {
    fclose(file);
  }
  return cave;
}

int count_neighbors(t_cave *cave, int x, int y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int ni = x + i, nj = y + j;
      if (!(ni == x && nj == y)) {  // пропускаем саму клетку
        if (ni >= 0 && ni < cave->n && nj >= 0 && nj < cave->m) {
          count += cave->matrix[ni][nj];
        } else
          count++;  // клетка за пределами матрицы считается живой
      }
    }
  }
  return count;
}

bool simulate(t_cave *cave) {
  bool changes = false;  // Флаг для отслеживания изменений
  bool repeats = false;
  int **new_matrix = create_cave_matrix(cave->n, cave->m);

  // Проверка успешности выделения памяти
  if (new_matrix == NULL) {
    fprintf(stderr, "Ошибка выделения памяти для матриц\n");
    return false;  // Возвращаем false в случае ошибки
  }
  int flag = 1;
  for (int i = 0; i < cave->n; i++) {  // Основной цикл симуляции
    for (int j = 0; j < cave->m; j++) {
      int neighbors = count_neighbors(cave, i, j);
      if (cave->matrix[i][j] == 1) {
        new_matrix[i][j] = (neighbors < cave->death_limit) ? 0 : 1;  // смерть
      } else {
        new_matrix[i][j] = (neighbors > cave->birth_limit) ? 1 : 0;  // рождение
      }
      if (new_matrix[i][j] != cave->matrix[i][j]) {
        changes = true;  // Зафиксировано изменение
      }
    }
  }
  if (cave->iteration >= 1) {  // Сравниваем только начиная со 2 итерации
    repeats = compare_matrices(cave->prev_matrix, new_matrix, cave->n, cave->m);
    if (!changes || (cave->iteration >= 2 && repeats)) {
      free_cave_matrix(new_matrix, cave->n);
      flag = 0;  // Выход, если нет изменений или состояние повторяется
    }
  }
  if (flag) {
    copy_matrix(cave->matrix, cave->prev_matrix, cave->n, cave->m);
    copy_matrix(new_matrix, cave->matrix, cave->n, cave->m);
    cave->iteration++;
    free_cave_matrix(new_matrix, cave->n);
  }
  if (flag == 0) {
    changes = false;
  }
  return changes;  // Возвращаем 1, если были изменения, и
                   // 0, если не было
}

void copy_matrix(int **src, int **dest, int n, int m) {
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      dest[i][j] = src[i][j];
    }
  }
}

bool compare_matrices(int **matrix1, int **matrix2, int n, int m) {
  bool check = true;
  if (matrix1 == NULL || matrix2 == NULL) {
    check = false;
  }
  for (int i = 0; i < n && check; i++) {
    if (matrix1[i] == NULL || matrix2[i] == NULL) {
      fprintf(stderr, "Ошибка: одна из строк матриц равна NULL\n");
      check = false;
    }
    for (int j = 0; j < m && check; j++) {
      if (matrix1[i][j] != matrix2[i][j]) {
        check = false;
      }
    }
  }
  return check;
}

void free_cave_matrix(int **matrix, int rows) {
  if (matrix != NULL) {
    for (int i = 0; i < rows; i++) {
      if (matrix[i] != NULL) free(matrix[i]);
    }
    free(matrix);
  }
}

t_cave *initialize_cave(int birth_limit, int death_limit, int choice,
                        const char *filename, int n, int m, int chance) {
  t_cave *cave = malloc(sizeof(t_cave));
  int success = 1;
  if (cave == NULL) {
    perror("Ошибка выделения памяти для структуры t_cave");
    success = 0;
  }
  if (success) {
    cave->matrix = NULL;
    cave->prev_matrix = NULL;
    cave->birth_limit = birth_limit;
    cave->death_limit = death_limit;
    cave->iteration = 0;
    if (choice == 1) {  // Считываем пещеру из файла
      cave = initialize_from_file(cave, filename, &success);
    } else if (choice == 2) {
      cave->n = n;
      cave->m = m;
      cave->matrix = create_cave_matrix(cave->n, cave->m);
      cave->prev_matrix = create_cave_matrix(cave->n, cave->m);
      if (cave->matrix == NULL || cave->prev_matrix == NULL) {
        success = 0;
      } else {
        fill_matrix_random(cave, chance);
        for (int i = 0; i < cave->n; i++) {
          for (int j = 0; j < cave->m; j++) {
            cave->prev_matrix[i][j] = 0;
          }
        }
      }
    } else {
      success = 0;
    }
  }
  cave = not_success(cave, success);
  return cave;
}

t_cave *initialize_from_file(t_cave *cave, const char *filename, int *success) {
  t_cave *file_cave = read_cave_from_file(filename);
  if (file_cave == NULL) {
    *success = 0;
  } else {
    cave->n = file_cave->n;
    cave->m = file_cave->m;
    cave->matrix = create_cave_matrix(cave->n, cave->m);
    cave->prev_matrix = create_cave_matrix(cave->n, cave->m);
    if (cave->matrix == NULL || cave->prev_matrix == NULL) {
      *success = 0;
    } else {
      for (int i = 0; i < cave->n; i++) {
        for (int j = 0; j < cave->m; j++) {
          cave->matrix[i][j] = file_cave->matrix[i][j];
          cave->prev_matrix[i][j] = 0;
        }
      }
    }
    free_cave_matrix(file_cave->matrix, file_cave->n);
    free_cave_matrix(file_cave->prev_matrix, file_cave->n);
    free(file_cave);
  }
  return cave;
}

t_cave *not_success(t_cave *cave, int success) {
  if (!success) {
    if (cave != NULL) {
      if (cave->matrix != NULL) {
        free_cave_matrix(cave->matrix, cave->n);
      }
      if (cave->prev_matrix != NULL) {
        free_cave_matrix(cave->prev_matrix, cave->n);
      }
      free(cave);
    }
    cave = NULL;
  }
  return cave;
}
