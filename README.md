# Procedural terrain generation - Diamond-Square-Algorithm

A simple c program that uses the **Diamond-Square algorithm** to generate a height field exported as a triangle mesh in the .obj format.

This implementation is a personal adaptation of the solution to an assignment of the **Realtime Computer Graphics** course at the **Technical University of Munich**.

### Overview

- Procedural terrain generation using the Diamond-Square algorithm
- Configurable terrain resolution and dimensions
- Adjustable height and roughness parameters
- Export with shared vertices and triangle faces in the .obj format
- Command-line interface for configuring generation parameters

## Algorithm

The Diamond-Square algorithm iteratively subdivides a grid to generate terrain at multiple spatial scales.

1. **Corner initialization:** Assign random height values to the four corners of the height map.
2. **Diamond step:** Calculate the center of each square from the average of its four corner heights, then add a random displacement.
3. **Square step:** Calculate the remaining grid points from the average of their neighboring heights, adding another random displacement.
4. **Subdivision:** Halve the step size and repeat the diamond and square steps until the grid is fully populated.

The random displacement is sampled from a standard normal distribution and scaled down at each iteration. The roughness parameter controls how quickly the displacement magnitude decreases, influencing the amount of fine-scale variation in the generated terrain.

## Build

1. Clone the repository and navigate to the project directory

2. Compile the program using the provided Makefile:

```bash
make
```

This produces the `diamond_square` executable.

The project uses compiler warning flags (`-Wall -Wextra`) and optimization (`-O2`). The math library is linked with `-lm`.

To remove the compiled executable:

```bash
make clean
```

## Usage

Run the program with:

```bash
./diamond_square
```
**Visualization of the output:**
<img width="1086" height="633" alt="Plane" src="https://github.com/user-attachments/assets/31a12117-ceae-4c18-9a72-229993a19ca9" />

### Command-line options

| Option | Parameter | Description | Default |
|---|---|---|---|
| `-s` | Integer | Grid resolution; must be of the form \(2^n + 1\) | `129` |
| `-x` | Float | Half-width of the terrain along the X axis | `4.0` |
| `-y` | Float | Half-depth of the terrain along the Y axis | `4.0` |
| `-h` | Float | Vertical height multiplier | `1.0` |
| `-r` | Float | Controls how quickly random displacement decreases between iterations | `1.0` |
| `-o` | String | Filename | `terrain.obj` |
| `-H` | — | Display the help message | — |

The resolution must be \(2^n + 1\), for example `33`, `65`, `129`, or `257`.

**Example:**
```bash
./diamond_square -s 257 -x 10.0 -y 8.0 -h 5.0 -r 1.05 -o mountain.obj
```
<img width="782" height="710" alt="Mountain" src="https://github.com/user-attachments/assets/92f8578b-826e-4e7c-a692-837f99b038f4" />



