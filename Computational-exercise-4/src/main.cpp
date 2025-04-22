#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <semaphore>
#include <mutex>
#include <condition_variable>
#include <algorithm>

constexpr int NUM_JOGADORES = 4;

std::mutex cout_mutex;

class JogoDasCadeiras {
private:
    int jogadores_restantes;
    int cadeiras;
    std::vector<std::thread> jogadores;
    std::vector<int> ocupacao;
    std::binary_semaphore musica{1};
    std::counting_semaphore<NUM_JOGADORES>* cadeira_sem;
    bool musica_parada = false;
    bool jogo_ativo = true;

public:
    JogoDasCadeiras()
        : jogadores_restantes(NUM_JOGADORES), cadeiras(NUM_JOGADORES - 1) {
        cadeira_sem = new std::counting_semaphore<NUM_JOGADORES>(cadeiras);
        ocupacao.resize(cadeiras, 0);
    }

    ~JogoDasCadeiras() {
        delete cadeira_sem;
    }

    void iniciar() {
        std::cout << "-----------------------------------------------\n";
        std::cout << "Bem-vindo ao Jogo das Cadeiras Concorrente!\n";
        std::cout << "-----------------------------------------------\n\n";

        for (int i = 0; i < NUM_JOGADORES; ++i) {
            jogadores.emplace_back(&JogoDasCadeiras::jogador, this, i + 1);
        }

        while (jogadores_restantes > 1) {
            iniciar_rodada();

            if (jogadores_restantes == 1) {
                jogo_ativo = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                int vencedor = encontrar_vencedor();
                std::cout << "-----------------------------------------------\n\n";
                std::cout << "🏆 Vencedor: Jogador P" << vencedor << "! Parabéns! 🏆\n";
                std::cout << "-----------------------------------------------\n\n";
                std::cout << "Obrigado por jogar o Jogo das Cadeiras Concorrente!\n";
                break;
            }

            cadeiras--;
        }

        for (auto& t : jogadores) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    void iniciar_rodada() {
        delete cadeira_sem;
        cadeira_sem = new std::counting_semaphore<NUM_JOGADORES>(cadeiras);
        ocupacao.assign(cadeiras, 0);

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "Iniciando rodada com " << jogadores_restantes << " jogadores e " << cadeiras << " cadeiras.\n";
            std::cout << "A música está tocando... 🎵\n\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "> A música parou! Os jogadores estão tentando se sentar...\n\n";
        }

        musica.acquire();
        musica_parada = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    void jogador(int id) {
        while (jogo_ativo) {
            while (!musica_parada && jogo_ativo) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            if (!jogo_ativo) break;

            if (cadeira_sem->try_acquire()) {
                for (int i = 0; i < ocupacao.size(); ++i) {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    if (ocupacao[i] == 0) {
                        ocupacao[i] = id;
                        break;
                    }
                }
            } else {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "-----------------------------------------------\n";
                for (int i = 0; i < ocupacao.size(); ++i) {
                    std::cout << "[Cadeira " << i + 1 << "]: Ocupada por P" << ocupacao[i] << "\n";
                }
                std::cout << "\nJogador P" << id << " não conseguiu uma cadeira e foi eliminado!\n";
                std::cout << "-----------------------------------------------\n\n";
                jogadores_restantes--;
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            musica_parada = false;
            musica.release();
        }
    }

    int encontrar_vencedor() {
        for (int i = 1; i <= NUM_JOGADORES; ++i) {
            bool ainda_no_jogo = std::find(ocupacao.begin(), ocupacao.end(), i) != ocupacao.end();
            if (ainda_no_jogo)
                return i;
        }
        return -1;
    }
};

int main() {
    JogoDasCadeiras jogo;
    jogo.iniciar();
    return 0;
}
