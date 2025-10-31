// kvstore.h — shared between library and server

#ifndef KVSTORE_H
#define KVSTORE_H

// exposed functions from the dynamic library
const char *kv_get(const char *key);
void kv_set(const char *key, const char *value);

#endif
