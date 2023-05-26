#include "conway/conway_engine.h"

ConwayEngine::ConwayEngine(int pHeight, int pWidth) : height(pHeight), width(pWidth) {
    cur_grid = new int[height * width];
    new_grid = new int[height * width];

    for (int i = 0; i < (height * width); i++) {
        cur_grid[i] = 0;
        new_grid[i] = 0;
    }
}

void ConwayEngine::Clean() {
    delete[] cur_grid;
    delete[] new_grid;
}

ConwayEngine::~ConwayEngine() {
    Clean();
}

void ConwayEngine::SwapGrids() {
    std::swap(cur_grid, new_grid);
}

void ConwayEngine::SetLife(int y, int x, int val)
{
    new_grid[x + height * y] = val;
}

int ConwayEngine::GetLife(int y, int x)
{
    return cur_grid[x + height * y];
}

void ConwayEngine::SetInitialState(int y, int x, const std::vector<std::string>& rows) {
    for (int i = 0; i < rows.size(); i++)
        for (int j = 0; j < rows[i].size(); j++)
            SetLife((y + i) % height, (x + j) % width, rows[i][j] == '1');
    SwapGrids();
}

int ConwayEngine::GetNumNeighbors(int y, int x)
{
    int count = 0;
    count += GetLife((height - 1 + y) % height, (width - 1 + x) % width);
    count += GetLife((height - 1 + y) % height, (width     + x) % width);
    count += GetLife((height - 1 + y) % height, (width + 1 + x) % width);
    count += GetLife((height     + y) % height, (width - 1 + x) % width);
    count += GetLife((height     + y) % height, (width + 1 + x) % width);
    count += GetLife((height + 1 + y) % height, (width - 1 + x) % width);
    count += GetLife((height + 1 + y) % height, (width     + x) % width);
    count += GetLife((height + 1 + y) % height, (width + 1 + x) % width);
    return count;
}

void ConwayEngine::Process() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int neighbors = GetNumNeighbors(i, j);
            switch(neighbors) {
            case 3:
                SetLife(i, j, 1);
                break;
            case 2:
                SetLife(i, j, GetLife(i, j));
                break;
            default:
                SetLife(i, j, 0);
            }
        }
    }
    SwapGrids();
}