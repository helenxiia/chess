#include <memory>
#include <vector>
#include "piece.h"
#include "board.h"
#include "cell.h"

using namespace std;

Piece::Piece(int color, int value) : cell{nullptr}, color{color}, id{-1}, value{value}, is_taken{false}, has_not_moved{true} {}

Piece::~Piece() {
    // detach all cells in board
    if (board) {
        vector<vector<Cell*>> all_cells = board->get_ref_board();
        for (auto row : all_cells) {
            for (auto cell : row) {
                this->detach(cell);
            }
        }
    }
    // detach from board
    this->detach(board);
}

void Piece::set_cell(Cell *c) {
    cell = c;
    // this->cell->notify();
}

Cell* Piece::get_cell() {
    return cell;
}

int Piece::get_color() { return color; }

int Piece::get_value() { return value; }

int Piece::get_id() { return id; }

void Piece::set_id(int i) { id = i; }

void Piece::set_is_taken(bool b) {
    is_taken = b;
}

int Piece::get_threats() { return threats; }

void Piece::set_has_not_moved() {
    has_not_moved = false;
}

bool Piece::get_has_not_moved() {
    return has_not_moved;
}

int Piece::valid_move(Cell *cell) {
    if (valid_moves.count(cell)) {
        return valid_moves.at(cell);
    }
    return 0;
}

std::unordered_map<Cell*, int> Piece::get_valid_moves() {
    return valid_moves;
}

void Piece::print_piece() {
    print();
}

void Piece::create_valid_moves() {
    valid_moves.clear();
    vector<vector<Cell*>> cur_board = get_board()->get_ref_board();
    Cell *cur_cell = get_cell();
    int cur_row = cur_cell->get_row();
    int cur_col = cur_cell->get_col();
    generate_moves(cur_board, cur_cell, cur_row, cur_col);
}

// modify valid moves
void Piece::modify_valid_moves(Cell *cell, int i) {
    valid_moves[cell] = i;
}

// number of valid moves
int Piece::num_valid_moves() {
    return valid_moves.size();
}

// get the board
Board *Piece::get_board() {
    return board;
}

// set the board
void Piece::set_board(Board *b) {
    board = b;
    // attach to all cells in board
    vector<vector<Cell*>> all_cells = board->get_ref_board();
    for (auto row : all_cells) {
        for (auto cell : row) {
            this->attach(cell);
        }
    }
    // attach to board
    this->attach(board);
} 

// notify observers
void Piece::notifyObservers() {
    // notify all cells
    vector<vector<Cell*>> all_cells = board->get_ref_board();
    for (auto row : all_cells) {
        for (auto cell : row) {
            cell->notify();
        }
    }
    board->notify();
}
