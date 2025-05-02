#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <semaphore>
#include <chrono>
#include <random>

class JogoDasCadeiras {
private:
    int cadeiras;
    std::vector<std::string> jogadores;
    std::unique_ptr<std::binary_semaphore> semaphore;
    std::condition_variable music_cv;
    std::mutex mtx;
    bool musica_tocando = true;

public:
    JogoDasCadeiras(int num_jogadores)
        : cadeiras(num_jogadores - 1),
          semaphore(std::make_unique<std::binary_semaphore>(num_jogadores - 1)) {
        for (int i = 1; i <= num_jogadores; ++i) {
            jogadores.push_back("P" + std::to_string(i));
        }
    }

    void iniciar_rodada() {
        std::unique_lock<std::mutex> lock(mtx);
        musica_tocando = true;
        std::cout << "-----------------------------------------------\n";
        std::cout << "Iniciando rodada com " << jogadores.size() << " jogadores e " << cadeiras << " cadeiras.\n";
        std::cout << "A música está tocando... 🎵\n";
        semaphore = std::make_unique<std::binary_semaphore>(cadeiras);
    }

    void parar_musica() {
        std::unique_lock<std::mutex> lock(mtx);
        musica_tocando = false;
        music_cv.notify_all();
        std::cout << "A música parou! Os jogadores estão tentando se sentar...\n";
    }

    bool tentar_ocupar_cadeira(const std::string& jogador) {
        if (semaphore->try_acquire()) {
            std::cout << jogador << " conseguiu uma cadeira.\n";
            return true;
        }
        return false;
    }

    void eliminar_jogador() {
        std::unique_lock<std::mutex> lock(mtx);
        std::vector<std::string> ocupadas(cadeiras, "Vazia");

        for (auto it = jogadores.begin(); it != jogadores.end(); ++it) {
            if (tentar_ocupar_cadeira(*it)) {
                for (auto& cadeira : ocupadas) {
                    if (cadeira == "Vazia") {
                        cadeira = *it;
                        break;
                    }
                }
            } else {
                std::cout << "\n-----------------------------------------------\n";
                for (int i = 0; i < ocupadas.size(); ++i) {
                    std::cout << "[Cadeira " << i + 1 << "]: Ocupada por " << ocupadas[i] << "\n";
                }
                std::cout << "\nJogador " << *it << " não conseguiu uma cadeira e foi eliminado!\n";
                std::cout << "-----------------------------------------------\n";
                jogadores.erase(it);
                break;
            }
        }
        cadeiras--;
    }

    bool jogo_ativo() {
        return jogadores.size() > 1;
    }

    void exibir_vencedor() {
        if (jogadores.size() == 1) {
            std::cout << "\n🏆 Vencedor: " << jogadores[0] << "! Parabéns! 🏆\n";
            std::cout << "-----------------------------------------------\n";
        }
    }

    void aguardar_musica() {
        std::unique_lock<std::mutex> lock(mtx);
        music_cv.wait(lock, [this]() { return !musica_tocando; });
    }
};

void coordenador(JogoDasCadeiras& jogo) {
    while (jogo.jogo_ativo()) {
        jogo.iniciar_rodada();
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 3 + 1));
        jogo.parar_musica();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        jogo.eliminar_jogador();
    }
    jogo.exibir_vencedor();
}

void jogador(JogoDasCadeiras& jogo, const std::string& id) {
    while (jogo.jogo_ativo()) {
        jogo.aguardar_musica();
        if (!jogo.tentar_ocupar_cadeira(id)) {
            break;
        }
    }
}

int main() {
    int num_jogadores;
    std::cout << "-----------------------------------------------\n";
    std::cout << "Bem-vindo ao Jogo das Cadeiras Concorrente!\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Digite o número de jogadores: ";
    std::cin >> num_jogadores;

    JogoDasCadeiras jogo(num_jogadores);
    std::vector<std::thread> threads;

    for (int i = 1; i <= num_jogadores; ++i) {
        threads.emplace_back(jogador, std::ref(jogo), "P" + std::to_string(i));
    }

    std::thread coord(coordenador, std::ref(jogo));

    for (auto& t : threads) {
        t.join();
    }
    coord.join();

    std::cout << "Obrigado por jogar o Jogo das Cadeiras Concorrente!\n";
    return 0;
}
