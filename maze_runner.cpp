#include <stdio.h>
#include <stack>
#include <cstdlib>
#include <unistd.h>
#include <thread>

// Matriz de char representando o labirinto
char** maze;

// Numero de linhas e colunas do labirinto
int num_rows = 0;
int num_cols = 0;

// Representação de uma posição
struct pos_t {
    int i;
    int j;
};

// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
std::stack<pos_t> valid_positions;


// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
    pos_t initial_pos = {-1, -1}; // Inicializa com valor inválido

    FILE * pFile;
    pFile = fopen(file_name, "r");
    if (!pFile) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    fscanf(pFile, "%d %d\n", &num_rows, &num_cols);

    maze = (char**)malloc(num_rows * sizeof(char*));
    for (int i = 0; i < num_rows; ++i) {
        maze[i] = (char*)malloc(num_cols * sizeof(char));
        for (int j = 0; j < num_cols; ++j) {
            fscanf(pFile, "%c ", &maze[i][j]);
            if (maze[i][j] == 'e') {
                initial_pos.i = i;
                initial_pos.j = j;
            }
        }
    }
    
    fclose(pFile);
    return initial_pos;
}

// Função que imprime o labirinto com um atraso
void print_maze_with_delay() {
    for (int i = 0; i < num_rows; ++i) {
        for (int j = 0; j < num_cols; ++j) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
    usleep(50000); // Adiciona um atraso
}

// Função responsável pela navegação.
// Recebe como entrada a posição initial e retorna um booleando indicando se a saída foi encontrada
bool walk(pos_t initial_pos) {
    valid_positions.push(initial_pos);
    
    while (!valid_positions.empty()) {
        int val = 0;
        pos_t current_pos = valid_positions.top();
        valid_positions.pop();
        maze[current_pos.i][current_pos.j] = 'o'; // Marcar posição como atual
        system("clear"); // Limpar a tela

        // Saída encontrada
        if (maze[current_pos.i][current_pos.j] == 's') return true;
        if (current_pos.i > 0 && maze[current_pos.i - 1][current_pos.j] == 's') return true;
        if (current_pos.i < num_rows - 1 && maze[current_pos.i + 1][current_pos.j] == 's') return true;
        if (current_pos.j > 0 && maze[current_pos.i][current_pos.j - 1] == 's') return true;
        if (current_pos.j < num_cols - 1 && maze[current_pos.i][current_pos.j + 1] == 's') return true;        
        
        // Verificar posições adjacentes válidas e adicioná-las na pilha
        if (current_pos.i > 0 && maze[current_pos.i - 1][current_pos.j] == 'x') {
            valid_positions.push({current_pos.i - 1, current_pos.j});
            val += 1;
        }
        if (current_pos.i < num_rows - 1 && maze[current_pos.i + 1][current_pos.j] == 'x') {
            valid_positions.push({current_pos.i + 1, current_pos.j});
            val += 1;
        }
        if (current_pos.j > 0 && maze[current_pos.i][current_pos.j - 1] == 'x') {
            valid_positions.push({current_pos.i, current_pos.j - 1});
            val += 1;
        }
        if (current_pos.j < num_cols - 1 && maze[current_pos.i][current_pos.j + 1] == 'x') {
            valid_positions.push({current_pos.i, current_pos.j + 1});
            val += 1;
        }
        maze[current_pos.i][current_pos.j] = '.'; // Marcar posição como explorada

    }
    return false; // Saída não encontrada
}

void *thread_func(pos_t initial_pos) {
    bool exit_found = walk(initial_pos);
    print_maze_with_delay(); // Imprimir o labirinto
}

int main(int argc, char *argv[])
{
	// carregar o labirinto com o nome do arquivo recebido como argumento
    pos_t initial_pos = load_maze(argv[1]);
    if (initial_pos.i == -1 && initial_pos.j == -1) {
        printf("Posição inicial não encontrada no labirinto.\n");
        return 1;
    }

    bool exit_found = walk(initial_pos);
    std::thread thread_func(initial_pos);

    if (exit_found) {
        printf("Saída encontrada!\n");
    } else {
        printf("Saída não encontrada.\n");
    }

    return 0;
}

