#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>

#define LIM 10000
struct timeval start, stop;
// Função de Lucas-Lehmer para verificar se um número de Mersenne é primo
bool lucaslehmer(mpz_t p){
    mpz_t s, temp, m;
    mpz_init(s);
    mpz_init(temp);
    mpz_init(m);

    // Calcula 2^p - 1
    mpz_ui_pow_ui(m, 2, mpz_get_ui(p));  // m = 2^p
    mpz_sub_ui(m, m, 1);  // m = 2^p - 1

    // Inicializa s com 4
    mpz_set_ui(s, 4);

    // Limite de iterações (p - 2)
    unsigned long int p_minus_2 = mpz_get_ui(p) - 2;

    for(unsigned long int i = 0; i < p_minus_2; i++){
        // Calcula s = (s^2 - 2) % m
        mpz_pow_ui(temp, s, 2);  // temp = s^2
        mpz_sub_ui(temp, temp, 2);  // temp = s^2 - 2
        mpz_mod(s, temp, m);  // s = (s^2 - 2) % (2^p - 1)
    }

    // Se s == 0, então o número é primo
    bool resultado = (mpz_cmp_ui(s, 0) == 0);

    mpz_clear(s);
    mpz_clear(temp);
    mpz_clear(m);

    return resultado;
}

// Função para verificar se um número é primo
bool checar_primo(int i){
    if (i == 2) return true; // O 2 é o único número par primo
    if (i % 2 == 0) return false;  // Elimina os pares
    for(int j = 3; j <= sqrt(i); j += 2){
        if(i % j == 0){
            return false;
        }
    }
    return true;
}

int main(){
    mpz_t p, temp;
    mpz_init(p);
    mpz_init(temp);
    gettimeofday(&start, NULL);
    // Itera sobre os números ímpares até LIM
    for(int i = 3; i <= LIM; i += 2){
        if(checar_primo(i)){
            mpz_set_ui(p, i);  // Define p como o número primo

            // Calcula 2^p - 1
            mpz_ui_pow_ui(temp, 2, i);  // temp = 2^p
            mpz_sub_ui(temp, temp, 1);  // temp = 2^p - 1

            // Imprime o número de Mersenne para depuração
            //gmp_printf("Numero de Mersenne para i = %d: ", i);
            //mpz_out_str(stdout, 10, temp);
            //printf("\n");

            // Verifica se o número de Mersenne é primo
            if(lucaslehmer(p)){
                // Alterei a mensagem aqui conforme você pediu
                printf("Primo de Mersenne encontrado para i = %d: ", i);
                mpz_out_str(stdout, 10, temp);
                printf("\n");
            }
        }
    }

    mpz_clear(p);
    mpz_clear(temp);
    gettimeofday(&stop, NULL);
    double tempo = \
    (((double)(stop.tv_sec)*1000.0 + (double)(stop.tv_usec/1000.0)) - \
    ((double)(start.tv_sec)*1000.0 + (double)(start.tv_usec/1000.0)));
    fprintf(stdout, "Time elapsed: %g ms\n", tempo);
    return 0;
}
