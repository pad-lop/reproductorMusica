#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <codecvt>

#include <locale>

#include <SFML/Audio.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

bool reproduciendo = false;
bool enPausa = false;
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
        system("pause");
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
    Nodo<T> *ptrCabeza;

    bool reproducirCancion();
    void gestionarPausa(bool &isPaused);

public:
    ListaCircular();
    ~ListaCircular();

    sf::Music musicPlayer;
    void reproducirMusica();
    void pausarMusica();


    void buscar_por_posicion(int posicion);
    void buscar_nombre(T);
    void modificar(T, T, T, T, T, T, T);
    void eliminar(T);

    void invertir();
    void ordenar_por_nombre();
    void vaciar();

    void imprimir_lista();

    void agregar_al_final(T, T, T, T, T, T);
    void agregar_al_principio(T, T, T, T, T, T);

    void ordenar_descendentemente();
    void ordenar_ascendentemente();

    void guardar_en_archivo(string);
    void cargar_desde_archivo(string);

    // Herramientas
    // bool validarDirectorio(const T &directorio_);
};

template <typename T>
ListaCircular<T>::ListaCircular()
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
void ListaCircular<T>::guardar_en_archivo(string nombre_archivo)
{
    fstream archivo;
    archivo.open(nombre_archivo, ios::out);

    Nodo<T> *temp = ptrCabeza;

    if (temp != nullptr)
    {
        do
        {
            archivo << temp->id << "," << temp->nombre << "," << temp->artista << "," << temp->album << "," << temp->genero << "," << temp->directorio << endl;
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
        T id, nombre, artista, album, genero, directorio;
        getline(archivo, id, ',');
        getline(archivo, nombre, ',');
        getline(archivo, artista, ',');
        getline(archivo, album, ',');
        getline(archivo, genero, ',');
        getline(archivo, directorio);

        if (!id.empty() && !nombre.empty())
        {
            agregar_al_final(id, nombre, artista, album, genero, directorio);
        }
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
void ListaCircular<T>::agregar_al_principio(T id_, T nombre_, T artista_, T album_, T genero_, T directorio_)
{
    Nodo<T> *nuevoNodo = new Nodo<T>(id_, nombre_, artista_, album_, genero_, directorio_);
    nuevoNodo->duracion = getAudioFileDuration(directorio_);

    if (!ptrCabeza)
    {
        ptrCabeza = nuevoNodo;
        nuevoNodo->siguiente = ptrCabeza;
    }
    else
    {
        Nodo<T> *temp = ptrCabeza;
        bool existeID = false;

        do
        {
            if (temp->id == id_)
            {
                existeID = true;
                cout << "\033[1;31m ERR: ID previamente registrado \033[0m\n"
                     << endl;

                delete nuevoNodo;
                system("pause");
                break;
            }
            temp = temp->siguiente;
        } while (temp != ptrCabeza);

        if (!existeID)
        {
            Nodo<T> *ultimo = ptrCabeza;
            while (ultimo->siguiente != ptrCabeza)
            {
                ultimo = ultimo->siguiente;
            }

            nuevoNodo->siguiente = ptrCabeza;
            ptrCabeza = nuevoNodo;
            ultimo->siguiente = ptrCabeza;
        }
    }
}

template <typename T>
void ListaCircular<T>::agregar_al_final(T id_, T nombre_, T artista_, T album_, T genero_, T directorio_)
{
    Nodo<T> *nuevoNodo = new Nodo<T>(id_, nombre_, artista_, album_, genero_, directorio_);
    nuevoNodo->duracion = getAudioFileDuration(directorio_);
    Nodo<T> *temp = ptrCabeza;

    int contador = 0;
    if (!ptrCabeza)
    {
        ptrCabeza = nuevoNodo;
    }
    else
    {
        if (ptrCabeza->id == id_)
        {
            cout << "\033[1;31m ERR: ID previamente registrado \033[0m\n"
                 << endl;
            system("pause");
        }
        else
        {
            while (temp->siguiente != ptrCabeza)
            {
                if (temp->siguiente->id == id_)
                {
                    contador++;
                }
                temp = temp->siguiente;
            }

            if (contador == 0)
            {
                nuevoNodo->siguiente = temp->siguiente;
                temp->siguiente = nuevoNodo;
            }
        }
    }
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
            temp->imprimir_nodo();
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
            temp->imprimir_nodo();
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
void ListaCircular<T>::modificar(T id_, T nuevo_id, T nuevo_nombre, T nuevo_artista, T nuevo_album, T nuevo_genero, T nuevo_directorio)
{
    Nodo<T> *temp = ptrCabeza;
    bool modificado = false;

    do
    {
        if (temp->id == id_)
        {
            temp->id = nuevo_id;
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
        }

        current = current->siguiente;
    } while (current != ptrCabeza);

    cout << endl
         << "\033[1;32mLista Ordenada Descendentemente por ID \033[0m\n"
         << endl;
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

    cout << "\033[1;32m Lista Vacia \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::reproducirMusica()
{
    if (!ptrCabeza)
    {
        cout << "La lista de reproduccion esta vacia." << endl;
        system("pause");
        return;
    }

    std::thread reproduccionThread(&ListaCircular<T>::reproducirCancion, this);
        std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex para acceder a las variables compartidas

musicPlayer.play();
enPausa = false;
cout << "Reproduccion reanudada." << endl;

    reproduccionThread.join(); // Espera a que el hilo de reproducción termine
}



template <typename T>
void ListaCircular<T>::pausarMusica()
{
    if (!ptrCabeza)
    {
        cout << "La lista de reproduccion esta vacia." << endl;
        system("pause");
        return;
    }


 musicPlayer.pause();
                enPausa = true;
                cout << "Reproduccion pausada." << endl;
}


template <typename T>
bool ListaCircular<T>::reproducirCancion()
{
    std::lock_guard<std::mutex> lock(mtx); // Bloquea el mutex para acceder a las variables compartidas

    if (reproduciendo)
    {
        cerr << "Ya se está reproduciendo una canción." << endl;
        return false;
    }

    reproduciendo = true;

    if (filesystem::path(ptrCabeza->directorio).extension() != ".wav")
    {
        cerr << "Formato de archivo no compatible: " << ptrCabeza->directorio << endl;
        reproduciendo = false;
        return false;
    }

    if (!musicPlayer.openFromFile(ptrCabeza->directorio))
    {
        cerr << "Error al abrir el archivo de audio: " << ptrCabeza->directorio << endl;
        reproduciendo = false;
        return false;
    }

    cout << "Reproduciendo la siguiente canción..." << endl;
    musicPlayer.play();
    cout << "Reproduciendo: " << ptrCabeza->nombre << " - Artista: " << ptrCabeza->artista << endl;

    // Esperar hasta que la reproducción termine o se detenga manualmente
    while (musicPlayer.getStatus() == sf::Music::Playing && reproduciendo)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    reproduciendo = false;
    return true;
}

int main()
{
    ListaCircular<string> lista_circular;
    int opc1;
    string opc1_validar;

    do
    {
        system("cls");
        lista_circular.imprimir_lista();

        cout << endl
             << "----- MENU -----" << endl
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
             << "15) Siguiente Cancion\n"
             << "16) Anterior Cancion\n"

             << "0) Salir\n"

             << endl;
        validarParametroEntero(&opc1, opc1_validar, "Selecciona una opcion");

        cout << endl;

        string nuevo_id, nuevo_nombre, nuevo_artista, nuevo_album, nuevo_genero, nuevo_directorio;

        string id, nombre, artista, album, genero, directorio;

        string id_validar;
        int id_int;

        string nombre_archivo = "logica.txt";

        switch (opc1)
        {
        case 1:
            validarParametroEntero(&id_int, id_validar, "Ingrese el ID");
            id = to_string(id_int);

            cout << "--> Ingrese el  Nombre: ";
            getline(cin, nombre);
            cout << "--> Ingrese el  Artista: ";
            getline(cin, artista);
            cout << "--> Ingrese el  Album: ";
            getline(cin, album);
            cout << "--> Ingrese el  Genero: ";
            getline(cin, genero);
            cout << "--> Ingrese el  Directorio: ";
            getline(cin, directorio);

            directorio = convertirRuta(directorio);

            lista_circular.agregar_al_principio(id, nombre, artista, album, genero, directorio);
            break;
        case 2:
            validarParametroEntero(&id_int, id_validar, "Ingrese el ID");
            id = to_string(id_int);

            cout << "--> Ingrese el  Nombre: ";
            getline(cin, nombre);
            cout << "--> Ingrese el  Artista: ";
            getline(cin, artista);
            cout << "--> Ingrese el  Album: ";
            getline(cin, album);
            cout << "--> Ingrese el  Genero: ";
            getline(cin, genero);
            cout << "--> Ingrese el  Directorio: ";
            getline(cin, directorio);

            directorio = convertirRuta(directorio);

            lista_circular.agregar_al_final(id, nombre, artista, album, genero, directorio);
            break;
        case 3:
            int posicion;
            validarParametroEntero(&posicion, opc1_validar, "Ingrese la posicion a buscar");
            lista_circular.buscar_por_posicion(posicion);
            break;
        case 4:
            validarParametroEntero(&id_int, id_validar, "Ingrese el ID a Modificar");
            id = to_string(id_int);

            validarParametroEntero(&id_int, id_validar, "Ingrese el nuevo ID");
            nuevo_id = to_string(id_int);

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

            lista_circular.modificar(id, nuevo_id, nuevo_nombre, nuevo_genero, nuevo_album, nuevo_genero, nuevo_directorio);
            break;
        case 5:
            cout << "--> Ingrese el ID a eliminar: ";
            cin >> id;
            cin.ignore();

            lista_circular.eliminar(id);
            break;
        case 6:
            lista_circular.ordenar_ascendentemente();
            break;
        case 7:
            lista_circular.ordenar_descendentemente();
            break;
        case 8:
            lista_circular.ordenar_por_nombre();
            break;
        case 9:
            lista_circular.invertir();
            break;
        case 10:
            lista_circular.vaciar();
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
        case 15:
            // Siguiente canción
            break;
        case 16:
            // Anterior canción
            break;
        default:
            break;
        }
    } while (opc1 != 0);

    return 0;
}
