#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string directory = "C:/Users/Luisf/Desktop/Estructuras de Datos/Proyecto Modulo 3/archivo.txt";

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



class File
{
private:
public:
    void WriteToFile();
    void ReadToFile();
};
void File::WriteToFile()
{
    fstream file;
    file.open(directory, ios::app);

    string key;
    string name;
    string address;
    string phone;

    cout << endl << "Ingrese el texto para el archivo" << endl;

    cout << "ID: "; cin >> key;
    file << key << endl;
    cout << "NOMBRE: "; cin >> name;
    file << name << endl;
    cout << "DIRECCION: "; cin >> address;
    file << address << endl;
    cout << "TELEFONO: "; cin >> phone;
    file << phone << endl;



    file.close();
}
void File::ReadToFile()
{
    fstream file;
    char data[200];
    
    file.open(directory, ios::in);
    file >> data;
    

    cout << endl << "Texto dentro del archivo:" << endl << endl;
    while (!file.eof())
    {
        cout << data << endl; 
        file >> data;         
    }
    file.close(); 
}
int main(int argc, char **argv)
{
    int opc = 0;
    string opc_validar;
    do
    {
        cout << endl << "--- Menu ---" << endl;
        cout << "1) Write" << endl;
        cout << "2) Read" << endl;
        validarParametroEntero(&opc, opc_validar, "Seleccione una opcion");
        switch (opc)
        {
        case 1:
        {
            File f;
            f.WriteToFile();
        }
        break;
        case 2:
        {
            File f;
            f.ReadToFile();
        }
        break;
        }
        
    } while (true);
    return 0;
}