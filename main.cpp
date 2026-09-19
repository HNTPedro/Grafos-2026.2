#include <iostream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

// Estrutura que representa a ligação (arco ou aresta)
struct Conexao {
    string id;
    string destino;
    double peso;
};

// Estrutura que representa um Vértice e a sua Lista de Adjacência
struct Vertice {
    string id;
    vector<Conexao> adjacentes; // Lista de adjacência
};

// Classe principal do Grafo
class Grafo {
    private:
        vector<Vertice> vertices; // Conjunto V de vértices
        bool direcionado;         // Flag que indica se o grafo é dirigido ou não

        int buscarVertice(const string& id) const {
            for (size_t i = 0; i < vertices.size(); i++) {
                if (vertices[i].id == id) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

        // Função auxiliar recursiva para Busca em Profundidade (DFS)
        bool dfsRecursivo(int atualIdx, int destIdx, vector<bool>& visitado, vector<string>& caminho, int nivel) const {
            visitado[atualIdx] = true;
            caminho.push_back(vertices[atualIdx].id);

            // Condição de parada: encontrou o vértice procurado
            if (atualIdx == destIdx) {
                return true;
            }

            // Visita os vértices adjacentes
            for (const auto& conexao : vertices[atualIdx].adjacentes) {
                int proxIdx = buscarVertice(conexao.destino);
                if (proxIdx != -1 && !visitado[proxIdx]) {
                    
                    // Desenha a árvore graficamente no terminal
                    for(int i = 0; i < nivel; i++) cout << "    ";
                    cout << "|-- [" << vertices[atualIdx].id << "] -> [" << conexao.destino << "]\n";

                    // Chamada recursiva simulando a Pilha (LIFO)
                    if (dfsRecursivo(proxIdx, destIdx, visitado, caminho, nivel + 1)) {
                        return true; // Propaga a confirmação de que encontrou o destino
                    }
                }
            }

            // Backtracking: se não encontrou o destino neste caminho, remove da trilha atual
            caminho.pop_back();
            return false;
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
            cout << "Vertice '" << id << "' e conexoes removidos\n";
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
                        cout << "Valor da ligacao '" << idLigacao << "': " << conexao.peso << "\n";
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

        // Aplicar Algoritmo de PRIM e exibir a Árvore Geradora Mínima (AGM) no terminal
        void aplicarPrim(const string& verticeInicial) const {
            if (direcionado) {
                cout << "Aviso: O algoritmo de Prim e projetado principalmente para grafos nao direcionados.\n";
            }
            if (vertices.empty()) {
                cout << "O grafo esta vazio\n";
                return;
            }

            int startIdx = buscarVertice(verticeInicial);
            if (startIdx == -1) {
                cout << "Erro: Vertice inicial '" << verticeInicial << "' nao encontrado no grafo.\n";
                return;
            }

            int n = vertices.size();
            vector<bool> visitado(n, false);
            visitado[startIdx] = true;

            struct ArestaMST {
                string id;
                string origem;
                string destino;
                double peso;
            };

            vector<ArestaMST> agm;
            double custoTotal = 0.0;
            int arestasAdicionadas = 0;

            while (arestasAdicionadas < n - 1) {
                double minPeso = numeric_limits<double>::max();
                ArestaMST melhorAresta = {"", "", "", 0.0};
                int proximoVertice = -1;
                bool encontrou = false;

                for (int i = 0; i < n; i++) {
                    if (visitado[i]) {
                        for (const auto& conexao : vertices[i].adjacentes) {
                            int destIdx = buscarVertice(conexao.destino);
                            if (destIdx != -1 && !visitado[destIdx]) {
                                if (conexao.peso < minPeso) {
                                    minPeso = conexao.peso;
                                    melhorAresta = {conexao.id, vertices[i].id, conexao.destino, conexao.peso};
                                    proximoVertice = destIdx;
                                    encontrou = true;
                                }
                            }
                        }
                    }
                }

                if (!encontrou) {
                    cout << "Nota: O grafo e desconexo. A AGM cobrira apenas a componente alcancavel a partir de '" << verticeInicial << "'.\n";
                    break;
                }

                visitado[proximoVertice] = true;
                agm.push_back(melhorAresta);
                custoTotal += melhorAresta.peso;
                arestasAdicionadas++;
            }

            cout << "\n========= Arvore Geradora Minima (AGM) - PRIM =========\n";
            cout << "Custo total da AGM: " << custoTotal << "\n";
            cout << "Arestas pertencentes a AGM:\n";
            for (const auto& aresta : agm) {
                cout << "  [" << aresta.origem << "] --(ID: " << aresta.id << " / Peso: " << aresta.peso << ")--> [" << aresta.destino << "]\n";
            }
            cout << "=======================================================\n";
        }

        // Operação de Busca em Profundidade Guiada (DFS)
        void aplicarDFS(const string& origem, const string& destino) const {
            int idxOrigem = buscarVertice(origem);
            int idxDestino = buscarVertice(destino);

            if (idxOrigem == -1 || idxDestino == -1) {
                cout << "Erro: Vertice de origem ou destino nao encontrado no grafo.\n";
                return;
            }

            cout << "\n========= Busca em Profundidade (DFS) =========\n";
            cout << "Procurando caminho de '" << origem << "' para '" << destino << "'...\n\n";
            cout << "Arvore de Busca Gerada:\n";
            cout << "[" << origem << "]\n";

            vector<bool> visitado(vertices.size(), false);
            vector<string> caminho; // Rastreia o caminho exato tomado até o destino

            bool encontrou = dfsRecursivo(idxOrigem, idxDestino, visitado, caminho, 1);

            cout << "\nResultado da Busca:\n";
            if (encontrou) {
                cout << "Destino encontrado! Caminho percorrido: ";
                for (size_t i = 0; i < caminho.size(); i++) {
                    cout << caminho[i];
                    if (i < caminho.size() - 1) cout << " -> ";
                }
                cout << "\n";
            } else {
                cout << "Nenhum caminho encontrado de '" << origem << "' para '" << destino << "'.\n";
            }
            cout << "===============================================\n";
        }

        // Algoritmo de Roy para encontrar Componentes Conexas / Fortemente Conexas
        void aplicarRoy() const {
            int n = vertices.size();
            if (n == 0) {
                cout << "O grafo esta vazio.\n";
                return;
            }

            vector<bool> disponivel(n, true);
            int numDisponiveis = n;
            int numComponente = 1;

            cout << "\n========= Algoritmo de Roy (Componentes Conexas) =========\n";

            while (numDisponiveis > 0) {
                int v = -1;
                // Escolher um vértice 'v' qualquer não marcado/disponível
                for (int i = 0; i < n; ++i) {
                    if (disponivel[i]) {
                        v = i;
                        break;
                    }
                }

                if (v == -1) break;

                vector<bool> marcacaoPositiva(n, false);
                vector<bool> marcacaoNegativa(n, false);

                // Marcar v com (+) e (-)
                marcacaoPositiva[v] = true;
                marcacaoNegativa[v] = true;

                // Propagar rotulação (+)
                // Marcar w com (+) se tiver como sucessor um vértice já marcado com (+)
                bool mudou;
                do {
                    mudou = false;
                    for (int w = 0; w < n; ++w) {
                        if (disponivel[w] && !marcacaoPositiva[w]) {
                            for (const auto& conexao : vertices[w].adjacentes) {
                                int sucIdx = buscarVertice(conexao.destino);
                                if (sucIdx != -1 && disponivel[sucIdx] && marcacaoPositiva[sucIdx]) {
                                    marcacaoPositiva[w] = true;
                                    mudou = true;
                                    break;
                                }
                            }
                        }
                    }
                } while (mudou);

                // Propagar rotulação (-)
                // Marcar w com (-) se tiver como antecessor um vértice já marcado com (-)
                do {
                    mudou = false;
                    for (int u = 0; u < n; ++u) { 
                        if (disponivel[u] && marcacaoNegativa[u]) {
                            for (const auto& conexao : vertices[u].adjacentes) {
                                int w = buscarVertice(conexao.destino);
                                if (w != -1 && disponivel[w] && !marcacaoNegativa[w]) {
                                    marcacaoNegativa[w] = true;
                                    mudou = true;
                                }
                            }
                        }
                    }
                } while (mudou);

                // Conjunto Si = vértices marcados com (+) e (-) simultaneamente
                cout << (direcionado ? "Componente Fortemente Conexa " : "Componente Conexa ") << numComponente << ": { ";
                for (int i = 0; i < n; ++i) {
                    if (disponivel[i] && marcacaoPositiva[i] && marcacaoNegativa[i]) {
                        cout << vertices[i].id << " ";
                        disponivel[i] = false; // Retirar Si de V (conjunto de vértices disponíveis)
                        numDisponiveis--;
                    }
                }
                cout << "}\n";
                numComponente++;
            }
            cout << "==========================================================\n";
        }
};

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
    cout << "12. Aplicar algoritmo de PRIM (AGM)\n";
    cout << "13. Aplicar Busca em Profundidade (DFS - Guiada)\n";
    cout << "14. Aplicar algoritmo de Roy (Componentes Conexas)\n";
    cout << "0. Sair\n";
    cout << "===================================\n";
    cout << "Escolha uma opcao: ";
}

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
            case 1: {
                string idVertice;
                cout << "Digite o ID do novo vertice: ";
                cin >> idVertice;
                grafo.inserirVertice(idVertice);
                break;
            }

            case 2: {
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
            }
        
            case 3: {
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
            }

            case 4: {
                string idVertice;
                cout << "Digite o ID do vertice: ";
                cin >> idVertice;
                grafo.removerVertice(idVertice);
                break;
            }

            case 5: {
                string idLigacao;
                cout << "Digite o ID da aresta/arco: ";
                cin >> idLigacao;
                grafo.removerLigacao(idLigacao);
                break;
            }

            case 6: {
                string v1, v2;
                cout << "Digite o primeiro vertice: ";
                cin >> v1;
                cout << "Digite o segundo vertice: ";
                cin >> v2;
                grafo.saoAdjacentes(v1, v2);
                break;
            }
                        
            case 7: {
                string idLigacao;
                cout << "Digite o ID da aresta/arco: ";
                cin >> idLigacao;
                grafo.retornarValor(idLigacao);
                break;
            }

            case 8: {
                string idLigacao;
                cout << "Digite o ID da aresta/arco: ";
                cin >> idLigacao;
                grafo.retornarExtremidades(idLigacao);
                break;
            }

            case 9:
                grafo.mostrarMatrizAdjacencia();
                break;

            case 10:
                grafo.mostrarMatrizIncidencia();
                break;

            case 11:
                grafo.mostrarGrafo();
                break;

            case 12: {
                string idInicio;
                cout << "Digite o ID do vertice de inicio para o algoritmo de PRIM: ";
                cin >> idInicio;
                grafo.aplicarPrim(idInicio);
                break;
            }

            case 13: {
                string origem, destino;
                cout << "Digite o ID do vertice de origem (saida): ";
                cin >> origem;
                cout << "Digite o ID do vertice de destino (chegada): ";
                cin >> destino;
                grafo.aplicarDFS(origem, destino);
                break;
            }

            case 14: {
                grafo.aplicarRoy();
                break;
            }

            case 0:
                cout << "Encerrando programa...\n";
                break;

            default:
                cout << "Opcao invalida!\n";
        }

    } while (opcao != 0);

    return 0;
}