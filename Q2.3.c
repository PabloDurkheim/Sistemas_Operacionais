#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Estrutura que representa uma variável de condição
struct condvar {
    sem_t semaforo;       // Semáforo que controla o acesso à seção crítica (inicializado como 1, indicando que está "acordado")
    sem_t wait_sem;       // Semáforo usado para sincronizar as threads (inicializado como 0, indicando que está "dormindo")
    int waiting_threads;  // Número de threads atualmente esperando
};

// Inicializa a variável de condição
void condvar_init(struct condvar *c) {
    sem_init(&c->semaforo, 0, 1); // Inicializa o semáforo como 1 (acordado, permitindo o acesso à seção crítica)
    sem_init(&c->wait_sem, 0, 0); // Inicializa o semáforo de espera como 0 (dormindo, nenhuma thread está esperando inicialmente)
    c->waiting_threads = 0;       // Inicialmente, nenhum thread está esperando
}

// Bloqueia a thread atual até que seja acordada por outra thread
void condvar_wait(struct condvar *c) {
    sem_wait(&c->semaforo);      // Adquire o semáforo, bloqueando outras threads de entrarem na seção crítica (torna-se "dormindo")
    c->waiting_threads++;        // Incrementa o número de threads esperando
    sem_post(&c->semaforo);      // Libera o semáforo para outras threads

    sem_wait(&c->wait_sem);      // Aguarda até que seja acordado por outra thread (torna-se "dormindo")
}

// Acorda uma thread que está esperando
void condvar_signal(struct condvar *c) {
    if (c->waiting_threads > 0) {
        sem_post(&c->wait_sem);   // Acorda uma thread que está esperando (torna-se "acordado")
        c->waiting_threads--;     // Decrementa o número de threads esperando
    }
}

// Acorda todas as threads que estão esperando
void condvar_broadcast(struct condvar *c) {
    while (c->waiting_threads > 0) {
        sem_post(&c->wait_sem);   // Acorda todas as threads que estão esperando (tornam-se "acordadas")
        c->waiting_threads--;     // Decrementa o número de threads esperando
    }
}

// Função para uso em uma thread
void *thread_func(void *arg) {
    struct condvar *cv = (struct condvar *)arg;

    //sleep(2); // Simula algum trabalho antes de entrar na seção crítica

    printf("Thread entrou na seção crítica.\n");

    // Realiza algum trabalho na seção crítica

    printf("Thread saiu da seção crítica.\n");

    condvar_signal(cv); // Acorda uma thread que pode estar esperando

    return NULL;
}

int main() {
    pthread_t threads[3];
    struct condvar cv;
    condvar_init(&cv);
	
    // Criação de 3 threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&threads[i], NULL, thread_func, &cv);
    }
    
    // Aguarda o término de cada thread criada
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
