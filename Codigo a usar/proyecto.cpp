#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

int pausar = 0;
double contador = 0;

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

void validarParametroEntero(int *x, string x_prevalidar, string texto, bool importante = false)
{
    do
    {
        if (importante == true)
        {
            cout << "\033[1;38;5;166m--> " << texto << ": \033[0m";
            cin >> x_prevalidar;
        }
        else
        {
            cout << "--> " << texto << ": ";
            cin >> x_prevalidar;
        }

        if (!esEntero(x_prevalidar))
        {
            cout << "\033[1;38;5;196mERR: Dato no Valido - Debe ser Entero\033[0m" << endl;
        }
    } while (!esEntero(x_prevalidar));
    *x = stod(x_prevalidar);
}

class Nodo
{

public:
    string nombre;

    int fila;
    int id;
    int tiempo;

    int tiempoPadre = 5;
    int tiempoMovimiento = 0;

    int movimiento = 0;

    int fila_actual;

    bool avanzar = false;
    bool siendoAtendido = false;

    int turno;

    Nodo *siguiente;

    Nodo(int id, string nombre, int fila, int tiempo, int fila_actual) : nombre(nombre), fila(fila), id(id), tiempo(tiempo), siguiente(nullptr), fila_actual(fila_actual) {}
};

class Cola
{
private:
    Nodo *frente;
    Nodo *final;

public:
    Cola(int numeroFila) : numeroFila(numeroFila), frente(nullptr), final(nullptr){};
    bool cola_vacia();
    void encolar(int id, string nombre, int fila, int tiempo, int fila_actual);
    void descolar();
    void mostrarFrente();
    void mostrarFinal();
    void mostrarCola(string nombre);
    void mostrarColaVertical(string nombre);
    void gotoxy(int x, int y);
    Nodo *getFrente() const { return frente; }

    int numeroFila;

    int contadorNodos = 0;
};

bool Cola::cola_vacia()
{
    return (frente == nullptr);
}

void Cola::encolar(int id, string nombre, int fila, int tiempo, int fila_actual)
{
    Nodo *nuevoNodo = new Nodo(id, nombre, fila, tiempo, fila_actual);

    if (cola_vacia())
    {
        frente = nuevoNodo;
        final = nuevoNodo;
        contadorNodos++;
        final->turno = contadorNodos;
        cout << endl
             << "\033[1;38;5;35mCliente agregado a la fila\033[0m" << endl;
    }
    else
    {
        final->siguiente = nuevoNodo;
        final = nuevoNodo;
        contadorNodos++;
        final->turno = contadorNodos;
        cout << endl
             << "\033[1;38;5;35mCliente agregado a la fila\033[0m" << endl;
    }
}

void Cola::descolar()
{
    if (!cola_vacia())
    {
        Nodo *temp = frente;
        frente = frente->siguiente;
        delete temp;
    }
}

void Cola::mostrarFrente()
{
    if (!cola_vacia())
    {
        cout << endl
             << "El inicio es: " << frente->id << endl;
        system("pause");
    }
    else
    {
        cout << endl
             << "La cola está vacía." << endl;
        system("pause");
    }
}

void Cola::mostrarFinal()
{
    if (!cola_vacia())
    {
        cout << endl
             << "El final es: " << final->id << endl;
        system("pause");
    }
    else
    {
        cout << endl
             << "La cola está vacía." << endl;
        system("pause");
    }
}

void Cola::mostrarCola(string nombreFila)
{

    Nodo *temp = frente;

    int indice = 0;

    int posicionFinal = 75;

    if (temp != nullptr)
    {
        temp->avanzar = true;
    }

    int ANCHO = 25;
    int ALTO = 10;
    while (temp != nullptr)
    {

        if (((temp->movimiento) >= ANCHO) && (temp->siguiente != nullptr))
        {

            temp->siguiente->avanzar = true;
        }

        if (temp->avanzar)
        {
            gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 4);
            cout << "(" << temp->id << ") " << temp->nombre << " \033[36m" << temp->tiempo << "s\033[0m";

            if (temp->fila != 0)
            {
                gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 5);
            }

            if (temp->fila == 4)
            {
                cout << "Atencion a Clientes";
            }
            else if (temp->fila == 5)
            {
                cout << "Gerencia";
            }

            if (numeroFila > 1 && numeroFila < 5)
            {
                if (temp->fila == 1)
                {
                    cout << "Ventanilla"
                         << " - A" << temp->turno;

                    if(temp->siendoAtendido == true){
                        gotoxy(62, 43 + indice);
                        cout<< "A" << temp->turno << " " << temp->nombre;}
                    else{
                        gotoxy(110, 43 + indice);
                        cout<< "A" << temp->turno << " " << temp->nombre;}
                }
                else if (temp->fila == 2)
                {
                    cout << "Ventanilla"
                         << " - B" << temp->turno;

                    if(temp->siendoAtendido == true){
                        gotoxy(62, 43 + 1+indice);
                        cout<< "B" << temp->turno << " " << temp->nombre;}
                    else{
                        gotoxy(90, 43 + indice);
                        cout<< "B" << temp->turno << " " << temp->nombre;}


                }
            }
            else
            {
                if (temp->fila == 1)
                {
                    cout << "Ventanilla";
                }
                else if (temp->fila == 2)
                {
                    cout << "Ventanilla";
                }
                else if (temp->fila == 3)
                {
                    cout << "Ventanilla";
                }
            }

            gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 6);
            cout << "     O";

            if (temp->tiempoMovimiento % 2 == 0)
            {
                gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 7);
                cout << "    " << char(218) << char(197) << char(217);
                gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 8);
                cout << "    " << char(218) << char(193) << char(191);
            }
            else
            {
                gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 7);
                cout << "    " << char(192) << char(197) << char(191);
                gotoxy(temp->movimiento, (temp->fila_actual * ALTO) + 8);
                cout << "    " << char(218) << char(193) << char(191);
            }

            if (temp->movimiento == (posicionFinal))
            {
                temp->siendoAtendido = true;
            }
            else if (temp->movimiento < posicionFinal - (indice * ANCHO))
            {
                temp->movimiento += 5;
            }
        }
        temp = temp->siguiente;
        indice++;
    }

    int y = (numeroFila * 10);

    for (int i = 60; i < 122; i++)
    {
        gotoxy(i, 41);
        cout << "#";
    }

    gotoxy(65, 42); cout << "Atendido";
    gotoxy(100, 42 ); cout << "Siguiente";

    
   for (int i = 60; i < 122; i++)
    {
        gotoxy(i, 50);
        cout << "#";
    }

   for (int i = 41; i < 50; i++)
    {
        gotoxy(60, i);
        cout << "#";
    }


    for (int i = 41; i < 50; i++)
    {
        gotoxy(85, i);
        cout << "#";
    } // Horizontal
    for (int i = 41; i < 50; i++)
    {
        gotoxy(121, i);
        cout << "#";
    } // Horizontal

    for (int i = 0; i < 123; i++)
    {
        gotoxy(i, y + ALTO);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Horizontal

    gotoxy(105, y + 5);
    cout << "    O";
    gotoxy(105, y + 6);
    cout << "   " << char(192) << char(197) << char(217);

    for (int i = 99; i < 121; i++)
    {
        gotoxy(i, y + 7);
        cout << char(205);
    } // Horizontal

    for (int i = 99; i < 121; i++)
    {
        gotoxy(i, y + 1);
        cout << char(205);
    } // Horizontal
    for (int i = 99; i < 121; i++)
    {
        gotoxy(i, y + 3);
        cout << char(205);
    }

    gotoxy(98, y + 1);
    cout << char(201); // Esquina Superior Izquierda
    gotoxy(121, y + 1);
    cout << char(187); // Esquina Superior Derecha

    for (int i = y + 2; i < y + 9; i++)
    {
        gotoxy(98, i);
        cout << char(186);
    } // Vertical
    for (int i = y + 2; i < y + 9; i++)
    {
        gotoxy(121, i);
        cout << char(186);
    }

    gotoxy(98, y + 3);
    cout << char(204); // Esquina Doble (T)
    gotoxy(121, y + 3);
    cout << char(185);
    gotoxy(98, y + 7);
    cout << char(204); // Esquina Doble (T)
    gotoxy(121, y + 7);
    cout << char(185);

    for (int i = 99; i < 121; i++)
    {
        gotoxy(i, y + 9);
        cout << char(205);
    } // Horizontal

    gotoxy(98, y + 9);
    cout << char(200); // Esquina Inferior Izquierda
    gotoxy(121, y + 9);
    cout << char(188); // Esquina Inferior Derecha

    gotoxy(100, y + 2);
    cout << nombreFila;
}

void Cola::mostrarColaVertical(string nombreFila)
{

    Nodo *temp = frente;

    int indice = 0;

    int posicionFinal = 30;

    if (temp != nullptr)
    {
        temp->avanzar = true;
    }
    int ANCHO = 25;
    int ALTO = 10;

    while (temp != nullptr)
    {

        if (((temp->movimiento) >= ALTO) && (temp->siguiente != nullptr))
        {
            temp->siguiente->avanzar = true;
        }

        if (temp->avanzar)
        {
            gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 4);
            cout << "(" << temp->id << ") " << temp->nombre << " \033[36m" << temp->tiempo << "s\033[0m";

            gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 6);
            cout << "     O";

            if (temp->tiempoMovimiento % 2 == 0)
            {
                gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 7);
                cout << "    " << char(218) << char(197) << char(217);
                gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 8);
                cout << "    " << char(218) << char(193) << char(191);
            }
            else
            {
                gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 7);
                cout << "    " << char(192) << char(197) << char(191);
                gotoxy((temp->fila_actual * ANCHO), temp->movimiento + 8);
                cout << "    " << char(218) << char(193) << char(191);
            }

            if (temp->movimiento == (posicionFinal))
            {
                temp->siendoAtendido = true;
            }
            else if (temp->movimiento < posicionFinal - (indice * ALTO))
            {
                temp->movimiento += 5;
            }
        }
        temp = temp->siguiente;
        indice++;
    }

    int y = 40;
    int x = (numeroFila * 25);

    gotoxy(x + 5, y + 5);
    cout << "    O";
    gotoxy(x + 5, y + 6);
    cout << "   " << char(192) << char(197) << char(217);

    for (int i = x; i < x + 20; i++)
    {
        gotoxy(i, y + 7);
        cout << char(205);
    } // Horizontal

    for (int i = x; i < x + 20; i++)
    {
        gotoxy(i, y + 1);
        cout << char(205);
    } // Horizontal
    for (int i = x; i < x + 20; i++)
    {
        gotoxy(i, y + 3);
        cout << char(205);
    }

    gotoxy(x, y + 1);
    cout << char(201); // Esquina Superior Izquierda
    gotoxy(x + 20, y + 1);
    cout << char(187); // Esquina Superior Derecha

    for (int i = y + 2; i < y + 9; i++)
    {
        gotoxy(x, i);
        cout << char(186);
    } // Vertical
    for (int i = y + 2; i < y + 9; i++)
    {
        gotoxy(x + 20, i);
        cout << char(186);
    }

    gotoxy(x, y + 3);
    cout << char(204); // Esquina Doble (T)
    gotoxy(x + 20, y + 3);
    cout << char(185);

    gotoxy(x, y + 7);
    cout << char(204); // Esquina Doble (T)
    gotoxy(x + 20, y + 7);
    cout << char(185);

    for (int i = x + 1; i < x + 20; i++)
    {
        gotoxy(i, y + 9);
        cout << char(205);
    } // Horizontal

    gotoxy(x, y + 9);
    cout << char(200); // Esquina Inferior Izquierda
    gotoxy(x + 20, y + 9);
    cout << char(188); // Esquina Inferior Derecha

    gotoxy(x + 1, y + 2);
    cout << nombreFila;

    for (int i = y - 40; i < y + 13; i++)
    {
        gotoxy(x - 3, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical
    for (int i = y - 40; i < y + 12; i++)
    {
        gotoxy(x - 2, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical
    for (int i = y - 40; i < y + 11; i++)
    {
        gotoxy(x - 1, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical

    for (int i = 123; i < 172; i++)
    {
        gotoxy(i, y + ALTO);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Horizontal

    for (int i = y - 40; i < y + 13; i++)
    {
        gotoxy(171, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical
    for (int i = y - 40; i < y + 12; i++)
    {
        gotoxy(172, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical
    for (int i = y - 40; i < y + 11; i++)
    {
        gotoxy(173, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical

    for (int i = y - 40; i < y + 11; i++)
    {
        gotoxy(146, i);
        cout << "\033[1;37m" << char(219) << "\033[0m";
    } // Vertical
}

void Cola::gotoxy(int x, int y)
{
    HANDLE hcon;
    hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hcon, dwPos);
}

bool pausado = false;

void updateTimer(Cola &colaGeneral, Cola &colaCajero, Cola &Ventanilla1, Cola &Ventanilla2, Cola &ColaAtencionClientes, Cola &ColaGerencia)
{
    while (true)
    {
        if (!pausado)
        {
            system("cls");
            contador++;
            cout << "Contador: " << contador << " - Para Pausar: Presione cualquier numero y Enter rapidamente" << endl;

            if (!colaGeneral.cola_vacia())
            {
                Nodo *temp = colaGeneral.getFrente();
                temp->tiempoMovimiento++;
                if (temp->tiempoPadre == 0 && temp->siendoAtendido)
                {

                    if (temp->fila == 1)
                    {
                        Ventanilla1.encolar(temp->id, temp->nombre, temp->fila, temp->tiempo, temp->fila + 1);
                    }
                    else if (temp->fila == 2)
                    {
                        Ventanilla2.encolar(temp->id, temp->nombre, temp->fila, temp->tiempo, temp->fila + 1);
                    }
                    else if (temp->fila == 4)
                    {
                        ColaAtencionClientes.encolar(temp->id, temp->nombre, temp->fila, temp->tiempo, temp->fila + 1);
                    }
                    else if (temp->fila == 5)
                    {
                        ColaGerencia.encolar(temp->id, temp->nombre, temp->fila, temp->tiempo, temp->fila + 1);
                    }

                    colaGeneral.descolar();
                }
                else if (temp->tiempoPadre > 0 && temp->siendoAtendido)
                {
                    temp->tiempoPadre -= 1;
                }
            }

            if (!colaCajero.cola_vacia())
            {
                Nodo *temp = colaCajero.getFrente();
                temp->tiempoMovimiento++;
                if (temp->tiempo == 0 && temp->siendoAtendido)
                {
                    colaCajero.descolar();
                }
                else if (temp->tiempo > 0 && temp->siendoAtendido)
                {
                    temp->tiempo -= 1;
                }
            }

            if (!Ventanilla1.cola_vacia())
            {
                Nodo *temp = Ventanilla1.getFrente();
                temp->tiempoMovimiento++;
                if (temp->tiempo == 0 && temp->siendoAtendido)
                {
                    Ventanilla1.descolar();
                }
                else if (temp->tiempo > 0 && temp->siendoAtendido)
                {
                    temp->tiempo -= 1;
                }
            }

            if (!Ventanilla2.cola_vacia())
            {
                Nodo *temp = Ventanilla2.getFrente();
                temp->tiempoMovimiento++;
                if (temp->tiempo == 0 && temp->siendoAtendido)
                {
                    Ventanilla2.descolar();
                }
                else if (temp->tiempo > 0 && temp->siendoAtendido)
                {
                    temp->tiempo -= 1;
                }
            }

            if (!ColaGerencia.cola_vacia())
            {
                Nodo *temp = ColaGerencia.getFrente();
                temp->tiempoMovimiento++;

                if (temp->tiempo == 0 && temp->siendoAtendido)
                {
                    ColaGerencia.descolar();
                }
                else if (temp->tiempo > 0 && temp->siendoAtendido)
                {
                    temp->tiempo -= 1;
                }
            }

            if (!ColaAtencionClientes.cola_vacia())
            {
                Nodo *temp = ColaAtencionClientes.getFrente();
                temp->tiempoMovimiento++;

                if (temp->tiempo == 0 && temp->siendoAtendido)
                {
                    ColaAtencionClientes.descolar();
                }
                else if (temp->tiempo > 0 && temp->siendoAtendido)
                {
                    temp->tiempo -= 1;
                }
            }

            cout << endl;
            colaCajero.mostrarCola("Fila Cajero");
            colaGeneral.mostrarCola("Fila General - 5s");
            Ventanilla1.mostrarCola("Ventanilla 1");
            Ventanilla2.mostrarCola("Ventanilla 2");

            ColaGerencia.mostrarColaVertical("Gerencia");
            ColaAtencionClientes.mostrarColaVertical("Atencion a Clientes");
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void mostrarMenu(Cola &colaGeneral, Cola &colaCajero)
{
    int opcion;
    string opcion_validar;

    do
    {
        cout << endl
             << "\033[1;38;5;166m---------- MENU ----------\033[0m" << endl;
        cout << "1) Encolar Clientes General" << endl;
        cout << "2) Encolar Clientes ATM" << endl;
        cout << "5) Pausar / Reanudar" << endl;
        cout << "6) Salir" << endl;
        cout << endl;

        validarParametroEntero(&opcion, opcion_validar, "Seleccione una opcion", true);

        if (opcion != 5 && pausado == false)
        {
            opcion = 5;
        }

        int id, fila, tiempo;
        string nombre;

        string id_validar, fila_validar, tiempo_validar;

        string turno;

        int random = 0;

        switch (opcion)
        {
        case 1:
            cout << endl
                 << "\033[1;38;5;166m---- Alta de Cliente - Fila General ----\033[0m" << endl;
            validarParametroEntero(&id, id_validar, "Id");
            cout << "--> Nombre: ";
            cin >> nombre;

            cout << endl
                 << "1) Atencion a Clientes" << endl
                 << "2) Gerencia" << endl
                 << "3) Ventanilla" << endl;
            do
            {
                validarParametroEntero(&fila, fila_validar, "Fila");
                if (fila > 3 || fila < 1)
                {
                    cout << "\033[1;38;5;196mOpcion no valida \033[0m" << endl;
                }
            } while (fila > 3 || fila < 1);

            if (fila == 3)
            {
                fila = 1 + (rand() % 2);
            }
            else if (fila == 1)
            {
                fila = 4;
            }
            else if (fila == 2)
            {
                fila = 5;
            }

            tiempo = 5 + (rand() % 15);
            cout << endl
                 << "\033[36mTiempo de Espera: " << tiempo << "\033[0m";

            colaGeneral.encolar(id, nombre, fila, tiempo, 1);
            break;
        case 2:
            cout << endl
                 << "\033[1;38;5;166m---- Alta de Cliente - Fila Cajero ----\033[0m" << endl;
            validarParametroEntero(&id, id_validar, "Id");
            cout << "--> Nombre: ";
            cin >> nombre;

            tiempo = 5 + (rand() % 15);
            cout << endl
                 << "\033[36mTiempo de Espera: " << tiempo << "\033[0m";

            colaCajero.encolar(id, nombre, fila = 0, tiempo, 0);
            break;
        case 5:
            pausado = !pausado;
            system("cls");
            cout << "     PROGRAMA EN PAUSA" << endl;
            break;
        case 6:
            cout << endl
                 << "Saliendo del programa." << endl;
            system("pause");
            exit(0);
            break;
        default:
            cout << "\033[1;38;5;196mOpcion no valida. Intente de nuevo. \033[0m" << endl;
            break;
        }
    } while (opcion != 6);
}

int main()
{
    int opcion;
    Cola colaCajero(0);
    Cola colaGeneral(1);

    Cola Ventanilla1(2);
    Cola Ventanilla2(3);

    Cola ColaAtencionClientes(5);
    Cola ColaGerencia(6);

    std::thread timerThread(updateTimer, std::ref(colaGeneral), std::ref(colaCajero), std::ref(Ventanilla1), std::ref(Ventanilla2), std::ref(ColaAtencionClientes), std::ref(ColaGerencia));
    std::thread menuThread(mostrarMenu, std::ref(colaGeneral), std::ref(colaCajero));

    timerThread.join();
    menuThread.join();

    return 0;
}