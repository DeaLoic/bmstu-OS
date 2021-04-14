char *outputNames[] = {
    "pid", //- идентификатор процесса",          // ID процесса
    "comm", //- имя исполняемого файла",     // Имя файла
    "state", // - состояние [RSDTtWXxKP]",        // Состояние процесса
    "ppid", // - ID процесса-родителя",         // ID родительского процесса
    "pgrp", // - ID группы",          // ID группы процесса
    "session", // - ID сессии",      // ID сессии процесса
    "tty_nr", // - управляющий терминал",       // упр терминал процесса
    "tpgid", // - ID группы процессов переднего плана управляющего терминала",       // ID внешней группы процессов контролирующего терминала
    "flags", // - флаги ядра",        // Флаги ядра процесса
    "minflt", //- количество минорных ошибок (не включают ошибки загрузки страниц)",       // Количество минорных ошибок процесса (Минорные ошибки не включают ошибки загрузки страниц памяти с диска)
    "cminflt", // - кол-во минорных ошибок дочернего",      // Количество минорных ошибок дочерних процессов (Минорные ошибки не включают ошибки загрузки страниц памяти с диска)
    "majflt", // - количество major ошибок процесса",       // Количество Мажоных ошибок процесса
    "cmajflt", // - кол-во major ошибок дочернего процесса",      // Количество Мажоных ошибок дочерних процессов процесса
    "utime", // - время в т.к. процесс выполнялся в режиме пользователя",        // Количество времени, в течение которого этот процесс был запланирован в пользовательском режиме
    "stime", // - время в т.к. процесс выполнялся в режиме ядра",        // Количество времени, в течение которого этот процесс был запланирован в режиме ядра
    "cutime", // - врмея ожидания завершения потомков, вып-хся в режиме пользователя - ",       // Количество времени, в течение которого ожидаемые дети этого процесса были запланированы в пользовательском режиме
    "cstime", // - врмея ожидания завершения потомков, вып-хся в режиме ядра - ",       // Количество времени, в течение которого ожидаемые дети этого процесса были запланированы в режиме ядра
    "priority", // - приоритет процесса - ",     // Приоритет процесса
    "nice",         // nice
    "num_threads", // - количество потоков - ",  // Количество потоков
    "itrealvalue",  // Время в тиках до следующего SIGALRM отправленного в процесс из-за интервального таймера.
    "starttime", // - время с начала запуска системы до запуска процесса - ",  // Время запуска процесса 
    "vsize", // - размер виртуальной памяти в байтах - ",        // Объем виртуальной памяти в байтах
    "rss", // - кол-во страниц, загруженных в физ память - ",          // Resident Set Size: Количество страниц процесса в физической памяти.
    "rsslim",       // Текущий лимит в байтах на RSS процесса
    "startcode",    // Адрес, над которым может работать текст программы
    "endcode",      // Адрес, над которым может работать текст программы
    "startstack",   // Адрес начала (т. е. дна) стека
    "kstkesp",      // Текущее значение ESP (Stack pointer), найденное на странице стека ядра для данного процесса.
    "kstkeip",      // Текущее значение EIP (instruction pointer)
    "signal",       // Растровое изображение отложенных сигналов, отображаемое в виде десятичного числа
    "blocked",      // Р заблокированных сигналов, отображаемое в виде десятичного числа
    "sigignore",    // Растровое изображение игнорированных сигналов, отображаемое в виде десятичного числа
    "sigcatch",     // Растровое изображение пойманных сигналов, отображаемое в виде десятичного числа.
    "wchan",        // Канал, в котором происходит ожидание процесса.
    "nswap",        // Количество страниц, поменявшихся местами
    "cnswap",       // Накопительный своп для дочерних процессов
    "exit_signal",  // Сигнал, который будет послан родителю, когда процесс будет завершен.
    "processor",    // Номер процессора, на котором было последнее выполнение.
    "rt_priority",  // Приоритет планирования в реальном времени- число в диапазоне от 1 до 99 для процессов, запланированных в соответствии с политикой реального времени
    "policy",       // Политика планирования
    "delayacct_blkio_tics", // Общие блочные задержки ввода/вывода
    "quest_time",   // Гостевое время процесса
    "cquest_time",  // Гостевое время  дочерних процессов
    "start_data",   // Адрес, над которым размещаются инициализированные и неинициализированные данные программы (BSS).
    "end_data",     // Адрес, под которым размещаются инициализированные и неинициализированные данные программы (BSS).
    "start_brk",    // Адрес, выше которого куча программ может быть расширена с помощью brk.
    "arg_start",    // Адрес, над которым размещаются аргументы командной строки программы (argv).
    "arg_end",      // Адрес, под которым размещаются аргументы командной строки программы (argv).
    "env_start",    // Адрес, над которым размещена программная среда
    "env_end",      // Адрес, под которым размещена программная среда
    "exit_code"     // Состояние выхода потока в форме, сообщаемой waitpid
};

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define BASE "/proc/"

#define LEN 3
#define BUFF_SIZE 0x1000
#define LEN_DIR 1
#define DIFF_LEN 1
char* files[LEN + DIFF_LEN] = {"cmdline", "environ", "statm", "maps"};
char* dirs[LEN_DIR] = {"fd"};

int writeFileToFile(char* pathSource, FILE *dest, int typeBreak);
int writeDirToFile(char* curDir, FILE *dest);
int writeLinkToFile(char* curLink, FILE *dest);
int printStatFileToFile(char* pathSource, FILE *dest);

int main(int argc, char* argv[])
{
    char* pid = argv[1];
    char* destFile = argv[2];
    char basePath[100];
    sprintf(basePath, "%s%s/", BASE, pid);
    FILE* f = fopen(destFile, "w");

    /* Тут была попытка в обобщение. Мне стыдно, правда. */
    for (int i = 0; i < LEN + DIFF_LEN; i++)
    {
        char curFile[200];
        sprintf(curFile, "%s%s", basePath, files[i]);
        printf("%s  %s\n", curFile, destFile);
        
        int a = 1;
        if (i >= 1 && i <= 2)
        {
            a = 0;
        }
        writeFileToFile(curFile, f, a);
    }

    for (int i = 0; i < LEN_DIR; i++)
    {
        char curFile[200];
        sprintf(curFile, "%s%s", basePath, dirs[i]);
        printf("%s  %s\n", curFile, destFile);
        
        writeDirToFile(curFile, f);
    }

    char path[200];
    sprintf(path, "%s%s", basePath, "stat");
    fprintf(f, "\n");
    printStatFileToFile(path, f);
    fprintf(f, "\n");

    sprintf(path, "%s%s", basePath, "cwd");
    fprintf(f, "\n");
    writeLinkToFile(path, f);
    fprintf(f, "\n");

    sprintf(path, "%s%s", basePath, "exe");
    fprintf(f, "\n");
    writeLinkToFile(path, f);
    fprintf(f, "\n");
    fclose(f);
}

int writeFileToFile(char* pathSource, FILE *dest, int typeBreak)
{
    FILE *source = fopen(pathSource, "r");

    char buf[BUFF_SIZE];
    fprintf(dest, "\n\n%s\n", pathSource);
    int size = 0;
    int n = 1;
    while ((size = fread(buf, 1, BUFF_SIZE, source)) > 0)
    {        
        for (int i = 0; i < size; i++)
        {
            if (buf[i] == 0)
            {
                buf[i] = 10;
            }
        }
        buf[size - 1] = 0;
        
        if (typeBreak == 0)
        {
            char* content = strtok(buf, " ");
            while (content != NULL)
            {
                fprintf(dest, "%d. %s\n", n, content);
                content = strtok(NULL, " ");
                n++;
            }
        }
        else
        {
            fprintf(dest, "%s", buf);
        }
    }

    fclose(source);
}

int writeDirToFile(char* curDir, FILE *dest)
{
    struct dirent *dirp;
    DIR *dp;

    dp = opendir(curDir);
    
    FILE *source = fopen(curDir, "r");

    char buf[BUFF_SIZE];
    fprintf(dest, "\n\n%s\n", curDir);
    int size = 0;
    int n = 1;
    while ((dirp = readdir(dp)) != NULL)
    {
        char path[BUFF_SIZE];
        sprintf(path, "%s/%s", curDir, dirp->d_name);
        fprintf(dest, "%d. ", n);
        writeLinkToFile(path, dest);
        fprintf(dest, "\n");
        n++;
    }

    fclose(source);
}

int writeLinkToFile(char* curLink, FILE *dest)
{
    char string[BUFF_SIZE];
    int size = readlink(curLink, string, BUFF_SIZE);
    string[size] = 0;
    fprintf(dest, "%s -> %s", curLink, string);

}

int printStatFileToFile(char* pathSource, FILE *dest)
{
    FILE *source = fopen(pathSource, "r");

    char buf[BUFF_SIZE];
    fprintf(dest, "\n\n%s\n", pathSource);
    int size = 0;
    int n = 1;
    while ((size = fread(buf, 1, BUFF_SIZE, source)) > 0)
    {        
        for (int i = 0; i < size; i++)
        {
            if (buf[i] == 0)
            {
                buf[i] = 10;
            }
        }
        buf[size - 1] = 0;
        
        char* content = strtok(buf, " ");
        while (content != NULL)
        {
            fprintf(dest, "%d. %20s    %s\n", n, content, outputNames[n - 1]);
            content = strtok(NULL, " ");
            n++;
        }
    }

    fclose(source);
}



