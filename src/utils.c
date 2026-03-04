#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"
#include "macros.h"
#include "ft_traceroute.h"

extern traceroute_state_t state;


// (*) errorLogger

void errorLogger(char *message, int status) {
    if (message != NULL) {
        fprintf(stderr, "%s: %s\n", state.program_name, message);
    } else {
        fprintf(stderr, "%s: (NULL)\n", state.program_name);
    }
    exit(status);
}

// (*) debugLogger

void debugLogger(char *message) {
    if (PROD) {
        return ;
    }
    if (message != NULL) {
        printf("debug: %s\n", message);
        return ;
    }
    printf("debug: (NULL)\n"); 
}

// (*) infoLogger 

void infoLogger(char *message) {
    if (message != NULL) {
        printf("%s: info: %s\n", state.program_name, message);
        return ;
    }
    printf("%s: info: (NULL)\n", state.program_name);
}
