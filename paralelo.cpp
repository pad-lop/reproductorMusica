#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>
#include <mutex>

class MusicPlayer {
public:
    MusicPlayer(const std::string& filePath) {
        if (!music.openFromFile(filePath)) {
            std::cerr << "Error al cargar el archivo de música.\n";
            std::exit(1);
        }
        music.setLoop(true);
        isPaused = false;
    }

    void play() {
        music.play();
    }

    void pause() {
        if (!isPaused) {
            music.pause();
            isPaused = true;
            std::cout << "Música pausada.\n";
        } else {
            std::cout << "La música ya está pausada.\n";
        }
    }

    void resume() {
        if (isPaused) {
            music.play();
            isPaused = false;
            std::cout << "Música reanudada.\n";
        } else {
            std::cout << "La música no está pausada.\n";
        }
    }

    void stop() {
        music.stop();
    }

private:
    sf::Music music;
    bool isPaused;
};

class MusicController {
public:
    MusicController(MusicPlayer& player) : musicPlayer(player) {}

    void startControl() {
        while (true) {
            int command;
            std::cout << "Ingrese el comando (1) pausar / (2) reanudar / (3) detener: ";
            std::cin >> command;

            std::unique_lock<std::mutex> lock(mutex);

            switch (command) {
            case 1:
                musicPlayer.pause();
                break;
            case 2:
                musicPlayer.resume();
                break;
            case 3:
                musicPlayer.stop();
                return;
            default:
                std::cout << "Comando no reconocido.\n";
            }
        }
    }

private:
    MusicPlayer& musicPlayer;
    std::mutex mutex;
};

int main() {
    MusicPlayer musicPlayer("C:/Users/Luisf/Desktop/Estructuras de Datos/Proyecto Modulo 3/Musica/Franz Ferdinand - This fffire - New Version.wav");

    std::thread musicThread([&]() {
        musicPlayer.play();
    });

    MusicController musicController(musicPlayer);
    std::thread controlThread([&]() {
        musicController.startControl();
    });

    musicThread.join();
    controlThread.join();

    return 0;
}
