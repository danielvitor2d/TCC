#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

int calcularCusto(const vector<int>& casamento, const vector<vector<int>>& homens, const vector<vector<int>>& mulheres) {
  int custo = 0;
  int n = casamento.size();

  for (int h = 0; h < n; h++) {
    int mulher = casamento[h];

    auto posMulher = find(homens[h].begin(), homens[h].end(), mulher) - homens[h].begin();

    auto posHomem = find(mulheres[mulher].begin(), mulheres[mulher].end(), h) - mulheres[mulher].begin();

    custo += (posMulher + posHomem);
  }

  return custo;
}

bool isEstavel(const vector<int>& casamento, const vector<vector<int>>& homens, const vector<vector<int>>& mulheres) {
  int n = casamento.size();
  
  for (int h = 0; h < n; ++h) {
    int mulherAtual = casamento[h];
    
    // Verificar se há uma mulher que o homem prefere mais do que sua parceira atual
    for (int i = 0; i < n; ++i) {
      int mulherPreferida = homens[h][i];
      
      // Se o homem preferir essa mulher mais do que a parceira atual
      if (mulherPreferida == mulherAtual) {
        break; // Ele já está com sua parceira preferida, sair do loop
      }

      int parceiroAtualDaMulher = find(casamento.begin(), casamento.end(), mulherPreferida) - casamento.begin();
      
      // Verificar se a mulher preferida também prefere esse homem em relação ao parceiro atual
      auto posHomem = find(mulheres[mulherPreferida].begin(), mulheres[mulherPreferida].end(), h) - mulheres[mulherPreferida].begin();
      auto posParceiroAtual = find(mulheres[mulherPreferida].begin(), mulheres[mulherPreferida].end(), parceiroAtualDaMulher) - mulheres[mulherPreferida].begin();

      if (posHomem < posParceiroAtual) {
        return false; // Casamento instável
      }
    }
  }

  return true; // Se passar por todos os homens e mulheres, o casamento é estável
}

int menorCustoCasamento(vector<vector<int>>& homens, vector<vector<int>>& mulheres) {
  int n = homens.size();
  vector<int> casamento(n);

  for (int i = 0; i < n; i++) casamento[i] = i;

  int menorCusto = INT_MAX;

  do {
    if (isEstavel(casamento, homens, mulheres)) { // Verifica se é estável
      int custoAtual = calcularCusto(casamento, homens, mulheres);
      if (custoAtual == 33) {

        for (int i = 0; i < n; ++i) {
          cout << i + 1 << ' ' << casamento[i] + 1 << '\n';
        }

        cout << custoAtual << '\n';

        exit(0);
      }
      menorCusto = min(menorCusto, custoAtual);
    }
  } while (next_permutation(casamento.begin(), casamento.end()));

  return menorCusto;
}

int main() {
  int n;
  cin >> n;

  vector<vector<int>> homens(n, vector<int>(n));
  vector<vector<int>> mulheres(n, vector<int>(n));

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> homens[i][j];
      homens[i][j]--;
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      cin >> mulheres[i][j];
      mulheres[i][j]--;
    }
  }

  int resultado = menorCustoCasamento(homens, mulheres);
  cout << resultado << '\n';

  return 0;
}
