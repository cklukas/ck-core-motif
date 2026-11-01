/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Mrm/Mrm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "Mrm modern C: %s\n", #test); return 1; } } while (0)

static unsigned allocations, releases, creations, cleanups;

static char *allocate(Cardinal size)
{
    ++allocations;
    return malloc(size);
}

static void release(char *memory)
{
    ++releases;
    free(memory);
}

static Widget create(Widget parent, String name, ArgList args, Cardinal count)
{
    (void)name;
    (void)args;
    (void)count;
    ++creations;
    return parent;
}

static void cleanup(Widget object)
{
    (void)object;
    ++cleanups;
}

int main(void)
{
    URMResourceContextPtr context = NULL;
    WCIClassDescPtr descriptor = NULL;
    CHECK(UrmGetResourceContext(allocate, release, 32, &context) == MrmSUCCESS);
    CHECK(context != NULL && allocations == 2);
    strcpy(context->data_buffer, "retained");
    CHECK(UrmResizeResourceContext(context, 64) == MrmSUCCESS);
    CHECK(strcmp(context->data_buffer, "retained") == 0);
    CHECK(UrmFreeResourceContext(context) == MrmSUCCESS);
    CHECK(allocations == releases);
    CHECK(MrmRegisterClassWithCleanup(0, NULL, "ModernCConstructor", create,
                                     NULL, cleanup) == MrmSUCCESS);
    CHECK(Urm__WCI_LookupClassDescriptor("ModernCConstructor", &descriptor)
          == MrmSUCCESS);
    CHECK(descriptor != NULL);
    CHECK(descriptor->creator(NULL, "probe", NULL, 0) == NULL);
    descriptor->cleanup(NULL);
    CHECK(creations == 1 && cleanups == 1);
    puts("PASS typed Mrm allocation, resize, constructor and cleanup callbacks");
    return 0;
}
