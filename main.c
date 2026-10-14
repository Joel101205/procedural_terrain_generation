#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int isPowerOfTwo(int x) {
  return (x != 0) && ((x & (x - 1)) == 0);
}


void initializeCorners(float* heightMap, int size) {

  heightMap[0] = (float) rand() / RAND_MAX;
  heightMap[size - 1] = (float) rand() / RAND_MAX;
  heightMap[(size - 1) * size] = (float) rand() / RAND_MAX;
  heightMap[((size - 1) * size) + (size - 1)] = (float) rand() / RAND_MAX;
}

// geneartte a standard normal distribution (mean = 0.0, stddev = 1.0)
double generate_gaussian() {
  
  double u1 = (double) rand() / RAND_MAX;
  double u2 = (double) rand() / RAND_MAX;

  // prevent log(0) error
  if (u1 < 1e-7) u1 = 1e-7;

  return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

void diamondStep(int x, int y, int step, float scale, int size, float* heightMap) {
  int halfStep = step / 2;

  int total = 0;
  float value = 0;

  int left = x - halfStep >= 0;
  int right = x + halfStep < size;
  int up = y - halfStep >= 0;
  int down = y + halfStep < size;

  if (up && left) {
    total++;
    value += heightMap[(y - halfStep) * size + (x - halfStep)];
  }
  if (up && right) {
    total++;
    value += heightMap[(y - halfStep) * size + (x + halfStep)];
  }
  if (down && left) {
    total++;
    value += heightMap[(y + halfStep) * size + (x - halfStep)];
  }
  if (down && right) {
    total++;
    value += heightMap[(y + halfStep) * size + (x + halfStep)];
  }

  heightMap[y * size + x] = value / total + (float) generate_gaussian() * scale;
}

void squareStep(int x, int y, int step, float scale, int size, float* heightMap) {
  int halfStep = step / 2;

  int total = 0;
  float value = 0;

  if (x - halfStep >= 0) {
    value += heightMap[y * size + (x - halfStep)];
    total++;
  }

  if (x + halfStep < size) {
    value += heightMap[y * size + (x + halfStep)];
    total++;
  }

  if (y - halfStep >= 0) {
    value += heightMap[((y - halfStep) * size) + x];
    total++;
  }
  
  if (y + halfStep < size) {
    value += heightMap[((y + halfStep) * size) + x];
    total++;
  }

  heightMap[y * size + x] = value / total + (float) generate_gaussian() * scale;

}

void generate(int size, float roughness, float *heightMap) {
  float scale = 1.0f;

  for (int step = size - 1; step > 1; step /= 2) {
    int halfstep = step / 2;

    for (int y = halfstep; y < size; y += step) {
      for (int x = halfstep; x < size; x += step) {
        diamondStep(x, y, step, scale, size, heightMap);
      }
    }

    for (int y = 0; y < size; y += step) {
      for (int x = halfstep; x < size; x += step) {
        squareStep(x, y, step, scale, size, heightMap);
      }
    }

    for (int y = halfstep; y < size; y += step) {
      for (int x = 0; x < size; x +=step) {
        squareStep(x, y, step, scale, size, heightMap);
      }
    }

    if (roughness == 1.0f) {
      scale /= 2.0f;
    } else {
      scale /= pow(2.0, roughness);
    }
  }
}

void writeVertexPositions(int size, float xScale, float yScale, float heightScale, float *heightMap, FILE *file) {
  char buff[64];

  for (int y = 0; y < size; y++) {
    for (int x = 0; x < size; x++) {
      float xpos = xScale * ((float) x / (float) (size - 1) * 2.0f - 1.0f);
      float ypos = yScale * ((float) y / (float) (size - 1) * 2.0f - 1.0f);
      float zpos = heightScale * heightMap[y * size + x];

      snprintf(buff, sizeof(buff), "v %f %f %f\n", xpos, ypos, zpos);
      fwrite(&buff, sizeof(char), strlen(buff), file);
    }
  }
}

void writeToFile(int size, float xScale, float yScale, float heightScale, float *heightMap, FILE *file) {
  writeVertexPositions(size, xScale, yScale, heightScale, heightMap, file);

  char buff[64];

  for (int y = 0; y < size - 1; y++) {
    for (int x = 0; x < size - 1; x++) {
      int idx0 = y * size + x + 1;
      int idx1 = y * size + (x + 1) + 1;
      int idx2 = (y + 1) * size + (x + 1) + 1;
      int idx3 = (y + 1) * size + x + 1;

      snprintf(buff, sizeof(buff), "f %d %d %d\n", idx0, idx1, idx2);
      fwrite(&buff, sizeof(char), strlen(buff), file);

      snprintf(buff, sizeof(buff), "f %d %d %d\n", idx0, idx2, idx3);
      fwrite(&buff, sizeof(char), strlen(buff), file);
    }
  }
}

// heightMap[i][j] == heightMap[i * size + j]
void generate_terrain(int size, float roughness, float *heightMap) {
  initializeCorners(heightMap, size);
  generate(size, roughness, heightMap);
}

// display help message if the option -H is given
void display_help_message() {
  printf("Help Message\n");
  printf("This program procedurally generates a terrain, using the diamond-square-algorithm.\n");
  printf("The output is stored in a .obj-file.\n");
  printf("Input options:\n");
  printf("-s (int):             resolution/amount of vertices; most be 2^n + 1.\n");
  printf("-x (float):           width of the terrain.\n");
  printf("-y (float):           depth of the terrain.\n");
  printf("-h (float):           height of the terrain.\n");
  printf("-r (float):           controls the magnitude of the random Gaussian displacement added during each iteration.\n");
  printf("                      high roughness value: greater random displacement.\n");
  printf("-o (String)           name of the output file.\n");
  printf("-H                    display help message.\n");
  printf("Default input:        ./diamond_square -s 129 -x 4.0 -y 4.0 -h 1.0 -r 1.0 -o terrain.obj\n");
}


int main(int argc, char *argv[])
{
  srand((unsigned)time(NULL));

  int size = 129;  // -s resolution: how many vertices/details  
  float xScale = 4.0f;  // -x width
  float yScale = 4.0f;  // -y depth
  float heightScale = 1.0f; // -h height
  float roughness = 1.0f; // -r controls how quickly the random displacement decreases (higher roughness = smoother terrain)
  char *filename = "terrain.obj"; // -o name of the output file

  int opt;
  while ((opt = getopt(argc, argv, "s:x:y:h:r:o:H")) != -1) {
    switch (opt) {
      case 's':
        size = atoi(optarg);
        break;
      case 'x':
        xScale = atof(optarg);
        break;
      case 'y':
        yScale = atof(optarg);
        break;
      case 'h':
        heightScale = atof(optarg);
        break;
      case 'r':
        roughness = 1 / atof(optarg);
        break;
      case 'o':
        filename = optarg;
        break;
      case 'H':
        display_help_message();
        break;
      default:
        printf("No valid input, using default values. Use -H to see input options.");
        break;
    }
  }

  if (!isPowerOfTwo(size-1)) {
    printf("Size must be 2^n + 1");
    return EXIT_FAILURE;
  }

  float* heightMap = malloc(size * size * sizeof(float));

  if (heightMap == NULL) {
    perror("malloc failed");
    return EXIT_FAILURE;
  }

  FILE *file = fopen(filename, "w");

  if (file == NULL) {
    perror("fopen");
    free(heightMap);
    return EXIT_FAILURE;
  }

  generate_terrain(size, roughness, heightMap);

  writeToFile(size, xScale, yScale, heightScale, heightMap, file);

  free(heightMap);
  fclose(file);

  return EXIT_SUCCESS;
}

