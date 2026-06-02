#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "file.bin"

typedef struct
{
    char *data;
    size_t size;
    size_t seek_start;
} word_t;

/* Открыть файл или закончить выполнение программы. */
FILE *fopen_or_exit(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, mode);

    if (!file) {
        perror("[fopen] ");
        exit(1);
    }

    return file;
}

/* Инициализирует тестовый файл для сортировки. */
void init_file()
{
    uint16_t words_count = 5;
    char *papaya = "Papaya";
    char *lychee = "Lychee";
    char *grapefruit = "Grapefruit";
    char *apricot = "Apricot";
    char *dragon_fruit = "Dragon fruit";

    FILE *file = fopen_or_exit(FILENAME, "wb");

    fwrite(&words_count, sizeof(words_count), 1, file);

    // К строке добавляем еще 1 байт поскольку нам нужен нуль-терминал.
    fwrite(papaya, strlen(papaya) + 1, 1, file);
    fwrite(lychee, strlen(lychee) + 1, 1, file);
    fwrite(grapefruit, strlen(grapefruit) + 1, 1, file);
    fwrite(apricot, strlen(apricot) + 1, 1, file);
    fwrite(dragon_fruit, strlen(dragon_fruit) + 1, 1, file);

    fclose(file);
}

/* Сбросить данные из структуры слова. */
void clear_word(word_t *word)
{
    if (word->data) {
        free(word->data);
        word->data = NULL;
    }

    // seek_start ставим от uint16_t что бы пропустить счетчик слов.
    word->seek_start = sizeof(uint16_t);
    word->size = 0;
}

/* Прочитать слово по индексу. */
void read_word_at(uint16_t index, word_t *word, FILE *file)
{
    (void)word;

    int ch;

    if (word->data) {
        clear_word(word);
    }

    fseek(file, sizeof(uint16_t), SEEK_SET);

    for (uint16_t i = 0; i < index; i++) {
        while ((ch = fgetc(file)) != '\0' && ch != EOF) {
            // Перебираем не нужные данные.
        }

        if (ch == EOF) {
            clear_word(word);
            return;
        }
    }

    int start_pos = ftell(file);
    if (start_pos == -1) {
        clear_word(word);
        return;
    }

    size_t capacity = 16;
    size_t len = 0;
    char *buffer = malloc(capacity);
    if (!buffer) {
        perror("[read_word_at] malloc: ");
        exit(1);
    }

    while ((ch = fgetc(file)) != EOF) {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *newbuffer = realloc(buffer, capacity);
            if (!newbuffer) {
                free(buffer);
                perror("[read_word_at] realloc: ");
                exit(2);
            }
            buffer = newbuffer;
        }

        buffer[len++] = (char)ch;
        if (ch == '\0') {
            break;
        }
    }

    word->data = buffer;
    word->size = len;
    word->seek_start = start_pos;
}

/* Меняет два слова местами. */
void swap_words(word_t *word_a, word_t *word_b, FILE *file)
{
    // 1. Временно сохраняем данные слова B (включая '\0')
    char *tmp = malloc(word_b->size);
    if (!tmp) {
        perror("swap_words malloc");
        exit(1);
    }
    memcpy(tmp, word_b->data, word_b->size);

    // 2. Записываем слово A на место B
    fseek(file, word_b->seek_start, SEEK_SET);
    fwrite(word_a->data, 1, word_a->size, file);

    // 3. Записываем сохранённое слово B на место A
    fseek(file, word_a->seek_start, SEEK_SET);
    fwrite(tmp, 1, word_b->size, file);

    free(tmp);

    // 4. Обмениваем поля seek_start (позиции слов в файле)
    size_t tmp_pos = word_a->seek_start;
    word_a->seek_start = word_b->seek_start;
    word_b->seek_start = tmp_pos;

    // // 5. Обмениваем data и size, чтобы структуры соответствовали новому расположению
    // char *tmp_data = word_a->data;
    // size_t tmp_size = word_a->size;
    // word_a->data = word_b->data;
    // word_a->size = word_b->size;
    // word_b->data = tmp_data;
    // word_b->size = tmp_size;
}

/* Потом поменять. Типо вот правила для сортировки. */
int compare(const void *a, const void *b)
{
    const word_t *na = (const word_t *)a;
    const word_t *nb = (const word_t *)b;
    return strcmp(na->data, nb->data);
}

/* Пузырьковая сортировка. */
void bubble_sort(uint16_t words_count, FILE *file, int (*cmp)(const void *a, const void *b))
{
    word_t *word_a = (word_t *)calloc(1, sizeof(word_t));
    word_t *word_b = (word_t *)calloc(1, sizeof(word_t));

    fseek(file, sizeof(uint16_t), SEEK_SET);

    for (uint16_t i = 0; i < words_count - 1; i++) {
        for (uint16_t j = 0; j < words_count - i - 1; j++) {
            read_word_at(j, word_a, file);
            read_word_at(j + 1, word_b, file);

            if (cmp(word_a, word_b) > 0) {
                swap_words(word_a, word_b, file);
            }
        }
    }

    free(word_a->data);
    free(word_b->data);
    free(word_a);
    free(word_b);
}

/* Точка входа в программу. */
int main(int argc, char *argv[])
{
    (void)argc;

    if (strcmp(argv[1], "init") == 0) {
        init_file();
    }

    if (strcmp(argv[1], "sort") == 0) {
        FILE *file = fopen_or_exit(FILENAME, "rb+");
        uint16_t words_count;
        fread(&words_count, sizeof(words_count), 1, file);

        bubble_sort(words_count, file, compare);

        fclose(file);
    }

    return 0;
}

/*
char *tmp = malloc(word_b->size);
memcpy(tmp, word_b->data, word_b->size);
// пишем word_a на место word_b
fseek(file, word_b->seek_start, SEEK_SET);
fwrite(word_a->data, 1, word_a->size, file);
// пишем tmp на место word_a
fseek(file, word_a->seek_start, SEEK_SET);
fwrite(tmp, 1, word_b->size, file);
free(tmp);
// обновляем seek_start в структурах (для корректности)
long tmp_pos = word_a->seek_start;
word_a->seek_start = word_b->seek_start;
word_b->seek_start = tmp_pos;
*/