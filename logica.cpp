#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <codecvt>
#include <locale>
#include <thread>
#include <chrono>
#include <mutex>

#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Sleep.hpp>

using namespace std;

bool reproducir = false;

std::mutex mtx;

bool esEntero(const string &dato)
{
    int caracteresRaros = 0;

    for (int i = 0; i < dato.size(); i++)
    {
        if ((i == 0) && (dato[i] == '-'))
        {
            caracteresRaros--;
        }

        if (!isdigit(dato[i]))
        {
            caracteresRaros++;
        }
    }
    if (caracteresRaros == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void validarParametroEntero(int *x, string x_prevalidar, string texto)
{
    do
    {
        cout << "--> " << texto << ": ";
        getline(cin, x_prevalidar);

        if (!esEntero(x_prevalidar))
        {
            cout << "\033[1;31m ERR: Dato no Valido - Debe ser Entero \033[0m\n"
                 << endl;
        }
    } while (!esEntero(x_prevalidar));
    *x = stod(x_prevalidar);
}

double getAudioFileDuration(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening file " << filename << std::endl;
        return -1.0; // Indicates an error
    }

    // Read the chunk ID
    char chunkID[4];
    file.read(chunkID, 4);

    if (std::string(chunkID, 4) != "RIFF")
    {
        std::cerr << "Not a valid RIFF file" << std::endl;
        system("pause");

        return -1.0; // Indicates an error
    }

    // Read the file size
    uint32_t fileSize;
    file.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));

    // Read the format (should be "WAVE")
    char format[4];
    file.read(format, 4);

    if (std::string(format, 4) != "WAVE")
    {
        std::cerr << "Not a valid WAVE file" << std::endl;

        system("pause");
        return -1.0; // Indicates an error
    }

    // Search for the "fmt " subchunk
    while (true)
    {
        char subchunkID[4];
        file.read(subchunkID, 4);

        if (std::string(subchunkID, 4) == "fmt ")
        {
            // Skip the next 4 bytes (subchunk size)
            file.seekg(4, std::ios::cur);

            // Read the audio format (should be 1 for PCM)
            uint16_t audioFormat;
            file.read(reinterpret_cast<char *>(&audioFormat), sizeof(audioFormat));

            if (audioFormat != 1)
            {
                std::cerr << "Unsupported audio format" << std::endl;
                system("pause");

                return -1.0; // Indicates an error
            }

            // Read the number of channels (1 for mono, 2 for stereo)
            uint16_t numChannels;
            file.read(reinterpret_cast<char *>(&numChannels), sizeof(numChannels));

            // Read the sample rate
            uint32_t sampleRate;
            file.read(reinterpret_cast<char *>(&sampleRate), sizeof(sampleRate));

            // Skip the next 6 bytes
            file.seekg(6, std::ios::cur);

            // Read the bits per sample
            uint16_t bitsPerSample;
            file.read(reinterpret_cast<char *>(&bitsPerSample), sizeof(bitsPerSample));

            // Search for the "data" subchunk
            while (true)
            {
                char subchunkID[4];
                file.read(subchunkID, 4);

                if (std::string(subchunkID, 4) == "data")
                {
                    // Skip the next 4 bytes (subchunk size)
                    file.seekg(4, std::ios::cur);

                    // Calculate the duration based on file size
                    double duration = static_cast<double>(fileSize - 44) / (numChannels * bitsPerSample / 8.0) / sampleRate;

                    // Close the file and return the duration
                    file.close();
                    return duration;
                }
                else
                {
                    // Skip the subchunk size
                    uint32_t subchunkSize;
                    file.read(reinterpret_cast<char *>(&subchunkSize), sizeof(subchunkSize));
                    file.seekg(subchunkSize, std::ios::cur);
                }
            }
        }
        else
        {
            // Skip the subchunk size
            uint32_t subchunkSize;
            file.read(reinterpret_cast<char *>(&subchunkSize), sizeof(subchunkSize));
            file.seekg(subchunkSize, std::ios::cur);
        }
    }
}

string convertirRuta(const string &rutaOriginal)
{
    string rutaConvertida;

    for (char caracter : rutaOriginal)
    {
        switch (caracter)
        {
        case '\\':
            // Reemplazar barras invertidas por barras normales
            rutaConvertida.push_back('/');
            break;
        case '"':
            // Omitir comillas
            break;
        default:
            // Mantener los demas caracteres sin cambios
            rutaConvertida.push_back(caracter);
        }
    }

    // Eliminar la barra invertida al final, si la hay
    while (!rutaConvertida.empty() && rutaConvertida.back() == '/')
    {
        rutaConvertida.pop_back();
    }

    return rutaConvertida;
}

template <typename T>
class Nodo
{
public:
    T id;
    T nombre;
    T artista;
    T album;
    T genero;
    T directorio;

    int duracion;

    Nodo *siguiente;

    Nodo();
    Nodo(T, T, T, T, T, T);
    ~Nodo();

    void imprimir_nodo();
};

template <typename T>
Nodo<T>::Nodo()
{
    id = T();
    nombre = T();

    artista = T();
    album = T();
    genero = T();
    directorio = T();

    siguiente = this;
}

template <typename T>
Nodo<T>::Nodo(T id_, T nombre_, T artista_, T album_, T genero_, T directorio_)
{
    id = id_;
    nombre = nombre_;

    artista = artista_;
    album = album_;
    genero = genero_;
    directorio = directorio_;

    siguiente = this;
}

template <typename T>
Nodo<T>::~Nodo() {}

template <typename T>
void Nodo<T>::imprimir_nodo()
{
    cout << "   ID: " << id << endl;
    cout << "   Nombre: " << nombre << endl;
    cout << "   Artista: " << artista << endl;
    cout << "   Album: " << album << endl;
    cout << "   Genero: " << genero << endl;
    cout << "   Directorio: " << directorio << endl;
    cout << "   Duracion: " << duracion << endl;
}

template <typename T>
class ListaCircular
{
private:
    int contadorIDs;

public:
    ListaCircular();
    ~ListaCircular();

    Nodo<T> *ptrCabeza;

    bool reproduciendo = false;
    sf::Music musicPlayer;

    void reproducirMusica();

 void incrementarContadorID();
    Nodo<T> *encontrarUltimoNodo();
    void reiniciarContadorIDs();

    void pausarMusica();
    void reproducirCancion();

    void vaciar();

    void imprimir_lista();

    void agregar_al_final(T, T, T, T, T);
    void agregar_al_principio(T, T, T, T, T);

    void guardar_en_archivo(string);
    void cargar_desde_archivo(string);
};

template <typename T>
ListaCircular<T>::ListaCircular() : contadorIDs(0)
{
    ptrCabeza = nullptr;
}

template <typename T>
ListaCircular<T>::~ListaCircular()
{
    while (ptrCabeza)
    {
        Nodo<T> *temp = ptrCabeza;
        if (temp->siguiente == ptrCabeza)
        {
            ptrCabeza = nullptr;
        }
        else
        {
            ptrCabeza = ptrCabeza->siguiente;
            Nodo<T> *ultimo = ptrCabeza;
            while (ultimo->siguiente != temp)
            {
                ultimo = ultimo->siguiente;
            }
            ultimo->siguiente = ptrCabeza;
        }
        delete temp;
    }
}

template <typename T>
void ListaCircular<T>::reiniciarContadorIDs()
{
    contadorIDs = 0;
}

template <typename T>
void ListaCircular<T>::guardar_en_archivo(string nombre_archivo)
{
    fstream archivo;
    archivo.open(nombre_archivo, ios::out);

    Nodo<T> *temp = ptrCabeza;

    if (temp != nullptr)
    {
        do
        {
            archivo << temp->nombre << "," << temp->artista << "," << temp->album << "," << temp->genero << "," << temp->directorio << endl;
            temp = temp->siguiente;
        } while (temp != ptrCabeza);
    }

    archivo.close();
}

template <typename T>
void ListaCircular<T>::cargar_desde_archivo(string nombre_archivo)
{
    vaciar();

    fstream archivo;
    archivo.open(nombre_archivo, ios::in);

    if (!archivo)
    {
        cout << "\033[1;31m ERR: No se pudo abrir el archivo \033[0m" << endl;
        system("pause");

        return;
    }

    while (!archivo.eof())
    {
        T nombre, artista, album, genero, directorio;

        getline(archivo, nombre, ',');
        getline(archivo, artista, ',');
        getline(archivo, album, ',');
        getline(archivo, genero, ',');
        getline(archivo, directorio);

        agregar_al_final(nombre, artista, album, genero, directorio);
    }

    archivo.close();
}

/*
template <typename T>
bool ListaCircular<T>::validarDirectorio(const T &directorio_)
{
    do {

    }
    if (filesystem::exists(directorio_))
    {
        return true
    }
    else
    {
        cout << "\033[1;31m ERR: Directorio no valido o archivo no encontrado \033[0m\n"
             << endl;
        system("pause");

        return false
    }
}
*/

template <typename T>
void ListaCircular<T>::agregar_al_principio(T nombre_, T artista_, T album_, T genero_, T directorio_)
{
    incrementarContadorID();
    Nodo<T> *nuevoNodo = new Nodo<T>(to_string(contadorIDs), nombre_, artista_, album_, genero_, directorio_);
    nuevoNodo->duracion = getAudioFileDuration(directorio_);

    if (!ptrCabeza)
    {
        ptrCabeza = nuevoNodo;
        nuevoNodo->siguiente = ptrCabeza;
    }
    else
    {
        Nodo<T> *ultimo = encontrarUltimoNodo();
        nuevoNodo->siguiente = ptrCabeza;
        ptrCabeza = nuevoNodo;
        ultimo->siguiente = ptrCabeza;
    }
}

template <typename T>
void ListaCircular<T>::agregar_al_final(T nombre_, T artista_, T album_, T genero_, T directorio_)
{
    incrementarContadorID();
    Nodo<T> *nuevoNodo = new Nodo<T>(to_string(contadorIDs), nombre_, artista_, album_, genero_, directorio_);
    nuevoNodo->duracion = getAudioFileDuration(directorio_);

    if (!ptrCabeza)
    {
        ptrCabeza = nuevoNodo;
    }
    else
    {
        Nodo<T> *temp = ptrCabeza;
        while (temp->siguiente != ptrCabeza)
        {
            if (temp->siguiente->id == to_string(contadorIDs))
            {
                incrementarContadorID();
                nuevoNodo->id = to_string(contadorIDs);
                temp = ptrCabeza; // Reinicia la búsqueda desde el principio
            }
            else
            {
                temp = temp->siguiente;
            }
        }
        nuevoNodo->siguiente = temp->siguiente;
        temp->siguiente = nuevoNodo;
    }
}

template <typename T>
void ListaCircular<T>::incrementarContadorID()
{
    contadorIDs++;
}

template <typename T>
Nodo<T> *ListaCircular<T>::encontrarUltimoNodo()
{
    Nodo<T> *temp = ptrCabeza;
    while (temp->siguiente != ptrCabeza)
    {
        temp = temp->siguiente;
    }
    return temp;
}

template <typename T>
void ListaCircular<T>::imprimir_lista()
{
    Nodo<T> *temp = ptrCabeza;

    if (!ptrCabeza)
    {
        cout << "\033[1;31m Lista vacia \033[0m\n"
             << endl
             << endl;
    }
    else
    {
        cout << "\033[1;32m Lista de Canciones \033[0m" << endl
             << endl;
        int count = 1;

        do
        {
            cout << "Cancion " << count << ":" << endl;
            temp->imprimir_nodo();
            cout << endl;
            temp = temp->siguiente;
            count++;
        } while (temp != ptrCabeza);
    }
}

template <typename T>
void ListaCircular<T>::vaciar()
{
    while (ptrCabeza)
    {
        Nodo<T> *temp = ptrCabeza;
        if (temp->siguiente == ptrCabeza)
        {
            ptrCabeza = nullptr;
        }
        else
        {
            ptrCabeza = temp->siguiente;
            Nodo<T> *ultimo = ptrCabeza;
            while (ultimo->siguiente != temp)
            {
                ultimo = ultimo->siguiente;
            }
            ultimo->siguiente = ptrCabeza;
        }
        delete temp;
    }

    cout << endl
         << "\033[1;32m Lista Vacia \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::reproducirMusica()
{
    if (ptrCabeza)
    {
        reproduciendo = true;
        musicPlayer.play();
    }
    else
    {
        cout << "\033[1;31m Lista vacía \033[0m\n"
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::pausarMusica()
{
    if (ptrCabeza)
    {
        musicPlayer.pause();
    }
    else
    {
        cout << "\033[1;31m Lista vacía \033[0m\n"
             << endl;
    }
}

class Menu
{
public:
    static int mostrarMenuPrincipal();
    static void ejecutarOpcion(int opcion, ListaCircular<string> &lista_circular);
};

int Menu::mostrarMenuPrincipal()
{
    int opcion;
    string opcion_validar;

    do
    {
        cout << "----- MENU -----" << endl
             << endl
             << "1) Insertar al Principio\n"
             << "2) Insertar al Final\n"
             << "3) Buscar por Posicion\n"
             << "4) Modificar\n"
             << "5) Remover\n"
             << "6) Ordenar Ascendente\n"
             << "7) Ordenar Descendente\n"
             << "8) Ordenar por Nombre\n"
             << "9) Invertir\n"
             << "10) Vaciar Lista\n"
             << "11) Guardar en .txt - Se sobreescribe el txt\n"
             << "12) Cargar de .txt - Se sobreescribe la lista\n"
             << "13) Reproducir Musica\n"
             << "14) Pausar Musica\n"
             << endl
             << "0) Salir\n"
             << endl;

        cout << "Ingrese la opcion deseada: ";
        getline(cin, opcion_validar);
        opcion = atoi(opcion_validar.c_str());
    } while (opcion < 1 || opcion > 18);

    return opcion;
}

void Menu::ejecutarOpcion(int opcion, ListaCircular<string> &lista_circular)
{
    string id, nombre, artista, album, genero, directorio;

    int id_int;
    string id_validar, posicion_validar;

    string nuevo_id, nuevo_nombre, nuevo_artista, nuevo_album, nuevo_genero, nuevo_directorio;

    string nombre_archivo = "logica.txt";

    switch (opcion)
    {
    case 1:

        cout << "--> Ingrese el Nombre: ";
        getline(cin, nombre);
        cout << "--> Ingrese el Artista: ";
        getline(cin, artista);
        cout << "--> Ingrese el Album: ";
        getline(cin, album);
        cout << "--> Ingrese el Genero: ";
        getline(cin, genero);
        cout << "--> Ingrese el Directorio: ";
        getline(cin, directorio);

        directorio = convertirRuta(directorio);

        lista_circular.agregar_al_principio(nombre, artista, album, genero, directorio);
        break;
    case 2:

        cout << "--> Ingrese el Nombre: ";
        getline(cin, nombre);
        cout << "--> Ingrese el Artista: ";
        getline(cin, artista);
        cout << "--> Ingrese el Album: ";
        getline(cin, album);
        cout << "--> Ingrese el Genero: ";
        getline(cin, genero);
        cout << "--> Ingrese el Directorio: ";
        getline(cin, directorio);

        directorio = convertirRuta(directorio);

        lista_circular.agregar_al_final(nombre, artista, album, genero, directorio);
        break;

    case 11:
        lista_circular.guardar_en_archivo(nombre_archivo);
        cout << "\033[1;32m Lista guardada en el archivo exitosamente \033[0m\n"
             << endl;
        system("pause");
        break;
    case 12:
        lista_circular.cargar_desde_archivo(nombre_archivo);
        cout << "\033[1;32m Lista cargada desde el archivo exitosamente \033[0m\n"
             << endl;
        system("pause");

        break;
    case 13:
        lista_circular.reproducirMusica();
        break;
    case 14:
        lista_circular.pausarMusica();
        break;

    case 0:
        exit(0);
    default:
        cout << "Opcion no valida." << endl;
    }
}

void hiloReproducirMusica(ListaCircular<string> &lista_circular)
{
    while (true)
    {
        if (lista_circular.ptrCabeza && lista_circular.reproduciendo)
        {
            std::lock_guard<std::mutex> lock(mtx);

            if (filesystem::path(lista_circular.ptrCabeza->directorio).extension() != ".wav")
            {
                cerr << "Formato de archivo no compatible: " << lista_circular.ptrCabeza->directorio << endl;
                return;
            }

            if (!lista_circular.musicPlayer.openFromFile(lista_circular.ptrCabeza->directorio))
            {
                cerr << "Error al abrir el archivo de audio: " << lista_circular.ptrCabeza->directorio << endl;
                return;
            }

            lista_circular.musicPlayer.play();

            // Esperar la duración de la canción actual
            sf::sleep(lista_circular.musicPlayer.getDuration());

            // Pausar si no se desea reproducir la siguiente canción
            if (!lista_circular.reproduciendo)
            {
                lista_circular.musicPlayer.pause();
                continue;
            }

            lista_circular.ptrCabeza = lista_circular.ptrCabeza->siguiente;
        }
        else if (!lista_circular.reproduciendo)
        {
            lista_circular.musicPlayer.pause();
        }
    }
}

void hiloEjecutarMenu(ListaCircular<string> &lista_circular)
{
    int opcion;
    do
    {
        system("cls");
        cout << endl
             << "Reproduciendo: " << lista_circular.reproduciendo << endl
             << endl;
        lista_circular.imprimir_lista();
        opcion = Menu::mostrarMenuPrincipal();

        Menu::ejecutarOpcion(opcion, lista_circular);

    } while (opcion != 0);
}

int main()
{
    ListaCircular<string> lista_circular;

    std::thread hiloMenu(hiloEjecutarMenu, std::ref(lista_circular));
    std::thread hiloReproduccion(hiloReproducirMusica, std::ref(lista_circular));

    hiloMenu.join();         // Espera a que termine el hiloMenu
    hiloReproduccion.join(); // Espera a que termine el hiloMenu

    return 0;
}