/*
 * CSV.C --Unit tests for CSV file I/O.
 *
 * Contents:
 * create_file()   --Create a file with some prepared content.
 * csv_copy_file() --Copy a CSV file via the csv_*() API.
 * load_file()     --Slurp a file into a text buffer.
 * cmp_file()      --compare the contents of two files.
 *
 * Remarks:
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <xtd/tap.h>
#include <xtd/csv.h>
#include <xtd.h>
#include <xtd/estring.h>

#define  CSV_PATH_MAX 4096

const char *input =
    "int,float,string\n"
    "0,0.0,\n" "123456,3.14159e+0,hello world\n" "-987654,-.0001,goodbye\n";

/*
 * create_file() --Create a file with some prepared content.
 */
static int create_file(const char *path, const char *content)
{
    FILE *fp;

    if ((fp = fopen(path, "w")) && fputs(content, fp) != 0)
    {
        fclose(fp);
        return 1;
    }
    return 0;
}

/*
 * csv_copy_file() --Copy a CSV file via the csv_*() API.
 */
static int csv_copy_file(const char *path1, const char *path2)
{
    CSVFilePtr in;
    CSVFilePtr out;
    Atom value[100];                   /* REVISIT: magic numbers */
    char bytes[4096];

    if ((in = csv_open(path1, "r")) == NULL)
    {
        return 0;
    }
    if ((out = csv_open(path2, "w", in->n_field, in->field)) == NULL)
    {
        csv_close(in);
        return 0;
    }

    while (csv_read(in, in->n_field, value, NEL(bytes), bytes))
    {
        csv_write(out, out->n_field, value);
    }
    csv_close(in);
    csv_close(out);
    return 1;
}

/*
 * load_file() --Slurp a file into a text buffer.
 *
 * Parameters:
 * path --the file to load
 * size --the maximum No. of bytes to read.
 * str  --the buffer to load the data into
 */
static int load_file(const char *path, size_t size, char *str)
{
    FILE *fp;
    int n;

    if ((fp = fopen(path, "r")) != NULL && (n = fread(str, 1, size, fp)) > 0)
    {
        str[n] = '\0';
        return 1;
    }
    return 0;                          /* error: couldn't open or read */
}

/*
 * cmp_file() --compare the contents of two files.
 */
static int cmp_file(const char *path1, const char *path2)
{
    char content1[4096];
    char content2[4096];

    load_file(path1, sizeof(content1), content1);
    load_file(path2, sizeof(content2), content2);

    return strcmp(content1, content2);
}

/*
 * main...
 */
int main(void)
{
    char path1[CSV_PATH_MAX];
    char path2[CSV_PATH_MAX];
    CSVFilePtr in, out;
    Atom value[10];
    char bytes[4096];

    sprintf(path1, "csv-1-%d.tmp", getpid());
    sprintf(path2, "csv-2-%d.tmp", getpid());

    plan_tests(7);


    ok(csv_open("bogus/path", "r") == NULL,
       "can't open non-existing file for reading");
    create_file(path1, input);

    ok((in = csv_open(path1, "r")) != NULL, "open existing file for reading");
    ok(csv_read(in, in->n_field, value, NEL(bytes), bytes), "read from file");
    ok(!csv_write(in, in->n_field, value), "can't write to readonly file");
    ok((out = csv_open(path2, "w", in->n_field, in->field)) != NULL,
       "open file for writing");
    ok(!csv_read(out, out->n_field, value, NEL(bytes), bytes),
       "can't read from writeonly file");

    /*
     * TODO:
     * open in append mode
     * append to existing file
     * csv_read/csv_scan returns index of first field that failed
     * for(types): test scanf conversion
     * for(types): test printf conversion
     * "\n" escape sequences are parsed on input, converted on output
     * fields are quoted if they contain '"'
     */

    /*
     * copy a file via the CSV API.
     */
    csv_copy_file(path1, path2);
    ok(cmp_file(path1, path2) == 0, "simple file copy");
    unlink(path1);
    unlink(path2);
    return exit_status();
}
