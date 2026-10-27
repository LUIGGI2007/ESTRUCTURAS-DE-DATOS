
#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <cctype>
#include <stdexcept>
using namespace std;

// Determina la prioridad de los operadores
int prioridad(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

bool esOperador(char c) {
    return c == '+' || c == '-' ||
           c == '*' || c == '/' || c == '^';
}

// Convierte una expresion infija a postfija
string infijaAPostfija(string expresion) {
    stack<char> pila;
    string resultado;

    for (int i = 0; i < (int)expresion.size(); i++) {
        char c = expresion[i];

        if (isspace((unsigned char)c)) continue;

        // Reconocer numeros completos y decimales
        if (isdigit((unsigned char)c) || c == '.') {
            string numero;

            while (i < (int)expresion.size() &&
                  (isdigit((unsigned char)expresion[i]) ||
                   expresion[i] == '.')) {
                numero += expresion[i];
                i++;
            }

            i--;
            resultado += numero + " ";
        }
        else if (isalpha((unsigned char)c)) {
            resultado += string(1, c) + " ";
        }
        else if (c == '(') {
            pila.push(c);
        }
        else if (c == ')') {
            while (!pila.empty() && pila.top() != '(') {
                resultado += pila.top();
                resultado += " ";
                pila.pop();
            }

            if (pila.empty())
                throw runtime_error("Parentesis incorrectos");

            pila.pop();
        }
        else if (esOperador(c)) {
            while (!pila.empty() && pila.top() != '(' &&
                  (prioridad(pila.top()) > prioridad(c) ||
                  (prioridad(pila.top()) == prioridad(c) &&
                   c != '^'))) {
                resultado += pila.top();
                resultado += " ";
                pila.pop();
            }

            pila.push(c);
        }
        else {
            throw runtime_error("Caracter no reconocido");
        }
    }

    while (!pila.empty()) {
        if (pila.top() == '(')
            throw runtime_error("Parentesis incorrectos");

        resultado += pila.top();
        resultado += " ";
        pila.pop();
    }

    return resultado;
}

// Convierte postfija a prefija usando otra pila
string postfijaAPrefija(string postfija) {
    stack<string> pila;
    string token;
    stringstream entrada(postfija);

    while (entrada >> token) {
        if (token.size() == 1 && esOperador(token[0])) {
            if (pila.size() < 2)
                throw runtime_error("Expresion incorrecta");

            string derecha = pila.top();
            pila.pop();

            string izquierda = pila.top();
            pila.pop();

            pila.push(token + " " + izquierda + " " + derecha);
        }
        else {
            pila.push(token);
        }
    }

    if (pila.size() != 1)
        throw runtime_error("Expresion incorrecta");

    return pila.top();
}

// Evalua una expresion postfija numerica
double evaluarPostfija(string postfija) {
    stack<double> pila;
    string token;
    stringstream entrada(postfija);

    while (entrada >> token) {
        if (token.size() == 1 && esOperador(token[0])) {
            if (pila.size() < 2)
                throw runtime_error("Expresion incorrecta");

            double b = pila.top();
            pila.pop();

            double a = pila.top();
            pila.pop();

            double resultado = 0;

            switch (token[0]) {
                case '+': resultado = a + b; break;
                case '-': resultado = a - b; break;
                case '*': resultado = a * b; break;
                case '/':
                    if (b == 0)
                        throw runtime_error("Division por cero");
                    resultado = a / b;
                    break;
                case '^': resultado = pow(a, b); break;
            }

            pila.push(resultado);
        }
        else {
            size_t pos = 0;
            double numero = stod(token, &pos);

            if (pos != token.size())
                throw runtime_error(
                    "Para evaluar, ingresa solo numeros"
                );

            pila.push(numero);
        }
    }

    if (pila.size() != 1)
        throw runtime_error("Expresion incorrecta");

    return pila.top();
}


int main() {
    int opcion;
    string expresion;

    do {
        cout << "\n=================================\n";
        cout << "   CALCULADORA DE NOTACIONES\n";
        cout << "=================================\n";
        cout << "1. Infija a postfija\n";
        cout << "2. Infija a prefija\n";
        cout << "3. Evaluar expresion numerica\n";
        cout << "4. Mostrar ambas notaciones\n";
        cout << "5. Salir\n";
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion)) {
            cout << "Opcion invalida.\n";
            break;
        }

        if (opcion == 5) break;

        if (opcion < 1 || opcion > 5) {
            cout << "Opcion no valida.\n";
            continue;
        }

        cout << "Ingrese la expresion infija: ";
        cin >> ws;
        getline(cin, expresion);

        try {
            string postfija = infijaAPostfija(expresion);

            switch (opcion) {
                case 1:
                    cout << "Postfija: " << postfija << "\n";
                    break;

                case 2:
                    cout << "Prefija: "
                         << postfijaAPrefija(postfija) << "\n";
                    break;

                case 3:
                    break;

                case 4:
                    cout << "Infija: " << expresion << "\n";
                    cout << "Prefija: "
                         << postfijaAPrefija(postfija) << "\n";
                    cout << "Postfija: " << postfija << "\n";
                    break;
            }

            // Mostrar el resultado numerico en todas las opciones
            cout << "Resultado: "
                 << evaluarPostfija(postfija) << "\n";
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }

    } while (true);

    cout << "Programa finalizado.\n";
    return 0;
}
