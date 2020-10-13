/*
 * CONFIG.C --Program configuration processing facilities.
 *
 * Contents:
 * get_config_path() --Return (and possibly initialise) the config search path.
 * opt_find_name_()  --Find an opts definition by its name.
 * opt_ini_()        --IniProc callback to configure values from ini parsing.
 * config_load()     --Load the program configuration.
 * config_load_ini() --Apply configuration stored in a ".ini" config file
 */

#include <xtd.h>
#include <systools.h>
#include <estring.h>
#include <getopts.h>
#include <date.h>
#include <ini.h>
#include <log.h>
#include <config.h>
#include <option.h>

#ifndef CONFIG_PATH                    /* fallback config path */
#define CONFIG_PATH "/usr/local/etc", "/etc"
#endif /* CONFIG_PATH */
#ifndef CONFIG_PATH_VAR
#define CONFIG_PATH_VAR "CONFIGPATH"
#endif /* CONFIG_PATH_VAR */

static const char *default_config_path[] = {
    ".", NULL /* ($HOME) */ , CONFIG_PATH, NULL
};

static const char **config_path;

/*
 * get_config_path() --Return (and possibly initialise) the config search path.
 */
static const char **get_config_path(void)
{
    if (config_path == NULL)
    {
        char *path = getenv(CONFIG_PATH_VAR);

        if (path != NULL)
        {                              /* get path from environment */
            config_path = (const char **) new_str_list(path, ':');
        }
        else
        {                              /* add home to default path */
            default_config_path[1] = getenv("HOME");
            config_path = default_config_path;
        }
    }
    return config_path;
}

/*
 * opt_find_name_() --Find an opts definition by its name.
 *
 * Parameters:
 * opts   --specifies the list of opts definitions
 * name     --specifies the name
 *
 * Returns: (OptionPtr)
 * Success: the opts definition; Failure: NULL.
 */
static OptionPtr opt_find_name_(Option opts[], const char *name)
{
    for (OptionPtr opt = opts; opt != NULL && opt->name != NULL; ++opt)
    {
        if (strcmp(name, opt->name) == 0)
        {
            return opt;                /* success */
        }
    }
    return NULL;                       /* failure */
}

/*
 * opt_ini_() --IniProc callback to configure values from ini parsing.
 *
 * Parameters:
 * ini      --the ini parser context
 * section  --the current section, or NULL
 * name     --the name of the "variable" just parsed
 * value    --the value just parsed
 * data     --caller context  data: (the opts list)
 *
 * Returns: (int)
 * keep-parsing: 1; abort-parsing: 0.
 *
 * Remarks:
 * This routine prefixes the parsed name with the section name (if any)
 * e.g.
 * [foo]
 * bar=1
 * ==> will be matched as "foo_bar=1".
 */
static int opt_ini_(IniPtr ini, const char *section,
                    const char *name, const char *value, void *data)
{
    OptionPtr opts = (OptionPtr) data;
    OptionPtr opt;
    char full_name[LINE_MAX];

    if (section != NULL)
    {                                  /* add section name as a prefix */
        vstrcat(full_name, section, "_", name, (char *) NULL);
        name = full_name;
    }
    else
    {
        strcpy(full_name, name);
    }
    estrsub(full_name, '_', '-', 1);

    if ((opt = opt_find_name_(opts, full_name)) == NULL)
    {
        info("unknown configuration value \"%s\"", full_name);
        return 1;                      /* unknown name, that's OK */
    }
    if (!opt->set)
    {
        if (opt->proc != NULL && !opt->proc(full_name, value, opt->data))
        {
            ini_err(ini, "failed to process %s config value \"%s\"",
                    full_name, value);
            return 0;                  /* failure: bad name or value */
        }
        opt->set = 1;
    }
    return 1;                          /* success */
}

/*
 * config_load() --Load the program configuration.
 *
 * Parameters:
 * argc     --count of the number of arguments to process
 * argv     --the arguments to process (e.g. from main(...))
 * file     --the configuration file to load (or NULL)
 * opts   --specifies the list of opts definitions
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 */
int config_load(int argc, char *argv[], const char *config_file,
                Option opts[])
{
    for (int i = 0; opts[i].name != NULL; ++i)
    {                                  /* reset each parameter's "set" flag */
        opts[i].set = 0;
    }
    if (!opt_getopts_long(argc, argv, opts))
    {
        err("failed to process command line arguments");
        return 0;
    }

    if (config_file == NULL)
    {                                  /* fake up file from argv[0] */
        (void) config_load_ini(path_basename(argv[0]), NULL, opts);
    }
    else if (!config_load_ini(config_file, NULL, opts))
    {
        log_sys(LOG_ERR, "failed to load configuration file \"%s\"",
                config_file);
        return 0;                      /* error: config file */
    }

    if (!opt_defaults(opts))
    {
        err("failed to configure defaults");
        return 0;
    }
    return 1;
}

/*
 * config_load_ini() --Apply configuration stored in a ".ini" config file
 *
 * Parameters:
 * file     --the configuration file to load
 * section  --the configuration file section to load ???
 * opts   --specifies the list of opts definitions
 *
 * Returns: (int)
 * Success: 1; Failure: 0.
 *
 * Remarks:
 * This routine searches for the filename based on a search path
 * that is currently hardcoded.  If an absolute path is specified,
 * it is used unaltered.
 */
int config_load_ini(const char *file, const char *UNUSED(section),
                    Option opts[])
{
    char config_file[FILENAME_MAX];
    char config_path[FILENAME_MAX];
    IniPtr ini = NULL;
    int status = 1;                     /* OK so far... */

    vstrcat(config_file, file, ".conf", (char *) NULL);

    if (*file != '.' && *file != '/')
    {                                  /*  relative path: simply open it */
        const char *dir;

        if ((dir = resolve_path(get_config_path(), config_file)) == NULL)
        {
            debug("cannot find configuration \"%s\"", config_file);
            return 0;                  /* error: can't find file? */
        }
        vstrcat(config_path, dir, "/", config_file, (char *) NULL);
    }
    debug("loading configuration \"%s\"", config_path);
    if ((ini = ini_fopen(config_path)) != NULL)
    {
        int status = ini_parse(ini, opt_ini_, opts);

        ini_close(ini);
    }
    else
    {
        debug("cannot load configuration \"%s\"", config_file);
        status = 0;
    }
    return status;
}
