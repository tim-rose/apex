/*
 * TFILE.C --Unit tests for the "tfile" functions.
 *
 * Remarks:
 * TBD.
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include <apex/tap.h>
#include <apex/test.h>
#include <apex/systools.h>
#include <apex/tfile.h>

int main(void)
{
    char path[FILENAME_MAX];
    char *root = getenv("TMPDIR");
    struct stat stat_buf;
    TFILE *tfp;
    char record[] = "test-record";
    time_t t;

    plan_tests(14);
    time(&t);

    if (root == NULL)
    {
        root = (char *) ".";
    }

    sprintf(path, "%s/%s", root, "tfile.txt");
    ok_number((tfp = tfopen(path, t, NULL, NULL)), !=, NULL, "0x%p", "simple path");
    unlink(path);

    sprintf(path, "%s/%s", root, "tfile_bogus");
    touch(path);
    sprintf(path, "%s/%s", root, "tfile_bogus/tfile.txt");
    ptr_eq((tfp = tfopen(path, t, NULL, NULL)), NULL, "bogus path");
    sprintf(path, "%s/%s", root, "tfile_bogus");
    unlink(path);                      /* remove bogus file */

    sprintf(path, "%s/%s", root, "tfile-%Y-%m-%d.txt");
    ok_number((tfp = tfopen(path, t, NULL, NULL)), !=, NULL, "0x%p", "template path");

    ok(stat(tfp->path, &stat_buf) == 0
       && stat_buf.st_size == 0, "template path: %s exists", tfp->path);

    /* file output tests... */
    number_eq(tfprintf(tfp, t, "%s\n", record), sizeof record, "%z",
       "tfprintf returns n-chars");
    ok(tfwrite(record, (sizeof record) - 1, 1, tfp, t) == 1,
       "tfwrite returns n-objects");
    tfwrite("\n", 1, 1, tfp, t);       /* terminate the record */
    number_eq(stat(tfp->path, &stat_buf), 0, "%d", "can stat() tfile");
    number_eq(stat_buf.st_size, (sizeof record) * 2, "%z",
       "file output is line buffered");
    strcpy(path, tfp->path);           /* remember the path... */
    tfprintf(tfp, t + 24 * 60 * 60, "%s\n", record);
    ok(strcmp(tfp->path, path) != 0, "output file changes automatically");

    unlink(path);                      /* cleanup */
    unlink(tfp->path);
    tfclose(tfp, t);

    sprintf(path, "%s/%s", root, "tfile-%Y-%m-%d.txt");
    ok_number((tfp = tfopen(path, t, "prologue\n", "epilogue\n")),
        !=, NULL, "0x%p", "open with prologue");
    ok(stat(tfp->path, &stat_buf) == 0
       && stat_buf.st_size == 9, "prologue written to new file");
    strcpy(path, tfp->path);           /* remember the path... */
    tfclose(tfp, t);
    ok(stat(path, &stat_buf) == 0
       && stat_buf.st_size == 18, "epilogue written when closing file");

    sprintf(path, "%s/%s", root, "tfile-%Y-%m-%d.txt");
    ok_number((tfp = tfopen(path, t, "prologue\n", "epilogue\n")),
        !=, NULL, "0x%p", "re-open with prologue");
    ok(stat(tfp->path, &stat_buf) == 0
       && stat_buf.st_size == 18, "nothing written to existing file");
    unlink(tfp->path);
    tfclose(tfp, t);
    return exit_status();
}
