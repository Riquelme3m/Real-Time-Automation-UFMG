#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>

// Classe TicTacToe
class TicTacToe
{
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex;                   // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv;          // Variável de condição para alternância de turnos
    char current_player;                      // Jogador atual ('X' ou 'O')
    bool game_over;                           // Estado do jogo
    char winner;                              // Vencedor do jogo

public:
    TicTacToe()
    {
        // Inicializar o tabuleiro e as variáveis do jogo
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                board[i][j] = ' ';
            }
        }
        current_player='X';
        game_over = false;
    }

    void display_board()
    {
        // Exibir o tabuleiro no console
        std::cout << "Current board:" << std::endl;
        std::cout<<std::endl;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if(j<2){
                    std::cout << board[i][j] << "  |";
                }
                else{
                    std::cout << board[i][j] ;
                }
            }
            std::cout<<std::endl;
            if (i != 2)
            {
                std::cout << "-----------" << std::endl;
            }
        }
        std::cout << std::endl;

        if(check_draw()){
            std::cout<<"It's a draw"<<std::endl;
            return;
        }
        if(check_win(current_player)){
            std::cout<<"Player "<<current_player<<" wins!"<<std::endl;
            return;
        }
        if(is_game_over())return;
    }

    bool make_move(char player, int row, int col)
    {
        // Implementar a lógica para realizar uma jogada no tabuleiro
        // Utilizar mutex para controle de acesso
        // Utilizar variável de condição para alternância de turnos
        std::unique_lock<std::mutex> lock(board_mutex); // I lock the mutex so that only one player can play at a time
        turn_cv.wait(lock, [&]
                     { return player == current_player; });
        
        board[row][col]=player;

        current_player= (player == 'X')?'O':'X';
        
        turn_cv.notify_one();
        
        return true;
    }

    bool check_win(char player)
    {
        // Verificar se o jogador atual venceu o jogo

        for (int i = 0; i < 3; ++i)
        {
            if (board[i][0]==player &&
                board[i][0] == board[i][1] &&
                board[i][1] == board[i][2] )
                return true;
        }

        
        for (int i = 0; i < 3; ++i)
        {
            if (board[0][i] == player &&
                board[0][i] == board[1][i] &&
                board[1][i] == board[2][i])
                return true;
        }

        
        if (board[0][0] == player &&
            board[0][0] == board[1][1] &&
            board[1][1] == board[2][2])
            return true;

        if (board[0][2] == player &&
            board[0][2] == board[1][1] &&
            board[1][1] == board[2][0])
            return true;

        return false;

    }

    bool check_draw()
    {
        // Verificar se houve um empate
        bool isDraw = true;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(board[i][j]==' ')isDraw=false;
            }
        } 
        return isDraw;
    }

    bool is_game_over()
    {
        // Retornar se o jogo terminou
        
        for (int i = 0; i < 3; ++i)
        {
            if (board[i][0] != ' ' &&
                board[i][0] == board[i][1] &&
                board[i][1] == board[i][2])
                return true;
        }

        
        for (int i = 0; i < 3; ++i)
        {
            if (board[0][i] != ' ' &&
                board[0][i] == board[1][i] &&
                board[1][i] == board[2][i])
                return true;
        }

        
        if (board[0][0] != ' ' &&
            board[0][0] == board[1][1] &&
            board[1][1] == board[2][2])
            return true;

        if (board[0][2] != ' ' &&
            board[0][2] == board[1][1] &&
            board[1][1] == board[2][0])
            return true;

        
        bool full = true;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (board[i][j] == ' ')
                    full = false;

        return full; 
    }

    char get_winner()
    {
        // Retornar o vencedor do jogo ('X', 'O', ou 'D' para empate)

        if(check_draw()){
            return 'D';
        }
        return current_player=='X'? 'O':'X';

    }
    bool isPositionAvailable(int row,int column){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(board[row][column]==' ')return true;
            }
        }
        return false;
    }
};

// Classe Player
class Player
{
private:
    TicTacToe &game;      // Referência para o jogo
    char symbol;          // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe &g, char s, std::string strat)
        : game(g), symbol(s), strategy(strat) {}

    void play()
    {
        // Executar jogadas de acordo com a estratégia escolhida
        if(strategy=="sequential")play_sequential();
        else{
            play_random();
        }
        
    }

private:
    void play_sequential()
    {
        // Implementar a estratégia sequencial de jogadas
        if(!game.is_game_over()){
            for(int i=0;i<3;i++){
                for(int j=0;j<3;j++){
                    if(game.isPositionAvailable(i,j)==true){
                        game.make_move(symbol,i,j);
                    }
                }
            }
        }
        

    }

    void play_random()
    {
        // Implementar a estratégia aleatória de jogadas

        while(!game.is_game_over()){
            int i = rand()%3;
            int j = rand()%3;

        if(game.isPositionAvailable(i,j)==true){
            game.make_move(symbol,i,j);
        
        }
        }
        

    }
};

// Função principal
int main()
{
    // Inicializar o jogo e os jogadores
    TicTacToe *game = new TicTacToe();
    Player *p1 = new Player(*game, 'X', "sequential");
    Player *p2 = new Player(*game, 'O', "random");

    // Criar as threads para os jogadores

    std::thread t1(&Player::play,p1);
    std::thread t2(&Player::play,p2);

    // Aguardar o término das threads

    // Exibir o resultado final do jogo

    while(!game->is_game_over()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        system("clear");
        game->display_board();
        
    }


    t1.join();
    t2.join();

    delete game;
    delete p1;
    delete p2;

    return 0;
}
