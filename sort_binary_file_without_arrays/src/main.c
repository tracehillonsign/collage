#include "common.h"
#include "utils.h"

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

void swap_words(word_t *word_a, word_t *word_b, FILE *file)
{
    fseek(file, word_a->seek_start, SEEK_SET);
    word_b->seek_start = ftell(file);
    fwrite(word_b->data, sizeof(char), word_b->size, file);

    word_a->seek_start = ftell(file);
    fwrite(word_a->data, sizeof(char), word_a->size, file);
}

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

            if (cmp(word_a, word_b) < 0) {
                swap_words(word_a, word_b, file);
            }
        }
    }

    free(word_a->data);
    free(word_b->data);
    free(word_a);
    free(word_b);
}

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

    if (strcmp(argv[1], "test") == 0) {
        FILE *file = fopen_or_exit(FILENAME, "rb+");
        word_t *word = calloc(1, sizeof(word_t));

        uint16_t count_words;
        fread(&count_words, sizeof(count_words), 1, file);

        for (int i = count_words - 1; i >= 0; i--) {
            read_word_at((uint16_t)i, word, file);
            printf("%s | %ld | %ld\n", word->data, word->size, word->seek_start);
        }

        free(word);
        fclose(file);
    }

    return 0;
}