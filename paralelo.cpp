#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mutex;

bool isPaused = false;

// Función que controla la reproducción de la música
void musicControl(sf::Music &music)
{
    while (true)
    {
        int command;
        std::cout << "Ingrese el comando (1) pausar / (2) reanudar / (3) detener";
        std::cin >> command;

        std::unique_lock<std::mutex> lock(mutex);

        switch (command)
        {

        case 1:
            if (!isPaused)
            {
                music.pause();
                isPaused = true;
                std::cout << "Música pausada.\n";
            }
            else
            {
                std::cout << "La música ya está pausada.\n";
            }
            break;

        case 2:
            if (isPaused)
            {
                music.play();
                isPaused = false;
                std::cout << "Música reanudada.\n";
            }
            else
            {
                std::cout << "La música no está pausada.\n";
            }
            break;

        case 3:
            music.stop();
            break;

        default:
            std::cout << "Comando no reconocido.\n";
        }
    }
}

int main()
{
    // Cargar el archivo de música en formato WAV
    sf::Music music;
    if (!music.openFromFile("C:/Users/Luisf/Desktop/Estructuras de Datos/Proyecto Modulo 3/Musica/Franz Ferdinand - This fffire - New Version.wav"))
    {
        std::cerr << "Error al cargar el archivo de música.\n";
        return 1;
    }

    // Crear un hilo para la reproducción de música
    std::thread musicThread([&]()
                            {
                                music.setLoop(true); // Repetir la música
                                music.play();        // Reproducir la música
                            });

    // Crear un hilo para el control de la música
    std::thread controlThread([&]()
                              { musicControl(music); });

    // Esperar a que ambos hilos terminen
    musicThread.join();
    controlThread.join();

    return 0;
}
