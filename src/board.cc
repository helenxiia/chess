#include "board.h"

#include "cell.h"
#include "move.h"
#include "player.h"
#include "piece.h"
#include "textdisplay.h"


using namespace std;

// static map
map<int, float> Board::score = map<int, float>();

// constructor
Board::Board() : the_board{vector<vector<unique_ptr<Cell>>>()}, previous_moves{vector<unique_ptr<Move>>()}, 
                    pieces{vector<unique_ptr<Piece>>()}, players{vector<unique_ptr<Player>>()}, turn{0}, count{0}, 
                    currently_playing{true}, td{unique_ptr<TextDisplay>{new TextDisplay(this)}} {}

// destructor
Board::~Board() {} 

// set board
void Board::set_board(int row, int col) {
    for (int i = 0; i < row; ++i) {
        vector<unique_ptr<Cell>> line;
        for (int j = 0; j < col; ++j) {
            // create new cells
            line.emplace_back(unique_ptr<Cell>{new Cell(i, j)});
        }
        the_board.emplace_back(move(line));
    }
}

// get text display
TextDisplay *Board::get_td() {
    return td.get();
}

// get number of players
int Board::get_players_size() {
    return (int) players.size();
}

// add player
void Board::add_player(Player *player) {
    player->set_board(this);
    players.emplace_back(unique_ptr<Player>{player});
}

// get player
Player *Board::get_player(int i) {
    return players.at(i).get();
}

// set piece on board
void Board::set_piece(int row, int col, Piece *piece) {
    Cell *cell = the_board[row][col].get();
    cell->set_piece(piece);
    piece->set_cell(cell);
    piece->set_board(this);
    // add to pieces
    auto p = unique_ptr<Piece>{piece};
    pieces.emplace_back(move(p));
    // add to player
    players.at(piece->get_color())->add_piece(piece);
}

// get piece using id
Piece* Board::get_piece(int id) {
    if (id == -1) return nullptr;
    for (auto &piece : pieces) {
        if (piece->get_id() == id) {
            return piece.get();
        }
    } 
    return nullptr;
}

// get reference to all pieces
vector<Piece*> Board::get_ref_pieces() {
    vector<Piece*> ps;
    for (int i = 0; i < (int) pieces.size(); ++i) {
        ps.emplace_back(pieces.at(i).get());
    }
    return ps;
}

// get reference to board
vector<vector<Cell*>> Board::get_ref_board() {
    vector<vector<Cell*>> ref_board;
    for (int i = 0; i < (int) the_board.size(); ++i) {
            vector<Cell*> ref_row;
        for (int j = 0; j < (int) the_board.at(i).size(); ++j) {
            Cell *c = the_board[i][j].get();
            ref_row.emplace_back(c);
        }
        ref_board.emplace_back(ref_row);
    }
    return ref_board;
}

// modify scoreboard
void Board::modify_score(int player, float point) {
    if (!score.count(player)) { // player is not in map yet
        score.insert({player, point});
    } else {
        auto it = score.find(player); // find the current score
        it->second = it->second + point; // add new score to cumulative score
    }
}

// get a score
int Board::get_score(int player) {
    return score.at(player);
}

// get size of score
int Board::score_size() {
    return score.size();
}

// set turn
void Board::set_turn(int color) {
    turn = color;
}

// add a move
void Board::add_move(Move *m) {
    previous_moves.emplace_back(move(unique_ptr<Move>{m}));
}

// get move
Move *Board::get_move(int i) {
    if (previous_moves.size() != 0)  {
        return previous_moves.at(i).get();
    }
    return nullptr;
}

int Board::get_count() {
    return count;
}

// player resigned
int Board::resign() {
    for (int i = 0; i < (int) players.size(); ++i) {
        if (players.at(i)->get_resign()) return i; 
    }
    return -1;
}

// reset the game
void Board::reset() {
    the_board.clear();
    previous_moves.clear();
    pieces.clear();
    players.clear();
    currently_playing = false;
    turn = 0;
    count = 0;
}

// undoes a single move
void Board::undo() {
    int c = count - 1;
    if (previous_moves.size() != 0) {
        Move *last_move = previous_moves.at(previous_moves.size() - 1).get();
        while (last_move->get_turn() == c) {
            last_move->get_init_cell()->set_piece(last_move->get_cur_piece());
            last_move->get_cur_piece()->set_cell(last_move->get_init_cell());
            if (last_move->get_last_piece()) {
                last_move->get_final_cell()->set_piece(last_move->get_last_piece());
                last_move->get_last_piece()->set_cell(last_move->get_final_cell());
            } else {
                last_move->get_final_cell()->remove_piece();
            }
            int rowi = last_move->get_init_cell()->get_row();
            int coli = last_move->get_init_cell()->get_col();
            int rowf = last_move->get_final_cell()->get_row();
            int colf = last_move->get_final_cell()->get_col();
            if (last_move->get_cur_piece()->get_value() == 1) {
                if (rowi - rowf == 2 || rowi - rowf == -2) {
                    last_move->get_cur_piece()->set_has_not_moved(true);
                } else if (rowf == 8 || rowf == 0) {
                    last_move->get_cur_piece()->create_unique_status();
                }
            } else if (last_move->get_cur_piece()->get_value() == 10) {
                if (coli - colf == 2 || coli - colf == -2) {
                    last_move->get_cur_piece()->set_has_not_moved(true);
                }
            } else if (last_move->get_cur_piece()->get_value() == 5) {
                last_move->get_cur_piece()->set_has_not_moved(true);
            }
            previous_moves.pop_back();
            if (previous_moves.size() == 0) break;
            last_move = previous_moves.at(previous_moves.size() - 1).get();
        }
        // create all valid moves for all pieces
        for (int i = 0; i < (int) pieces.size(); ++i) {
            if (pieces[i]->get_value() != 10) {
                pieces[i]->create_valid_moves();
            }
        }
        // notify pieces observers
        for (int i = 0; i < (int) pieces.size(); ++i) {
            pieces[i]->notifyObservers();
        }
        // recreate valid moves for kings to reasses the situation
        for (int i = 0; i < (int) pieces.size(); ++i) {
            if (pieces[i]->get_value() == 10) {
                pieces[i]->create_valid_moves();
            }
        }
        count = c;
        turn = c % players.size();
    }
    td->print_board("chess", -1);
}

// run the game
void Board::run(vector<string> player_names, bool undo_mode) {
    currently_playing = true;
    // first reset game if needed
    if (!player_names.size() == 0) {
        reset();
        currently_playing = true;
        // make players
        try {
            create_players(player_names);
            // initialize board
            init();
        } catch (invalid_argument &r) {
            cerr << "Invalid Player Inputted: Please Declare Human or Computer" << endl;
            currently_playing = false;
        }
    } else {
        td->print_board("chess", -1);
    }
    // initialize score
    for (int i = 0; i < get_players_size(); ++i) {
        if (score.count(i) == 0) {
            modify_score(i, 0);
        }
    }
    // assign id to pieces
    for (int i = 0; i < (int) pieces.size(); ++i) {
        pieces.at(i)->set_id(i);
    }
    // attach cells to all pieces
    for (int i = 0; i < (int) the_board.size(); ++i) {
        for (int j = 0; j < (int) the_board.at(i).size(); ++j) {
            the_board[i][j]->set_all_pieces(get_ref_pieces());
        }
    }
    // create all valid moves for all pieces
    for (int i = 0; i < (int) pieces.size(); ++i) {
        if (pieces[i]->get_value() != 10) {
            pieces[i]->create_valid_moves();
        }
    }
    // notify pieces observers
    for (int i = 0; i < (int) pieces.size(); ++i) {
        pieces[i]->notifyObservers();
    }
    // recreate valid moves for kings to reasses the situation
    for (int i = 0; i < (int) pieces.size(); ++i) {
        if (pieces[i]->get_value() == 10) {
            pieces[i]->create_valid_moves();
        }
    }
    // run
    while(currently_playing) { // while game is playing
        string s;
        if (undo_mode) {
            cout << "Enter 'play' to play a move or 'undo' to undo: " << endl;
            while (getline(cin, s)) {
                if (s == "undo") {
                    undo();
                    break;
                } else if (s == "play") {
                    break;
                } else {
                    cout << "Please enter 'play' to play a move or 'undo' to undo: " << endl;
                }
            }
        }
        if (players.size() == 0) break; // no players so no game is being played
        Player *cur_player = players.at(turn).get(); // get which player is playing, based on the turn
        try {
            // check if game over
            if (game_over()) {
                players.clear(); // players leave
                currently_playing = false;
                break;
            }
            // attempt to make a move
            vector<int> move_info = cur_player->move();
            if (move_info.size() == 0) { 
                cout << "No Move Made" << endl; 
                break;
            } else if (move_info.size() == 1) {
                if (game_over()) {
                    players.clear(); // players leave
                    currently_playing = false;
                    break;
                }
            }

            // create move
            Piece *last_piece = get_piece(move_info[5]); // gets piece ptr based on id
            Piece *cur_piece = get_piece(move_info[4]);
            Cell *init_cell = the_board.at(move_info[0]).at(move_info[1]).get();
            Cell *fini_cell = the_board.at(move_info[2]).at(move_info[3]).get();
            Move *move  = new Move(last_piece, cur_piece, init_cell, fini_cell, count);
            previous_moves.emplace_back(unique_ptr<Move>{move});

            // create all valid moves for all pieces
            for (int i = 0; i < (int) pieces.size(); ++i) {
                if (pieces[i]->get_value() != 10) {
                    pieces[i]->create_valid_moves();
                }
            }
            // notify pieces observers
            for (int i = 0; i < (int) pieces.size(); ++i) {
                pieces[i]->notifyObservers();
            }
            // recreate valid moves for kings to reasses the situation
            for (int i = 0; i < (int) pieces.size(); ++i) {
                if (pieces[i]->get_value() == 10) {
                    pieces[i]->create_valid_moves();
                }
            }

            if (state_valid() != turn) {
                // increment turn
                if (turn == (int) players.size() - 1) { // it is last player's turn
                    turn = 0;
                } else {
                    ++turn;
                }
                // increment count
                ++count;
                td->print_board("chess", -1);
            } else {
                cout << "Game State Is Invalid" << endl;
                // increment turn
                if (turn == (int) players.size() - 1) { // it is last player's turn
                    turn = 0;
                } else {
                    ++turn;
                }
                // increment count
                ++count;
                undo();
            }
            // display
        } catch (...) { // probably should define some error here
            // throw invalid move
        }
    }
    cout << "End Of Game!" << endl;
    // for (auto &move : previous_moves) {
    //     move->print();
    // }
    reset();
}
