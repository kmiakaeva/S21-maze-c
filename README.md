# School 21. Maze

Implementation of the Maze project. This project generates and renders perfect mazes and caves using various algorithms, with additional functionality for maze-solving and reinforcement learning.

## Features Overview

| Part                        | Description                                                              | Status |
| --------------------------- | ------------------------------------------------------------------------ |--------|
| Maze Generation             | Generates perfect mazes using Eller's algorithm                          |   ☑️   |
| Maze Rendering              | Displays mazes in the console with pseudo-graphics                       |   ☑️   |
| Maze Solving                | Shows the solution to any maze on the screen                             |   ☑️   |
| Cave Generation             | Creates caves using cellular automaton                                   |   ☑️   |
| Reinforcement Learning      | Uses Q-learning to find the shortest path in a maze                      |   ☑️   |

## Project Structure

- The program is written in C language following standard code writing practices.
- Source code is located in the `src` folder.
- `Makefile` is provided for building the project, with standard GNU targets: `all`, `install`, `uninstall`, `clean`, and `tests`.
- The program provides a console-based user interface and supports loading mazes from files.

### Maximum Maze Size

- The maze and cave maximum size is 50x50.

## Run Locally

### Prerequisites

- Ensure you have a C compiler installed.
- Make sure to have the `Check` library for unit testing.
- For code coverage, install `gcov` and `lcov`.

### Steps

1. **Clone the repository**

    ```bash
    git clone git@github.com:kmiakaeva/S21-maze-c.git
    ```

2. **Navigate to the project directory**

    ```bash
    cd S21-maze-c/src
    ```

3. **Build the project**

    ```bash
    make
    ```

4. **Run the program**

    ```bash
    ./maze
    ```

5. **Run tests**

    ```bash
    make tests
    ```

6. **Generate code coverage report**

    ```bash
    make gcov_report
    ```

### Additional Commands

1. **Clean build files**

    ```bash
    make clean
    ```

2. **Install the program**

    ```bash
    make install
    ```

3. **Uninstall the program**

    ```bash
    make uninstall
    ```

## Features in Detail

### Load Maze from File
- The program includes an option to load a maze from a file.
- There are 3 predefined maze files included in the project.
- Loaded mazes are rendered on the console using pseudo-graphics, maintaining the specified wall thickness and cell size.

### Maze Generation
- Generates a perfect maze using Eller's algorithm.
- The user defines the maze's dimensions (rows and columns).
- Generated mazes are displayed on the screen and can be saved to a file.

### Maze Solving
- Allows the user to set start and end points within the maze.
- Displays the solution path on the console, highlighting it differently from walls and fields.

### Cave Generation (Bonus)
- Uses cellular automaton to generate caves.
- Supports step-by-step rendering with user-defined birth and death limits for cells.

### Reinforcement Learning (Bonus)
- Implements Q-learning for finding the shortest path in a maze.
- Allows interaction with a trained agent, starting from a user-defined point.

## Testing
- Unit tests are provided for maze generation, solving, and cave generation modules.
- Tests are executed using the `Check` library.
