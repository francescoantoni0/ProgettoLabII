#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "datatypes/datatypes.h"

int main(const int argc, char *argv[]) {

	const char help_prompt[] = "Usage: %s <file_grafo> <file_archi> [n_thread] [hash_size] [n_mutex]\n";

	if (argc < 3) {
		fprintf(stderr, help_prompt, argv[0]);
		return 1;
	}
	int opt, n_thread = 3, hash_size = 100000, n_mutex = 1000;
	char *filename_grafo = argv[1];
	char *filename_archi = argv[2];
	while ((opt = getopt(argc, argv, ":tHMh:")) != -1) {
		switch (opt) {
			case 't':
				n_thread = (int) strtol(optarg, NULL, 10);
				if (n_thread <= 0) {
					fprintf(stderr, "Invalid number of threads: %s\n", optarg);
					return 1;
				}
				break;
			case 'H':
				// TODO: controllo sulla correttezza della grandezza passata
				hash_size = (int) strtol(optarg, NULL, 10);
				if (hash_size <= 0) {
					fprintf(stderr, "Invalid hash size: %s\n", optarg);
					return 1;
				}
				break;
			case 'M':
				// TODO: mettere un controllo sensato
				n_mutex = (int) strtol(optarg, NULL, 10);
				if (n_mutex <= 0) {
					fprintf(stderr, "Invalid number of mutex: %s\n", optarg);
					return 1;
				}
				break;
			case 'h':
			default:
				fprintf(stdout, help_prompt, optarg);
				break;
		}
	}
}
