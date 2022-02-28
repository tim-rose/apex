/*
 * PIDFILE.C --Daemon pidfile helper functions..
 *
 * Contents:
 * unlink_pidfile() --Remove the pidfile we created at startup, if any.
 * create_pidfile() --Create a file containing the current process ID.
 *
 * Remarks:
 * TODO: add singleton handling.
 *
 */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <xtd.h>
#include <systools.h>
#include <estring.h>

static char pidfile_path[NAME_MAX];

/*
 * unlink_pidfile() --Remove the pidfile we created at startup, if any.
 */
void unlink_pidfile(void)
{
    if (!STREMPTY(pidfile_path))
    {
        unlink(pidfile_path);
    }
}

/*
 * create_pidfile() --Create a file containing the current process ID.
 *
 * Parameters:
 * path --the filename to write the pid to
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine adds an "atexit" handler to remove the pidfile.
 */
int create_pidfile(const char *path)
{
    char pid[20];
    int pid_fd;
    int status;

    if (STREMPTY(path))
    {
        return 0;       /* failure: empty path!? */
    }
    strncpy(pidfile_path, path, sizeof(pidfile_path));
    pidfile_path[sizeof(pidfile_path) - 1] = '\0';

    if ((pid_fd = open(pidfile_path, (O_RDWR | O_CREAT | O_TRUNC), 0644)) < 0)
    {
        return 0;       /* failure: cannot open pidfile */
    }
    sprintf(pid, "10%d\n", getpid());  /* HBP UUCP lock file format */
    SYS_RETRY(status, write(pid_fd, pid, strlen(pid)));
    if (status < 0)
    {
        return 0;       /* failure: cannot write to pidfile */
    }

    close(pid_fd);
    atexit(unlink_pidfile);
    return 1;
}
