/*
 * WAKEUP.C --signal other process(es) with a sighup.
 *
 * Contents:
 * load_pidfile() --Load a process ID from a pid file.
 * main()         --Signal some processes.
 *
 * Remarks:
 * This program is intended to run as setuid root.  It is a privileged
 * program designed to safely do a benign action, that can be used
 * to force asynchronous/event-driven behaviour in the rest of the system
 * at all privilege levels.
 *
 */
/* LCOV_EXCL_START */
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <xtd.h>
#include <xtd/getopts.h>
#include <xtd/strparse.h>
#include <xtd/config.h>
#include <xtd/option.h>
#include <xtd/symbol.h>
#include <xtd/log.h>

static Enum signals[] = {
    {"HUP", SIGHUP},
    {"SIGHUP", SIGHUP},
    {"ALRM", SIGALRM},
    {"SIGALRM", SIGALRM},
    {"USR1", SIGUSR1},
    {"SIGUSR1", SIGUSR1},
    {"USR2", SIGUSR2},
    {"SIGUSR2", SIGUSR2}
};

static const char *sig_name;
static const char *run_dir;
static int print_version;

static char prologue[] = "wakeup [-s signal] pid...";
static Option opts[] = {
    {
     .opt = 's',.name = "signal",.value_name = "signal",
     .value = "SIGALRM",
     .doc = "send the specified signal to the specified processes",
     .proc = opt_string,.data = (void *) &sig_name},
    {
     .opt = 'r',.name = "rundir",.value_name = "dir",
     .value = "/var/run",
     .doc = "search for process PID files in the specified directory",
     .proc = opt_string,.data = (void *) &run_dir},
    {
     .opt = 'V',.name = "version",
     .doc = "print program version and exit",
     .proc = opt_bool,.data = (void *) &print_version},
    OPTION_LOG,
    {.opt = 0}
};

static char epilogue[] =
    "Allowed Signals:\n" "    SIGHUP, SIGALRM, SIGUSR1, SIGUSR2";

/*
 * load_pidfile() --Load a process ID from a pid file.
 *
 * Parameters:
 * base --the basename of the pid file
 * pid  --returns the pid that was read from the file.
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
static int load_pidfile(char *base, int *pid)
{
    char pidfile[LINE_MAX];
    FILE *fp;

    sprintf(pidfile, "%s/%s.pid", run_dir, base);
    if ((fp = fopen(pidfile, "r")) != NULL)
    {
        if (fscanf(fp, "%d", pid) != 1)
        {
            err("%s: cannot read PID", pidfile);
            fclose(fp);
            return 0;
        }
        fclose(fp);
        return 1;
    }
    return 0;
}

/*
 * main() --Signal some processes.
 */
int main(int argc, char *argv[])
{
    int pid, sig_id = -1;
    int status = EXIT_SUCCESS;

    log_init("wakeup");

    if (!config_load(argc, argv, NULL, opts))
    {
        opt_usage(prologue, opts, epilogue);
        exit(2);
    }
    if (print_version)
    {
        printf("wakeup version %s\n", xtd_version);
    }

    if (!str_enum(sig_name, NEL(signals), signals, &sig_id))
    {
        log_quit(2, "unrecognised signal \"%s\"", sig_name);
    }

    for (; optind < argc; ++optind)
    {
        pid = -1;
        if (!str_int(argv[optind], &pid) && !load_pidfile(argv[optind], &pid))
        {
            warning("unrecognised process: \"%s\"", argv[optind]);
            continue;
        }
        if (pid < 0)
        {
            warning("forbidden process ID: %d", pid);
            continue;
        }
        info("sending signal %s (%d) to process %d", sig_name, sig_id, pid);
        if (kill(pid, sig_id) != 0)
        {
            log_sys(LOG_ERR, "cannot wakeup process %d", pid);
            status = EXIT_FAILURE;
        }
    }
    exit(status);
}

/* LCOV_EXCL_STOP */
