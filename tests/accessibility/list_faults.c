/* Target one optional bridge allocation, never native List storage or D-Bus. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <string.h>
extern void *__libc_calloc(size_t, size_t);
static unsigned int armed, hits;
static _Thread_local int resolving;
void motif_list_fault_arm(void) { armed = 1; hits = 0; }
void motif_list_fault_clear(void) { armed = 0; }
unsigned int motif_list_fault_hits(void) { return hits; }
void *calloc(size_t count, size_t size) {
  if (armed && !resolving) {
    Dl_info info;
    resolving = 1;
    int found = dladdr(__builtin_return_address(0), &info);
    resolving = 0;
    if (found && info.dli_fname && strstr(info.dli_fname, "/libXm.so.")) {
      ++hits; return NULL;
    }
  }
  return __libc_calloc(count, size);
}
