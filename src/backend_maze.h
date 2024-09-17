#ifndef MAZE_H
#define MAZE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  int x, y;
} Point;

int rand_range(int n);
int create_matrix(int ***matrix, int rows, int cols);
int check_allocate(int error);
void free_matrix(int **maze, int **maze_vert, int **maze_hor);
void create_vert_line(int **maze, int **maze_vert, int cols, int rows_ind);
void create_hor_line(int **maze, int **maze_hor, int cols, int rows,
                     int rows_ind);
void create_new_set(int **maze, int cols, int rows_ind, int *set);
void generate_maze(int **maze, int **maze_vert, int **maze_hor, int rows,
                   int cols);
void check_last_row(int **maze, int **maze_vert, int rows_ind, int cols);
void merge_cells(int **maze, int row, int cols, int col1);
void copy_and_reset_row(int **maze, int **maze_hor, int cols, int rows_ind);
int open_file(int argc, const char *argv, int *rows, int *cols,
              int ***maze_vert, int ***maze_hor, int ***maze);
void fill_file(int **maze_vert, int **maze_hor, int *rows, int *cols, int error,
               void *file, int number);
void save_in_file(int **maze_vert, int **maze_hor, int *rows, int *cols);

bool is_valid_move(int x, int y, int n, int m, bool visited[n][m]);
bool dfs(int n, int m, int **right_walls, int **bottom_walls,
         bool visited[n][m], Point current, Point end, Point path[],
         int *path_length);

#endif  // MAZE_H