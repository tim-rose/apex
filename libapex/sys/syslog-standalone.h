/**
 * @brief Syslog definitions for standalone (non-syslog) systems.
 *
 * This file contains declarations for the constants etc. used by the
 * syslog(3) system common to Unix (POSIX?) systems.  It is provided
 * for reference, and possible use in constructing a shim.
 */
#ifndef APEX_SYSLOG_STANDALONE_H
#define APEX_SYSLOG_STANDALONE_H
#include <stdio.h>
/**
 * @brief Syslog priorities.
 */
typedef enum
{
    LOG_EMERG = 0,                     /* system is unusable */
    LOG_ALERT = 1,                     /* action must be taken immediately */
    LOG_CRIT = 2,                      /* critical conditions */
    LOG_ERR = 3,                       /* error conditions */
    LOG_WARNING = 4,                   /* warning conditions */
    LOG_NOTICE = 5,                    /* normal but significant condition */
    LOG_INFO = 6,                      /* informational */
    LOG_DEBUG = 7                      /* debug-level messages */
} SyslogPriority;

/**
 * @brief Syslog facilities.
 */
typedef enum
{
    LOG_KERN = (0 << 3),               /* kernel messages */
    LOG_USER = (1 << 3),               /* random user-level messages */
    LOG_MAIL = (2 << 3),               /* mail system */
    LOG_DAEMON = (3 << 3),             /* system daemons */
    LOG_AUTH = (4 << 3),               /* authorization messages */
    LOG_SYSLOG = (5 << 3),             /* messages generated internally by syslogd */
    LOG_LPR = (6 << 3),                /* line printer subsystem */
    LOG_NEWS = (7 << 3),               /* network news subsystem */
    LOG_UUCP = (8 << 3),               /* UUCP subsystem */
    LOG_CRON = (9 << 3),               /* clock daemon */
    LOG_AUTHPRIV = (10 << 3),          /* authorization messages (private) */
    LOG_FTP = (11 << 3),               /* ftp daemon */
    LOG_NETINFO = (12 << 3),           /* NetInfo */
    LOG_REMOTEAUTH = (13 << 3),        /* remote authentication/authorization */
    LOG_INSTALL = (14 << 3),           /* installer subsystem */
    LOG_RAS = (15 << 3),               /* Remote Access Service (VPN/PPP) */
} SyslogFacility;

/*
 * other codes, reserved for system use
 */
#define	LOG_LOCAL0	(16<<3)
#define	LOG_LOCAL1	(17<<3)
#define	LOG_LOCAL2	(18<<3)
#define	LOG_LOCAL3	(19<<3)
#define	LOG_LOCAL4	(20<<3)
#define	LOG_LOCAL5	(21<<3)
#define	LOG_LOCAL6	(22<<3)
#define	LOG_LOCAL7	(23<<3)

#define	LOG_NFACILITIES	24             /* current number of facilities */
#define	LOG_FACMASK	0x03f8             /* mask to extract facility part */

/* facility of pri */
#define	LOG_FAC(_priority)	(((_priority) & LOG_FACMASK) >> 3)

/*
 * Macros for constructing setlogmask()'s mask parameter.
 */
#define	LOG_MASK(_priority) (1 << (_priority))
#define	LOG_UPTO(_priority) ((1 << ((_priority)+1)) - 1)

/*
 * Option flags for openlog().
 */
#define	LOG_PID		0x01               /* log the pid with each message */
#define	LOG_CONS	0x02               /* log on the console if errors in sending */
#define	LOG_ODELAY	0x04               /* delay open until first syslog() (default) */
#define	LOG_NDELAY	0x08               /* don't delay open */
#define	LOG_NOWAIT	0x10               /* don't wait for console forks: DEPRECATED */
#define	LOG_PERROR	0x20               /* log to stderr as well */

void openlog(const char *identity, int options, int facility);
void closelog(void);
void syslog(int level, const char *text, ...);
int setlogmask(int mask);

/*
 * SYSLOG_NAMES: define textual names for Syslog's enums.
 */
#ifdef SYSLOG_NAMES
typedef struct _code
{
    const char *c_name;
    int c_val;
} CODE;

CODE prioritynames[] = {
    {"emerg", LOG_EMERG,},
    {"alert", LOG_ALERT,},
    {"crit", LOG_CRIT,},
    {"err", LOG_ERR,},
    {"warning", LOG_WARNING,},
    {"notice", LOG_NOTICE,},
    {"info", LOG_INFO,},
    {"debug", LOG_DEBUG,},
    {NULL, -1,}
};

CODE facilitynames[] = {
    {"auth", LOG_AUTH,},
    {"authpriv", LOG_AUTHPRIV,},
    {"cron", LOG_CRON,},
    {"daemon", LOG_DAEMON,},
    {"ftp", LOG_FTP,},
    {"install", LOG_INSTALL},
    {"kern", LOG_KERN,},
    {"lpr", LOG_LPR,},
    {"mail", LOG_MAIL,},
    {"netinfo", LOG_NETINFO,},
    {"ras", LOG_RAS},
    {"remoteauth", LOG_REMOTEAUTH},
    {"news", LOG_NEWS,},
    {"syslog", LOG_SYSLOG,},
    {"user", LOG_USER,},
    {"uucp", LOG_UUCP,},
    {"local0", LOG_LOCAL0,},
    {"local1", LOG_LOCAL1,},
    {"local2", LOG_LOCAL2,},
    {"local3", LOG_LOCAL3,},
    {"local4", LOG_LOCAL4,},
    {"local5", LOG_LOCAL5,},
    {"local6", LOG_LOCAL6,},
    {"local7", LOG_LOCAL7,},
    {NULL, -1,}
};
#endif /* SYSLOG_NAMES */
#endif /* APEX_SYSLOG_STANDALONE_H */
