#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define P 0xffffffffffffffc5ull
#define G 5

static inline uint64_t
mul_mod_p(uint64_t a, uint64_t b) {
    uint64_t m = 0;
    while (b) {
        if (b&1) {
            uint64_t t = P - a;
            if (m >= t) {
                m -= t;
            } else {
                m += a;
            }
        }
        if (a >= P - a) {
            a = a * 2 - P;
        } else {
            a = a * 2;
        }
        b >>= 1;
    }
    return m;
}

static inline uint64_t
pow_mod_p(uint64_t a, uint64_t b) {
    if (b == 1) {
        return a;
    }
    uint64_t t = pow_mod_p(a, b>>1);
    t = mul_mod_p(t, t);
    if (b % 2) {
        t = mul_mod_p(t, a);
    }
    return t;
}

static inline uint64_t
powmodp(uint64_t a, uint64_t b) {
    if (a == 0)
        return 1;
    if (b == 0)
        return 1;
    if (a > P)
        a %= P;
    return pow_mod_p(a, b);
}

uint64_t
dh64_private_key() {
    uint64_t a = rand();
    uint64_t b = rand() & 0xFFFF;
    uint64_t c = rand() & 0xFFFF;
    uint64_t d = (rand() & 0xFFFF) + 1;
    return a << 48 | b << 32 | c << 16 | d;
}

uint64_t 
dh64_public_key(const uint64_t private_key) {
    return powmodp(G, private_key);
}

uint64_t
dh64_secret(const uint64_t private_key, const uint64_t another_public_key) {
    return powmodp(another_public_key, private_key);
}

int 
_secret(lua_State *L) {
    const char *s_priv_key = lua_tostring(L, 1);
    const char *s_pub_key = lua_tostring(L, 2);
    uint64_t private_key = 0;
    uint64_t public_key = 0;
    sscanf(s_priv_key, "%lu", &private_key);
    sscanf(s_pub_key, "%lu", &public_key);
    uint64_t secret_key = dh64_secret(private_key, public_key);
    char * buffer = NULL;
    asprintf(&buffer, "%lu",secret_key);
    lua_pushstring(L, buffer);
    free(buffer);
    return 1;
}

int 
_pair(lua_State *L) {
    uint64_t private_key = dh64_private_key();
    uint64_t public_key = dh64_public_key(private_key);
    char *buffer = NULL;
    asprintf(&buffer, "%lu", private_key);
    lua_pushstring(L, buffer);
    free(buffer);
    asprintf(&buffer, "%lu", public_key);
    lua_pushstring(L, buffer);
    free(buffer);
    return 2;
}

int 
luaopen_dh64_c(lua_State *L) {
    srand(time(NULL));
    luaL_Reg reg[] = {
        {"pair" , _pair },
        {"secret" , _secret },
        {NULL,NULL},
    };
    luaL_register(L, "dh64.c", reg);
    return 1;
}
