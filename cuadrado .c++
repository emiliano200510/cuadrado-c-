#include <iostream>
#include <unistd.h>  // Para usleep
#include <termios.h> // Para manipular el terminal
#include <cstdio>    // Para getchar()

void gotoxy(int x, int y) {
    // Usar secuencias de escape ANSI para mover el cursor
    std::cout << "\033[" << y << ";" << x << "H";
}

class Cuadrado {
private:
    int lado;

public:
    Cuadrado(int lado) : lado(lado) {}

    int obtenerLado() const {
        return lado;
    }

    void Dibujar(int posX, int posY) const {
        for (int fila = 0; fila < lado; ++fila) {
            gotoxy(posX, posY + fila);
            for (int columna = 0; columna < lado; ++columna) {
                std::cout << "* ";
            }
        }
        std::cout.flush();
    }

    void Borrar(int posX, int posY) const {
        for (int fila = 0; fila < lado; ++fila) {
            gotoxy(posX, posY + fila);
            for (int columna = 0; columna < lado; ++columna) {
                std::cout << "  ";
            }
        }
        std::cout.flush();
    }
};

void set_input_mode(bool enable_raw_mode) {
    static struct termios oldt, newt;

    if (enable_raw_mode) {
        tcgetattr(STDIN_FILENO, &oldt); // Guarda la configuración actual
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO); // Desactiva el modo canónico y el eco
        tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Aplica la nueva configuración
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restaura la configuración original
    }
}

void Actualizar(Cuadrado& cuadro, int& posX, int& posY, int anchoMax, int altoMax) {
    if (std::cin.rdbuf()->in_avail()) {  // Verifica si hay entrada disponible
        char tecla = getchar();  // Lee un carácter de la entrada estándar
        int deltaX = 0, deltaY = 0;

        switch (tecla) {
            case 'a':
                // Si x no está en el borde izquierdo, se mueve a la izquierda
                deltaX = (posX > 0) ? -1 : 0;
                break;
            case 's':
                // Si y es menor al borde inferior considerando la altura del cuadrado, se mueve hacia abajo
                deltaY = (posY < altoMax - cuadro.obtenerLado()) ? 1 : 0;
                break;
            case 'd':
                // Si x no está en el borde derecho considerando el ancho del cuadrado, se mueve hacia la derecha
                deltaX = (posX < anchoMax - cuadro.obtenerLado() * 2) ? 1 : 0;
                break;
            case 'w':
                // Si y no está en el borde superior, se mueve hacia arriba
                deltaY = (posY > 0) ? -1 : 0;
                break;
            case 27:  // Escape
                set_input_mode(false);  // Restaurar configuración original
                exit(0);
        }

        if (deltaX != 0 || deltaY != 0) {
            cuadro.Borrar(posX, posY);
            posX += deltaX;
            posY += deltaY;
            cuadro.Dibujar(posX, posY);
        }
    }
}

int main() {
    int x = 0, y = 0, lado = 0;
    std::cout << "Ingrese la longitud del lado del cuadrado: ";
    std::cin >> lado;

    // Limpiar la pantalla
    std::cout << "\033[2J";

    Cuadrado cuadrado(lado);

    const int anchoMax = 80;
    const int altoMax = 25;

    set_input_mode(true);  // Habilitar el modo raw para leer caracteres sin bloqueo

    cuadrado.Dibujar(x, y); // Dibujar el cuadrado inicial

    while (true) {
        usleep(100000);  // 100 ms
        Actualizar(cuadrado, x, y, anchoMax, altoMax);
    }

    set_input_mode(false);  // Restaurar configuración original
    return 0;
}
   