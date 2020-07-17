# XTD -- A Library of Extensions for C

This project contains C code that implements
some variations, wrappers and additions to the C standard library,
that I've found useful enough to write, collect and document.

In some ways, it's similar in spirit but not scope to Rusty Russell's
[CCAN](http://ccodearchive.net) project, although xtd is more
a personal statement about how I write software than an attempt
to acquire and distribute as much re-usable code as possible.

## What's Inside

* manual page

### Application Startup

* controlled by command line options
* (as much as possible) every short option has a corresponding long option
* options used consistenty across a "suite" of programs (e.g. -v == verbose, etc.)
* (has an option for, and) can output simple usage/help.

### Configuration

* search for configuration file using a PATH-like environment variable, fallback to "standard" places (., ~, /usr/local/etc, /etc)
* INI syntax is OK, sections are optional?
* every long command-line option is a config variable
* default values loaded from config file, fallback to hardcoded defaults.

### Logging

* log entries are written atomically
* configurable by user at runtime via environment variables
* uses *syslog* priorities
* can filter by priority
* can filter by module/component
* can emit timestamps in a user-speicfied format
* can emit program context (file, line No.)
* can output to *stderr*, *syslog*
* *stderr* output does tty-sensitive "pretty printing"
* can be extended with a custom log handler
* integrates with *perror*
* integrates with assert macro, NDEBUG etc.

### ...and lots more.

TBD

## Requirements

* [makeshift](https://github.com/tim-rose/makeshift)

## Installation

```bash
make build test install
```

## Getting Started

TBD
