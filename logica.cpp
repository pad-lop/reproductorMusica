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

#include <algorithm>
#include <random>

#include <windows.h>
#include <cstdlib>

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>


#include <sstream>  // Necesario para usar stringstream
#include <cstring>  // Necesario para usar strcpy
#include <conio.h>
#include <mmsystem.h>

using namespace std;
std::mutex mtx;

const char *red = "\033[31m";
const char *green = "\033[32m";
const char *yellow = "\033[33m";
const char *blue = "\033[34m";
const char *magenta = "\033[35m";
const char *cyan = "\033[36m";
const char *white = "\033[37m";
const char *orange = "\033[38;5;208m";
const char *pink = "\033[38;5;206m";
const char *purple = "\033[38;5;141m";
const char *teal = "\033[38;5;45m";
const char *gray = "\033[38;5;240m";

// Reset ANSI escape code
const char *resetCode = "\033[0m";



const char* colors[] = {
        "\033[31m", // Red
        "\033[32m", // Green
        "\033[33m", // Yellow
        "\033[34m", // Blue
        "\033[35m", // Magenta
        "\033[36m", // Cyan
        "\033[38;5;208m", // Orange
        "\033[38;5;206m", // Pink
        "\033[38;5;141m", // Purple
        "\033[38;5;45m",  // Teal
    };


void gotoxy(int x, int y) {
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

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
    bool continuar = true;
    do
    {
        gotoxy(145, 26);
        cout << "                              ";

        gotoxy(145, 26);
        cout << texto << ": ";
        getline(cin, x_prevalidar);

        gotoxy(145, 27);
        cout << "                              ";
        
        if (x_prevalidar.empty())
        {
            gotoxy(145, 27);
            cout << red << "ERR: Dato no Ingresado" << resetCode << endl;
        }
        else if (!esEntero(x_prevalidar))
        {
            gotoxy(145, 27);
            cout << red << "ERR: Dato no Valido" << resetCode << endl;

        }
        else
        {
            continuar = false;
        }

    } while (continuar);

    *x = stoi(x_prevalidar);
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



void printCharacterAt(int x, int y, char asciiChar) {
    gotoxy(x, y);
    std::cout << asciiChar;
}

void printLine(int x, int y, char asciiChar, int length, const std::string& direction) {
    if (direction == "horizontal") {
        for (int i = 0; i < length; ++i) {
            printCharacterAt(x + i, y, asciiChar);
        }
    } else if (direction == "vertical") {
        for (int i = 0; i < length; ++i) {
            printCharacterAt(x, y + i, asciiChar);
        }
    } else {
        std::cerr << "Invalid direction. Use 'horizontal' or 'vertical'." << std::endl;
    }
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

    Nodo *anterior;  // Puntero al nodo anterior
    Nodo *siguiente; // Puntero al siguiente nodo
    Nodo *actual;    // Puntero al siguiente nodo

    Nodo();
    Nodo(T, T, T, T, T, T);
    ~Nodo();

    Nodo<T> *avanzar(int n);

    void imprimir_nodo(int x, int y);
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
    anterior = nullptr;
    siguiente = nullptr;
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

    anterior = nullptr;
    siguiente = nullptr;
}

template <typename T>
Nodo<T>::~Nodo() {}

template <typename T>
void Nodo<T>::imprimir_nodo(int x, int y)
{
    gotoxy(x+1, y); cout << id; 
    gotoxy(x+6, y); cout << nombre;
    gotoxy(x+35, y);cout << artista ;
    gotoxy(x+65, y);cout << album;
    gotoxy(x+100, y);cout << genero;
}

template <typename T>
Nodo<T> *Nodo<T>::avanzar(int n)
{
    Nodo<T> *temp = this;

    for (int i = 0; i < n && temp != nullptr; ++i)
    {
        temp = temp->siguiente;
    }

    return temp;
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

    int contadorBarra = 0;
    bool aumentarBarra = false;

    sf::Music musicPlayer;

    void reproducirMusica();
    void incrementarContadorID();
    Nodo<T> *encontrarUltimoNodo();

    void siguienteCancion();
    void anteriorCancion();
    void pausarMusica();

    void reiniciarContadorIDs();

    void vaciar();
    void imprimir_lista();
    void agregar_al_final(T, T, T, T, T);
    void agregar_al_principio(T, T, T, T, T);
    void guardar_en_archivo(string);
    void cargar_desde_archivo(string);



    void buscar_por_posicion(int posicion);
    void modificar(T, T, T, T, T, T);
    void eliminar(T);

    void ordenar_descendentemente();
    void ordenar_ascendentemente();
    void ordenar_por_nombre();
    void ordenar_aleatoriamente();

    void invertir();

    void buscar_por_id(T);
    void buscar_nombre(T);
    void buscar_artista(T);
    void buscar_genero(T);

    void displaySpectrogram(const sf::SoundBuffer &buffer);
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
            delete temp;
        }
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
    reiniciarContadorIDs();

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

        if (!nombre.empty() && !artista.empty() && !album.empty() && !genero.empty() && !directorio.empty())
        {
            agregar_al_final(nombre, artista, album, genero, directorio);
        }
    }

    archivo.close();
}

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
        nuevoNodo->anterior = ptrCabeza;
        nuevoNodo->actual = nuevoNodo;
        siguienteCancion();
    }
    else
    {
        Nodo<T> *ultimo = encontrarUltimoNodo();
        nuevoNodo->siguiente = ptrCabeza;
        nuevoNodo->anterior = ultimo;
        ptrCabeza->anterior = nuevoNodo; // Se añade esta línea para ajustar el nodo anterior del primer nodo
        ptrCabeza = nuevoNodo;
        ultimo->siguiente = ptrCabeza;
        nuevoNodo->actual = nuevoNodo;
        siguienteCancion();
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
        nuevoNodo->siguiente = ptrCabeza;
        nuevoNodo->anterior = ptrCabeza;
        nuevoNodo->actual = nuevoNodo;
    }
    else
    {
        Nodo<T> *ultimo = encontrarUltimoNodo();
        nuevoNodo->siguiente = ptrCabeza;
        nuevoNodo->anterior = ultimo;
        ultimo->siguiente = nuevoNodo;
        nuevoNodo->actual = nuevoNodo;
        ptrCabeza->anterior = nuevoNodo; // Se añade esta línea para ajustar el nodo anterior del primer nodo
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
void ListaCircular<T>::modificar(T id_, T nuevo_nombre, T nuevo_artista, T nuevo_album, T nuevo_genero, T nuevo_directorio)
{
    Nodo<T> *temp = ptrCabeza;
    bool modificado = false;

    do
    {
        if (temp->id == id_)
        {
            temp->nombre = nuevo_nombre;
            temp->artista = nuevo_artista;
            temp->album = nuevo_album;
            temp->genero = nuevo_genero;
            temp->directorio = nuevo_directorio;

            temp->duracion = getAudioFileDuration(nuevo_directorio);

            cout << "\033[1;32m Cancion modificado. \033[0m" << endl
                 << endl;
            modificado = true;
            break;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!modificado)
    {
        cout << "\033[1;31m Cancion no encontrado. No se realizo ninguna modificacion. \033[0m\n"
             << endl
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::eliminar(T id_)
{
    Nodo<T> *temp = ptrCabeza;
    Nodo<T> *prev = nullptr;
    bool eliminado = false;

    do
    {
        if (temp->id == id_)
        {
            if (prev == nullptr)
            {
                if (temp->siguiente == temp)
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
            }
            else
            {
                prev->siguiente = temp->siguiente;
            }

            delete temp;
            cout << "\033[1;32m Cancion eliminado. \033[0m\n"
                 << endl
                 << endl;

            eliminado = true;
            break;
        }
        prev = temp;
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!eliminado)
    {
        cout << "\033[1;31m Cancion no encontrado. No se realizo ninguna eliminacion. \033[0m\n"
             << endl
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::invertir()
{
    if (!ptrCabeza || !ptrCabeza->siguiente)
    {
        return; // La lista esta vacia o solo tiene un elemento, no se puede invertir.
    }

    Nodo<T> *current = ptrCabeza;
    Nodo<T> *prev = nullptr;

    do
    {
        Nodo<T> *temp = current->siguiente;
        current->siguiente = prev;
        prev = current;
        current = temp;
    } while (current != ptrCabeza);

    // Actualizar el puntero de la cabeza para que apunte al ultimo elemento invertido.
    ptrCabeza->siguiente = prev;
    ptrCabeza = prev;
}

template <typename T>
void ListaCircular<T>::ordenar_aleatoriamente()
{
    if(ptrCabeza){
        
    
    // Obtén la semilla actual del reloj para generar números aleatorios diferentes en cada ejecución
    srand(time(nullptr));

    // Contamos la cantidad de nodos en la lista
    int cantidad_nodos = 0;
    Nodo<T> *temp = ptrCabeza;

    do
    {
        cantidad_nodos++;
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    // Aplicamos el algoritmo de Fisher-Yates para ordenar aleatoriamente los nodos
    for (int i = cantidad_nodos - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);

        // Intercambiamos los datos de los nodos
        std::swap(temp->id, temp->avanzar(j)->id);
        std::swap(temp->nombre, temp->avanzar(j)->nombre);
        std::swap(temp->artista, temp->avanzar(j)->artista);
        std::swap(temp->album, temp->avanzar(j)->album);
        std::swap(temp->genero, temp->avanzar(j)->genero);
        std::swap(temp->directorio, temp->avanzar(j)->directorio);
        std::swap(temp->duracion, temp->avanzar(j)->duracion);
    }
    }
}

template <typename T>
void ListaCircular<T>::ordenar_por_nombre()
{
    Nodo<T> *current = ptrCabeza;

    if (current == nullptr)
    {
        cout << "La lista esta vacia. No se puede ordenar." << endl;
        return;
    }

    do
    {
        Nodo<T> *min = current;
        Nodo<T> *temp = current->siguiente;

        while (temp != ptrCabeza)
        {
            if (temp->nombre < min->nombre)
            {
                min = temp;
            }
            temp = temp->siguiente;
        }

        if (min != current)
        {
            swap(current->id, min->id);
            swap(current->nombre, min->nombre);
            swap(current->artista, min->artista);
            swap(current->album, min->album);
            swap(current->genero, min->genero);
            swap(current->directorio, min->directorio);
            swap(current->duracion, min->duracion);
        }

        current = current->siguiente;
    } while (current != ptrCabeza);

    cout << endl
         << "\033[1;32mLista Ordenada Alfabeticamente \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::ordenar_ascendentemente()
{
    if (ptrCabeza == nullptr)
    {
        cout << "La lista esta vacia. No se puede ordenar." << endl;
        return;
    }

    Nodo<T> *current = ptrCabeza;

    do
    {
        Nodo<T> *min = current;
        Nodo<T> *temp = current->siguiente;

        while (temp != ptrCabeza)
        {
            int id1 = stoi(min->id);  // Convertir ID a entero
            int id2 = stoi(temp->id); // Convertir ID a entero

            if (id2 < id1) // Comparar IDs como numeros enteros
            {
                min = temp;
            }
            temp = temp->siguiente;
        }

        if (min != current)
        {
            swap(current->id, min->id);
            swap(current->nombre, min->nombre);
            swap(current->artista, min->artista);
            swap(current->album, min->album);
            swap(current->genero, min->genero);
            swap(current->directorio, min->directorio);
            swap(current->duracion, min->duracion);
        }

        current = current->siguiente;
    } while (current != ptrCabeza);

    cout << endl
         << "\033[1;32mLista Ordenada Ascendentemente por ID \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::ordenar_descendentemente()
{
    if (ptrCabeza == nullptr)
    {
        cout << "La lista esta vacia. No se puede ordenar." << endl;
        return;
    }

    Nodo<T> *current = ptrCabeza;

    do
    {
        Nodo<T> *max = current;
        Nodo<T> *temp = current->siguiente;

        while (temp != ptrCabeza)
        {
            int id1 = stoi(max->id);  // Convertir ID a entero
            int id2 = stoi(temp->id); // Convertir ID a entero

            if (id2 > id1) // Comparar IDs como numeros enteros
            {
                max = temp;
            }
            temp = temp->siguiente;
        }

        if (max != current)
        {
            swap(current->id, max->id);
            swap(current->nombre, max->nombre);
            swap(current->artista, max->artista);
            swap(current->album, max->album);
            swap(current->genero, max->genero);
            swap(current->directorio, max->directorio);
            swap(current->duracion, max->duracion);
        }

        current = current->siguiente;
    } while (current != ptrCabeza);

    cout << endl
         << "\033[1;32mLista Ordenada Descendentemente por ID \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::buscar_por_posicion(int posicion)
{
    if (ptrCabeza == nullptr)
    {
        cout << "La lista esta vacia. No se puede buscar por posicion." << endl;
        return;
        system("pause");
    }

    Nodo<T> *temp = ptrCabeza;
    int count = 1;

    do
    {
        if (count == posicion)
        {
            cout << "\033[1;32m Cancion encontrado en la posicion " << posicion << " \033[0m" << endl
                 << endl;

            temp->imprimir_nodo(0, 0);
            
            cout << endl;
            system("pause");

            return;
        }
        temp = temp->siguiente;
        count++;
    } while (temp != ptrCabeza);

    cout << "\033[1;31m Cancion no encontrado en la posicion " << posicion << " \033[0m" << endl
         << endl;

    system("pause");
}

template <typename T>
void ListaCircular<T>::buscar_nombre(T nombre_)
{
    Nodo<T> *temp = ptrCabeza;
    bool encontrado = false;

    do
    {
        if (temp->nombre == nombre_)
        {
            cout << "\033[1;32m Cancion encontrado \033[0m" << endl
                 << endl;
            temp->imprimir_nodo(0, 0);
            cout << endl;
            system("pause");
            encontrado = true;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!encontrado)
    {
        cout << "\033[1;31m No se encontraron Canciones con ese nombre. \033[0m\n"
             << endl
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::buscar_por_id(T id_)
{
    Nodo<T> *temp = ptrCabeza;
    bool encontrado = false;

    do
    {
        if (temp->id == id_)
        {
            cout << "\033[1;32m Canción encontrada \033[0m" << endl
                 << endl;
            temp->imprimir_nodo(0, 0);
            cout << endl;
            system("pause");
            encontrado = true;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!encontrado)
    {
        cout << "\033[1;31m No se encontraron canciones con ese ID. \033[0m\n"
             << endl
             << endl;
        system("pause");
    }
}

template <typename T>
void ListaCircular<T>::buscar_artista(T artista_)
{
    Nodo<T> *temp = ptrCabeza;
    bool encontrado = false;

    do
    {
        if (temp->artista == artista_)
        {
            cout << "\033[1;32m Canción encontrada \033[0m" << endl
                 << endl;
            temp->imprimir_nodo(0, 0);
            cout << endl;
            system("pause");
            encontrado = true;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!encontrado)
    {
        cout << "\033[1;31m No se encontraron canciones con ese artista. \033[0m\n"
             << endl
             << endl;
        system("pause");
    }
}

template <typename T>
void ListaCircular<T>::buscar_genero(T genero_)
{
    Nodo<T> *temp = ptrCabeza;
    bool encontrado = false;

    do
    {
        if (temp->genero == genero_)
        {
            cout << "\033[1;32m Canción encontrada \033[0m" << endl
                 << endl;
            temp->imprimir_nodo(0, 0);
            cout << endl;
            system("pause");
            encontrado = true;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!encontrado)
    {
        cout << "\033[1;31m No se encontraron canciones con ese género. \033[0m\n"
             << endl
             << endl;
        system("pause");
    }
}

template <typename T>
void ListaCircular<T>::imprimir_lista()
{
    Nodo<T> *temp = ptrCabeza;

    if (!ptrCabeza)
    {
        gotoxy(70, 23); cout << "\033[1;31m Lista vacia \033[0m";
    }
    else
    {
        printLine(1,0,char(205),142, "horizontal");
        printLine(0,1,char(186),7, "vertical");
        
        printCharacterAt(0, 0, char(201));

        printCharacterAt(0, 2, char(204));
        printCharacterAt(0, 4, char(204));
        printCharacterAt(0, 8, char(204));
       
        gotoxy(45, 1); cout<< "Cancion Seleccionada    ---   Duracion: " << ptrCabeza->duracion << " segundos";


        printLine(1,2,char(205),142, "horizontal");
        
        ptrCabeza->imprimir_nodo(5,3);

        printLine(1,4,char(205),142, "horizontal");

        gotoxy(2, 9);
        cout << "ID      CANCION                        ARTISTA                          ALBUM                              GENERO" << endl;
        printLine(1,10,char(205),142, "horizontal");

        printCharacterAt(143, 10, char(185));
        printCharacterAt(0, 10, char(204));

        printCharacterAt(145-2, 1+1, char(206));
        printCharacterAt(145-2, 1+3, char(185));

        printCharacterAt(145-2, 1-1, char(203));

        
        int count = 1;
        do
        {
            temp->imprimir_nodo(2, 11+count);
            temp = temp->siguiente;
            count++;
        } while (temp != ptrCabeza);

        cout << endl;
    }
}

template <typename T>
void ListaCircular<T>::vaciar()
{
    contadorBarra = 0;
    aumentarBarra = false;

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
        musicPlayer.setLoop(true);
        reproduciendo = true;
        musicPlayer.play();
        
        aumentarBarra = true;

    }
    else
    {
        musicPlayer.setLoop(false);

        cout << "\033[1;31m Lista vacia \033[0m\n"
             << endl;
        system("pause");
    }
}

template <typename T>
void ListaCircular<T>::pausarMusica()
{

    if (ptrCabeza)
    {
        musicPlayer.pause();
        aumentarBarra = false;

    }
    else
    {
        cout << "\033[1;31m Lista vacia \033[0m\n"
             << endl;
        system("pause");
    }
}


template <typename T>
void ListaCircular<T>::siguienteCancion()
{
    if (ptrCabeza)
    {
        ptrCabeza = ptrCabeza->siguiente;
        if (reproduciendo)
        {
            musicPlayer.stop();
            musicPlayer.openFromFile(ptrCabeza->directorio);
            musicPlayer.play();

            contadorBarra = 0;
            aumentarBarra = true;

        }

    }
    else
    {

        cout << "\033[1;31m Lista vacía \033[0m\n"
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::anteriorCancion()
{
    if (ptrCabeza)
    {
        ptrCabeza = ptrCabeza->anterior;
        if (reproduciendo)
        {
            musicPlayer.stop();
            musicPlayer.openFromFile(ptrCabeza->directorio);
            musicPlayer.play();

            contadorBarra = 0;
            aumentarBarra = true;
        }
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
    static int mostrarMenuPrincipal( ListaCircular<string> &lista_circular);
    static void ejecutarOpcion(int opcion, ListaCircular<string> &lista_circular);
};

int Menu::mostrarMenuPrincipal( ListaCircular<string> &lista_circular)
{
    int opcion;
    string opcion_validar;

    do
    {
    
        int x = 145;
        int y = 3;
        gotoxy(x, y + 1); cout << green << "1) Insertar al Principio" << resetCode << endl;
        gotoxy(x, y + 2); cout << green << "2) Insertar al Final" << resetCode << endl;
        gotoxy(x, y + 3); cout << cyan << "3) Modificar" << resetCode << endl;
        gotoxy(x, y + 4); cout << cyan << "4) Remover" << resetCode << endl;
        gotoxy(x, y + 5); cout << orange << "5) Invertir" << resetCode << endl;
        gotoxy(x, y + 6); cout << orange << "6) Vaciar Lista" << resetCode << endl;


        gotoxy(x, y + 7); if(lista_circular.ptrCabeza){cout<< purple;}else{cout <<red;} cout << "7) Ordenar Ascendente" << resetCode << endl;
        gotoxy(x, y + 8); if(lista_circular.ptrCabeza){cout<< purple;}else{cout <<red;} cout  << "8) Ordenar Descendente" << resetCode << endl;
        gotoxy(x, y + 9); if(lista_circular.ptrCabeza){cout<< purple;}else{cout <<red;} cout  << "9) Ordenar Alfabeticamente" << resetCode << endl;
        gotoxy(x, y + 10); if(lista_circular.ptrCabeza){cout<< purple;}else{cout <<red;} cout  << "10) Ordenar Aleatorio" << resetCode << endl;
     
        gotoxy(x, y + 11); if(lista_circular.ptrCabeza){cout<< teal;}else{cout <<red;} cout << "11) Buscar por Posicion" << resetCode << endl;
        gotoxy(x, y + 12); if(lista_circular.ptrCabeza){cout<< teal;}else{cout <<red;} cout << "12) Buscar por ID" << resetCode << endl;
        gotoxy(x, y + 13); if(lista_circular.ptrCabeza){cout<< teal;}else{cout <<red;} cout << "13) Buscar por Nombre" << resetCode << endl;
        gotoxy(x, y + 14); if(lista_circular.ptrCabeza){cout<< teal;}else{cout <<red;} cout << "14) Buscar por Artista" << resetCode << endl;
        gotoxy(x, y + 15); if(lista_circular.ptrCabeza){cout<< teal;}else{cout <<red;} cout << "15) Buscar por Genero" << resetCode << endl;


        gotoxy(x, y + 16); cout << orange << "16) Guardar en .txt" << resetCode << endl;
        gotoxy(x, y + 17); cout << orange << "17) Cargar de .txt" << resetCode << endl;
        gotoxy(x, y + 18); cout << green << "18) Reproducir Musica" << resetCode << endl;
        gotoxy(x, y + 19); cout << green << "19) Pausar Musica" << resetCode << endl;
        gotoxy(x, y + 20); cout << green << "20) Siguiente Cancion" << resetCode << endl;
        gotoxy(x, y + 21); cout << green << "21) Anterior Cancion" << resetCode << endl;

        validarParametroEntero(&opcion, opcion_validar, "Seleccione una opcion");

    } while (opcion < 1 || opcion > 21);

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

        gotoxy(0,0);
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
        gotoxy(0,0);
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

    case 3:
        validarParametroEntero(&id_int, id_validar, "ID a Modificar");
        id = to_string(id_int);

        gotoxy(0,0);
        cout << "--> Ingrese el nuevo Nombre: ";
        getline(cin, nuevo_nombre);
        cout << "--> Ingrese el nuevo Artista: ";
        getline(cin, nuevo_artista);
        cout << "--> Ingrese el nuevo Album: ";
        getline(cin, nuevo_album);
        cout << "--> Ingrese el nuevo Genero: ";
        getline(cin, nuevo_genero);
        cout << "--> Ingrese el nuevo Directorio: ";
        getline(cin, nuevo_directorio);
        nuevo_directorio = convertirRuta(nuevo_directorio);

        lista_circular.modificar(id, nuevo_nombre, nuevo_genero, nuevo_album, nuevo_genero, nuevo_directorio);
        lista_circular.siguienteCancion();

        break;
    case 4:

        validarParametroEntero(&id_int, id_validar, "ID a Eliminar");
        id = to_string(id_int);

        lista_circular.eliminar(id);
        lista_circular.siguienteCancion();

        break;

    case 5:
        lista_circular.invertir();
        lista_circular.siguienteCancion();

        break;
    case 6:
        lista_circular.vaciar();
        lista_circular.musicPlayer.stop();
        break;

    case 7:
        lista_circular.ordenar_ascendentemente();
        lista_circular.siguienteCancion();
        break;
    case 8:
        lista_circular.ordenar_descendentemente();
        lista_circular.siguienteCancion();

        break;
    case 9:
        lista_circular.ordenar_por_nombre();
        lista_circular.siguienteCancion();
        break;

    case 10:
        lista_circular.ordenar_aleatoriamente();
        lista_circular.siguienteCancion();
        break;

    case 11:
        int posicion;
        validarParametroEntero(&posicion, posicion_validar, "Posicion a buscar");
        gotoxy(0,0);
        lista_circular.buscar_por_posicion(posicion);
        break;

    case 12:
        validarParametroEntero(&id_int, id_validar, "ID a Buscar");
        id = to_string(id_int);
        gotoxy(0,0);
        lista_circular.buscar_por_id(id);
        break;

    case 13:
        cout << "--> Canción a buscar: ";
        getline(cin, nombre);
        gotoxy(0,0);
        lista_circular.buscar_nombre(nombre);
        break;

    case 14:
        cout << "--> Artista a buscar: ";
        getline(cin, artista);
        gotoxy(0,0);
        lista_circular.buscar_artista(artista);
        break;

    case 15:
        cout << "--> Género a buscar: ";
        getline(cin, genero);
        gotoxy(0,0);
        lista_circular.buscar_genero(genero);
        break;

    case 16:
        lista_circular.guardar_en_archivo(nombre_archivo);
        gotoxy(0,0);
        cout << "\033[1;32m Lista guardada en el archivo exitosamente \033[0m\n"
             << endl;
        break;
    case 17:
        lista_circular.cargar_desde_archivo(nombre_archivo);
        gotoxy(0,0);
        cout << "\033[1;32m Lista cargada desde el archivo exitosamente \033[0m\n"
             << endl;
        break;
    case 18:
        lista_circular.reproducirMusica();
        break;
    case 19:
        lista_circular.pausarMusica();
        break;
    case 20:
        lista_circular.siguienteCancion();
        break;
    case 21:
        lista_circular.anteriorCancion();
        break;
    default:
        system("cls");
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

            lista_circular.siguienteCancion();
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
    while (true)
    {

        system("cls");
        
        // TABLA CANCIONES

        int x = 1;
        int y = 8;

        printLine(x, y, char(205), 142, "horizontal");
        printLine(x, y+27, char(205), 142, "horizontal");

        printLine(x-1, y+1, char(186), 26, "vertical");


        printCharacterAt(x-1, y, char(201));
        printCharacterAt(x-1, y+27, char(200));

   

        // TABLA INSTRUCCIONES

        x = 145; 
        y = 1;

        printLine(x-2, y, char(186), 34, "vertical");
        printLine(x+30, y, char(186), 34, "vertical");

        gotoxy(x+8,y);cout << "INSTRUCCIONES";
        printLine(x-1, y+1, char(205), 31, "horizontal");

        printLine(x-1, y-1, char(205), 31, "horizontal");
        printLine(x-1, y+34, char(205), 31, "horizontal");

        printCharacterAt(x+30, y-1, char(187));
        printCharacterAt(x+30, y+34, char(188));

        printCharacterAt(x-2, y-1, char(201));


        printLine(x-1, y+24, char(205), 31, "horizontal");


        // Special corners
        printCharacterAt(x-2, y+34, char(202));
        printCharacterAt(x-2, y+7, char(185));

        printCharacterAt(x-2, y+1, char(204));
        printCharacterAt(x+30, y+1, char(185));


        printCharacterAt(x-2, y+24, char(204));
        printCharacterAt(x+30, y+24, char(185));


       

        lista_circular.imprimir_lista();
        opcion = Menu::mostrarMenuPrincipal(lista_circular);

        Menu::ejecutarOpcion(opcion, lista_circular);
    }
}

void hiloCargarBarra(ListaCircular<string> &lista_circular)
{
    // carga inicial
    std::this_thread::sleep_for(std::chrono::seconds(2));

    float porcentaje;
    int convertedPorcentaje;

    while (true)
    {
        const int numColors = sizeof(colors) / sizeof(colors[0]);
        if (lista_circular.ptrCabeza)
        {


            porcentaje = static_cast<float>(lista_circular.contadorBarra) / static_cast<float>(lista_circular.ptrCabeza->duracion);
            convertedPorcentaje = static_cast<int>(porcentaje * 135) + 1;

            int currentColorIndex = lista_circular.contadorBarra % numColors;
            cout << colors[currentColorIndex] ;

            printLine(4, 6, char(178), convertedPorcentaje, "horizontal");
            cout << resetCode;

            if (lista_circular.aumentarBarra)
            {
                lista_circular.contadorBarra++;
            }

         
        }
        else
        {
            lista_circular.contadorBarra = 0;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    ListaCircular<string> lista_circular;

    std::thread hiloBarra(hiloCargarBarra, std::ref(lista_circular));
    std::thread hiloMenu(hiloEjecutarMenu, std::ref(lista_circular));
    std::thread hiloReproduccion(hiloReproducirMusica, std::ref(lista_circular));

    hiloBarra.join();
    hiloMenu.join();         // Espera a que termine el hiloMenu
    hiloReproduccion.join(); // Espera a que termine el hiloMenu


    return 0;
}
