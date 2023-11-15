#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

class MusicPlayer {
public:
    MusicPlayer(const std::string& filePath) : isPaused(false) {
        if (!music.openFromFile(filePath)) {
            throw std::runtime_error("Error al cargar el archivo de música.");
        }
    }

    void play() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!isPaused) {
            music.play();
        }
    }

    void pause() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!isPaused) {
            music.pause();
            isPaused = true;
        }
    }

    void resume() {
        std::lock_guard<std::mutex> lock(mutex);
        if (isPaused) {
            music.play();
            isPaused = false;
        }
    }

    void stop() {
        std::lock_guard<std::mutex> lock(mutex);
        music.stop();
        isPaused = false;
    }

private:
    sf::Music music;
    std::mutex mutex;
    std::atomic<bool> isPaused;
};

void musicControl(MusicPlayer& musicPlayer) {
    while (true) {
        int command;
        std::cout << "Ingrese el comando (1) pausar / (2) reanudar / (3) detener / (4) salir: ";
        std::cin >> command;

        switch (command) {
        case 1:
            musicPlayer.pause();
            std::cout << "Música pausada.\n";
            break;
        case 2:
            musicPlayer.resume();
            std::cout << "Música reanudada.\n";
            break;
        case 3:
            musicPlayer.stop();
            std::cout << "Música detenida.\n";
            break;
        case 4:
            return; // Salir del bucle y finalizar el hilo
        default:
            std::cout << "Comando no reconocido.\n";
        }
    }
}

int main() {
    try {
        MusicPlayer musicPlayer("C:/Users/Luisf/Desktop/Estructuras de Datos/Proyecto Modulo 3/Musica/Franz Ferdinand - This fffire - New Version.wav");

        std::thread musicThread([&]() {
            
            musicPlayer.play();
        });

        std::thread controlThread([&]() {
            musicControl(musicPlayer);
        });

        musicThread.join();
        controlThread.join();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
