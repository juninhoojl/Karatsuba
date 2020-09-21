//  Karatsuba
//  main.c
//  27/08/20
//  Disciplina: CIC110 - Análise e Projeto de Algoritmos I
//  Grupo: José Luiz Junior e Isabela Corsi

#include <stdio.h>
#include <stdlib.h>

#define CIMA(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define BAIXO(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))

//faz um shift de tamanho sh para a esquerda em um inteiro A com n digitos
// Tamanho do shift (sh), digitos (n), array de signed char (A)
// 00032
// 03200
void shift(int sh, int n, signed char *A){
    
    for(int i = 0; i < n-sh; i++){
        A[i] = A[i+sh];
    }
    for(int i = n - sh; i < n; i++){
        A[i] = 0;
    }

}

void cp_mem(void *origem, void *destino, size_t n){

   // Casting da origem e destino para tipo char
    char *sorigem = (char *)origem, *sdestino = (char *)destino;
  
   // Copia da origem para destino
    for(int i=0; i<n; i++){
        sdestino[i] = sorigem[i];
    }
    
}

// Cola uma parte do array em cada string P1 e P2
void split(int n, signed char *X, signed char *P1, signed char *P2){
    
    cp_mem(X, P1, (n/2)*sizeof(signed char));
    cp_mem(X+(n/2)*sizeof(signed char), P2, (n/2)*sizeof(signed char));
    
}


//Soma o inteiro A (representado por um vetor de single digits) com a digitos
// com o inteiro B com b digitos, e coloca o resultado em C que já chega aqui alocado
// com c posições, para funcionar c tem que ser maior que max(a, b) + 1
void sum(int a, signed char *A, int b, signed char *B, int c, signed char *C){
    signed char carry = 0;
    
    //O i aqui vai ser um contador que vai fazer a gente andar de traz para frente no vetor
    //como A, B e C tem tamanhos diferentes, precisamos corrigir os indices, por exemplo, para
    //pegar o digito menos significativo, precisamos olhar no A[a-1] e em B[b-1]
    //o resultado da soma desses digitos vai para C[c-1] podendo gerar um carry para C[c-2]
    for(int i = 1; i <= c; i++){
        //fA e fB são os digitos de A e B, se por acaso A e B acabaram, esses digitos são 0
        signed char fA = 0;
        signed char fB = 0;
        if(a >= i) fA = A[a-i];
        if(b >= i) fB = B[b-i];
        //r pode ter 2 digitos por exemplo 9 + 9 = 18
        int r = fA + fB + carry;
        //pega o valor na casa das unidades, por exemplo o 8 do 18
        C[c-i] = r % 10;
        //pega o carry, o valor que vai somar na próxima casa, por exemplo o 1 do 18
        carry = (r - C[c-i])/10;
    }
}


void sub(int a, signed char *A, int b, signed char *B, int c, signed char *C){
    signed char carry = 0;
    
    for(int i = 1; i <= c; i++){
        //fA e fB são os digitos de A e B, se por acaso A e B acabaram, esses digitos são 0
        signed char fA = 0;
        signed char fB = 0;
        if(a >= i){
            fA = A[a-i];
        }
        if(b >= i){
            fB = B[b-i];
        }
        fB = fB + carry;
        if(fB <= fA){
            C[c-i] = fA - fB;
            carry = 0;
        }else{
            C[c-i] = 10 + fA - fB;
            carry = 1;
        }
    }
}


//Multiplica um inteiro A  de n digitos por um inteiro de um digito B e salva em C
void single_multi(int n, signed char *A, signed char B, int m, signed char *C){
    int carry = 0;
    for(int i = 1; i <= n; i++){
        int r = (A[n-i] * B) + carry;
        C[m-i] = r % 10; //pega o valor na casa das unidades, por exemplo o 8 do 18
        carry = (r - C[m-i])/10; // pega por exemplo o 1 do 18
    }
    C[m-n-1] = carry;
}


//faz a multiplicação tradicional
// n = quantidade de digitos de A e B
// m eh a quantidade de digitos do resultado (no caso, 2n)
// C eh o array de resultado

// 0 - 9
void tradicional_multi(int n, signed char *A, signed char *B, int m, signed char *C){
    
    // Aloca um vetor T para usar de auxiliar para receber resultados intermediarios
    // do mesmo tamanho que o resultado final
    signed char * T = calloc(m, sizeof(signed char));
    for(int i = 1; i <= n; i++){
        
        single_multi(n, A, B[n-i], m, T);
        //Eu fiz esse shift explicito, eu suponho que seja mais eficiente
        //fazer ele implicito.
        shift(i-1,m,T);
        sum(m, T, m, C, m, C);
        for(int j = 0; j < m; j++) T[j] = 0;
    }
    free(T);
    return;
}



int zeros(signed char *V, int n){

    int i = 0;

    while(!V[i] && i < n){ // Ou acabou o vetor caso seja inteiro zero
        i++;
    }

    return i;
}



void karatsuba(int m, signed char *K, signed char *J, int z, signed char *Z){
    

    int n = (m%2 != 0) ? (m+1) : m;
    
    signed char * X = calloc(n, sizeof(signed char));
    signed char * Y = calloc(n, sizeof(signed char));
    
    cp_mem(K, X+(n-m)*sizeof(unsigned char), n-(n-m));
    cp_mem(J, Y+(n-m)*sizeof(unsigned char), n-(n-m));
    
    
    int tamX = n-zeros(X, n);
    int tamY = n-zeros(Y, n);
    
    if(tamX <= 2 && tamY <=2 ){
        
        // Somente se nao for zero
        if (tamX != 0 && tamY != 0) {
            tradicional_multi(n, X, Y, z, Z);
        }
        
        free(X);
        free(Y);
        return;
    }

    signed char * A = calloc(n/2, sizeof(signed char));
    signed char * B = calloc(n/2, sizeof(signed char));
    signed char * C = calloc(n/2, sizeof(signed char));
    signed char * D = calloc(n/2, sizeof(signed char));
        // O V1 maior vai facilitar o shift
    signed char * V1 = calloc(2*n, sizeof(signed char)); // Armazena A*C
    // Aqui nao precisa ser tao grande porque nao faz shift
    signed char * V2 = calloc(n, sizeof(signed char)); // Armazea B*D
    signed char * S1 = calloc(n, sizeof(signed char)); // Armazena A+B
    signed char * S2 = calloc(n, sizeof(signed char)); // Armazena C+D
    signed char * V3 = calloc(2*n, sizeof(signed char)); // Armazena S1*S2
    signed char * D1 = calloc(2*n, sizeof(signed char)); // Armazena V3 - V2
    signed char * V4 = calloc(2*n, sizeof(signed char)); // Armazena D1 - V1
    
    split(n, X, A, B);
    split(n, Y, C, D);
    karatsuba(n/2, A, C, 2*n, V1);
    karatsuba(n/2, B, D, n, V2);
    sum(n/2, A, n/2, B, n, S1);
    sum(n/2, C, n/2, D, n, S2);

    // Qual tem menos zeros eh o maior
    int zs1 = zeros(S1, n);
    int zs2 = zeros(S2, n);

    // Ja confere se a quantidade de zeros eh igual a n entao nao faz nada
    if(zs1 != n && zs2 != n){ // Somente vai multiplicar se nao for tudo zero
        // Multiplica
        int digitos = zs1 > zs2 ? n-zs2 : n-zs1;
        
        karatsuba(digitos, S1+(n-digitos)*sizeof(unsigned char), S2+(n-digitos)*sizeof(unsigned char), 2*n, V3);
    }

    sub(2*n, V3, n, V2, 2*n, D1);
    sub(2*n, D1, 2*n, V1, 2*n, V4);
    shift(n, 2*n, V1);
    shift(n/2, 2*n, V4);
    sum(2*n, V4, n, V2, z, Z);
    sum(z, Z, 2*n, V1, z, Z);
    
    free(X);
    free(Y);
    free(A);
    free(B);
    free(C);
    free(D);
    free(S1);
    free(S2);
    free(V3);
    free(D1);
    free(V2);
    free(V4);
    free(V1);

    return;
 
}


int main(int argc, const char * argv[]) {


    int n = 0;
    char aux;
    scanf("%d",&n);
    
    //pega quebra de linha
    getchar();
    
    signed char * X = calloc(n, sizeof(signed char));
    signed char * Y = calloc(n, sizeof(signed char));
    signed char * R = calloc(2*n, sizeof(signed char));
    
    
    for(int i = 0; i < n; i++){
        scanf("%c", &aux);
        X[i] = atoi(&aux);
    }
    
    getchar();
    
    for(int i = 0; i < n; i++){
        scanf("%c", &aux);
        Y[i] = atoi(&aux);
    }

    karatsuba(n, X, Y, 2*n, R);

    int zR = zeros(R, 2*n);
    
    for(int i = zR; i < 2*n; i++){
        printf("%d", R[i]);
    }
    printf("\n");
    
    free(R);
    free(X);
    free(Y);

    return 0;
}
