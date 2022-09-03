#include <csv.h>

void create_csv()
{
    FILE *file_csv;

    file_csv = fopen("log.csv", "w");
    fprintf(file_csv, "Data/Hora, TI, TE, TR, PID\n");
    fclose(file_csv);
}


void add_in_csv(float ti, float te, float tr, float pid)
{
    FILE *file_csv;
    time_t rawtime;
    struct tm *timeinfo;
    char date_time[81];

    file_csv = fopen("log.csv", "a+");

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(date_time, 81, "%F %X", timeinfo);

    fprintf(file_csv, "%s, %.4f, %.4f, %.4f, %.4f\n", date_time, ti, te, tr, pid);

    fclose(file_csv);
}
