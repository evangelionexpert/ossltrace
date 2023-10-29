#include <stdlib.h>
#include <openssl/ssl.h>
#include <dlfcn.h>

int 
main() {
    void * meme = malloc(105);
    free(meme);

    SSL_library_init();
    // SSL_write(NULL, NULL, 0);
    dlopen(NULL, 123);
}