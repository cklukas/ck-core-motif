/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Direct-source allocation ownership test, not an installed-library fixture.
 * Compile together with TextUnicode.c and --wrap=malloc,--wrap=calloc.
 * Only allocations made directly by these objects are wrapped; ICU's shared
 * library allocations are deliberately outside this deterministic seam. */
#include <Xm/TextUnicode.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *__real_malloc(size_t);
void *__real_calloc(size_t, size_t);
void *__wrap_malloc(size_t);
void *__wrap_calloc(size_t, size_t);
static size_t allocation_count, failure_index;
static int armed;
static unsigned int assertions;
static int refuse(void) {
    if (!armed) return 0;
    ++allocation_count;
    return failure_index && allocation_count == failure_index;
}
void *__wrap_malloc(size_t bytes) {
    return refuse() ? NULL : __real_malloc(bytes);
}
void *__wrap_calloc(size_t count, size_t bytes) {
    return refuse() ? NULL : __real_calloc(count, bytes);
}
static void arm(size_t failure) {
    allocation_count = 0; failure_index = failure; armed = 1;
}
static size_t disarm(void) {
    armed = 0; return allocation_count;
}
static void require(int condition, const char *message) {
    ++assertions;
    if (!condition) {
        fprintf(stderr, "incremental allocation ownership: %s\n", message);
        exit(1);
    }
}
#define LIMIT 512
/* Capture all observable map state without calling an allocator. */
typedef struct {
    size_t bytes, scalars, graphemes;
    XmTextUnicodeMapUsage usage;
    size_t scalar_bytes[LIMIT], grapheme_scalars[LIMIT];
    size_t previous[LIMIT], next[LIMIT], byte_scalars[LIMIT];
    int boundary[LIMIT], exact_byte[LIMIT];
} Snapshot;
static void capture(const XmTextUnicodeMap *map, Snapshot *s) {
    size_t n;
    s->bytes = XmTextUnicodeByteCount(map);
    s->scalars = XmTextUnicodeScalarCount(map);
    s->graphemes = XmTextUnicodeGraphemeCount(map);
    require(s->bytes < LIMIT && s->scalars < LIMIT && s->graphemes < LIMIT,
            "fixture snapshot capacity exceeded");
    require(XmTextUnicodeMapGetUsage(map, &s->usage), "usage unavailable");
    for (n = 0; n <= s->scalars; ++n) {
        require(XmTextUnicodeByteAtScalar(map, n, &s->scalar_bytes[n]), "scalar mapping unavailable");
        require(XmTextUnicodePreviousGrapheme(map, n, &s->previous[n]), "previous boundary unavailable");
        require(XmTextUnicodeNextGrapheme(map, n, &s->next[n]), "next boundary unavailable");
        s->boundary[n] = XmTextUnicodeIsGraphemeBoundary(map, n);
    }
    for (n = 0; n <= s->graphemes; ++n)
        require(XmTextUnicodeScalarAtGrapheme(map, n, &s->grapheme_scalars[n]), "grapheme mapping unavailable");
    for (n = 0; n <= s->bytes; ++n) {
        s->byte_scalars[n] = 0;
        s->exact_byte[n] = XmTextUnicodeScalarAtByte(map, n, &s->byte_scalars[n]);
    }
}
static void unchanged(const XmTextUnicodeMap *map, const Snapshot *s) {
    Snapshot actual = {0};
    capture(map, &actual);
    require(actual.bytes == s->bytes && actual.scalars == s->scalars &&
            actual.graphemes == s->graphemes, "failed operation changed counts");
    require(!memcmp(&actual.usage, &s->usage, sizeof(s->usage)), "failed operation changed usage");
    require(!memcmp(actual.scalar_bytes, s->scalar_bytes, (s->scalars + 1) * sizeof(size_t)), "failed operation changed scalar bytes");
    require(!memcmp(actual.grapheme_scalars, s->grapheme_scalars, (s->graphemes + 1) * sizeof(size_t)), "failed operation changed grapheme scalars");
    require(!memcmp(actual.previous, s->previous, (s->scalars + 1) * sizeof(size_t)), "failed operation changed previous boundaries");
    require(!memcmp(actual.next, s->next, (s->scalars + 1) * sizeof(size_t)), "failed operation changed next boundaries");
    require(!memcmp(actual.boundary, s->boundary, (s->scalars + 1) * sizeof(int)), "failed operation changed boundary membership");
    require(!memcmp(actual.exact_byte, s->exact_byte, (s->bytes + 1) * sizeof(int)), "failed operation changed byte exactness");
    require(!memcmp(actual.byte_scalars, s->byte_scalars, (s->bytes + 1) * sizeof(size_t)), "failed operation changed byte scalar mappings");
}
static const char source[] =
    "prefix unchanged\n"
    "A\314\201 and \360\237\207\272\360\237\207\270 flags\r\n"
    "middle paragraph\n"
    "suffix unchanged\n"
    "last paragraph";
static const char inserted[] = "new \360\237\221\251\342\200\215\360\237\222\273\njoined\r\n";
int main(void) {
    XmTextUnicodeMap *original, *retained, *derived, *derived_retained, *result;
    Snapshot before = {0}, changed = {0};
    size_t create_allocations, replace_allocations, n, observed;
    original = XmTextUnicodeMapCreate(source, sizeof(source) - 1);
    require(original != NULL, "initial map creation failed");
    retained = XmTextUnicodeMapRetain(original);
    require(retained == original, "retained snapshot changed identity");
    derived = XmTextUnicodeMapReplace(original, 20, 30, inserted, sizeof(inserted) - 1);
    require(derived != NULL, "initial delta failed");
    derived_retained = XmTextUnicodeMapRetain(derived);
    require(derived_retained == derived, "retained delta changed identity");
    capture(original, &before); capture(derived, &changed);

    arm(0);
    result = XmTextUnicodeMapCreate(source, sizeof(source) - 1);
    create_allocations = disarm();
    require(result != NULL && create_allocations > 0, "creation allocation seam did not activate");
    unchanged(result, &before); XmTextUnicodeMapFree(result);
    for (n = 1; n <= create_allocations; ++n) {
        arm(n);
        result = XmTextUnicodeMapCreate(source, sizeof(source) - 1);
        observed = disarm();
        require(observed >= n && result == NULL, "creation allocation failure was not atomic");
        unchanged(original, &before); unchanged(retained, &before);
        unchanged(derived, &changed); unchanged(derived_retained, &changed);
        result = XmTextUnicodeMapCreate(source, sizeof(source) - 1);
        require(result != NULL, "creation did not recover after disarming");
        unchanged(result, &before); XmTextUnicodeMapFree(result);
    }

    arm(0);
    result = XmTextUnicodeMapReplace(original, 20, 30, inserted, sizeof(inserted) - 1);
    replace_allocations = disarm();
    require(result != NULL && replace_allocations > 0, "delta allocation seam did not activate");
    unchanged(result, &changed); XmTextUnicodeMapFree(result);
    for (n = 1; n <= replace_allocations; ++n) {
        arm(n);
        result = XmTextUnicodeMapReplace(original, 20, 30, inserted, sizeof(inserted) - 1);
        observed = disarm();
        require(observed >= n && result == NULL, "delta allocation failure was not atomic");
        unchanged(original, &before); unchanged(retained, &before);
        unchanged(derived, &changed); unchanged(derived_retained, &changed);
        result = XmTextUnicodeMapReplace(retained, 20, 30, inserted, sizeof(inserted) - 1);
        require(result != NULL, "delta did not recover after disarming");
        unchanged(result, &changed); XmTextUnicodeMapFree(result);
    }
    /* Release the original handles before continuing from retained versions.
     * The derived map also shares untouched bodies with the original map. */
    XmTextUnicodeMapFree(original); XmTextUnicodeMapFree(derived);
    unchanged(retained, &before); unchanged(derived_retained, &changed);
    result = XmTextUnicodeMapReplace(derived_retained, 0, 0, "x", 1);
    require(result != NULL, "edit after owner release failed");
    XmTextUnicodeMapFree(derived_retained);
    require(XmTextUnicodeScalarCount(result) == changed.scalars + 1,
            "new version lost shared bodies after prior version release");
    XmTextUnicodeMapFree(retained); XmTextUnicodeMapFree(result);
    printf("incremental-map-faults: %u assertions; %lu creation and %lu delta allocation failures; direct-source ownership only\n",
           assertions, (unsigned long)create_allocations, (unsigned long)replace_allocations);
    return 0;
}
