#include "move.h"
#include "piece.h"
#include "cell.h"

#include <iostream>

using namespace std;

// constructor
Move::Move(Piece *last_piece, Piece *current_piece, Cell *initial_cell, Cell *final_cell, int turn) :
            last_piece{last_piece}, current_piece{current_piece}, 
            initial_cell{initial_cell}, final_cell{final_cell}, turn{turn} {}

// destructor
Move::~Move() {}

// get initial row and column
int Move::get_init_row() {
    return initial_cell->get_row();
}

int Move::get_init_col() {
    return initial_cell->get_col();
}

// get final row and column
int Move::get_fin_row() {
    return final_cell->get_row();
}

int Move::get_fin_col() {
    return final_cell->get_col();
}

// get current and last piece
Piece *Move::get_cur_piece() {
    return current_piece;
}

Piece *Move::get_last_piece() {
    return last_piece;
}

// print
void Move::print() {
    cout << endl;
    cout << "Turn: " << turn << endl;
    cout << "Current Piece: ";
    current_piece->print_piece();
    cout << " " << current_piece->get_id() << endl;
    cout << "Initial Cell: " << initial_cell->get_row() << " " << initial_cell->get_col() << endl;
    cout << "Final Cell: " << final_cell->get_row() << " " << final_cell->get_col() << endl;
    cout << endl;
}
