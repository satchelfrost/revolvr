#pragma once

#include "pch.h"

class ConwayEngine {
    int* new_grid;
    int* cur_grid;

public:
    const int height;
    const int width;

    ConwayEngine(int height, int width);
    ~ConwayEngine();
    void SetInitialState(int y, int x, const std::vector<std::string>& rows);
    void Process();
    int GetLife(int y, int x);

private:
    void Clean();
    void SwapGrids();
    void SetLife(int y, int x, int val);
    int GetNumNeighbors(int y, int x);
};