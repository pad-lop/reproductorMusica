#include <iostream>
#include <string>

#include <fstream>
#include <cstdint>
#include <filesystem>

using namespace std;


std::string convertirRuta(const std::string &rutaOriginal)
{
    std::string rutaConvertida;

    for (char caracter : rutaOriginal)
    {
        if (caracter == '\\')
        {
            // Reemplazar barras invertidas por barras normales
            rutaConvertida.push_back('/');
        }
        else if (caracter == '"')
        {
            // Omitir comillas
            continue;
        }
        else
        {
            // Mantener los demás caracteres sin cambios
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

int main()
{
    std::string rutaOriginal;

    cout << "Ruta Original:";
    getline(cin, rutaOriginal);

    std::string rutaConvertida = convertirRuta(rutaOriginal);

    std::cout << endl << "Ruta original: " << rutaOriginal << std::endl;
    std::cout << "Ruta convertida: " << rutaConvertida << std::endl;

    system("pause");
    return 0;
}
