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
        perror("[fopen_or_exit] fopen: ");
        exit(1);
    }

    return file;
}

/* Создаем тестовый файл в котором проводится сортировка. */
void init_test_file()
{
    uint16_t words_count = 5;
    char *papaya = "Papaya";
    char *lychee = "Lychee";
    char *grapefruit = "Grapefruit";
    char *apricot = "Apricot";
    char *dragon_fruit = "Dragon fruit";

    FILE *file = fopen_or_exit(FILENAME, "wb");

    fwrite(&words_count, sizeof(words_count), 1, file);

    /* К строке добавляем еще 1 байт по скольку нам нужен нуль-терминал. */
    fwrite(papaya, strlen(papaya) + 1, 1, file);
    fwrite(lychee, strlen(lychee) + 1, 1, file);
    fwrite(grapefruit, strlen(grapefruit) + 1, 1, file);
    fwrite(apricot, strlen(apricot) + 1, 1, file);
    fwrite(dragon_fruit, strlen(dragon_fruit) + 1, 1, file);

    fclose(file);
}

/* CMP по сравнению строк. */
int cmp(const void *a, const void *b)
{
    const word_t *na = (const word_t *)a;
    const word_t *nb = (const word_t *)b;
    return strcmp(na->data, nb->data);
}

/* Поменять слова местами в файле. */
int swap(word_t *word_a, word_t *word_b, FILE *file)
{
    fseek(file, word_b->size, SEEK_SET);
    word_a->seek_start = word_b->seek_start;
    fwrite(word_b->data, strlen(word_b->data) + 1, 1, file);
    word_b->seek_start = ftell(file);
    fwrite(word_a->data, sizeof(word_a->data) + 1, 1, file);

    return 0;
}

void read_word(word_t *word, FILE *file)
{
    int ch;

    word->seek_start = ftell(file);
    while ((ch = fgetc(file)) != '\0' && ch != EOF) {
        word->data = realloc(word->data, word->size + 2);
        word->data[word->size] = (char)ch;
        word->size++;
    }
    word->data[word->size] = '\0';
}

void clear_word(word_t *word)
{
    if (word->data) {
        free(word->data);
        word->data = NULL;
    }
    word->size = 0;
    word->seek_start = 0;
}

/* Сортировка пузырьком. */
int bubble_sort(uint16_t words_count, FILE *file, int (*cmp)(const void *a, const void *b))
{
    (void)cmp;

    word_t *word_a = (word_t *)calloc(1, sizeof(word_t));
    word_t *word_b = (word_t *)calloc(1, sizeof(word_t));

    /* Ставим курсор в начало файла после счетчика слов. */
    fseek(file, sizeof(words_count), SEEK_SET);

    printf("[DEBUG : bubble_sort] WORDS_COUNT = '%d'\n", words_count);

    for (uint16_t i = 0; i < words_count - 1; i++) {
        for (uint16_t j = 0; j < words_count - i - 1; j++) {
            // read_word(word_a, file);
            // read_word(word_b, file);

            // printf("%s | %s\n", word_a->data, word_b->data);

            // clear_word(word_a);
            // clear_word(word_b);

            printf("I = '%d' | J = '%d'\n", i, j);
        }
    }

    free(word_a);
    free(word_b);

    return 0;
}

/* Точка входа в программу. */
int main(int argc, char *argv[])
{
    (void)argc;

    if (strcmp(argv[1], "init") == 0) {
        init_test_file();
    }

    if (strcmp(argv[1], "sort") == 0) {
        uint16_t words_count;
        FILE *file = fopen_or_exit(FILENAME, "rb+");

        fread(&words_count, sizeof(words_count), 1, file);

        bubble_sort(words_count, file, cmp);
    }

    return 0;
}