#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>

using namespace std;

class Song
{
private:
    int id;
    string nombre_cancion;
    string artista;
    string album;
    string duracion;
    string path;

public:
    Song(int _id, const string &_nombre_cancion, const string &_artista, const string &_album, const string &_duracion, const string &_path)
        : id(_id), nombre_cancion(_nombre_cancion), artista(_artista), album(_album), duracion(_duracion), path(_path) {}

    int getId() const
    {
        return id;
    }

    string getPath() const
    {
        return path;
    }

    const string &getNombreCancion() const
    {
        return nombre_cancion;
    }

    const string &getArtista() const
    {
        return artista;
    }

    void displayInfo() const
    {
        cout << "Reproduciendo: " << nombre_cancion << " - Artista: " << artista << " - Álbum: " << album << " - Duración: " << duracion << endl;
    }
};

class Playlist
{
private:
    string name;

public:
    struct Node
    {
        Song song;
        Node *nextSong;
        Node *prevSong;

        Node(const Song &_song) : song(_song), nextSong(nullptr), prevSong(nullptr) {}
    };

    Node *currentSong;
    sf::Music musicPlayer;

    Playlist(const string &_name) : name(_name), currentSong(nullptr) {}

    void addSong(const Song &song)
    {
        Node *newSong = new Node(song);
        if (!currentSong)
        {
            currentSong = newSong;
            currentSong->nextSong = currentSong;
            currentSong->prevSong = currentSong;
        }
        else
        {
            newSong->nextSong = currentSong->nextSong;
            newSong->prevSong = currentSong;
            currentSong->nextSong->prevSong = newSong;
            currentSong->nextSong = newSong;
        }
    }

    void searchSongByName(const string &query)
    {
        if (!currentSong)
        {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }

        Node *startSong = currentSong;
        do
        {
            if (currentSong->song.getNombreCancion().find(query) != string::npos)
            {
                currentSong->song.displayInfo();
            }
            currentSong = currentSong->nextSong;
        } while (currentSong != startSong);
    }

    void searchSongByArtist(const string &query)
    {
        if (!currentSong)
        {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }

        Node *startSong = currentSong;
        do
        {
            if (currentSong->song.getArtista().find(query) != string::npos)
            {
                currentSong->song.displayInfo();
            }
            currentSong = currentSong->nextSong;
        } while (currentSong != startSong);
    }

    void removeSong(int id)
    {
        if (!currentSong)
        {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }

        Node *startSong = currentSong;
        do
        {
            if (currentSong->song.getId() == id)
            {
                if (currentSong == currentSong->nextSong)
                {
                    delete currentSong;
                    currentSong = nullptr;
                }
                else
                {
                    currentSong->prevSong->nextSong = currentSong->nextSong;
                    currentSong->nextSong->prevSong = currentSong->prevSong;
                    Node *temp = currentSong;
                    currentSong = currentSong->nextSong;
                    delete temp;
                }
                cout << "Canción eliminada con éxito." << endl;
                return;
            }
            currentSong = currentSong->nextSong;
        } while (currentSong != startSong);

        cout << "No se encontró ninguna canción con el ID especificado." << endl;
    }

    void sortAlphabetically()
    {
        if (!currentSong)
        {
            cout << "La lista de reproducción está vacía." << endl;
            return;
        }

        vector<Song> songs;
        Node *startSong = currentSong;

        do
        {
            songs.push_back(currentSong->song);
            currentSong = currentSong->nextSong;
        } while (currentSong != startSong);

        sort(songs.begin(), songs.end(), [](const Song &a, const Song &b)
             { return a.getNombreCancion() < b.getNombreCancion(); });

        currentSong = startSong;

        for (const Song &song : songs)
        {
            currentSong->song = song;
            currentSong = currentSong->nextSong;
        }

        cout << "Lista de reproducción ordenada alfabéticamente." << endl;
    }

    void play()
    {
        if (currentSong)
        {
            Node *startSong = currentSong;
            do
            {
                if (musicPlayer.openFromFile(currentSong->song.getPath()))
                {
                    cout << "Reproduciendo la siguiente canción..." << endl;
                    musicPlayer.play();
                    cout << "Reproduciendo: " << currentSong->song.getNombreCancion() << " - Artista: " << currentSong->song.getArtista() << endl;

                    bool isPaused = false;

                    while (musicPlayer.getStatus() == sf::Music::Playing)
                    {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                        {
                            if (!isPaused)
                            {
                                musicPlayer.pause();
                                isPaused = true;
                                cout << "Pausado" << endl;
                            }
                        }
                        else if (isPaused)
                        {
                            musicPlayer.play();
                            isPaused = false;
                            cout << "Reanudado" << endl;
                        }

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                        {
                            musicPlayer.stop();
                            break;
                        }
                    }

                    musicPlayer.stop();
                }
                else
                {
                    cerr << "Error al abrir el archivo de audio: " << currentSong->song.getPath() << endl;
                }

                currentSong = currentSong->nextSong;
            } while (currentSong != startSong);
        }
        else
        {
            cout << "La lista de reproducción está vacía." << endl;
        }
    }

    ~Playlist()
    {
        while (currentSong)
        {
            Node *temp = currentSong;
            currentSong = currentSong->nextSong;
            delete temp;
        }
    }
};

class Menu
{
public:
    void displayMenu()
    {
        cout << "=========================================" << endl;
        cout << "           Reproductor de Música         " << endl;
        cout << "=========================================" << endl;
        cout << "1. Agregar canción" << endl;
        cout << "2. Buscar canción por nombre" << endl;
        cout << "3. Buscar canción por artista" << endl;
        cout << "4. Eliminar canción" << endl;
        cout << "5. Ordenar alfabéticamente" << endl;
        cout << "6. Reproducir" << endl;
        cout << "7. Salir" << endl;
        cout << "=========================================" << endl;
        cout << "Seleccione una opción: ";
    }
};

int main()
{
    Playlist playlist("Mi Lista de Reproducción");
    Menu menu;
    int choice;
    bool exit = false;

    int id;
    string nombre_cancion, artista, album, duracion, path;
    string nombreBuscar; // Mueve la declaración aquí
    string artistaBuscar; // Mueve la declaración aquí

    while (!exit)
    {
        menu.displayMenu();
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            // Agregar canción
            cout << "Ingrese el ID de la canción: ";
            cin >> id;
            cin.ignore();
            cout << "Ingrese el nombre de la canción: ";
            getline(cin, nombre_cancion);
            cout << "Ingrese el artista: ";
            getline(cin, artista);
            cout << "Ingrese el álbum: ";
            getline(cin, album);
            cout << "Ingrese la duración (mm:ss): ";
            getline(cin, duracion);
            cout << "Ingrese la ruta del archivo de audio: ";
            getline(cin, path);
            playlist.addSong(Song(id, nombre_cancion, artista, album, duracion, path));
            break;
        case 2:
            // Buscar Por Nombre
            cout << "Ingrese el nombre de la canción a buscar: ";
            getline(cin, nombreBuscar);
            playlist.searchSongByName(nombreBuscar);
            break;
        case 3:
            // Buscar por Artista
            cout << "Ingrese el nombre del artista a buscar: ";
            getline(cin, artistaBuscar);
            playlist.searchSongByArtist(artistaBuscar);
            break;
        case 4:
            // Eliminar canción
            int idEliminar;
            cout << "Ingrese el ID de la canción a eliminar: ";
            cin >> idEliminar;
            cin.ignore();
            playlist.removeSong(idEliminar);
            break;
        case 5:
            // Ordenar alfabéticamente
            playlist.sortAlphabetically();
            break;
        case 6:
            // Reproducir
            playlist.play();
            break;
        case 7:
            // Salir
            exit = true;
            break;
        default:
            cout << "Opción no válida. Intente de nuevo." << endl;
            break;
        }
    }

    return 0;
}

