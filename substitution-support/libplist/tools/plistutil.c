/*
 * plistutil.c
 * Simple tool to convert a plist into different formats
 *
 * Copyright (c) 2009-2015 Martin Szulecki All Rights Reserved.
 * Copyright (c) 2008 Zach C. All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "plist/plist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

typedef struct _options
{
    char *in_file, *out_file;
    uint8_t debug, in_fmt, out_fmt;
} options_t;

static void print_usage(int argc, char *argv[])
{
    char *name = NULL;
    name = strrchr(argv[0], '/');
    printf("Usage: %s -i|--infile FILE [-o|--outfile FILE] [-d|--debug]\n", (name ? name + 1: argv[0]));
    printf("Convert a plist FILE from binary to XML format or vice-versa.\n\n");
    printf("  -i, --infile FILE\tThe FILE to convert from\n");
    printf("  -o, --outfile FILE\tOptional FILE to convert to or stdout if not used\n");
    printf("  -d, --debug\t\tEnable extended debug output\n");
    printf("\n");
}

static options_t *parse_arguments(int argc, char *argv[])
{
    int i = 0;

    options_t *options = (options_t *) malloc(sizeof(options_t));
    memset(options, 0, sizeof(options_t));

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--infile") || !strcmp(argv[i], "-i"))
        {
            if ((i + 1) == argc)
            {
                free(options);
                return NULL;
            }
            options->in_file = argv[i + 1];
            i++;
            continue;
        }

        if (!strcmp(argv[i], "--outfile") || !strcmp(argv[i], "-o"))
        {
            if ((i + 1) == argc)
            {
                free(options);
                return NULL;
            }
            options->out_file = argv[i + 1];
            i++;
            continue;
        }

        if (!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-d"))
        {
            options->debug = 1;
        }

        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            free(options);
            return NULL;
        }
    }

    if (!options->in_file)
    {
        free(options);
        return NULL;
    }

    return options;
}

int main(int argc, char *argv[])
{
    FILE *iplist = NULL;
    plist_t root_node = NULL;
    char *plist_out = NULL;
    uint32_t size = 0;
    int read_size = 0;
    char *plist_entire = NULL;
    struct stat filestats;
    options_t *options = parse_arguments(argc, argv);

    if (!options)
    {
        print_usage(argc, argv);
        return 0;
    }

    // read input file
    iplist = fopen(options->in_file, "rb");
    if (!iplist) {
        printf("ERROR: Could not open input file '%s': %s\n", options->in_file, strerror(errno));
        free(options);
        return 1;
    }

    memset(&filestats, '\0', sizeof(struct stat));
    fstat(fileno(iplist), &filestats);

    if (filestats.st_size < 8) {
        printf("ERROR: Input file is too small to contain valid plist data.\n");
        free(options);
        fclose(iplist);
        return -1;
    }

    plist_entire = (char *) malloc(sizeof(char) * (filestats.st_size + 1));
    read_size = fread(plist_entire, sizeof(char), filestats.st_size, iplist);
    fclose(iplist);

    // convert from binary to xml or vice-versa
    if (plist_is_binary(plist_entire, read_size))
    {
        plist_from_bin(plist_entire, read_size, &root_node);
        plist_to_xml(root_node, &plist_out, &size);
    }
    else
    {
        plist_from_xml(plist_entire, read_size, &root_node);
        plist_to_bin(root_node, &plist_out, &size);
    }
    plist_free(root_node);
    free(plist_entire);

    if (plist_out)
    {
        if (options->out_file != NULL)
        {
            FILE *oplist = fopen(options->out_file, "wb");
            if (!oplist) {
                printf("ERROR: Could not open output file '%s': %s\n", options->out_file, strerror(errno));
                free(options);
                return 1;
            }
            fwrite(plist_out, size, sizeof(char), oplist);
            fclose(oplist);
        }
        // if no output file specified, write to stdout
        else
            fwrite(plist_out, size, sizeof(char), stdout);

        free(plist_out);
    }
    else
        printf("ERROR: Failed to convert input file.\n");

    free(options);
    return 0;
}
