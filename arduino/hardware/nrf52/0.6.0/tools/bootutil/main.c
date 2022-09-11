#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "serialport.h"
#include "infohelper.h"
#include "commands.h"
#ifndef _WIN32
#include "gui.h"
#endif

#define BOOTUTIL_VER_MAJOR      (0U)
#define BOOTUTIL_VER_MINOR      (0U)
#define BOOTUTIL_VER_REVISION   (1U)

volatile int g_bootutil_serport_open = 0;
volatile int g_bootutil_gui_mode = 0;

static int
bootutil_err_too_many_instances(char *arg_name)
{
    LOGERR("multiple instances of \"%s\"\n",
            arg_name);
    return EINVAL;
}

static int
bootutil_err_missing_arg(char *arg_name)
{
    LOGERR("missing argument \"%s\"\n",
            arg_name);
    return EINVAL;
}

static int
bootutil_err_missing_value(char *arg_name)
{
    LOGERR("missing value for argument \"%s\"\n",
            arg_name);
    return EINVAL;
}

static void
bootutil_help(int rc)
{
    printf("Bootloader utility for the Adafruit nRF52 board family.\n");
    printf("Version: %u.%u.%u\n", BOOTUTIL_VER_MAJOR,
                                  BOOTUTIL_VER_MINOR,
                                  BOOTUTIL_VER_REVISION);
    printf("\n");
#ifdef _WIN32
    printf("Usage: bootutil.exe -d <TTY.DEVICE> [-c <cmd>] [-ghrv]\n");
#else
    printf("Usage: bootutil -d <TTY.DEVICE> [-c <cmd>] [-hrv]\n");
#endif
    printf("\n");
    printf("  -d <TTY.DEVICE> : serial/tty port name (mandatory)\n");
    printf("  -c <cmd>        : execute the specified command\n");
    printf("     reset        : perform a system reset (toggle DTR)\n");
#ifndef _WIN32
    printf("  -g              : show GUI\n");
#endif
    printf("  -h              : show help\n");
    printf("  -r              : perform a system reset (toggle DTR)\n");
    printf("  -v              : verbose output\n");
    printf("\n");
#ifdef _WIN32
    printf("Ex: $ bootutil.exe -d COM14 -r\n");
#else
    printf("Ex: $ bootutil -d /dev/tty.SLAB_USBtoUART -r\n");
#endif

    exit(rc);
}

static int
bootutil_serport_open(char *argv)
{
    int serport;

    serport = serialport_open(argv, 115200);
    if (serport != 0) {
        g_bootutil_serport_open = 1;
    }

    return 0;
}

static int
bootutil_serport_close(void)
{
    int rc;

    rc = serialport_close();
    g_bootutil_serport_open = 0;

    return rc;
}

int
main(int argc, char **argv)
{
    int ch;
    int rc = -1;
    int ttyflag = 0;
    int rstflag = 0;
    int cmdflag = 0;
    int vrbflag = 0;
    char cmd[256];
    char tty[PATH_MAX + NAME_MAX];

    memset(cmd, '\0', sizeof(cmd));
    memset(tty, '\0', sizeof(cmd));

    /* Display help menu on zero params or '?' */
    if (argc == 1) {
        bootutil_help(0);
    }
    if ((argc == 2) && (strcmp(argv[1], "?") == 0)) {
        bootutil_help(0);
    }

    /* Parse input params */
    while ((ch = getopt(argc, argv, "d:c:ghrv")) != -1) {
        switch (ch) {
            case 'd':
                /* Make sure we have a valid command (isn't another param) */
                if (optarg[0] == '-') {
                    rc = bootutil_err_missing_value("-d");
                    goto error;
                }
                ttyflag = 1;
                strncpy(tty, optarg, sizeof(tty)-1);
                break;
            case 'c':
                /* Check if we've already received a command */
                if (cmdflag == 1) {
                    exit(bootutil_err_too_many_instances("-c"));
                }
                /* Make sure we have a valid command (isn't another param) */
                if (optarg[0] == '-') {
                    rc = bootutil_err_missing_value("-c");
                    goto error;
                }
                cmdflag = 1;
                strncpy(cmd, optarg, sizeof(cmd)-1);
                break;
#ifndef _WIN32
            case 'g':
                g_bootutil_gui_mode = 1;
                break;
#endif
            case 'r':
                rstflag = 1;
                break;
            case 'v':
                vrbflag = 1;
                break;
            case 'h':
            case '?':
                bootutil_help(0);
                break;
        }
    }

    /* Make sure we have something to do first */
    if (!rstflag && !cmdflag && !g_bootutil_gui_mode) {
        LOGERR("no action requested\n");
        goto error;
    }

    /* Set logger level to verbose if requested */
    if (vrbflag == 1) {
        infohelper_set_infolevel(2);
    }

    /* Try to open the serial port */
    if (!ttyflag) {
        rc = bootutil_err_missing_arg("-d");
        goto error;
    } else {
        bootutil_serport_open(tty);
        /* Make sure the serial port was actually opened */
        if (g_bootutil_serport_open != 1) {
            rc = EINVAL;
            goto error;
        }
    }

    /* Check if a system reset was requested (perform this first) */
    if (rstflag) {
        rc = commands_cmd_reset();
        if (rc != 0) {
            goto error;
        }
    }

    /* Command handler */
    if (cmdflag) {
        /* Try to parse the supplied command */
        rc = commands_parse(cmd);
        if (rc != 0) {
            goto error;
        }
    }

#ifndef _WIN32
    /* GUI handler */
    if (g_bootutil_gui_mode) {
        rc = gui_init(tty);
        if (rc != 0) {
            goto error;
        }
    }
#endif

    /* Display the return code of any unhandled errors */
    if (rc != 0) {
        LOGERR("rc = 0x%04X (%u): %s\n", rc, rc, strerror(rc));
    }

error:
    bootutil_serport_close();
    exit(rc);
}
