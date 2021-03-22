// (C)2019 Sam Bingner


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "kerneldec.h"
#include "lzssdec.h"

static struct option long_options[] = {
    {"help",        no_argument,        0, 'h'},
    {"debug",       no_argument,        0, 'd'},
    {"input",       required_argument,  0, 'i'},
    {"kpp",         required_argument,  0, 'k'},
    {"output",      required_argument,  0, 'o'},
    {"quiet",       no_argument,        0, 'q'},
    {0,             0,                  0,  0 }
};

void usage(void)
{
        printf("Usage: kerneldec [OPTIONS]\n"
                        "\t-h, --help         Print this help\n"
                        "\t-d, --debug        increment debug level\n"
                        "\t-i, --input NAME   Input from NAME instead of stdin\n"
                        "\t-o, --output NAME  Output to NAME instead of stdout\n"
                        "\t-k, --kpp NAME     Save KPP to NAME\n"
                        "\t-q, --quiet        No non-error output\n"
                        );
}

int main(int argc,char**argv)
{
    FILE *input=stdin, *output=stdout;
    char *kppfile = NULL;
    const char *infile = "stdin";
    const char *outfile = "stdout";
    bool quiet=false;

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hdi:k:o:q", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                if (quiet) {
                    fprintf(stderr, "Can't have quiet debug output\n");
                    return -1;
                }
                g_debug++;
                break;
            case 'h':
                usage();
                exit(0);
                break;
            case 'i':
		infile = optarg;
		if (input != stdin)
		    fclose(input);
		input = fopen(infile, "r");
		break;
            case 'k':
		kppfile = optarg;
		break;
	    case 'o':
		outfile = optarg;
		if (output != stdin)
		    fclose(output);
		output = fopen(outfile, "w");
		break;
            case 'q':
                if (g_debug>0) {
                    fprintf(stderr, "Can't have quiet debug output\n");
                    return -1;
                }
                quiet=true;
                break;
	    default:
		usage();
		exit(1);
		break;
	}
    }

    if (!output || !input) {
	usage();
	return 1;
    }

    FILE *kppfh = NULL;
    if (kppfile != NULL) {
    }

    int rv = decompress_kernel(input, output, kppfh, quiet);

    if (output != stdout)
	fclose(output);

    if (input != stdout)
	fclose(input);

    if (kppfh) {
        fclose(kppfh);
    }

    return rv;
}
