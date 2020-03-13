/*
 * SYSTOOLS.H --Various convenient extensions/adaptions of POSIX functions.
 */
#ifndef SYSTOOLS_H
#define SYSTOOLS_H

#include <stdio.h>
#include <unistd.h>
#include <timeval.h>

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    char *get_env_variable(const char *name, char *default_value);
    void create_pidfile(const char *path);
    void unlink_pidfile(void);
    int wait_input(TimeValuePtr tv, size_t n_fd, int fd[]);
    const char *path_basename(const char *path);
    const char *path_dirname(const char *path, size_t n, char dir_buf[]);
    int make_path(const char *path);
    int link_path(const char *src, const char *dst);
    const char *resolve_path(const char *paths[], const char *file);
    FILE *open_path(const char *paths[], const char *base, const char *mode);
    FILE *open_env_path(const char *env, const char *base, const char *mode);
    int touch(const char *path);
#ifdef __cplusplus
}
#endif                                 /* C++ */
#endif                                 /* SYSTOOLS_H */
