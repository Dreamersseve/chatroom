#pragma once
#ifndef SHA_256_H
#define SHA_256_H

#include <stdlib.h>
#include <stdint.h>

typedef struct hash_context {
    uint8_t buffer[64];
    uint32_t state[8];
    uint32_t total[2];
} hash_context;

void hash_start(hash_context* ctx);
void hash_update(hash_context* ctx, uint8_t* input, size_t ilen);
void hash_finish(hash_context* ctx, uint8_t* output);
static void sha256_transform(hash_context* ctx, const uint8_t data[]);

#endif // SHA_256_H

