//
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<time.h>
#include <pthread.h>

#define S 10
#define R 9

pthread_cond_t budzenie_mikolaja;
pthread_cond_t pomoc_skrzata;
pthread_cond_t rozwozenie_prezentow;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_skrzatow = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reniferow = PTHREAD_MUTEX_INITIALIZER;
int gotowe_renifery = 0;
int gotowe_skrzaty = 0;

void *renifer(void *arg){

    while(1){


    srand(time(NULL));
    int spanie = rand()%10+1;
    usleep(spanie * 8000);
    pthread_mutex_lock(&mutex_reniferow);
    gotowe_renifery++;
    printf("renifer %d gotowy\n", gotowe_renifery);
        fflush(stdout);

        pthread_mutex_unlock(&mutex_reniferow);


        pthread_mutex_lock(&mutex);

        if(gotowe_renifery == 9){
        pthread_cond_signal(&budzenie_mikolaja);

    }
        pthread_cond_wait( &rozwozenie_prezentow, &mutex);
        pthread_mutex_lock(&mutex_reniferow);
        gotowe_renifery--;
        pthread_mutex_unlock(&mutex_reniferow);
        pthread_mutex_unlock(&mutex);

    }
};
void *skrzat(void *arg){
    while(1){



    srand(time(NULL));
    int spanie = rand()%10+1;
    usleep(spanie * 3000);

    pthread_mutex_lock(&mutex_skrzatow);
    gotowe_skrzaty++;
    printf("skrzat %d gotowy\n", gotowe_skrzaty);
    fflush(stdout);

    pthread_mutex_unlock(&mutex_skrzatow);



    pthread_mutex_lock(&mutex);


    if(gotowe_skrzaty >= 3){
        pthread_cond_signal(&budzenie_mikolaja);

    }

    pthread_cond_wait( &pomoc_skrzata, &mutex);
        pthread_mutex_lock(&mutex_skrzatow);
        gotowe_skrzaty--;
        pthread_mutex_unlock(&mutex_skrzatow);
    pthread_mutex_unlock(&mutex);
    }
};
void *mikolaj(void *arg){


    while(1) {
        pthread_mutex_lock(&mutex);

        printf("mikolaj gotowy\n");

        pthread_cond_wait(&budzenie_mikolaja, &mutex);

        pthread_mutex_lock(&mutex_skrzatow);
        pthread_mutex_lock(&mutex_reniferow);
        if (gotowe_renifery == 9) {
            printf("mikolaj: musze pomoc reniferom!\n");
            usleep(5000);
            pthread_cond_broadcast(&rozwozenie_prezentow);
        }else

        if (gotowe_skrzaty >= 3) {

            printf("mikolaj: musze pomoc %d skrzatom\n", gotowe_skrzaty);
            usleep(2000);
            pthread_cond_broadcast(&pomoc_skrzata);
        }
        fflush(stdout);
        pthread_mutex_unlock(&mutex_skrzatow);
        pthread_mutex_unlock(&mutex_reniferow);
        pthread_mutex_unlock(&mutex);

    }



};



int main() {
    pthread_t id[R + S + 1]; // Identyfikator wątku

int gotowe_renifery = 0;
int gotowe_skrzaty = 0;

        int resulttab = pthread_create(&id[0], NULL, mikolaj, &gotowe_renifery);
        if (resulttab != 0) {
            perror("Błąd przy tworzeniu reniferow");
            exit(EXIT_FAILURE);
        }



    for(int i = 1; i <= R; i++){
        int resulttab = pthread_create(&id[i], NULL, renifer, &gotowe_renifery);
        if (resulttab != 0) {
            perror("Błąd przy tworzeniu reniferow");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = R + 1; i <= R + S; i++){
        int resulttab = pthread_create(&id[i], NULL, skrzat, &gotowe_skrzaty);
        if (resulttab != 0) {
            perror("Błąd przy tworzeniu reniferow");
            exit(EXIT_FAILURE);
        }
    }



    // Oczekiwanie na zakończenie wątku

    for(int i = 0; i < S + R; i++){
        pthread_join(id[i], NULL);
    }

    // mozna tez detach



    printf("Main thread exiting.\n");
    pthread_exit(NULL);
    return 0;


    //mutex globalny lub static
    //static iniclalizuje sie tylko raz

    //w wypadku coutera zwracać lokalnego tempa, przed tym wtedy można unlockowac mutex

    // pthread_cond_t() czekanie na zmiane


    //signal trwa tylko chwile. gdy nikt nie czeka na niego, sygnal jest rpzegapioony

}
