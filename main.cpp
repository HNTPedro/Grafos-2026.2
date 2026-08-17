#include <iostream>
#include <vector>
#include <string>


using namespace std;

//Estruturas abaixo.

struct Conexao {
    string id;
    string destino;
    double peso;

};

struct Vertice{
    string id;
    vector<Conexao> adjacentes;
};

class Grafo {
    private:
        vector<Vertice> vertices;
        bool direcionado;
    public:
    //Métodos 


};

//funções do main
void mostrarMenu() {
    cout << "\n========== MENU DO GRAFO ==========\n";
    cout << "1. Inserir vertice\n";
    cout << "2. Inserir aresta\n";
    cout << "3. Inserir arco\n";
    cout << "4. Remover vertice\n";
    cout << "5. Remover aresta/arco\n";
    cout << "6. Verificar adjacencia\n";
    cout << "7. Retornar valor da aresta/arco\n";
    cout << "8. Retornar extremidades\n";
    cout << "9. Mostrar matriz de adjacencia\n";
    cout << "10. Mostrar matriz de incidencia\n";
    cout << "11. Mostrar grafo\n";
    cout << "0. Sair\n";
    cout << "===================================\n";
    cout << "Escolha uma opcao: ";
};


int main() {
    
int opcao;

    do {
        mostrarMenu();
        cin >> opcao;

        switch (opcao) {

            case 1:
                cout << "Inserir vertice\n";
                // futuramente: grafo.inserirVertice(...)
                break;

            case 2:
                cout << "Inserir aresta\n";
                // futuramente: grafo.inserirAresta(...)
                break;

            case 3:
                cout << "Inserir arco\n";
                // futuramente: grafo.inserirArco(...)
                break;

            case 4:
                cout << "Remover vertice\n";
                // futuramente: grafo.removerVertice(...)
                break;

            case 5:
                cout << "Remover aresta/arco\n";
                // futuramente: grafo.removerLigacao(...)
                break;

            case 6:
                cout << "Verificar adjacencia\n";
                // futuramente: grafo.saoAdjacentes(...)
                break;

            case 7:
                cout << "Retornar valor da aresta/arco\n";
                // futuramente: grafo.retornarValor(...)
                break;

            case 8:
                cout << "Retornar extremidades\n";
                // futuramente: grafo.retornarExtremidades(...)
                break;

            case 9:
                cout << "Mostrar matriz de adjacencia\n";
                // futuramente: grafo.mostrarMatrizAdjacencia()
                break;

            case 10:
                cout << "Mostrar matriz de incidencia\n";
                // futuramente: grafo.mostrarMatrizIncidencia()
                break;

            case 11:
                cout << "Mostrar grafo\n";
                // futuramente: grafo.mostrarGrafo()
                break;

            case 0:
                cout << "Encerrando programa...\n";
                break;

            default:
                cout << "Opcao invalida!\n";
        }

    } while (opcao != 0);

    return 0;



















    return 0;
}