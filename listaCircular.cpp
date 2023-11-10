#include <iostream>
#include <string>
using namespace std;

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
        cin >> x_prevalidar;

        if (!esEntero(x_prevalidar))
        {
            cout << "\033[1;31m ERR: Dato no Valido - Debe ser Entero \033[0m\n"
                 << endl;
        }
    } while (!esEntero(x_prevalidar));
    *x = stod(x_prevalidar);
}

template <typename T>
class Nodo
{
public:
    T id;
    T nombre;
    Nodo *siguiente;

    Nodo();
    Nodo(T, T);
    ~Nodo();

    void imprimir_nodo();
};

template <typename T>
Nodo<T>::Nodo()
{
    id = T();
    nombre = T();
    siguiente = this; 
}

template <typename T>
Nodo<T>::Nodo(T id_, T nombre_)
{
    id = id_;
    nombre = nombre_;
    siguiente = this;
}

template <typename T>
Nodo<T>::~Nodo() {}

template <typename T>
void Nodo<T>::imprimir_nodo()
{
    cout << "   ID: " << id << endl;
    cout << "   Nombre: " << nombre << endl;
}

template <typename T>
class ListaCircular
{
private:
    Nodo<T> *ptrCabeza;

public:
    ListaCircular();
    ~ListaCircular();

    void buscar_por_posicion(int posicion);
    void buscar_nombre(T);
    void modificar(T, T, T);
    void eliminar(T);

    void invertir();
    void ordenar_por_nombre();
    void vaciar();

    void imprimir_lista();

    void agregar_al_final(T, T);
    void agregar_al_principio(T, T);

    void ordenar_descendentemente();
    void ordenar_ascendentemente();
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
void ListaCircular<T>::agregar_al_principio(T id_, T nombre_)
{
    Nodo<T> *nuevo_nodo = new Nodo<T>(id_, nombre_);

    if (!ptrCabeza)
    {
        ptrCabeza = nuevo_nodo;
        nuevo_nodo->siguiente = ptrCabeza;
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
                delete nuevo_nodo; 
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

            nuevo_nodo->siguiente = ptrCabeza;
            ptrCabeza = nuevo_nodo;
            ultimo->siguiente = ptrCabeza;
        }
    }
}

template <typename T>
void ListaCircular<T>::agregar_al_final(T id_, T nombre_)
{
    Nodo<T> *nuevo_nodo = new Nodo<T>(id_, nombre_);
    Nodo<T> *temp = ptrCabeza;

    int contador = 0;
    if (!ptrCabeza)
    {
        ptrCabeza = nuevo_nodo;
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
                nuevo_nodo->siguiente = temp->siguiente;
                temp->siguiente = nuevo_nodo;
            }
        }
    }
}

template <typename T>
void ListaCircular<T>::buscar_por_posicion(int posicion)
{
    if (ptrCabeza == nullptr)
    {
        cout << "La lista está vacía. No se puede buscar por posición." << endl;
        return;
    }

    Nodo<T> *temp = ptrCabeza;
    int count = 1;

    do
    {
        if (count == posicion)
        {
            cout << "\033[1;32m Contacto encontrado en la posición " << posicion << " \033[0m" << endl
                 << endl;
            temp->imprimir_nodo();
            return;
        }
        temp = temp->siguiente;
        count++;
    } while (temp != ptrCabeza);

    cout << "\033[1;31m Contacto no encontrado en la posición " << posicion << " \033[0m" << endl
         << endl;
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
            cout << "\033[1;32m Contacto encontrado \033[0m" << endl
                 << endl;
            temp->imprimir_nodo();
            encontrado = true;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!encontrado)
    {
        cout << "\033[1;31m No se encontraron contactos con ese nombre. \033[0m\n"
             << endl
             << endl;
    }
}

template <typename T>
void ListaCircular<T>::modificar(T id_, T nuevo_id, T nuevo_nombre)
{
    Nodo<T> *temp = ptrCabeza;
    bool modificado = false;

    do
    {
        if (temp->id == id_)
        {
            temp->id = nuevo_id;
            temp->nombre = nuevo_nombre;
            cout << "\033[1;32m Contacto modificado. \033[0m" << endl
                 << endl;
            modificado = true;
            break;
        }
        temp = temp->siguiente;
    } while (temp != ptrCabeza);

    if (!modificado)
    {
        cout << "\033[1;31m Contacto no encontrado No se realizó ninguna modificación. \033[0m\n"
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
            cout << "\033[1;32m Contacto eliminado. \033[0m\n"
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
        cout << "\033[1;31m Contacto no encontrado. No se realizó ninguna eliminación. \033[0m\n"
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
        cout << "\033[1;31m Lista vacía \033[0m\n"
             << endl
             << endl;
    }
    else
    {
        cout << "\033[1;32m Lista de contactos \033[0m" << endl
             << endl;
        int count = 1;

        do
        {
            cout << "Contacto " << count << ":" << endl;
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
        return; // La lista está vacía o solo tiene un elemento, no se puede invertir.
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

    // Actualizar el puntero de la cabeza para que apunte al último elemento invertido.
    ptrCabeza->siguiente = prev;
    ptrCabeza = prev;
}

template <typename T>
void ListaCircular<T>::ordenar_por_nombre()
{
    Nodo<T> *current = ptrCabeza;

    if (current == nullptr)
    {
        cout << "La lista está vacía. No se puede ordenar." << endl;
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
         << "\033[1;32mLista Ordenada Alfabéticamente \033[0m\n"
         << endl;
}

template <typename T>
void ListaCircular<T>::ordenar_ascendentemente()
{
    if (ptrCabeza == nullptr)
    {
        cout << "La lista está vacía. No se puede ordenar." << endl;
        return;
    }

    Nodo<T> *current = ptrCabeza;

    do
    {
        Nodo<T> *min = current;
        Nodo<T> *temp = current->siguiente;

        while (temp != ptrCabeza)
        {
            int id1 = stoi(min->id); // Convertir ID a entero
            int id2 = stoi(temp->id); // Convertir ID a entero

            if (id2 < id1) // Comparar IDs como números enteros
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
        cout << "La lista está vacía. No se puede ordenar." << endl;
        return;
    }

    Nodo<T> *current = ptrCabeza;

    do
    {
        Nodo<T> *max = current;
        Nodo<T> *temp = current->siguiente;

        while (temp != ptrCabeza)
        {
            int id1 = stoi(max->id); // Convertir ID a entero
            int id2 = stoi(temp->id); // Convertir ID a entero

            if (id2 > id1) // Comparar IDs como números enteros
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
             << "3) Buscar por ID\n"
             << "4) Modificar\n"
             << "5) Remover\n"

             << "6) Ordenar Ascendente\n"
             << "7) Ordenar Descendente\n"

             << "8) Ordenar por Nombre\n"
             << "9) Invertir\n"
             << "10) Vaciar Lista\n"
             << "11) Salir\n"

             << endl;
        validarParametroEntero(&opc1, opc1_validar, "Selecciona una opcion");

        cout << endl;

        string id, nombre, nuevo_id, nuevo_nombre;
        string id_validar;
        int id_int;

        switch (opc1)
        {
        case 1:
            validarParametroEntero(&id_int, id_validar, "Ingrese el ID");
            id = to_string(id_int);

            cout << "--> Ingrese el Nombre: ";
            cin >> nombre;
            lista_circular.agregar_al_principio(id, nombre);
            break;
        case 2:
            validarParametroEntero(&id_int, id_validar, "Ingrese el ID");
            id = to_string(id_int);

            cout << "--> Ingrese el Nombre: ";
            cin >> nombre;
            lista_circular.agregar_al_final(id, nombre);
            break;
        case 3:
            int posicion;
            validarParametroEntero(&posicion, opc1_validar, "Ingrese la posición a buscar");
            lista_circular.buscar_por_posicion(posicion);
            break;
        case 4:
            cout << "--> Ingrese el ID a modificar: ";
            cin >> id;
            validarParametroEntero(&id_int, id_validar, "Ingrese el nuevo ID");
            nuevo_id = to_string(id_int);
            cout << "--> Ingrese el nuevo Nombre: ";
            cin >> nuevo_nombre;
            lista_circular.modificar(id, nuevo_id, nuevo_nombre);
            break;
        case 5:
            cout << "--> Ingrese el ID a eliminar: ";
            cin >> id;
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
        default:
            cout << "Opcion invalida" << endl;
            break;
        }

    } while (opc1 != 11);

    return 0;
}
