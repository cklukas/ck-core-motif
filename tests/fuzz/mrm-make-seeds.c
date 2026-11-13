/*
 * Writes the seed UID files of the mrm-uid target with the resource manager's
 * own writer (the same calls the UIL compiler makes), so the seeds are valid
 * by construction for the revision that is fuzzed.
 *
 *   mrm-make-seeds DIR
 *
 * literals.uid   integer, string, colour name and compound string literals
 * widgets.uid    the literals, plus widgets with arguments, resource
 *                references, a callback and a child
 * icon.uid       an icon image with a colour table and pixel data, a colour
 *                literal and an integer literal
 * index.uid      sixty integer literals: the index grows past one record
 */
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/Mrm.h>

#include "mrm-stubs.h"

static char *get_memory(unsigned n) { return XtMalloc(n); }
static void free_memory(char *p) { XtFree(p); }

static void check(Cardinal status, const char *what)
{
    if (status != MrmSUCCESS) {
        fprintf(stderr, "mrm-make-seeds: %s failed (%u)\n", what, status);
        exit(1);
    }
}

static IDBFile open_file(const char *dir, const char *name)
{
    char path[1024];
    IDBFile file = NULL;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    check(UrmIdbOpenFileWrite(path, NULL, "fuzz", "1.0", "module", "1.0", &file, NULL), "open for writing");
    return file;
}

static URMResourceContextPtr new_context(size_t size)
{
    URMResourceContextPtr ctx = NULL;
    check(UrmGetResourceContext(get_memory, free_memory, size, &ctx), "context");
    return ctx;
}

static void put_literal(IDBFile file, const char *name, MrmType type, const void *data, size_t size)
{
    URMResourceContextPtr ctx = new_context(size);
    memcpy(UrmRCBuffer(ctx), data, size);
    UrmRCSetSize(ctx, size);
    UrmRCSetType(ctx, type);
    UrmRCSetAccess(ctx, URMaPublic);
    check(UrmPutIndexedLiteral(file, (String) name, ctx), name);
    UrmFreeResourceContext(ctx);
}

static void literals(IDBFile file)
{
    static const unsigned char cstring[] = { 0xdf, 0x80, 0x06, 0x07, 0x0a, 0x05, 'H', 'e', 'l', 'l', 'o' };
    int width = 40;
    unsigned char color[RGMColorDescNameMinSize + 8] = { 0 };
    RGMColorDescPtr cd = (RGMColorDescPtr) color;

    put_literal(file, "width", MrmRtypeInteger, &width, sizeof width);
    put_literal(file, "title", MrmRtypeChar8, "Hello", 6);
    cd->mono_state = URMColorMonochromeForeground;
    cd->desc_type = URMColorDescTypeName;
    strcpy(cd->desc.name, "red");
    put_literal(file, "color", MrmRtypeColor, color, RGMColorDescNameMinSize + 4);
    put_literal(file, "label", MrmRtypeCString, cstring, sizeof cstring);
}

static void widgets(IDBFile file)
{
    URMResourceContextPtr ctx = new_context(512);
    MrmOffset cb;

    check(UrmCWRInit(ctx, "button", URMaPublic, FALSE), "widget init");
    check(UrmCWRSetClass(ctx, MrmwcUnknown, "XmPushButton", 0), "class");
    check(UrmCWRInitArglist(ctx, 2), "arglist");
    check(UrmCWRSetUncompressedArgTag(ctx, 0, "labelString"), "tag");
    check(UrmCWRSetArgResourceRef(ctx, 0, URMaPublic, URMgLiteral, MrmRtypeCString, URMrIndex, "label", 0), "ref");
    check(UrmCWRSetUncompressedArgTag(ctx, 1, "activateCallback"), "tag");
    check(UrmCWRSetArgCallback(ctx, 1, 1, &cb), "callback");
    check(UrmCWRSetCallbackItem(ctx, cb, 0, "activate", MrmRtypeInteger, 7), "callback item");
    check(UrmPutIndexedWidget(file, "button", ctx), "put widget");
    UrmFreeResourceContext(ctx);

    ctx = new_context(512);
    check(UrmCWRInit(ctx, "main_window", URMaPublic, FALSE), "widget init");
    check(UrmCWRSetClass(ctx, MrmwcUnknown, "XmMainWindow", 0), "class");
    check(UrmCWRInitArglist(ctx, 3), "arglist");
    check(UrmCWRSetUncompressedArgTag(ctx, 0, "width"), "tag");
    check(UrmCWRSetArgValue(ctx, 0, MrmRtypeInteger, 300), "value");
    check(UrmCWRSetUncompressedArgTag(ctx, 1, "title"), "tag");
    check(UrmCWRSetArgResourceRef(ctx, 1, URMaPublic, URMgLiteral, MrmRtypeChar8, URMrIndex, "title", 0), "ref");
    check(UrmCWRSetUncompressedArgTag(ctx, 2, "foreground"), "tag");
    check(UrmCWRSetArgResourceRef(ctx, 2, URMaPublic, URMgLiteral, MrmRtypeColor, URMrIndex, "color", 0), "ref");
    check(UrmCWRInitChildren(ctx, 1), "children");
    check(UrmCWRSetChild(ctx, 0, TRUE, URMaPublic, URMrIndex, "button", 0), "child");
    check(UrmPutIndexedWidget(file, "main_window", ctx), "put widget");
    UrmFreeResourceContext(ctx);
}

static void icon(IDBFile file)
{
    enum { width = 8, height = 4, colors = 3 };
    size_t table_at = sizeof(RGMIconImage);
    size_t table_size = offsetof(RGMColorTable, item) + colors * sizeof(RGMColorTableEntry);
    size_t desc_at = table_at + table_size;
    size_t desc_size = RGMColorDescNameMinSize + 4;
    size_t pixels_at = desc_at + desc_size;
    size_t total = pixels_at + height * ((width * 4 + 7) / 8);
    unsigned char *buf = calloc(1, total);
    RGMIconImagePtr ic = (RGMIconImagePtr) buf;
    RGMColorTablePtr ct = (RGMColorTablePtr) (buf + table_at);
    RGMColorDescPtr cd = (RGMColorDescPtr) (buf + desc_at);

    ic->validation = URMIconImageValid;
    ic->pixel_size = URMPixelSize4Bit;
    ic->width = width;
    ic->height = height;
    ic->ct_type = MrmRtypeColorTable;
    ic->color_table.ctoff = (MrmOffset) table_at;
    ic->pixel_data.pdoff = (MrmOffset) pixels_at;
    ct->validation = URMColorTableValid;
    ct->count = colors;
    ct->item[2].type = MrmRtypeColor;
    ct->item[2].color_item.coffs = (MrmOffset) desc_at;
    cd->mono_state = URMColorMonochromeForeground;
    cd->desc_type = URMColorDescTypeName;
    strcpy(cd->desc.name, "red");
    for (size_t i = pixels_at; i < total; i++) buf[i] = (unsigned char) (i & 0x21);
    put_literal(file, "icon", MrmRtypeIconImage, buf, total);
    free(buf);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: mrm-make-seeds DIR\n");
        return 2;
    }
    MrmInitialize();
    IDBFile file = open_file(argv[1], "literals.uid");
    literals(file);
    check(UrmIdbCloseFile(file, TRUE), "close");

    file = open_file(argv[1], "widgets.uid");
    literals(file);
    widgets(file);
    check(UrmIdbCloseFile(file, TRUE), "close");

    file = open_file(argv[1], "icon.uid");
    {
        int one = 1;
        unsigned char color[RGMColorDescNameMinSize + 8] = { 0 };
        RGMColorDescPtr cd = (RGMColorDescPtr) color;
        cd->mono_state = URMColorMonochromeBackground;
        cd->desc_type = URMColorDescTypeName;
        strcpy(cd->desc.name, "blue");
        put_literal(file, "width", MrmRtypeInteger, &one, sizeof one);
        put_literal(file, "color", MrmRtypeColor, color, RGMColorDescNameMinSize + 5);
    }
    icon(file);
    check(UrmIdbCloseFile(file, TRUE), "close");

    file = open_file(argv[1], "index.uid");
    for (int i = 0; i < 60; i++) {
        char name[32];
        snprintf(name, sizeof name, "value_number_%02d", i);
        put_literal(file, name, MrmRtypeInteger, &i, sizeof i);
    }
    check(UrmIdbCloseFile(file, TRUE), "close");
    return 0;
}
