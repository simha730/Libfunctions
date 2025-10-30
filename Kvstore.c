static int store_count = 0;

const char *kv_get(const char *key)
{
    for (int i = 0; i < store_count; i++) {
        if (strcmp(store[i].key, key) == 0)
            return store[i].value;
    }
    return NULL;
}

void kv_set(const char *key, const char *value)
{
    for (int i = 0; i < store_count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, BUF_SIZE - 1);
            store[i].value[BUF_SIZE - 1] = '\0';
            return;
        }
    }

    if (store_count < MAX_ENTRIES) {
        strncpy(store[store_count].key, key, BUF_SIZE - 1);
        strncpy(store[store_count].value, value, BUF_SIZE - 1);
        store[store_count].key[BUF_SIZE - 1] = '\0';
        store[store_count].value[BUF_SIZE - 1] = '\0';
        store_count++;
    }
}
