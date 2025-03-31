#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <thread>
#include <chrono>
#include <string>

// Representação do Labirinto
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position
{
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::stack<Position> valid_positions;

void divideStringIntoTwo(std::string &str, std::string &first, std::string &second)
{
    bool foundSpace = false;
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == ' ')
        {
            foundSpace = true;
        }
        else if (foundSpace == false)
        {
            first += str[i];
        }
        else
        {
            second += str[i];
        }
    }
}
int convertStrintIntoInteger(std::string s)
{
    int aux, res, j = 1;
    for (int i = s.size() - 1; i >= 0; i--)
    {
        aux = s[i] - '0';
        res += aux * j;
        j *= 10;
    }
    return res;
}

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string &file_name)
{
    std::ifstream ifs;
    Position result;

    try
    {
        // 1. Abra o arquivo especificado por file_name usando std::ifstream

        ifs.open(file_name, std::ifstream::in);
        std::string first_line;
        if (!ifs)
        {
            throw std::runtime_error("Arquivo não encontrado ou formato inválido ! Tente novamente. " + file_name);
        }
        if (ifs.is_open())
        {
            getline(ifs, first_line);
        }
        // 2. Leia o número de linhas e colunas do labirinto

        std::string aux_rows, aux_cols;
        divideStringIntoTwo(first_line, aux_rows, aux_cols);
        num_rows = convertStrintIntoInteger(aux_rows);
        num_cols = convertStrintIntoInteger(aux_cols);

        // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())

        maze.resize(num_rows);
        for (int i = 0; i < maze.size(); i++)
        {
            maze[i].resize(num_cols);
        }
        // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere

        std::string fileContent, line;

        if (ifs.is_open())
        {
            while (getline(ifs, line))
            {
                fileContent += line;
            }
        }
        int positionCounter = 0;
        for (int i = 0; i < num_rows; i++)
        {
            for (int j = 0; j < num_cols; j++)
            {
                maze[i][j] = fileContent[positionCounter++];
            }
        }

        // 5. Encontre e retorne a posição inicial ('e')

        for (int i = 0; i < num_rows; i++)
        {
            if (maze[i][0] == 'e')
            {
                result.row = i;
                result.col = 0;
            }
        }

        // 6. Trate possíveis erros (arquivo não encontrado, formato inválido, etc.)
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    // 7. Feche o arquivo após a leitura

    ifs.close();

    return result;
}

// Função para imprimir o labirinto
void print_maze()
{
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_cols; j++)
        {
            std::cout << maze[i][j];
        }
        std::cout << std::endl;
    }
}

// Função para verificar se uma posição é válida
bool is_valid_position(int row, int col)
{

    // TODO: Implemente esta função
    // 1. Verifique se a posição está dentro dos limites do labirinto
    //    (row >= 0 && row < num_rows && col >= 0 && col < num_cols)
    // 2. Verifique se a posição é um caminho válido (maze[row][col] == 'x')
    // 3. Retorne true se ambas as condições forem verdadeiras, false caso contrário

    if ((row >= 0 && row < num_rows && col >= 0 && col < num_cols) && ((maze[row][col] == 'x') || (maze[row][col] == 's')))
        return true;
    return false;
}

// Função principal para navegar pelo Labirinto
bool walk(Position pos)
{

    if (maze[pos.row][pos.col] == 's')
    {
        maze[pos.row][pos.col] == '.';
        system("cls");
        print_maze();
        return true;
    }
    maze[pos.row][pos.col] = 'o';
    system("cls");
    print_maze();

    maze[pos.row][pos.col] = '.';
    // TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')

    // 2. Chame print_maze() para mostrar o estado atual do labirinto

    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true

    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions

    if (pos.row - 1 >= 0)
    {

        // Verificando se a posição adjacente acima é válida
        if (is_valid_position(pos.row - 1, pos.col))
        {
            Position temp = {pos.row - 1, pos.col};
            valid_positions.push(temp);
        }
    }
    if (pos.row + 1 <= num_rows - 1)
    {

        // Verificando se a posição adjacente abaixo é válida
        if (is_valid_position(pos.row + 1, pos.col))
        {

            Position temp = {pos.row + 1, pos.col};
            valid_positions.push(temp);
        }
    }
    if (pos.col - 1 >= 0)
    {

        // Verificando se a posição adjacente esquerda é válida
        if (is_valid_position(pos.row, pos.col - 1))
        {
            Position temp = {pos.row, pos.col - 1};
            valid_positions.push(temp);
        }
    }
    if (pos.col + 1 <= num_cols - 1)
    {

        // Verificando se a posição adjacente direita é válida
        if (is_valid_position(pos.row, pos.col + 1))
        {
            Position temp = {pos.row, pos.col + 1};
            valid_positions.push(temp);
        }
    }

    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    while (!valid_positions.empty())
    {

        Position positionFromStack = valid_positions.top();
        valid_positions.pop();
        if (walk(positionFromStack))
        {
            maze[positionFromStack.row][positionFromStack.col] = 'o';
            return true;
        }
    }

    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false

    return false;
}

int main(int argc, char *argv[])
{
    // std::cout << "Starting..." << std::endl;

    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position initial_pos = load_maze(argv[1]);
    if (initial_pos.row == -1 || initial_pos.col == -1)
    {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }
    bool exit_found = walk(initial_pos);

    if (exit_found)
    {
        std::cout << "Saída encontrada!" << std::endl;
    }
    else
    {
        std::cout << "Não foi possível encontrar a saída." << std::endl;
    }

    return 0;
}

// Nota sobre o uso de std::this_thread::sleep_for:
//
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
//
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
//
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
//
// Isso pausará a execução por 50 milissegundos.
//
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.