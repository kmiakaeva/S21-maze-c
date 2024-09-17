#include "test.h"

int main() {
  int failed = 0;
  int success = 0;
  Suite *s21_maze_test[] = {test_s21_maze(), NULL};

  for (int i = 0; s21_maze_test[i] != NULL; i++) {
    SRunner *sr = srunner_create(s21_maze_test[i]);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);
    failed += srunner_ntests_failed(sr);
    success += srunner_ntests_run(sr);
    srunner_free(sr);
  }

  printf("\033[41m========= FAILED : %-3d =========\033[0m\n", failed);
  printf("\033[42m\033[30m========= SUCCESS: %-3d =========\033[0m\n", success);
  return failed == 0 ? 0 : 1;
}

START_TEST(s21_maze_1) {
  int rows, cols;
  int ref_rows = 10;
  int ref_cols = 10;
  int ref_maze_vert[] = {0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0,
                         1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1,
                         0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1,
                         0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1,
                         0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1,
                         1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1};
  int **maze = NULL;
  int **maze_vert = NULL;
  int **maze_hor = NULL;

  open_file(2, "files/maze_1", &rows, &cols, &maze_vert, &maze_hor, &maze);
  save_in_file(maze_vert, maze_hor, &rows, &cols);
  ck_assert_int_eq(rows, ref_rows);
  ck_assert_int_eq(cols, ref_cols);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      ck_assert_int_eq(ref_maze_vert[i * rows + j], maze_vert[i][j]);
    }
  }
  free_matrix(maze, maze_vert, maze_hor);
}
END_TEST

START_TEST(s21_maze_2) {
  int rows, cols;
  int **maze = NULL;
  int **maze_vert = NULL;
  int **maze_hor = NULL;

  open_file(1, NULL, &rows, &cols, &maze_vert, &maze_hor, &maze);
  free_matrix(maze, maze_vert, maze_hor);
}
END_TEST

START_TEST(s21_maze_3) {
  int rows, cols;
  int **maze = NULL;
  int **maze_vert = NULL;
  int **maze_hor = NULL;

  open_file(2, "files/maze_3", &rows, &cols, &maze_vert, &maze_hor, &maze);
  Point start = {0, 0};
  Point end = {10, 6};
  bool visited[50][50] = {false};
  Point path[rows * cols];
  int path_length = 0;
  dfs(rows, cols, maze_vert, maze_hor, visited, start, end, path, &path_length);

  free_matrix(maze, maze_vert, maze_hor);
}
END_TEST

START_TEST(s21_cave_1) {
  t_cave *cave = initialize_cave(4, 3, 1, "caves/ex_1.txt", 0, 0, 0);
  ck_assert_msg(cave->matrix[8][1] == 0, "Cave was` loaded incorrectly");
  ck_assert_msg(cave->matrix[0][9] == 0, "Cave was loaded incorrectly");
  ck_assert_msg(cave->matrix[8][2] == 0, "Cave was loaded incorrectly");
  ck_assert_msg(cave->matrix[3][7] == 0, "Cave was loaded incorrectly");
  ck_assert_msg(cave->m == 10, "Cave size m was loaded incorrectly");
  ck_assert_msg(cave->n == 10, "Cave size n was loaded incorrectly");
  for (int i = 0; i < 10; i++) {
    simulate(cave);
  }
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  ck_assert_msg(cave->matrix[8][1] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[0][9] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[8][1] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[3][7] == 1, "Incorrect algorithm");

  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_2) {
  t_cave *cave = initialize_cave(6, 1, 1, "caves/ex_2.txt", 0, 0, 0);
  for (int i = 0; i < 20; i++) {
    simulate(cave);
  }
  ck_assert_msg(cave->matrix[0][0] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[0][19] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[19][3] == 0, "Incorrect algorithm");
  ck_assert_msg(cave->matrix[19][19] == 1, "Incorrect algorithm");
  ck_assert_msg(cave->m == 20, "Cave size m was loaded incorrectly");
  ck_assert_msg(cave->n == 20, "Cave size n was loaded incorrectly");
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_3) {
  t_cave *cave = initialize_cave(2, 5, 1, "caves/ex_3.txt", 0, 0, 0);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  for (int i = 0; i < 20; i++) {
    simulate(cave);
  }
  /* Проверка, что все элементы матрицы равны 1*/
  int check_value = 1;
  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 0) check_value = 0;
    }
  }
  ck_assert_msg(check_value = 1, "Incorrect algorithm");
  ck_assert_msg(cave->m == 50, "Cave size m was loaded incorrectly");
  ck_assert_msg(cave->n == 50, "Cave size n was loaded incorrectly");
  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_4) {
  t_cave *cave = malloc(sizeof(t_cave));
  ck_assert_msg(cave != NULL, "cave memory allocation error");
  free(cave);
}
END_TEST

START_TEST(s21_cave_5) {
  t_cave *cave = malloc(sizeof(t_cave));
  ck_assert_msg(cave != NULL, "cave memory allocation error");
  cave->m = cave->n = 10;
  cave->matrix = create_cave_matrix(cave->n, cave->m);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  free_cave_matrix(cave->matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_6) {
  t_cave *cave = malloc(sizeof(t_cave));
  ck_assert_msg(cave != NULL, "cave memory allocation error");
  cave->m = cave->n = 50;
  cave->matrix = create_cave_matrix(cave->n, cave->m);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  int chance = 30;
  fill_matrix_random(cave, chance);
  int check_value = 0;
  float tmp = 0;
  int res = 0;
  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 1) check_value++;
    }
  }
  tmp = ((float)check_value / (cave->n * cave->m)) * 100;
  res = (int)tmp;
  ck_assert_msg(res <= chance + 2, "Incorrect chance algorithm");
  free_cave_matrix(cave->matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_7) {
  t_cave *cave = malloc(sizeof(t_cave));
  ck_assert_msg(cave != NULL, "cave memory allocation error");
  cave->m = cave->n = 20;
  cave->matrix = create_cave_matrix(cave->n, cave->m);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  int chance = 100;
  fill_matrix_random(cave, chance);
  int check_value = 0;
  float tmp = 0;
  int res = 0;
  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 1) check_value++;
    }
  }
  tmp = ((float)check_value / (cave->n * cave->m)) * 100;
  res = (int)tmp;
  ck_assert_msg(res == chance, "Incorrect chance algorithm");
  free_cave_matrix(cave->matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_8) {
  t_cave *cave = initialize_cave(4, 3, 2, "", 10, 10, 100);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  for (int i = 0; i < 20; i++) {
    simulate(cave);
  }
  /* Проверка, что все элементы матрицы равны 1*/
  int check_value = 1;
  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 0) check_value = 0;
    }
  }
  ck_assert_msg(check_value = 1, "Incorrect algorithm");
  ck_assert_msg(cave->m == 10, "Cave size m was loaded incorrectly");
  ck_assert_msg(cave->n == 10, "Cave size n was loaded incorrectly");
  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_9) {
  t_cave *cave = initialize_cave(4, 3, 2, "", 50, 50, 0);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  for (int i = 0; i < 20; i++) {
    simulate(cave);
  }
  /* Проверка, что все элементы матрицы равны 1*/
  int check_value = 1;
  for (int i = 0; i < cave->n; i++) {
    for (int j = 0; j < cave->m; j++) {
      if (cave->matrix[i][j] == 1) check_value = 0;
    }
  }
  ck_assert_msg(check_value = 1, "Incorrect algorithm");
  ck_assert_msg(cave->m == 50, "Cave size m was loaded incorrectly");
  ck_assert_msg(cave->n == 50, "Cave size n was loaded incorrectly");
  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}
END_TEST

START_TEST(s21_cave_10) {
  t_cave *cave = initialize_cave(10, 3, 2, "", 10, 10, 35);
  ck_assert_msg(cave->matrix != NULL, "cave memory allocation error");
  free_cave_matrix(cave->matrix, cave->n);
  free_cave_matrix(cave->prev_matrix, cave->n);
  free(cave);
}

START_TEST(s21_cave_11) {
  t_cave *cave = NULL;
  cave = initialize_cave(6, 1, 1, "caves/ex_555.txt", 0, 0, 35);
  ck_assert_msg(cave == NULL, "cave memory allocation error");
  free(cave);
}
END_TEST

START_TEST(s21_cave_12) {
  t_cave *cave = initialize_cave(4, 3, 1, "caves/ex_1.txt", 10, 10, 50);
  int **new_matrix = create_cave_matrix(10, 10);
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      new_matrix[i][j] = 0;
      cave->prev_matrix[i][j] = 1;
    }
  }
  bool result = compare_matrices(cave->prev_matrix, new_matrix, 10, 10);
  ck_assert_msg(result == false, "Matrices Equal");
  free_cave_matrix(cave->prev_matrix, 10);
  free_cave_matrix(new_matrix, 10);
  free_cave_matrix(cave->matrix, 10);
  free(cave);
}
END_TEST

START_TEST(s21_cave_13) {
  t_cave *cave = initialize_cave(4, 3, 1, NULL, 10, 10, 50);
  ck_assert_msg(cave == NULL, "Wrong filename");
  free(cave);
}
END_TEST

START_TEST(s21_cave_14) {
  int **new_matrix = create_cave_matrix(10, 10);
  int **prev_matrix = create_cave_matrix(10, 10);
  free_cave_matrix(new_matrix, 10);
  new_matrix = NULL;
  bool result = compare_matrices(new_matrix, prev_matrix, 10, 10);
  ck_assert_msg(result == false,
                "Ошибка: один или оба указателя на матрицы не равны NULL");
  free_cave_matrix(prev_matrix, 10);
}
END_TEST

START_TEST(s21_cave_15) {
  int **new_matrix = create_cave_matrix(10, 10);
  int **prev_matrix = create_cave_matrix(10, 10);
  free_cave_matrix(prev_matrix, 10);
  prev_matrix = NULL;
  bool result = compare_matrices(new_matrix, prev_matrix, 10, 10);
  ck_assert_msg(result == false,
                "Ошибка: один или оба указателя на матрицы не равны NULL");
  free_cave_matrix(new_matrix, 10);
}
END_TEST

START_TEST(s21_ml_1) {
  const char *maze_file = "files/maze_1";
  Maze *maze = load_ml_maze(maze_file);
  ck_assert_msg(maze != NULL, "Maze memory allocation error");
  Agent agent;
  initialize_agent(&agent, maze->rows * maze->cols, MAX_ACTIONS);
  ck_assert_msg(maze->rows == 10, "Maze size m-raws was loaded incorrectly");
  ck_assert_msg(maze->cols == 10, "Maze size n-raws was loaded incorrectly");
  train_agent(&agent, maze, 0, 0, 9, 9);
  free_ml_maze(maze);
}
END_TEST

START_TEST(s21_ml_2) {
  const char *maze_file = "files/maze_1";
  Maze *maze = load_ml_maze(maze_file);
  ck_assert_msg(maze != NULL, "Maze memory allocation error");
  Agent agent;
  initialize_agent(&agent, maze->rows * maze->cols, MAX_ACTIONS);
  ck_assert_msg(maze->rows == 10, "Maze size m-raws was loaded incorrectly");
  ck_assert_msg(maze->cols == 10, "Maze size n-raws was loaded incorrectly");
  train_agent(&agent, maze, 0, 0, 9, 9);
  int **path_matrix = create_path_matrix(maze->rows, maze->cols);
  ck_assert_msg(path_matrix[0][0] == 0, "Path matrix not correct");
  ck_assert_msg(path_matrix[9][9] == 0, "Path matrix not correct");
  save_path(0, 0, 9, 0, maze, &agent, path_matrix);
  ck_assert_msg(path_matrix[0][0] == 1, "Path matrix not correct");
  ck_assert_msg(path_matrix[9][9] == 1, "Path matrix not correct");
  free_path_matrix(path_matrix, maze->rows);
  free_ml_maze(maze);
}
END_TEST

Suite *test_s21_maze(void) {
  Suite *s = suite_create("\033[45m-=S21_MAZE=-\033[0m");
  TCase *tc = tcase_create("s21_maze");

  tcase_add_test(tc, s21_maze_1);
  tcase_add_test(tc, s21_maze_2);
  tcase_add_test(tc, s21_maze_3);
  tcase_add_test(tc, s21_cave_1);
  tcase_add_test(tc, s21_cave_2);
  tcase_add_test(tc, s21_cave_3);
  tcase_add_test(tc, s21_cave_4);
  tcase_add_test(tc, s21_cave_5);
  tcase_add_test(tc, s21_cave_6);
  tcase_add_test(tc, s21_cave_7);
  tcase_add_test(tc, s21_cave_8);
  tcase_add_test(tc, s21_cave_9);
  tcase_add_test(tc, s21_cave_10);
  tcase_add_test(tc, s21_cave_11);
  tcase_add_test(tc, s21_cave_12);
  tcase_add_test(tc, s21_cave_13);
  tcase_add_test(tc, s21_cave_14);
  tcase_add_test(tc, s21_cave_15);
  tcase_add_test(tc, s21_ml_1);
  tcase_add_test(tc, s21_ml_2);

  suite_add_tcase(s, tc);
  return s;
}