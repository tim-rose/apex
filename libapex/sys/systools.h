/*
 * SYSTOOLS.H --Various convenient extensions/adaptions of POSIX functions.
 *
 * SPDX-FileCopyrightText: 2025 Tim Rose
 * SPDX-License-Identifier: MIT
 */
#ifndef APEX_SYSTOOLS_H
#define APEX_SYSTOOLS_H

#include <stdio.h>
#include <unistd.h>
#include <apex/timeval.h>
#ifdef __WINNT__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#endif /* __WINNT__ */

#ifdef __cplusplus
extern "C"
{
#endif                                 /* C++ */
    char *get_env_variable(const char *name, char *default_value);
    int create_pidfile(const char *path);
    void unlink_pidfile(void);
    int wait_input(fd_set * input_set, fd_set * err_set, TimeValue *tv,
                   size_t n_fd, int fd[]);
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
#endif                                 /* APEX_SYSTOOLS_H */
