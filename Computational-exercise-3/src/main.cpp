#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <chrono>
#include <cstdlib>
#include <ctime>

class TicTacToe {
private:
    std::array<std::array<char,3>,3> board;
    std::mutex mtx;
    std::condition_variable cv;
    char current_player;
    bool game_over;
    char winner;

public:
    TicTacToe()
      : current_player('X'),
        game_over(false),
        winner(' ')
    {
        for (auto &row : board)
            row.fill(' ');
    }

    void display_board() {
        std::cout<<"Current board: "<<std::endl;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i][j]
                          << (j<2 ? " | " : "");
            }
            std::cout << "\n";
            if (i<2) std::cout << "-----------\n";
        }
        std::cout << "\n";
    }

    bool make_move(char player, int row, int col) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ 
            return game_over || player == current_player; 
        });

        if (game_over)
            return false;

        if (board[row][col] != ' ')
            return false;

        board[row][col] = player;

        if (check_win(player)) {
            game_over = true;
            winner   = player;
        }
        else if (check_draw()) {
            game_over = true;
            winner   = 'D';
        }
        else {
            current_player = (player=='X' ? 'O' : 'X');
        }

        cv.notify_all();
        return true;
    }

    bool check_win(char p) {
        for (int i = 0; i < 3; ++i)
            if ((board[i][0]==p && board[i][1]==p && board[i][2]==p) ||
                (board[0][i]==p && board[1][i]==p && board[2][i]==p))
                return true;
        if ((board[0][0]==p && board[1][1]==p && board[2][2]==p) ||
            (board[0][2]==p && board[1][1]==p && board[2][0]==p))
            return true;
        return false;
    }

    bool check_draw() {
        for (auto &row : board)
            for (char c : row)
                if (c == ' ') return false;
        return true;
    }

    bool is_game_over() {
        std::lock_guard<std::mutex> lock(mtx);
        return game_over;
    }

    char get_winner() {
        std::lock_guard<std::mutex> lock(mtx);
        return winner;
    }

    bool isPositionAvailable(int r, int c) {
        std::lock_guard<std::mutex> lock(mtx);
        return board[r][c] == ' ';
    }
};

class Player {
private:
    TicTacToe &game;
    char sym;
    bool sequential;

public:
    Player(TicTacToe &g, char s, const std::string &strat)
      : game(g), sym(s), sequential(strat == "sequential") {}

    bool flag = false;
    void play() {
        
        while (!game.is_game_over()) {
            bool moved = false;
            if (sequential) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                for (int i = 0; i < 3 && !moved; ++i) {
                    for (int j = 0; j < 3 && !moved; ++j) {
                        if (game.isPositionAvailable(i, j)) {
                            moved = game.make_move(sym, i, j);
                            if (moved && game.check_win(sym)) {
                                flag = true;
                            }
                        }
                        if (moved) break;    
                    }
                    if (moved) break;        
                }
            }
            
            else {
                int i = std::rand() % 3;
                int j = std::rand() % 3;
                if (game.isPositionAvailable(i, j))
                    moved = game.make_move(sym, i, j);
            }
            if(flag)break;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

int main() {
    
    std::srand(unsigned(std::time(nullptr)));

    TicTacToe *game = new TicTacToe();
    Player *p1       = new Player(*game, 'X', "sequential");
    Player *p2       = new Player(*game, 'O', "random");

    std::thread t1(&Player::play, p1);
    std::thread t2(&Player::play, p2);

    while (!game->is_game_over()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        system("clear");
        game->display_board();
    }

    t1.join();
    t2.join();

    system("clear");
    game->display_board();
    char w = game->get_winner();
    if (w == 'D')
        std::cout << "Result: Draw\n\n";
    else
        std::cout << "Player: " << w << " wins\n\n";

    delete game;
    delete p1;
    delete p2;
    return 0;
}
