#ifndef CAVE_H
#define CAVE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct S21_Cave {
  int n;  // количество строк
  int m;  // количество столбцов
  int **matrix;  // двумерный массив, представляющий пещеру
  int birth_limit;  // порог для рождения новой клетки
  int death_limit;    // порог для смерти клетки
  int **prev_matrix;  // матрица предыдущего состояния
  int iteration;      // номер итерации
} t_cave;

/// @brief Функция для инициализации пещеры
/// @param bl
/// @param dl
/// @param filename
/// @return cave
t_cave *initialize_cave(int birth_limit, int death_limit, int choice,
                        const char *filename, int n, int m, int chance);

/// @brief Функция для создания матрицы
/// @param n
/// @param m
/// @return matrix
int **create_cave_matrix(int n, int m);

/// @brief Функция для заполнения матрицы случайными значениями с учетом шанса
/// на начальную инициализацию
/// @param cave
/// @param chance
void fill_matrix_random(t_cave *cave, int chance);

/// @brief Функция для освобождения памяти
/// @param cave
void free_cave_matrix(int **matrix, int rows);

/// @brief Функция для чтения пещеры из файла
/// @param filename
/// @return cave
t_cave *read_cave_from_file(const char *filename);

/// @brief Инициализация пещеры из файла
/// @param cave
/// @param filename
/// @param success
/// @return
t_cave *initialize_from_file(t_cave *cave, const char *filename, int *success);

/// @brief Функция для подсчета соседей
/// @param cave
/// @param x
/// @param y
/// @return count
int count_neighbors(t_cave *cave, int x, int y);

/// @brief Функция для применения правил клеточного автомата
/// @param cave
bool simulate(t_cave *cave);

/// @brief Дополнительная функция обработки ошибкок
/// @param cave
/// @param success
/// @return cave
t_cave *not_success(t_cave *cave, int success);

/// @brief Функция копирования матриц
/// @param src
/// @param dest
/// @param n
/// @param m
void copy_matrix(int **src, int **dest, int n, int m);

/// @brief Функция сравнения матриц
/// @param matrix1
/// @param matrix2
/// @param n
/// @param m
/// @return check
bool compare_matrices(int **matrix1, int **matrix2, int n, int m);

#endif  // CAVE_H