#include "queen.h"

using namespace std;

// constructor
Queen::Queen(int color) : Piece{color} {}

// destructor
Queen::~Queen() {}

// print
void Queen::print() {
    if (get_color()) {
        cout << "q";
    } else {
        cout << "Q";
    }
}