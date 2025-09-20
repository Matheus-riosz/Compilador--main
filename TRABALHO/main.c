#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Definição da estrutura do token
typedef struct {
    int id;         // Código do token
    char lexema[50];// Texto do token
    int linha;      // Linha
    int coluna;     // Coluna
} Token;

// Função auxiliar para identificar se é palavra reservada
int eh_reservada(const char *s){
    const char *reservadas[] = {"program","var","begin","end","integer","real"};
    for(int i=0; i<6; i++){
        if(strcmp(s, reservadas[i])==0) return 1;
    }
    return 0;
}

// Função para analisar um caractere e gerar tokens
void analisar_arquivo(const char *nome_arquivo){
    FILE *entrada = fopen(nome_arquivo, "r");
    if(!entrada){
        printf("Arquivo %s não encontrado!\n", nome_arquivo);
        return;
    }

    // Criar pasta Saida se não existir
    system("mkdir Saida >nul 2>nul");

    FILE *saida = fopen("Saida/teste.lex", "w");
    if(!saida){
        printf("Não foi possível criar o arquivo de saída.\n");
        fclose(entrada);
        return;
    }

    fprintf(saida, "===== TABELA DE TOKENS =====\n");

    char c;
    int linha = 1, coluna = 0;
    char buffer[50];
    int buf_index = 0;

    while((c=fgetc(entrada))!=EOF){
        coluna++;

        if(c=='\n'){
            linha++;
            coluna=0;
        }

        if(isalpha(c)){ // Início de identificador ou palavra reservada
            buffer[0]=c;
            buf_index=1;
            int col_inicio = coluna;

            while(isalnum(c=fgetc(entrada)) || c=='_'){
                buffer[buf_index++] = c;
                coluna++;
            }
            buffer[buf_index]='\0';
            fseek(entrada, -1, SEEK_CUR); // Voltar 1 caractere

            int id_token = eh_reservada(buffer) ? 1 : 7; // 1 = reservada, 7 = identificador
            fprintf(saida, "(%d, %s) linha %d, col %d\n", id_token, buffer, linha, col_inicio);
        }
        else if(isdigit(c)){ // Número
            buffer[0]=c;
            buf_index=1;
            int col_inicio = coluna;

            while(isdigit(c=fgetc(entrada)) || c=='.'){
                buffer[buf_index++] = c;
                coluna++;
            }
            buffer[buf_index]='\0';
            fseek(entrada, -1, SEEK_CUR);

            fprintf(saida, "(8, %s) linha %d, col %d\n", buffer, linha, col_inicio); // 8 = número
        }
        else if(isspace(c)){
            continue; // Ignorar espaços
        }
        else{ // Operadores ou símbolos
            int id_token = 15; // Ponto e vírgula padrão
            if(c==':') {
                if((c=fgetc(entrada))=='='){ // := operador
                    fprintf(saida, "(10, :=) linha %d, col %d\n", linha, coluna);
                } else {
                    fseek(entrada, -1, SEEK_CUR);
                    fprintf(saida, "(17, :) linha %d, col %d\n", linha, coluna);
                }
            }
            else if(c==',') fprintf(saida, "(16, ,) linha %d, col %d\n", linha, coluna);
            else if(c==';') fprintf(saida, "(15, ;) linha %d, col %d\n", linha, coluna);
            else if(c=='.') fprintf(saida, "(15, .) linha %d, col %d\n", linha, coluna);
            else fprintf(saida, "(22, %c) linha %d, col %d\n", c, linha, coluna); // Outros símbolos
        }
    }

    fclose(entrada);
    fclose(saida);
    printf("Arquivo teste.lex gerado em Saida/\n");
}

int main(){
    analisar_arquivo("teste.mat");
    return 0;
}
