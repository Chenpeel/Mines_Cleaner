#include "fielddata.h"
#include <iostream>
#include <random>

FieldData *FieldData::instance = nullptr;
FieldData *FieldData::getInstance() {
  if (instance == nullptr) instance = new FieldData();
  return instance;
}

FieldData::~FieldData() { delete instance; }
FieldData::FieldData() : width(0), height(0), mines_num(0) { reset(); }

void FieldData::reset() {
  matrix.clear();
  initMatrix();
  deployMines();
  visited.clear();
  initVisited();
}

void FieldData::initMatrix() {
  matrix.resize(width);
  for (int i = 0; i < width; i++) {
    matrix[i].resize(height);
  }
}

void FieldData::deployMines() {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      matrix[i][j] = 0;
    }
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> pos_x(0, width - 1);
  std::uniform_int_distribution<> pos_y(0, height - 1);
  // 地雷比例 0.15 - 0.3
  std::uniform_int_distribution<> num((int)width * height * 0.08,
                                      (int)width * height * 0.15);
  mines_num = num(gen);
  int n = mines_num;
  while (n > 0) {
    int x = pos_x(gen);
    int y = pos_y(gen);
    if (matrix[x][y] != -1) {
      matrix[x][y] = -1;
      n--;
      updateSurrounding(x, y);
    } else
      continue;
  }
  // debugPrint();
  // for (int i = 0; i < width; i++) {
  //   for (int j = 0; j < height; j++) {
  //     std::cout << matrix[i][j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
}

void FieldData::initVisited() {
  visited.resize(width);
  for (int i = 0; i < width; i++) {
    visited[i].resize(height);
  }
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      visited[i][j] = 0;
    }
  }
}

void FieldData::setWidthHeight(int width, int height) {
  this->width = width;
  this->height = height;
}

void FieldData::updateSurrounding(int x, int y) {
  // Define the 8 directions
  int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                          {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  // Iterate over all directions
  for (int i = 0; i < 8; ++i) {
    int nx = x + directions[i][0];
    int ny = y + directions[i][1];

    // Check if (nx, ny) is within bounds
    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
      if (matrix[nx][ny] != -1) {
        matrix[nx][ny]++;
      }
    }
  }
}
