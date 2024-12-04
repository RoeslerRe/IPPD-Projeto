#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define LIM 10000
#define NUM_THREADS 4

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
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    // Criar um array para armazenar os números primos
    int primos[LIM / 2];
    int num_primos = 0;

    // Paralelizar a busca por primos
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        // Primeira thread apenas procura números primos até a raiz
        #pragma omp single
        {
            // Busca números primos até LIM
            for(int i = 3; i <= LIM; i += 2){
                if(checar_primo(i)){
                    primos[num_primos] = i;
                    num_primos++;
                }
            }
        }

        // Segunda thread calcula os números de Mersenne
        #pragma omp single
        {
            // Para cada primo, calcula o número de Mersenne
            for(int i = 0; i < num_primos; i++){
                mpz_set_ui(p, primos[i]);
                mpz_ui_pow_ui(temp, 2, primos[i]);  // temp = 2^p
                mpz_sub_ui(temp, temp, 1);  // temp = 2^p - 1
            }
        }

        // Threads 3-16 executam o teste de Lucas-Lehmer
        #pragma omp for
        for(int i = 0; i < num_primos; i++){
            mpz_set_ui(p, primos[i]);
            mpz_ui_pow_ui(temp, 2, primos[i]);  // temp = 2^p
            mpz_sub_ui(temp, temp, 1);  // temp = 2^p - 1

            if(lucaslehmer(p)){
                printf("Primo de Mersenne encontrado para p = %d: ", primos[i]);
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
