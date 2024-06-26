#pragma once 

#define _GNU_SOURCE

#include "firewall.h"
#include "logger.h"
#include "util/vector.h"
#include "util/templates/gen_singleton.h"

#include <openssl/ssl.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <assert.h>
#include <gnu/libc-version.h>
#include <gnu/lib-names.h>
#include <fcntl.h>


ORIG_FUNC_PTR_STORAGE(SSL_write);

int 
hooked_SSL_write(SSL *ssl, const void *buf, int num) {
    typedef int 
    (*SSL_write_callback) (SSL *, const void *, int);

    OSSLTRACE_LOG("\n*** SSL_write intercepted\nintermediate buffer size is %d, contents: \n\n", num)
    OSSLTRACE_LOG_BUF(buf, num)
    OSSLTRACE_LOG("\n")

    int retval;
    char *occurence = find_denylisted_strings_occurence(buf, num);
    if (occurence != NULL) {
        OSSLTRACE_LOG("\n!!! FOUND %s, PACKET REFUSED !!!\n", occurence);
        retval = -1;
    } else {
        retval = ((SSL_write_callback) GET_ORIG_FUNC_PTR(SSL_write))(ssl, buf, num);
    }

    OSSLTRACE_LOG("return value is %d\n***\n\n", retval)
    return retval;
}



ORIG_FUNC_PTR_STORAGE(SSL_read);

int 
hooked_SSL_read(SSL *ssl, void *buf, int num) {
    typedef int 
    (*SSL_read_callback) (SSL *, void *, int);

    OSSLTRACE_LOG("\n*** SSL_read intercepted\n")

    int retval = ((SSL_read_callback) GET_ORIG_FUNC_PTR(SSL_read))(ssl, buf, num);
    if (retval == -1) {
        OSSLTRACE_LOG("retval is -1, no buffer, sorry")
    } else {
        OSSLTRACE_LOG("intermediate buffer size is %d (num was %d) contents: \n\n", retval, num)
        OSSLTRACE_LOG_BUF(buf, retval)
        OSSLTRACE_LOG("\n")

        char *occurence = find_denylisted_strings_occurence(buf, retval);
        if (occurence != NULL) {
            OSSLTRACE_LOG("!!! FOUND %s, PACKET REFUSED !!!\n", occurence)
            retval = -1;
        }
    }

    OSSLTRACE_LOG("\n***\n")

    return retval;
}



ORIG_FUNC_PTR_STORAGE(SSL_get_verify_result);

static volatile int is_ca_ignored = -1;

static void
init_ca_ignore_mode() {
    char *ca_env = getenv(OSSLTRACE_IGNORE_CA_ENV_VAR);
    is_ca_ignored = ca_env != NULL ? 1 : 0;
}

long 
hooked_SSL_get_verify_result(const SSL *ssl) {
    typedef long 
    (*SSL_get_verify_result_callback) (const SSL *);

    if (is_ca_ignored == -1) {
        init_ca_ignore_mode();
    }

    OSSLTRACE_LOG("\n*** SSL_get_verify_result intercepted\n")

    long res = ((SSL_get_verify_result_callback) GET_ORIG_FUNC_PTR(SSL_get_verify_result))(ssl);
    if (is_ca_ignored) {
        OSSLTRACE_LOG("retval is %ld, though we'll return 0\n***\n", res)
        return 0;    
    } else {
        OSSLTRACE_LOG("retval is %ld, thus returning %ld\n***\n", res, res)
        return res;
    }

}



ORIG_FUNC_PTR_STORAGE(SSL_CTX_set_verify);

void 
hooked_SSL_CTX_set_verify(SSL_CTX *ctx, int mode, SSL_verify_cb verify) {
    typedef void 
    (*SSL_CTX_set_verify_callback) (SSL_CTX *, int, SSL_verify_cb);

    OSSLTRACE_LOG("\n*** SSL_CTX_set_verify intercepted\n")
    if (is_ca_ignored) {
        OSSLTRACE_LOG("setting SSL_VERIFY_NONE");
        ((SSL_CTX_set_verify_callback) GET_ORIG_FUNC_PTR(SSL_CTX_set_verify))(ctx, SSL_VERIFY_NONE, NULL);
    } else {
        OSSLTRACE_LOG("calling with no changes")
        ((SSL_CTX_set_verify_callback) GET_ORIG_FUNC_PTR(SSL_CTX_set_verify))(ctx, mode, verify);
    }

    OSSLTRACE_LOG("\n***\n")
}
