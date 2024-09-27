#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class Pessoa {
public:
  std::string nome;
  std::vector<int> preferencias;
  int parceiro;

  Pessoa(std::string n, std::vector<int> prefs) : nome(n), preferencias(prefs), parceiro(-1) {}

  bool prefereOutro(int outro, const std::vector<Pessoa>& parceirosAtuais) {
    return std::find(preferencias.begin(), preferencias.end(), outro) <
      std::find(preferencias.begin(), preferencias.end(), parceiro);
  }
};

class GaleShapley {
private:
  std::vector<Pessoa> homens;
  std::vector<Pessoa> mulheres;
  std::vector<bool> homemLivre;
  int livres;

public:
  GaleShapley(std::vector<Pessoa> h, std::vector<Pessoa> m)
    : homens(h), mulheres(m), homemLivre(h.size(), true), livres(h.size()) {}

  void casar() {
    while (livres > 0) {
      int hLivre = -1;
      for (int i = 0; i < homemLivre.size(); ++i) {
        if (homemLivre[i]) {
          hLivre = i;
          break;
        }
      }

      for (int pref : homens[hLivre].preferencias) {
        if (mulheres[pref].parceiro == -1) {
          // A mulher está livre
          mulheres[pref].parceiro = hLivre;
          homens[hLivre].parceiro = pref;
          homemLivre[hLivre] = false;
          --livres;
          break;
        } else if (mulheres[pref].prefereOutro(hLivre, homens)) {
          // A mulher prefere o novo homem
          int antigoParceiro = mulheres[pref].parceiro;
          homens[antigoParceiro].parceiro = -1;
          homemLivre[antigoParceiro] = true;

          mulheres[pref].parceiro = hLivre;
          homens[hLivre].parceiro = pref;
          homemLivre[hLivre] = false;
          break;
        }
      }
    }
  }

  void imprimirCasamentos() {
    for (size_t i = 0; i < homens.size(); ++i) {
      std::cout << homens[i].nome << " esta casado com " << mulheres[homens[i].parceiro].nome << std::endl;
    }
  }
};

int main() {
  std::vector<Pessoa> homens = {
    Pessoa("H1", {0, 1, 2}),
    Pessoa("H2", {2, 0, 1}),
    Pessoa("H3", {1, 0, 2})
  };

  std::vector<Pessoa> mulheres = {
    Pessoa("M1", {0, 1, 2}),
    Pessoa("M2", {2, 0, 1}),
    Pessoa("M3", {1, 2, 0})
  };

  GaleShapley gs(homens, mulheres);
  gs.casar();
  gs.imprimirCasamentos();

  return 0;
}
