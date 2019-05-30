#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define MAX 100000

//CRIANDO VARIAVEIS DO TIPO SEMAPHORE
typedef struct _rwlock_t {
   sem_t escrita;
   sem_t lock;
   int leitores;
} rwlock_t;

//INICIALIZANDO O SEMAFORO
void rwlock_init(rwlock_t *lock) {
   lock->leitores = 0;
   sem_init(&lock->lock, 0,1);
   sem_init(&lock->escrita,0, 1);
}

int nescritas,counter;

//VARIAVEL DE TRAVA
rwlock_t lock;

void *leitor(void *arg) {
   int local;
   sem_wait(&lock.lock);
   lock.leitores++;
   //SE EU FOR O PRIMEIRO LEITOR, IREI TRAVAR O ESCRITOR
   if (lock.leitores == 1)
      //TRAVANDO ESCRITA
      sem_wait(&lock.escrita);
   sem_post(&lock.lock);

   local = counter; //Leitura

   sem_wait(&lock.lock);
   lock.leitores--;
   //SE EU FOR O ÚLTIMO LEITOR, IREI DESTRAVAR O ESCRITOR
   if (lock.leitores == 0)
      //DESTRAVANDO ESCRITA
      sem_post(&lock.escrita);
   sem_post(&lock.lock);
   printf("Leitor %d leu %d\n", *((int*)arg),local);
}

void *escritor(void *arg) {
   int i;
   for (i = 0; i < nescritas; i++) {
      //SEMAFORO TRAVA SEÇÃO CRÍTICA PARA OUTRAS THREADS
      sem_wait(&lock.escrita);
      counter++; //Escrita
      //SEMAFORO LIBERA SEÇÃO CRÍTICA PARA OUTRAS THREADS
      sem_post(&lock.escrita);
      printf("Escreveu %d\n",counter);
      usleep(rand()%10000);
   }
   printf("Escritor Acabou\n");
   return NULL;
}

int main(int argc, char *argv[]) {
   int leitores,i;
   if (argc != 3) {
      fprintf(stderr, "uso: rwlock leitores nescritas\n");
      exit(1);
   }
   leitores = atoi(argv[1]);
   nescritas = atoi(argv[2]);

   rwlock_init(&lock);
   pthread_t esc, lei[MAX];
   //CRIANDO ESCRITOR
   pthread_create(&esc, NULL, escritor, NULL);
   for(i=0;i<leitores;i++){
      //CRIANDO LEITORES
      pthread_create(&lei[i], NULL, leitor, &i);
//      usleep(rand()%100000);
   }
   //ESPERANDO ESCRITOR FINALIZAR
   pthread_join(esc, NULL);
   for(i=0;i<leitores;i++){
      //ESPERANDO LEITORES FINALIZAREM
      pthread_join(lei[i], NULL);
   }
   printf("Acabou!\n");
   return 0;
}
