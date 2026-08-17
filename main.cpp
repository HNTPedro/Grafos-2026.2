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

        int buscarVertice(const string& id) const {
            for (size_t i = 0; i < vertices.size(); i++) {
                if (vertices[i].id == id) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

    public:
        Grafo(bool ehDirecionado = false) {
            direcionado = ehDirecionado;
        }

        bool ehDirecionado() const {
            return direcionado;
        }    
        
        bool inserirVertice(const string& id) {
                if (buscarVertice(id) != -1) {
                    cout << "Erro: O vertice '" << id << "' ja existe\n";
                    return false;
                }
                Vertice novoVertice;
                novoVertice.id = id;
                vertices.push_back(novoVertice);
                cout << "Vertice '" << id << "' inserido\n";
                return true;
        }
        bool inserirAresta(const string& origem, const string& destino, const string& idAresta, double peso) {
            if (direcionado) {
                cout << "Erro: Grafo direcionado, use a opcao inserir arco\n";
                return false;
            }
            int idxOrigem = buscarVertice(origem);
            int idxDestino = buscarVertice(destino);
            if (idxOrigem == -1 || idxDestino == -1) {
                cout << "Erro: Vertice de origem ou destino nao encontrado\n";
                return false;
            }
            Conexao ida;
            ida.id = idAresta;
            ida.destino = destino;
            ida.peso = peso;
            vertices[idxOrigem].adjacentes.push_back(ida);
            if (idxOrigem != idxDestino) {
                Conexao volta;
                volta.id = idAresta;
                volta.destino = origem;
                volta.peso = peso;
                vertices[idxDestino].adjacentes.push_back(volta);
            }

            cout << "Aresta '" << idAresta << "' inserida entre '" << origem << "' e '" << destino << "'.\n";
            return true;
        }

        bool inserirArco(const string& origem, const string& destino, const string& idArco, double peso) {
            if (!direcionado) {
                cout << "Erro: Grafo nao direcionado, use a opcao inserir aresta\n";
                return false;
            }
            
            int idxOrigem = buscarVertice(origem);
            int idxDestino = buscarVertice(destino);
            if (idxOrigem == -1 || idxDestino == -1) {
                cout << "Erro: Vertice origem/destino nao encontrado\n";
                return false;
            }
            Conexao ida;
            ida.id = idArco;
            ida.destino = destino;
            ida.peso = peso;
            vertices[idxOrigem].adjacentes.push_back(ida);

            cout << "Arco '" << idArco << "' inserido de '" << origem << "' para '" << destino << "'.\n";
            return true;
        }

        bool removerVertice(const string& id) {
            int idx = buscarVertice(id);
            if (idx == -1) {
                cout << "Erro: Vertice '" << id << "' nao encontrado\n";
                return false;
            }

            for (size_t i = 0; i < vertices.size(); i++) {
                if (i != static_cast<size_t>(idx)) {
                    vector<Conexao>& adj = vertices[i].adjacentes;
                    for (auto it = adj.begin(); it != adj.end(); ) {
                        if (it->destino == id) {
                            it = adj.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            }

            vertices.erase(vertices.begin() + idx);
            cout << "Vertice '" << id << "' e  conexoes removidos\n";
            return true;
        }

        bool removerLigacao(const string& idLigacao) {
            bool encontrou = false;
            for (size_t i = 0; i < vertices.size(); i++) {
                vector<Conexao>& adj = vertices[i].adjacentes;
                for (auto it = adj.begin(); it != adj.end(); ) {
                    if (it->id == idLigacao) {
                        it = adj.erase(it);
                        encontrou = true;
                    } else {
                        ++it;
                    }
                }
            }

            if (encontrou) {
                cout << "Aresta/Arco '" << idLigacao << "' removido com sucesso.\n";
                return true;
            } else {
                cout << "Erro: Aresta/Arco '" << idLigacao << "' nao encontrado\n";
                return false;
            }
        }

        bool saoAdjacentes(const string& v1, const string& v2) {
            int idx1 = buscarVertice(v1);
            int idx2 = buscarVertice(v2);
            if (idx1 == -1 || idx2 == -1) {
                cout << "Erro: Um dos vertices informados nao existem no grafo\n";
                return false;
            }

            bool v1_aponta_v2 = false;
            bool v2_aponta_v1 = false;
            for (const auto& conexao : vertices[idx1].adjacentes) {
                if (conexao.destino == v2) {
                    v1_aponta_v2 = true;
                    break;
                }
            }

            if (direcionado) {
                for (const auto& conexao : vertices[idx2].adjacentes) {
                    if (conexao.destino == v1) {
                        v2_aponta_v1 = true;
                        break;
                    }
                }

                if (v1_aponta_v2 && v2_aponta_v1) {
                    cout << "Sao adjacentes. Ha arcos em ambos os sentidos entre '" << v1 << "' e '" << v2 << "'.\n";
                    return true;
                } else if (v1_aponta_v2) {
                    cout << "Sao adjacentes. Existe um arco de '" << v1 << "' para '" << v2 << "'.\n";
                    return true;
                } else if (v2_aponta_v1) {
                    cout << "Sao adjacentes. Existe um arco de '" << v2 << "' para '" << v1 << "'.\n";
                    return true;
                } else {
                    cout << "Os vertices '" << v1 << "' e '" << v2 << "' nao sao adjacentes.\n";
                    return false;
                }
            } else {
                if (v1_aponta_v2) {
                    cout << "Sao adjacentes. Existe uma aresta conectando '" << v1 << "' e '" << v2 << "'.\n";
                    return true;
                } else {
                    cout << "Os vertices '" << v1 << "' e '" << v2 << "' nao sao adjacentes.\n";
                    return false;
                }
            }
        }

        void retornarValor(const string& idLigacao) const {
            for (const auto& vertice : vertices) {
                for (const auto& conexao : vertice.adjacentes) {
                    if (conexao.id == idLigacao) {
                        cout << "Valor da ligacao'" << idLigacao << "': " << conexao.peso << "\n";
                        return;
                    }
                }
            }
            
            cout << "Erro: Aresta/Arco '" << idLigacao << "' nao encontrado\n";
        }

        void retornarExtremidades(const string& idLigacao) const {
            for (const auto& vertice : vertices) {
                for (const auto& conexao : vertice.adjacentes) {
                    if (conexao.id == idLigacao) {
                        if (direcionado) {
                            cout << "O arco '" << idLigacao << "' sai de '" << vertice.id 
                                << "' e chega em '" << conexao.destino << "'.\n";
                        } else {
                            cout << "A aresta '" << idLigacao << "' conecta '" 
                                << vertice.id << "' e '" << conexao.destino << "'.\n";
                        }
                    }
                }
            }
            
            cout << "Erro: Aresta/Arco '" << idLigacao << "' nao encontrado\n";
        }

        void mostrarMatrizAdjacencia() const {
            int n = vertices.size();
            if (n == 0) {
                cout << "O grafo esta vazio\n";
                return;
            }

            vector<vector<int>> matriz(n, vector<int>(n, 0));
            for (int i = 0; i < n; i++) {
                for (const auto& conexao : vertices[i].adjacentes) {
                    int j = buscarVertice(conexao.destino);
                    if (j != -1) {
                        matriz[i][j] = 1;
                    }
                }
            }

            cout << "\n=== Matriz de Adjacencia ===\n";
            cout << "    ";
            for (int i = 0; i < n; i++) {
                cout << vertices[i].id << " ";
            }
            cout << "\n";
            for (int i = 0; i < n; i++) {
                cout << vertices[i].id << " | ";
                for (int j = 0; j < n; j++) {
                    cout << matriz[i][j] << " ";
                }
                cout << "\n";
            }
            cout << "============================\n";
        }

        void mostrarMatrizIncidencia() const {
            int numVertices = vertices.size();
            if (numVertices == 0) {
                cout << "O grafo esta vazio\n";
                return;
            }

            struct LigacaoUnica {
                string id;
                int origemIdx;
                int destinoIdx;
            };

            vector<LigacaoUnica> ligacoes;

            for (int i = 0; i < numVertices; i++) {
                for (const auto& conexao : vertices[i].adjacentes) {
                    bool jaExiste = false;
                    if (!direcionado) {
                        for (const auto& ligacao : ligacoes) {
                            if (ligacao.id == conexao.id) {
                                jaExiste = true;
                                break;
                            }
                        }
                    }

                    if (!jaExiste) {
                        int j = buscarVertice(conexao.destino);
                        if (j != -1) {
                            ligacoes.push_back({conexao.id, i, j});
                        }
                    }
                }
            }

            int numLigacoes = ligacoes.size();
            if (numLigacoes == 0) {
                cout << "O grafo nao possui arestas/arcos\n";
            }

            vector<vector<int>> matriz(numVertices, vector<int>(numLigacoes, 0));
            for (int j = 0; j < numLigacoes; j++) {
                int origem = ligacoes[j].origemIdx;
                int destino = ligacoes[j].destinoIdx;

                if (direcionado) {
                    matriz[origem][j] = 1;
                    matriz[destino][j] = -1; 
                } else {
                    matriz[origem][j] = 1; 
                    matriz[destino][j] = 1;
                }
            }

            cout << "\n=== Matriz de Incidencia ===\n";
            cout << "    "; 
            for (int j = 0; j < numLigacoes; j++) {
                cout << ligacoes[j].id << "\t";
            }
            cout << "\n";
            for (int i = 0; i < numVertices; i++) {
                cout << vertices[i].id << " | ";
                for (int j = 0; j < numLigacoes; j++) {
                    if (matriz[i][j] >= 0 && direcionado) cout << " "; 
                    
                    cout << matriz[i][j] << "\t";
                }
                cout << "\n";
            }
            cout << "============================\n";
        }

        void mostrarGrafo() const {
            if (vertices.empty()) {
                cout << "O grafo esta vazio\n";
                return;
            }

            cout << "\n=== Representacao ===\n";
            for (const auto& vertice : vertices) {
                cout << "[" << vertice.id << "] -> ";
                if (vertice.adjacentes.empty()) {
                    cout << "vazio";
                } else {
                    for (size_t i = 0; i < vertice.adjacentes.size(); i++) {
                        const auto& conexao = vertice.adjacentes[i];
                        cout << conexao.destino << " (id: " << conexao.id << ", peso: " << conexao.peso << ")";
                        if (i < vertice.adjacentes.size() - 1) {
                            cout << " -> ";
                        }
                    }
                }
                cout << "\n";
            }
            cout << "======================================================\n";
        }

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
    int tipoGrafo;
        cout << "Deseja criar um grafo:\n1. Nao Direcionado\n2. Direcionado\nOpcao: ";
        cin >> tipoGrafo;
        Grafo grafo(tipoGrafo == 2);
        
    int opcao;

        do {
            mostrarMenu();
            cin >> opcao;

            switch (opcao) {

                case 1:
                    string idVertice;
                    cout << "Digite o ID do novo vertice: ";
                    cin >> idVertice;
                    grafo.inserirVertice(idVertice);
                    break;

                case 2: 
                    string idAresta, origem, destino;
                    double peso;
                    cout << "Digite o ID da aresta: ";
                    cin >> idAresta;
                    cout << "Digite o vertice origem: ";
                    cin >> origem;
                    cout << "Digite o vertice destino: ";
                    cin >> destino;
                    cout << "Digite o peso da aresta: ";
                    cin >> peso;
                    grafo.inserirAresta(origem, destino, idAresta, peso);
                    break;
            
                case 3:
                    string idArco, origem, destino;
                    double peso;
                    cout << "Digite o ID do arco: ";
                    cin >> idArco;
                    cout << "Digite o vertice origem: ";
                    cin >> origem;
                    cout << "Digite o vertice destino: ";
                    cin >> destino;
                    cout << "Digite o peso do arco: ";
                    cin >> peso;
                    grafo.inserirArco(origem, destino, idArco, peso);
                    break;

                case 4:
                    string idVertice;
                    cout << "Digite o ID do vertice: ";
                    cin >> idVertice;
                    grafo.removerVertice(idVertice);
                    break;

                case 5:
                    string idLigacao;
                    cout << "Digite o ID da aresta/arco: ";
                    cin >> idLigacao;
                    grafo.removerLigacao(idLigacao);
                    break;

                case 6:
                    string v1, v2;
                    cout << "Digite o primeiro vertice: ";
                    cin >> v1;
                    cout << "Digite o segundo vertice: ";
                    cin >> v2;
                    grafo.saoAdjacentes(v1, v2);
                    break;
                            
                case 7:
                    string idLigacao;
                    cout << "Digite o ID da aresta/arco: ";
                    cin >> idLigacao;
                    grafo.retornarValor(idLigacao);
                    break;

                case 8:
                    string idLigacao;
                    cout << "Digite o ID da aresta/arco: ";
                    cin >> idLigacao;
                    grafo.retornarExtremidades(idLigacao);
                    break;

                case 9:
                    grafo.mostrarMatrizAdjacencia();
                    break;

                case 10:
                    grafo.mostrarMatrizIncidencia();
                    break;

                case 11:
                    grafo.mostrarGrafo();
                    break;

                case 0:
                    cout << "Encerrando programa...\n";
                    break;

                default:
                    cout << "Opcao invalida!\n";
            }

        } while (opcao != 0);

        return 0;
}