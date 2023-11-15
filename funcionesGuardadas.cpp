 void buscar_por_posicion(int posicion);
    void buscar_nombre(T);
    void modificar(T, T, T, T, T, T, T);
    void eliminar(T);


    
    void ordenar_descendentemente();
    void ordenar_ascendentemente();
    void ordenar_por_nombre();
    void invertir();



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




















/*
    case 3:
        int posicion;
        validarParametroEntero(&posicion, posicion_validar, "Ingrese la posicion a buscar");
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
    */