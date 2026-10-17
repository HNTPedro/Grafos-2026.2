#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

class Adjacencia {
private:
    string vizinhoId;
    string conexaoId;
public:
    Adjacencia(const string& vizinho = "", const string& conexao = "")
        : vizinhoId(vizinho), conexaoId(conexao) {}
    const string& getVizinhoId() const { return vizinhoId; }
    const string& getConexaoId() const { return conexaoId; }
};

class Vertice {
private:
    string id;
    double latitude;
    double longitude;
    vector<Adjacencia> adjacentes;
public:
    Vertice(const string& identificador = "", double lat = 0.0, double lon = 0.0)
        : id(identificador), latitude(lat), longitude(lon) {}
    const string& getId() const { return id; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
    const vector<Adjacencia>& getAdjacentes() const { return adjacentes; }
    bool adicionarAdjacencia(const string& vizinho, const string& conexao) {
        for (const auto& a : adjacentes)
            if (a.getConexaoId() == conexao && a.getVizinhoId() == vizinho) return false;
        adjacentes.emplace_back(vizinho, conexao);
        return true;
    }
    void limparAdjacencias() { adjacentes.clear(); }
};

class Conexao {
public:
    enum class Tipo { Arco, Aresta };
private:
    string id;
    string origem;
    string destino;
    double peso;
    Tipo tipo;
public:
    Conexao(const string& identificador, const string& origemId, const string& destinoId,
            double valorPeso, Tipo tipoConexao)
        : id(identificador), origem(origemId), destino(destinoId), peso(valorPeso), tipo(tipoConexao) {}
    const string& getId() const { return id; }
    const string& getOrigem() const { return origem; }
    const string& getDestino() const { return destino; }
    double getPeso() const { return peso; }
    Tipo getTipo() const { return tipo; }
    string getTipoTexto() const { return tipo == Tipo::Arco ? "ARCO" : "ARESTA"; }
};

class Grafo {
private:
    vector<Vertice> vertices;
    vector<Conexao> conexoes;
    bool direcionado;

    int indiceVertice(const string& id) const {
        for (size_t i = 0; i < vertices.size(); ++i)
            if (vertices[i].getId() == id) return static_cast<int>(i);
        return -1;
    }
    int indiceConexao(const string& id) const {
        for (size_t i = 0; i < conexoes.size(); ++i)
            if (conexoes[i].getId() == id) return static_cast<int>(i);
        return -1;
    }
    void reconstruirAdjacencias() {
        for (auto& v : vertices) v.limparAdjacencias();
        for (const auto& c : conexoes) {
            int oi = indiceVertice(c.getOrigem()), di = indiceVertice(c.getDestino());
            if (oi < 0 || di < 0) throw runtime_error("Conexao referencia vertice inexistente.");
            vertices[static_cast<size_t>(oi)].adicionarAdjacencia(c.getDestino(), c.getId());
            if (c.getTipo() == Conexao::Tipo::Aresta && c.getOrigem() != c.getDestino())
                vertices[static_cast<size_t>(di)].adicionarAdjacencia(c.getOrigem(), c.getId());
        }
    }
public:
    explicit Grafo(bool ehDirecionado = false) : direcionado(ehDirecionado) {}
    bool ehDirecionado() const { return direcionado; }
    const vector<Vertice>& getVertices() const { return vertices; }
    const vector<Conexao>& getConexoes() const { return conexoes; }
    bool existeVertice(const string& id) const { return indiceVertice(id) >= 0; }
    bool existeConexao(const string& id) const { return indiceConexao(id) >= 0; }
    const Vertice* buscarVertice(const string& id) const {
        int i = indiceVertice(id);
        return i < 0 ? nullptr : &vertices[static_cast<size_t>(i)];
    }
    bool adicionarVertice(const string& id, double lat, double lon, string& erro) {
        if (id.empty()) { erro = "O ID do vertice nao pode ficar vazio."; return false; }
        if (existeVertice(id)) { erro = "Ja existe um vertice com esse ID."; return false; }
        if (!isfinite(lat) || lat < -90.0 || lat > 90.0) {
            erro = "Latitude deve estar entre -90 e 90 graus."; return false;
        }
        if (!isfinite(lon) || lon < -180.0 || lon > 180.0) {
            erro = "Longitude deve estar entre -180 e 180 graus."; return false;
        }
        vertices.emplace_back(id, lat, lon);
        erro.clear(); return true;
    }
    bool adicionarConexao(const string& id, const string& origem, const string& destino,
                          double peso, Conexao::Tipo tipo, string& erro) {
        if (id.empty()) { erro = "O ID da conexao nao pode ficar vazio."; return false; }
        if (existeConexao(id)) { erro = "Ja existe uma conexao com esse ID."; return false; }
        if (tipo == Conexao::Tipo::Arco && !direcionado) {
            erro = "Nao e permitido criar arcos em um grafo nao direcionado. Use uma aresta."; return false;
        }
        if (tipo == Conexao::Tipo::Aresta && direcionado) {
            erro = "Nao e permitido criar arestas em um grafo direcionado. Use um arco."; return false;
        }
        if (!isfinite(peso)) { erro = "O peso deve ser um numero finito."; return false; }
        if (!existeVertice(origem) || !existeVertice(destino)) {
            erro = "A origem e o destino precisam existir no grafo."; return false;
        }
        conexoes.emplace_back(id, origem, destino, peso, tipo);
        try { reconstruirAdjacencias(); }
        catch (...) { conexoes.pop_back(); reconstruirAdjacencias(); erro = "Falha ao atualizar adjacencias."; return false; }
        erro.clear(); return true;
    }
    bool removerConexao(const string& id, string& erro) {
        int i = indiceConexao(id);
        if (i < 0) { erro = "Conexao nao encontrada."; return false; }
        conexoes.erase(conexoes.begin() + i); reconstruirAdjacencias(); erro.clear(); return true;
    }
    bool removerVertice(const string& id, string& erro) {
        int i = indiceVertice(id);
        if (i < 0) { erro = "Vertice nao encontrado."; return false; }
        conexoes.erase(remove_if(conexoes.begin(), conexoes.end(), [&](const Conexao& c) {
            return c.getOrigem() == id || c.getDestino() == id;
        }), conexoes.end());
        vertices.erase(vertices.begin() + i); reconstruirAdjacencias(); erro.clear(); return true;
    }
    void substituirPor(Grafo&& outro) {
        vertices = move(outro.vertices); conexoes = move(outro.conexoes); direcionado = outro.direcionado;
    }
    bool validar(string& erro) const {
        set<string> idsV, idsC;
        for (const auto& v : vertices) {
            if (v.getId().empty() || !idsV.insert(v.getId()).second ||
                !isfinite(v.getLatitude()) || v.getLatitude() < -90 || v.getLatitude() > 90 ||
                !isfinite(v.getLongitude()) || v.getLongitude() < -180 || v.getLongitude() > 180) {
                erro = "Vertice invalido, duplicado ou com coordenadas fora do intervalo."; return false;
            }
        }
        for (const auto& c : conexoes) {
            if (c.getId().empty() || !idsC.insert(c.getId()).second) {
                erro = "ID de conexao vazio ou duplicado."; return false;
            }
            if ((direcionado && c.getTipo() != Conexao::Tipo::Arco) ||
                (!direcionado && c.getTipo() != Conexao::Tipo::Aresta)) {
                erro = "O arquivo mistura tipo de conexao incompatível com o tipo do grafo."; return false;
            }
            if (!isfinite(c.getPeso()) || !idsV.count(c.getOrigem()) || !idsV.count(c.getDestino())) {
                erro = "Peso invalido ou conexao referencia vertice inexistente."; return false;
            }
        }
        erro.clear(); return true;
    }
    bool reconstruirParaCarga(string& erro) {
        if (!validar(erro)) return false;
        try { reconstruirAdjacencias(); }
        catch (...) { erro = "Falha ao reconstruir adjacencias."; return false; }
        return true;
    }
};

class PersistenciaGrafo {
private:
    fs::path pasta;
public:
    explicit PersistenciaGrafo(const fs::path& diretorio = "grafos") : pasta(diretorio) {}
    const fs::path& getPasta() const { return pasta; }
    bool garantirPasta(string& erro) const {
        error_code ec;
        if (!fs::exists(pasta, ec)) fs::create_directories(pasta, ec);
        if (ec || !fs::is_directory(pasta, ec)) { erro = "Nao foi possivel acessar a pasta de grafos."; return false; }
        erro.clear(); return true;
    }
    vector<fs::path> listarArquivos(string& erro) const {
        vector<fs::path> r;
        if (!garantirPasta(erro)) return r;
        error_code ec;
        for (fs::directory_iterator it(pasta, ec), fim; !ec && it != fim; it.increment(ec))
            if (it->is_regular_file() && it->path().extension() == ".grafo") r.push_back(it->path());
        if (ec) erro = "Erro ao listar arquivos."; else erro.clear();
        sort(r.begin(), r.end()); return r;
    }
    bool salvar(const Grafo& g, const string& nome, string& erro) const {
        if (nome.empty() || nome == "." || nome == ".." || nome.find('/') != string::npos || nome.find('\\') != string::npos) {
            erro = "Informe um nome valido, sem caminho."; return false;
        }
        if (!garantirPasta(erro)) return false;
        string n = nome;
        if (fs::path(n).extension() != ".grafo") n += ".grafo";
        fs::path destino = pasta / n, temp = destino; temp += ".tmp";
        ofstream out(temp, ios::trunc);
        if (!out) { erro = "Nao foi possivel abrir o arquivo para escrita."; return false; }
        out << "GRAFO_CPP_2\nDIRECIONADO " << (g.ehDirecionado() ? 1 : 0) << "\n";
        out << "VERTICES " << g.getVertices().size() << '\n' << setprecision(numeric_limits<double>::max_digits10);
        for (const auto& v : g.getVertices())
            out << "V " << quoted(v.getId()) << ' ' << v.getLatitude() << ' ' << v.getLongitude() << '\n';
        out << "CONEXOES " << g.getConexoes().size() << '\n';
        for (const auto& c : g.getConexoes())
            out << "C " << quoted(c.getId()) << ' ' << quoted(c.getOrigem()) << ' '
                << quoted(c.getDestino()) << ' ' << c.getPeso() << ' ' << c.getTipoTexto() << '\n';
        out << "FIM\n"; out.close();
        if (!out) { fs::remove(temp); erro = "Falha ao escrever arquivo."; return false; }
        error_code ec; fs::rename(temp, destino, ec);
        if (ec) { error_code er; fs::remove(destino, er); ec.clear(); fs::rename(temp, destino, ec); }
        if (ec) { fs::remove(temp); erro = "Falha ao finalizar arquivo: " + ec.message(); return false; }
        erro.clear(); return true;
    }
    bool carregar(const fs::path& caminho, Grafo& destino, string& erro) const {
        ifstream in(caminho);
        string token;
        if (!(in >> token) || token != "GRAFO_CPP_2") { erro = "Formato invalido ou versao antiga sem coordenadas."; return false; }
        int dir;
        if (!(in >> token >> dir) || token != "DIRECIONADO" || (dir != 0 && dir != 1)) { erro = "Campo DIRECIONADO invalido."; return false; }
        Grafo temp(dir == 1);
        size_t nv;
        if (!(in >> token >> nv) || token != "VERTICES" || nv > 10000000) { erro = "Quantidade de vertices invalida."; return false; }
        for (size_t i = 0; i < nv; ++i) {
            string tipo, id; double lat, lon;
            if (!(in >> tipo >> quoted(id) >> lat >> lon) || tipo != "V") { erro = "Registro de vertice incompleto."; return false; }
            string e; if (!temp.adicionarVertice(id, lat, lon, e)) { erro = e; return false; }
        }
        size_t nc;
        if (!(in >> token >> nc) || token != "CONEXOES" || nc > 10000000) { erro = "Quantidade de conexoes invalida."; return false; }
        for (size_t i = 0; i < nc; ++i) {
            string tipo, id, o, d, t; double peso;
            if (!(in >> tipo >> quoted(id) >> quoted(o) >> quoted(d) >> peso >> t) || tipo != "C") { erro = "Registro de conexao incompleto."; return false; }
            Conexao::Tipo ct;
            if (t == "ARCO") ct = Conexao::Tipo::Arco;
            else if (t == "ARESTA") ct = Conexao::Tipo::Aresta;
            else { erro = "Tipo de conexao desconhecido."; return false; }
            string e; if (!temp.adicionarConexao(id, o, d, peso, ct, e)) { erro = e; return false; }
        }
        if (!(in >> token) || token != "FIM" || (in >> token)) { erro = "Arquivo incompleto ou contem dados extras."; return false; }
        if (!temp.reconstruirParaCarga(erro)) return false;
        destino.substituirPor(move(temp)); erro.clear(); return true;
    }
};

class AlgoritmosGrafo {
private:
    static constexpr double RAIO_TERRA_KM = 6371.0088;
    static double radianos(double graus) { return graus * acos(-1.0) / 180.0; }
    static string escaparDot(const string& s) {
        string r;
        for (char c : s) { if (c == '\\' || c == '"') r += '\\'; if (c == '\n' || c == '\r') r += ' '; else r += c; }
        return r;
    }
    static const Conexao* conexaoPorId(const Grafo& g, const string& id) {
        for (const auto& c : g.getConexoes()) if (c.getId() == id) return &c;
        return nullptr;
    }
    static bool escreverDot(const Grafo& g, const string& dot, const string& jpg,
                            const map<string, int>* cores, const set<string>* verticesCaminho,
                            const set<string>* conexoesCaminho, string& erro) {
        ofstream out(dot, ios::trunc);
        if (!out) { erro = "Nao foi possivel criar " + dot; return false; }
        out << (g.ehDirecionado() ? "digraph" : "graph") << " G {\n"
            << "  graph [overlap=false, splines=true, bgcolor=white, ranksep=0.7];\n"
            << "  node [shape=circle, style=filled, fontname=Arial, color=\"#355C7D\"];\n"
            << "  edge [fontname=Arial, fontsize=10];\n";
        const vector<string> paleta = {"#F94144", "#277DA1", "#90BE6D", "#F9C74F", "#9B5DE5", "#F9844A", "#43AA8B", "#577590", "#F15BB5", "#B8DE6F", "#00BBF9", "#C77DFF"};
        for (const auto& v : g.getVertices()) {
            out << "  \"" << escaparDot(v.getId()) << "\" [label=\"" << escaparDot(v.getId()) << "\n(" << v.getLatitude() << ", " << v.getLongitude() << ")\"";
            if (cores) {
                auto it = cores->find(v.getId());
                if (it != cores->end()) out << ", fillcolor=\"" << paleta[static_cast<size_t>(it->second) % paleta.size()] << "\", fontcolor=black";
            } else if (verticesCaminho && verticesCaminho->count(v.getId())) {
                out << ", fillcolor=\"#FFD166\", penwidth=3, color=\"#D00000\"";
            } else out << ", fillcolor=\"#DCEBFA\"";
            out << "];\n";
        }
        for (const auto& c : g.getConexoes()) {
            bool destaque = conexoesCaminho && conexoesCaminho->count(c.getId());
            out << "  \"" << escaparDot(c.getOrigem()) << "\" " << (g.ehDirecionado() ? "->" : "--")
                << " \"" << escaparDot(c.getDestino()) << "\" [label=\"" << escaparDot(c.getId())
                << " | peso=" << c.getPeso() << "\"";
            if (destaque) out << ", color=\"#D00000\", fontcolor=\"#D00000\", penwidth=4";
            else out << ", color=\"#777777\", penwidth=1.2";
            out << "];\n";
        }
        out << "}\n"; out.close();
        if (!out) { erro = "Falha ao escrever " + dot; return false; }
        string comando = "dot -Tjpg \"" + dot + "\" -o \"" + jpg + "\"";
        if (system(comando.c_str()) != 0 || !fs::exists(jpg)) {
            erro = "Graphviz falhou; verifique se 'dot' esta instalado e no PATH. DOT preservado: " + dot; return false;
        }
        erro.clear(); return true;
    }
public:
    static double haversineKm(const Vertice& a, const Vertice& b) {
        double lat1 = radianos(a.getLatitude()), lat2 = radianos(b.getLatitude());
        double dlat = lat2 - lat1, dlon = radianos(b.getLongitude() - a.getLongitude());
        double h = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
        h = max(0.0, min(1.0, h));
        return 2.0 * RAIO_TERRA_KM * asin(sqrt(h));
    }
    static bool executarDsatur(const Grafo& g, string& erro) {
        if (g.getVertices().empty()) { erro = "O grafo nao possui vertices para colorir."; return false; }
        map<string, set<string>> vizinhos;
        map<string, int> cores;
        for (const auto& v : g.getVertices()) vizinhos[v.getId()];
        // Um laco impede uma coloracao propria: o vertice seria adjacente a si mesmo.
        for (const auto& c : g.getConexoes()) {
            if (c.getOrigem() == c.getDestino()) {
                erro = "DSATUR nao pode produzir coloracao propria quando existe laco no vertice '" + c.getOrigem() + "'.";
                return false;
            }
        }
        // DSATUR trata a adjacencia como nao direcionada para fins de coloracao.
        for (const auto& c : g.getConexoes()) {
            if (c.getOrigem() != c.getDestino()) {
                vizinhos[c.getOrigem()].insert(c.getDestino());
                vizinhos[c.getDestino()].insert(c.getOrigem());
            }
        }
        while (cores.size() < g.getVertices().size()) {
            string escolhido;
            size_t melhorSaturacao = 0, melhorGrau = 0;
            bool primeiro = true;
            for (const auto& v : g.getVertices()) {
                const string& id = v.getId();
                if (cores.count(id)) continue;
                set<int> coresVizinhas;
                for (const auto& n : vizinhos[id]) {
                    auto it = cores.find(n); if (it != cores.end()) coresVizinhas.insert(it->second);
                }
                size_t sat = coresVizinhas.size(), grau = vizinhos[id].size();
                if (primeiro || sat > melhorSaturacao || (sat == melhorSaturacao && grau > melhorGrau)) {
                    escolhido = id; melhorSaturacao = sat; melhorGrau = grau; primeiro = false;
                }
            }
            set<int> usadas;
            for (const auto& n : vizinhos[escolhido]) if (cores.count(n)) usadas.insert(cores[n]);
            int cor = 0; while (usadas.count(cor)) ++cor;
            cores[escolhido] = cor;
        }
        int quantidade = 0; for (const auto& p : cores) quantidade = max(quantidade, p.second + 1);
        if (!escreverDot(g, "grafo_original.dot", "grafo_original.jpg", nullptr, nullptr, nullptr, erro)) return false;
        if (!escreverDot(g, "grafo_dsatur.dot", "grafo_dsatur.jpg", &cores, nullptr, nullptr, erro)) return false;
        cout << "DSATUR concluiu: " << quantidade << " cor(es) utilizada(s).\n"
             << "Arquivos: grafo_original.dot, grafo_original.jpg, grafo_dsatur.dot, grafo_dsatur.jpg\n";
        for (const auto& v : g.getVertices()) cout << "  Vertice " << v.getId() << ": cor " << (cores[v.getId()] + 1) << '\n';
        erro.clear(); return true;
    }
    static bool executarAStar(const Grafo& g, const string& inicio, const string& destino, string& erro) {
        const Vertice* vInicio = g.buscarVertice(inicio); const Vertice* vDestino = g.buscarVertice(destino);
        if (!vInicio || !vDestino) { erro = "Vertice de inicio ou destino nao existe."; return false; }
        if (inicio == destino) {
            set<string> vs{inicio}; set<string> cs;
            if (!escreverDot(g, "grafo_caminho_astar.dot", "grafo_caminho_astar.jpg", nullptr, &vs, &cs, erro)) return false;
            cout << "Inicio e destino sao o mesmo vertice. Custo: 0 km.\n"; return true;
        }
        // Para manter a heuristica admissivel, os pesos usados como custos precisam estar em km.
        for (const auto& c : g.getConexoes()) if (c.getPeso() < 0) {
            erro = "A* nao aceita pesos negativos. Use pesos nao negativos em quilometros."; return false;
        }
        struct Estado { double f; string id; };
        struct Comparar { bool operator()(const Estado& a, const Estado& b) const { return a.f > b.f; } };
        priority_queue<Estado, vector<Estado>, Comparar> abertos;
        map<string, double> gScore;
        map<string, string> anteriorVertice, anteriorConexao;
        set<string> fechados;
        for (const auto& v : g.getVertices()) gScore[v.getId()] = numeric_limits<double>::infinity();
        gScore[inicio] = 0.0;
        abertos.push({haversineKm(*vInicio, *vDestino), inicio});
        while (!abertos.empty()) {
            string atual = abertos.top().id; abertos.pop();
            if (fechados.count(atual)) continue;
            if (atual == destino) break;
            fechados.insert(atual);
            const Vertice* vert = g.buscarVertice(atual);
            for (const auto& adj : vert->getAdjacentes()) {
                const Conexao* c = conexaoPorId(g, adj.getConexaoId());
                if (!c || fechados.count(adj.getVizinhoId())) continue;
                double tentativo = gScore[atual] + c->getPeso();
                if (tentativo < gScore[adj.getVizinhoId()]) {
                    gScore[adj.getVizinhoId()] = tentativo;
                    anteriorVertice[adj.getVizinhoId()] = atual;
                    anteriorConexao[adj.getVizinhoId()] = c->getId();
                    const Vertice* viz = g.buscarVertice(adj.getVizinhoId());
                    double h = haversineKm(*viz, *vDestino); // h(n) sempre em relacao ao destino escolhido
                    abertos.push({tentativo + h, adj.getVizinhoId()});
                }
            }
        }
        if (!isfinite(gScore[destino])) { erro = "Nao existe caminho entre os vertices informados."; return false; }
        set<string> verticesCaminho{destino}, conexoesCaminho;
        vector<string> caminho{destino};
        string atual = destino;
        while (atual != inicio) {
            if (!anteriorVertice.count(atual) || !anteriorConexao.count(atual)) {
                erro = "Falha ao reconstruir o caminho encontrado."; return false;
            }
            conexoesCaminho.insert(anteriorConexao[atual]);
            atual = anteriorVertice[atual]; verticesCaminho.insert(atual); caminho.push_back(atual);
        }
        reverse(caminho.begin(), caminho.end());
        if (!escreverDot(g, "grafo_caminho_astar.dot", "grafo_caminho_astar.jpg", nullptr,
                         &verticesCaminho, &conexoesCaminho, erro)) return false;
        cout << "Caminho A*: ";
        for (size_t i = 0; i < caminho.size(); ++i) cout << (i ? " -> " : "") << caminho[i];
        cout << "\nCusto total: " << gScore[destino] << " km (conforme os pesos das conexoes).\n"
             << "Arquivo visual: grafo_caminho_astar.jpg (DOT: grafo_caminho_astar.dot)\n";
        erro.clear(); return true;
    }
    static bool gerarVisualizacaoSimples(const Grafo& g, string& erro) {
        return escreverDot(g, "grafo.dot", "grafo.jpg", nullptr, nullptr, nullptr, erro);
    }
};

static string lerLinha(const string& msg) { cout << msg; string s; getline(cin, s); return s; }
static bool lerInteiro(const string& msg, int& n) {
    cout << msg; string s; if (!getline(cin, s)) return false; istringstream in(s); char extra;
    return (in >> n) && !(in >> extra);
}
static bool lerDouble(const string& msg, double& n) {
    cout << msg; string s; if (!getline(cin, s)) return false; istringstream in(s); char extra;
    return (in >> n) && !(in >> extra) && isfinite(n);
}
static bool confirmar(const string& msg) {
    string r = lerLinha(msg + " (s/n): "); return r == "s" || r == "S" || r == "sim" || r == "SIM";
}
static bool salvarInterativo(const Grafo& g, const PersistenciaGrafo& p) {
    string nome = lerLinha("Nome do arquivo (sem caminho): "), erro;
    if (p.salvar(g, nome, erro)) { cout << "Grafo salvo na pasta " << p.getPasta().string() << ".\n"; return true; }
    cout << "Erro ao salvar: " << erro << '\n'; return false;
}
static bool carregarInterativo(Grafo& g, const PersistenciaGrafo& p) {
    string erro; auto arquivos = p.listarArquivos(erro);
    if (!erro.empty()) { cout << erro << '\n'; return false; }
    if (arquivos.empty()) { cout << "Nenhum arquivo salvo na pasta " << p.getPasta().string() << ".\n"; return false; }
    cout << "Grafos disponiveis:\n";
    for (size_t i = 0; i < arquivos.size(); ++i) cout << i + 1 << ". " << arquivos[i].stem().string() << '\n';
    int n; if (!lerInteiro("Numero do arquivo (0 cancela): ", n) || n < 0 || static_cast<size_t>(n) > arquivos.size()) { cout << "Selecao invalida.\n"; return false; }
    if (n == 0) return false;
    if (!p.carregar(arquivos[static_cast<size_t>(n - 1)], g, erro)) { cout << "Erro ao carregar: " << erro << '\n'; return false; }
    cout << "Grafo carregado. Tipo: " << (g.ehDirecionado() ? "direcionado" : "nao direcionado") << ".\n"; return true;
}
static void mostrarResumo(const Grafo& g) {
    cout << "Tipo: " << (g.ehDirecionado() ? "direcionado" : "nao direcionado") << "\nVertices:\n";
    for (const auto& v : g.getVertices()) {
        cout << "  " << v.getId() << " (lat=" << v.getLatitude() << ", lon=" << v.getLongitude() << ") -> ";
        if (v.getAdjacentes().empty()) cout << "(sem adjacencias)";
        for (size_t i = 0; i < v.getAdjacentes().size(); ++i) {
            const auto& a = v.getAdjacentes()[i]; if (i) cout << ", ";
            cout << a.getVizinhoId() << " [" << a.getConexaoId() << "]";
        }
        cout << '\n';
    }
    cout << "Conexoes:\n";
    for (const auto& c : g.getConexoes()) cout << "  " << c.getId() << ": " << c.getOrigem()
        << (c.getTipo() == Conexao::Tipo::Arco ? " -> " : " -- ") << c.getDestino()
        << " peso=" << c.getPeso() << '\n';
}
static bool resolverAlteracoesNaoSalvas(
    const Grafo& g,
    const PersistenciaGrafo& p,
    bool& alterado,
    const string& acao
) {
    if (!alterado) {
        return true;
    }

    cout << "\nAviso: existem alteracoes nao salvas no grafo atual.\n";

    if (confirmar("Deseja salvar as alteracoes antes de " + acao + "?")) {
        if (!salvarInterativo(g, p)) {
            cout << "Operacao cancelada para evitar a perda das alteracoes.\n";
            return false;
        }

        alterado = false;
        return true;
    }

    if (!confirmar("Tem certeza de que deseja DESCARTAR as alteracoes nao salvas e " + acao + "?")) {
        cout << "Operacao cancelada. O grafo atual foi mantido.\n";
        return false;
    }

    return true;
}

static void executarMenu(Grafo& g, const PersistenciaGrafo& p) {
    bool alterado = false;
    while (true) {
        cout << "\n========== MENU DO GRAFO ==========\n"
             << "1. Criar novo grafo\n2. Criar vertice\n3. Criar arco\n4. Criar aresta\n"
             << "5. Apagar vertice\n6. Apagar conexao\n7. Gerar visualizacao simples\n"
             << "8. Salvar grafo\n9. Carregar outro grafo\n10. Mostrar lista de adjacencia\n"
             << "11. Executar DSATUR (original + colorido)\n12. Executar A* e destacar caminho\n0. Sair\n";
        int op; if (!lerInteiro("Opcao: ", op)) { if (cin.eof()) return; cout << "Entrada invalida.\n"; continue; }
        string erro;
        if (op == 1) {
            if (!resolverAlteracoesNaoSalvas(g, p, alterado, "criar outro grafo")) {
                continue;
            }

            int t;
            if (!lerInteiro("1. Direcionado  2. Nao direcionado: ", t) || (t != 1 && t != 2)) {
                cout << "Tipo invalido.\n";
                continue;
            }

            g = Grafo(t == 1);
            alterado = false;
            cout << "Novo grafo criado.\n";
        } else if (op == 2) {
            string id = lerLinha("ID do vertice: "); double lat, lon;
            if (!lerDouble("Latitude (-90 a 90): ", lat) || !lerDouble("Longitude (-180 a 180): ", lon)) { cout << "Coordenadas invalidas.\n"; continue; }
            if (g.adicionarVertice(id, lat, lon, erro)) { cout << "Vertice criado.\n"; alterado = true; } else cout << "Erro: " << erro << '\n';
        } else if (op == 3 || op == 4) {
            string id = lerLinha(op == 3 ? "ID do arco: " : "ID da aresta: ");
            string o = lerLinha("ID da origem: "), d = lerLinha("ID do destino: "); double peso;
            if (!lerDouble("Peso/custo (para A*, use quilometros): ", peso)) { cout << "Peso invalido.\n"; continue; }
            auto tipo = op == 3 ? Conexao::Tipo::Arco : Conexao::Tipo::Aresta;
            if (g.adicionarConexao(id, o, d, peso, tipo, erro)) { cout << "Conexao criada.\n"; alterado = true; } else cout << "Erro: " << erro << '\n';
        } else if (op == 5) {
            string id = lerLinha("ID do vertice a remover: ");
            if (g.removerVertice(id, erro)) { cout << "Vertice e conexoes incidentes removidos.\n"; alterado = true; } else cout << erro << '\n';
        } else if (op == 6) {
            string id = lerLinha("ID da conexao a remover: ");
            if (g.removerConexao(id, erro)) { cout << "Conexao removida.\n"; alterado = true; } else cout << erro << '\n';
        } else if (op == 7) {
            if (AlgoritmosGrafo::gerarVisualizacaoSimples(g, erro)) cout << "Gerados grafo.dot e grafo.jpg.\n"; else cout << erro << '\n';
        } else if (op == 8) { if (salvarInterativo(g, p)) alterado = false;
        } else if (op == 9) {
            if (!resolverAlteracoesNaoSalvas(g, p, alterado, "carregar outro grafo")) {
                continue;
            }

            Grafo novo;
            if (carregarInterativo(novo, p)) {
                g.substituirPor(move(novo));
                alterado = false;
            }
        } else if (op == 10) mostrarResumo(g);
        else if (op == 11) {
            if (AlgoritmosGrafo::executarDsatur(g, erro)) alterado = false; else cout << "DSATUR: " << erro << '\n';
        } else if (op == 12) {
            string inicio = lerLinha("ID do vertice inicial: "), destino = lerLinha("ID do vertice destino: ");
            if (!AlgoritmosGrafo::executarAStar(g, inicio, destino, erro)) cout << "A*: " << erro << '\n';
        } else if (op == 0) {
            if (!resolverAlteracoesNaoSalvas(g, p, alterado, "sair do menu")) {
                continue;
            }
            return;
        } else cout << "Opcao inexistente.\n";
    }
}

int main() {
    PersistenciaGrafo persistencia("grafos");
    while (true) {
        cout << "\n====== GERENCIADOR DE GRAFOS ======\n1. Criar grafo direcionado\n2. Criar grafo nao direcionado\n3. Carregar grafo salvo\n0. Sair\n";
        int op; if (!lerInteiro("Opcao: ", op)) { if (cin.eof()) return 0; cout << "Entrada invalida.\n"; continue; }
        if (op == 0) return 0;
        Grafo g;
        if (op == 1) g = Grafo(true);
        else if (op == 2) g = Grafo(false);
        else if (op == 3) { if (!carregarInterativo(g, persistencia)) continue; }
        else { cout << "Opcao inexistente.\n"; continue; }
        executarMenu(g, persistencia);
    }
}
