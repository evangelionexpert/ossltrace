#define _GNU_SOURCE

#include <stdlib.h>
#include <openssl/ssl.h>
#include <dlfcn.h>

// typedef int (*SSL_write_callback)(SSL *ssl, const void *buf, int num);
// SSL_write_callback __real_SSL_write = NULL;

// int 
// DLSYMMED_SSL_write(SSL *ssl, const void *buf, int num) {
//     if (__real_SSL_write == NULL) {
//         __real_SSL_write = dlsym(RTLD_NEXT, "SSL_write");
//     }

//     // write(2, buf, num);
//     // fprintf(stderr, "bufsize %d\n\n", num);
    
//     // const uint8_t * charbuf = (const uint8_t*) buf;
//     // for (int i = 0; i < num; i++) {
//     //     fprintf(stderr, "%2.2x %c\n", ((const uint8_t*) buf)[i], ((const uint8_t*) buf)[i]);
//     // }
//     // fprintf(stderr, "\ndone now\n\n\n");
    
//     return __real_SSL_write(ssl, buf, num);
// }

int 
main() {
    void * meme = malloc(105);
    free(meme);

    SSL_library_init();
    // SSL_write(NULL, NULL, 0);

    SSL_write(NULL, NULL, 0);
    // DLSYMMED_SSL_write(NULL, NULL, 0);
}