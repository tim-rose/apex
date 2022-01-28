/*
 * syslog() --Dummy syslog() for systems that don't have it.
 */
void openlog(void);
void syslog(void);
void closelog(void);
void setlogmask(void);

void openlog(void) { }
void syslog(void) { }
void closelog(void) { }
void setlogmask(void) {}
