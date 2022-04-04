#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {
  public:
    King(int color);
    ~King();
    void print();
    void generate_moves(std::vector<std::vector<Cell*>> board, Cell *cell, int row, int col);
};

#endif
