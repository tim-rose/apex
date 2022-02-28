/*
 * SYSTOOLS.C --Various extensions/adaptions of POSIX functions.
 *
 * Contents:
 * get_env_variable() --Return the value of an environment variable, or a default.
 * wait_input()       --Wait for input being available on some file descriptors
 * path_basename()    --Return the basename part of a path.
 * path_dirname()     --Return a copy of the directory part of a path.
 * make_path()        --Create the directory for a path.
 * link_path()        --Link one path to another, creating directories as needed.
 * resolve_path()     --Resolve a filename against a list of paths.
 * open_path()        --Open a file using path resolution.
 * open_env_path()    --Open a file with an environment-specified path.
 * touch()            --touch the specified file.
 *
 * Remarks:
 * "systools" is a kinda lame name for this stuff, but I guess it's no
 * worse than the usual "miscutils" library, and if you believe some,
 * there's an inevitability about it all:
 * http://video.google.com/videoplay?docid=2159021324062223592&ei=&hl=en
 *
 */
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <xtd.h>
#include <systools.h>
#include <estring.h>
#include <xtd.h>

#ifdef __WINNT__
#define MKDIR(path_, permissions_) mkdir(path_)
#else
#define MKDIR(path_, permissions_) mkdir(path_, permissions_)
#endif /* __WINNT__ */
/*
 * get_env_variable() --Return the value of an environment variable, or a default.
 *
 * Parameters:
 * name --the name of the environment variable
 * default_value --a default value, if currently not defined
 *
 * Returns: (char*)
 * The value of "name", or the caller supplied default.
 */
char *get_env_variable(const char *name, char *default_value)
{
    char *value;

    if ((value = getenv(name)) == NULL)
    {
        value = default_value;
    }
    return value;
}

/*
 * wait_input() --Wait for input being available on some file descriptors
 *
 * Parameters:
 * tv   --specifies a timeout
 * n_fd --specifies how many files to wait on
 * fd   --specifies an array of file descriptors
 *
 * Returns: (int)
 * The return value from select().
 *
 * Remarks:
 * This is a convenience function for the common case of using
 * select to wait for one or more inputs.
 */
int wait_input(fd_set * input_set, fd_set * err_set, TimeValuePtr tv,
               size_t n_fd, int fd[])
{
    int max_fd = -1;
    TimeValue timeout = *tv;           /* copy, because select() mutates */

    if (tv->tv_sec < 0)
    {                                  /* avoid EINVAL... */
        return 0;                      /* simulate timeout */
    }

    FD_ZERO(input_set);
    FD_ZERO(err_set);

    for (size_t i = 0; i < n_fd; ++i)
    {
        FD_SET(fd[i], input_set);
        FD_SET(fd[i], err_set);
        max_fd = MAX(max_fd, fd[i]);
    }
    return select(max_fd + 1, input_set, NULL, err_set, &timeout);
}

/*
 * path_basename() --Return the basename part of a path.
 *
 * Parameters:
 * path --specifies the full path
 *
 * Returns: (const char *)
 * The filename component of the path.
 */
const char *path_basename(const char *path)
{
    const char *base = strrchr(path, '/');

    return base == NULL ? path : base + 1;
}

/*
 * path_dirname() --Return a copy of the directory part of a path.
 *
 * Parameters:
 * path --specifies the full path
 * n    --specifies the size of the copy buffer
 * dir_buf --returns the copy of the directory component
 *
 * Returns: (const char *)
 * Success: dir_buf, containing a copy of the directory; Failure: NULL.
 */
const char *path_dirname(const char *path, size_t n, char dir_buf[])
{
    const char *base = path_basename(path);
    size_t dir_len = (size_t) (base - path);

    if (dir_len == 0)
    {
        if (n >= 2)
        {
            strcpy(dir_buf, ".");
            return dir_buf;            /* success: synthesised pwd */
        }
    }
    else
    {
        dir_len -= 1;                  /* skip trailing "/" */
        if (dir_len < n)
        {
            strncpy(dir_buf, path, dir_len);
            dir_buf[dir_len] = '\0';
            return dir_buf;            /* success: copy of dir component */
        }
    }
    return NULL;                       /* failure: buffer too small */
}

/*
 * make_path() --Create the directory for a path.
 *
 * Parameters:
 * path --the path containing possibly non-existing components.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine creates a full directory path, ala "mkdir -p".  It recurses
 * from the leaf to the root of the directory tree.
 */
int make_path(const char *path)
{
    struct stat path_info;
    char parent[FILENAME_MAX + 1];
    char *end;

    if (stat(path, &path_info) >= 0)
    {
        if ((path_info.st_mode & S_IFDIR))
        {
            return 1;                  /* success: already exists */
        }
        return 0;                      /* failure: exists, but not a dir */
    }

    if ((end = strrchr(path, '/')) != NULL)
    {
        size_t n = (size_t) (end - path);

        if (n >= NEL(parent))
        {
            errno = EINVAL;
            return 0;                  /* failure: path too big (for now!) */
        }
        strncpy(parent, path, n);
        parent[n] = '\0';

        if (!make_path(parent))
        {
            return 0;                  /* error: can't make parent */
        }
    }
    if (MKDIR(path, (mode_t) 0755) < 0)
    {
        return 0;                      /* failure: mkdir sets errno! */
    }
    return 1;                          /* success: mkdir succeeded */
}

/*
 * link_path() --Link one path to another, creating directories as needed.
 *
 * Parameters:
 * src --the path to link from
 * dst --the path to link to
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * TODO: handle arbitrary length paths...
 */
int link_path(const char *src, const char *dst)
{
    char path[FILENAME_MAX + 1];
    char *end = strrchr(dst, '/');

    if (end != NULL)
    {
        size_t n = (size_t) (end - dst);

        if (n >= NEL(path))
        {
            errno = EINVAL;
            return 0;                  /* failure: path too big (for now!) */
        }
        strncpy(path, dst, n);
        path[n] = '\0';
        if (!make_path(path))
        {
            return 0;                  /* failure: cannot make path */
        }
    }
    return link(src, dst) == 0;        /* success? --depends on link(2) */
}

/*
 * resolve_path() --Resolve a filename against a list of paths.
 *
 * Parameters:
 * paths    --specifies the paths as a NULL terminated list
 * base     --the basename
 *
 * Returns: (char *)
 * Success: the path element that matched; Failure: NULL.
 *
 * Remarks:
 * This routine will return successfully for the first path for which
 * stat returns OK.
 */
const char *resolve_path(const char *path[], const char *base)
{
    for (int i = 0; path[i] != NULL; ++i)
    {
        char file[FILENAME_MAX + 1];
        struct stat status;

        sprintf(file, "%s/%s", path[i], base);
        if (stat(file, &status) == 0)
        {
            return path[i];
        }
    }
    return NULL;
}

/*
 * open_path() --Open a file using path resolution.
 *
 * Parameters:
 * path    --specifies the path as a NULL terminated list
 * base     --the basename part of the file
 * mode     --the file mode
 *
 * Returns: (FILE *)
 * Success: the file pointer; Failure: NULL.
 *
 * Remarks:
 * This routine will return successfully for the first path for which
 * fopen succeeds.
 */
FILE *open_path(const char *path[], const char *base, const char *mode)
{
    for (int i = 0; path[i] != NULL; ++i)
    {
        char file[FILENAME_MAX + 1];
        FILE *fp;

        sprintf(file, "%s/%s", path[i], base);
        if ((fp = fopen(file, mode)) != NULL)
        {
            return fp;
        }
    }
    return NULL;
}

/*
 * open_env_path() --Open a file with an environment-specified path.
 *
 * Parameters:
 * env      --specifies the environment variable containing the path
 * base     --the basename part of the file
 * mode     --the file mode
 *
 * Returns: (FILE *)
 * Success: the file pointer; Failure: NULL.
 */
FILE *open_env_path(const char *env, const char *base, const char *mode)
{
    if (env == NULL || (env = getenv(env)) == NULL)
    {
        return NULL;                   /* error: no environment */
    }

    char **path = new_str_list(env, ':');
    FILE *fp = open_path((const char **) path, base, mode);

    free_str_list(path);
    return fp;
}

/*
 * touch() --touch the specified file.
 *
 * Parameters:
 * path --the pathname of the file to "touch"
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int touch(const char *path)
{
    FILE *fp;
    struct timeval times[2] = { {.tv_sec = 0} };
    time_t now;

    if ((fp = fopen(path, "a")) == NULL)
    {
        return 0;       /* failure: cannot open file */
    }

    fclose(fp);
    time(&now);
    times[0].tv_sec = times[1].tv_sec = now;
    utimes(path, times);
    return 1;
}
