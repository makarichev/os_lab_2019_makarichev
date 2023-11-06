#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Структура для передачи аргументов в поток
struct thread_args {
    int start;
    int end;
    int k;
    int mod;
    int* result;
};

// Функция для вычисления факториала по модулю
int factorial_mod(int k, int mod) {
    int result = 1;
    for (int i = 1; i <= k; i++) {
        result = (result * i) % mod;
    }
    return result;
}

// Функция для вычисления части факториала в отдельном потоке
void* factorial_mod_thread(void* args_ptr) {
    struct thread_args* args = (struct thread_args*) args_ptr;
    int result = 1;
    for (int i = args->start; i <= args->end; i++) {
        result = (result * i) % args->mod;
    }
    *(args->result) = result;
    return NULL;
}

// Основная функция программы
int main(int argc, char* argv[]) {
    // Парсинг аргументов командной строки
    if (argc != 5) {
        printf("Usage: %s k pnum mod\n", argv[0]);
        return 1;
    }
    int k = atoi(argv[1]);
    int pnum = atoi(argv[2]);
    int mod = atoi(argv[3]);

    // Разбиваем вычисление факториала на части для каждого потока
    int chunk_size = k / pnum;
    struct thread_args args[pnum];
    for (int i = 0; i < pnum; i++) {
        args[i].start = i * chunk_size + 1;
        args[i].end = (i == pnum - 1) ? k : (i + 1) * chunk_size;
        args[i].k = k;
        args[i].mod = mod;
        args[i].result = (int*) malloc(sizeof(int));
    }

    // Создаем потоки для вычисления частей факториала
    pthread_t threads[pnum];
    for (int i = 0; i < pnum; i++) {
        pthread_create(&threads[i], NULL, factorial_mod_thread, &args[i]);
    }

    // Дожидаемся завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    // Перемножаем результаты из каждого потока, чтобы получить итоговый результат
    int result = 1;
    for (int i = 0; i < pnum; i++) {
        result = (result * *(args[i].result)) % mod;
        free(args[i].result);
    }

    // Выводим результат
    printf("%d! mod %d = %d\n", k, mod, result);

    return 0;
}