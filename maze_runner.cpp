#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

// Matriz de char representando o labirinto
std::vector<std::vector<char>> maze;

// Número de linhas e colunas do labirinto
int num_rows = 0;
int num_cols = 0;

// Representação de uma posição
struct pos_t {
    int i;
    int j;
};

// Variável de controle para indicar quando a saída foi encontrada
bool exit_found = false;

// Função que lê o labirinto de um arquivo texto, carrega em memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
    pos_t initial_pos = {-1, -1}; // Inicializa com valor inválido

    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo" << std::endl;
        exit(1);
    }

    file >> num_rows >> num_cols;

    maze.resize(num_rows, std::vector<char>(num_cols));
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            file >> maze[i][j];
            if (maze[i][j] == 'e') {
                initial_pos.i = i;
                initial_pos.j = j;
            }
        }
    }
    
    return initial_pos;
}


void print_maze()
{
	while (!exit_found)
	{
		system("clear");
		for (int i = 0; i < num_rows; ++i) {
            for (int j = 0; j < num_cols; ++j) {
                printf("%c", maze[i][j]);
            }
		    printf("\n");
	    }
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

// Função responsável pela navegação.
// Recebe como entrada a posição initial
void walk(pos_t initial_pos) {
    std::vector<pos_t> valid_positions;
    valid_positions.push_back(initial_pos);

    while (!valid_positions.empty()) {
        if (exit_found) return;  // Verificar se a saída foi encontrada antes de continuar

        pos_t current_pos = valid_positions.back();
        valid_positions.pop_back();
        
        maze[current_pos.i][current_pos.j] = 'o'; // Marcar posição como atual
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Sleep for 500ms


        // Saída encontrada
        if (maze[current_pos.i][current_pos.j] == 's') {
            exit_found = true;
            return; // Saia da função quando a saída for encontrada
        }
        // Saída encontrada
        if (current_pos.i > 0 && maze[current_pos.i - 1][current_pos.j] == 's') {
            exit_found = true;
            return; // Saia da função quando a saída for encontrada
        }
        if (current_pos.i < num_rows - 1 && maze[current_pos.i + 1][current_pos.j] == 's') {
            exit_found = true;
            return; // Saia da função quando a saída for encontrada
        }
        if (current_pos.j > 0 && maze[current_pos.i][current_pos.j - 1] == 's') {
            exit_found = true;
            return; // Saia da função quando a saída for encontrada
        }
        if (current_pos.j < num_cols - 1 && maze[current_pos.i][current_pos.j + 1] == 's') {
            exit_found = true;
            return; // Saia da função quando a saída for encontrada
        } 

        // Verificar posições adjacentes válidas e adicioná-las na pilha
        std::vector<std::thread> new_threads; // Armazenar novas threads
        if (current_pos.i > 0 && maze[current_pos.i - 1][current_pos.j] == 'x') {
            new_threads.emplace_back(walk, pos_t{current_pos.i - 1, current_pos.j});
        }
        if (current_pos.i < num_rows - 1 && maze[current_pos.i + 1][current_pos.j] == 'x') {
            new_threads.emplace_back(walk, pos_t{current_pos.i + 1, current_pos.j});
        }
        if (current_pos.j > 0 && maze[current_pos.i][current_pos.j - 1] == 'x') {
            new_threads.emplace_back(walk, pos_t{current_pos.i, current_pos.j - 1});
        }
        if (current_pos.j < num_cols - 1 && maze[current_pos.i][current_pos.j + 1] == 'x') {
            new_threads.emplace_back(walk, pos_t{current_pos.i, current_pos.j + 1});
        }
        maze[current_pos.i][current_pos.j] = '.'; // Marcar posição como explorada
        // Iniciar novas threads para explorar bifurcações
        for (std::thread& thread : new_threads) {
            thread.join(); // Esperar a nova thread terminar
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_do_labirinto.txt>" << std::endl;
        return 1;
    }

    pos_t initial_pos = load_maze(argv[1]);
    if (initial_pos.i == -1 && initial_pos.j == -1) {
        std::cerr << "Posição inicial não encontrada no labirinto." << std::endl;
        return 1;
    }

    // Iniciar a primeira thread com a posição inicial
    std::thread first_thread(walk, initial_pos);

    first_thread.detach();

    print_maze();

    if (exit_found) {
        std::cout << "Saída encontrada!" << std::endl;
    } else {
        std::cout << "Saída não encontrada." << std::endl;
    }

    return 0;
}
