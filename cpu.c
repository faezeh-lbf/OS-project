#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#define PROCSTATFILE "/proc/stat"

void eprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

double cpuusage(void) {
    char buf[BUFSIZ];
    static unsigned long long lastuser, lastnice, lastsystem, lastidle;
    unsigned long long user, nice, system, idle;
    unsigned long long total;
    double percent;
    FILE *fp;

    if (lastuser && lastnice && lastsystem && lastidle) {
        fp = fopen(PROCSTATFILE, "r");
        if (!fp)
            eprintf("failed to open %s\n", PROCSTATFILE);
        fgets(buf, BUFSIZ, fp);
        sscanf(buf, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
        fclose(fp);

        percent = 0;
        total = 0;
        total += (user - lastuser);
        total += (nice - lastnice);
        total += (system - lastsystem);
        percent = total;
        total += (idle - lastidle);
        percent /= total;
        percent *= 100;
    }

    fp = fopen(PROCSTATFILE, "r");
    if (!fp)
        eprintf("failed to open %s\n", PROCSTATFILE);
    fgets(buf, BUFSIZ, fp);
    sscanf(buf, "cpu %llu %llu %llu %llu", &lastuser, &lastnice, &lastsystem, &lastidle);
    fclose(fp);

    return percent;
}

int main(void) {
    while (1) {
        printf("cpu usage:%f\n", cpuusage());
        sleep(1);
    }

    return EXIT_SUCCESS;
}
