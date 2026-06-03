#include "utils.h"
#include "common.h"

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