/* XPM input termination, comment, rendering and stream-error regression.
 * Build like check-transfer-storage.c; run under Xvfb with one writable task
 * directory argument. No files are created outside that directory.
 */
#include <Xm/XpmP.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <limits.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned long checks;

static int parse(const char *s)
{
    XpmImage image;
    XpmInfo info;
    size_t length = strlen(s) + 1;
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    size_t allocation = ((length + page - 1) / page + 1) * page;
    char *mapping = mmap(NULL, allocation, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    char *buffer;
    int status;
    CHECK(mapping != MAP_FAILED);
    CHECK(mprotect(mapping + allocation - page, page, PROT_NONE) == 0);
    buffer = mapping + allocation - page - length;
    memcpy(buffer, s, length);
    memset(&info, 0, sizeof(info));
    info.valuemask = XpmReturnComments | XpmReturnExtensions;
    alarm(3);
    status = XpmCreateXpmImageFromBuffer(buffer, &image, &info);
    alarm(0);
    if (status == XpmSuccess) {
        CHECK(image.width == 2 && image.height == 2 && image.ncolors == 2);
        CHECK(image.data[0] == 0 && image.data[1] == 1 &&
              image.data[2] == 1 && image.data[3] == 0);
        XpmFreeXpmImage(&image);
    } else CHECK(status == XpmFileInvalid);
    XpmFreeXpmInfo(&info);
    CHECK(munmap(mapping, allocation) == 0);
    return status;
}

static void prefixes(const char *source)
{
    size_t length = strlen(source), i;
    char *buffer = strdup(source);
    CHECK(buffer != NULL);
    for (i = 0; i < length; ++i) {
        char saved = buffer[i];
        buffer[i] = '\0';
        (void)parse(buffer);
        buffer[i] = saved;
    }
    CHECK(parse(source) == XpmSuccess);
    free(buffer);
}

int main(int argc, char **argv)
{
    const char *valid[] = {
        "/* XPM */\n/* hints */\nstatic char *i[]={\n\"2 2 2 1\",\n/* colors */\n\"a c #000000\",\n\"b c #ffffff\",\n/* pixels */\n\"ab\",\n\"ba\"};\n",
        "/* XPM */\nstatic char *i[]={\"2 2 2 2\",\"aa c black\",\"bb c white\",\"aabb\",\"bbaa\"};\n",
        "/* XPM */\nstatic char *i[]={\"2 2 2 4\",\"aaaa c black\",\"bbbb c white\",\"aaaabbbb\",\"bbbbaaaa\"};\n",
        "/* XPM */\nstatic char *i[]={\"2 2 2 1\",\"\200 c black\",\"\377 c white\",\"\200\377\",\"\377\200\"};\n",
        "/* XPM */\nstatic char *i[]={\"2 2 2 1 XPMEXT\",\"a c black\",\"b c white\",\"ab\",\"ba\",\"XPMEXT metadata\",\"value\",\"XPMENDEXT\"};\n",
        "! XPM2\n! hints\n2 2 2 1\na c black\nb c white\nab\nba\n",
        "! XPM2 Lisp\n; hints\n(setq i '(\"2 2 2 1\"\"a c black\"\"b c white\"\"ab\"\"ba\"))\n"
    };
    const char *broken[] = {
        "/* XPM */\n/* unterminated",
        "/* XPM */\n/* unterminated *",
        "/* XPM */\n/*",
        "! XPM2\n! unterminated comment",
        "! XPM2 Lisp\n; unterminated comment"
    };
    char path[4096];
    char oversized[256];
    char *readback = NULL;
    Display *display;
    unsigned i;
    CHECK(argc == 2);
    CHECK(snprintf(path, sizeof(path), "%s/input.xpm", argv[1]) < (int)sizeof(path));
    for (i = 0; i < sizeof(valid)/sizeof(valid[0]); ++i) prefixes(valid[i]);
    snprintf(oversized, sizeof(oversized),
             "/* XPM */\nstatic char *i[]={\"%llu 2 2 1\",\"a c black\",\"b c white\",\"ab\",\"ba\"};\n",
             (unsigned long long)UINT_MAX + 3);
    CHECK(parse(oversized) == XpmFileInvalid);
    for (i = 0; i < sizeof(broken)/sizeof(broken[0]); ++i) {
        XpmImage image;
        CHECK(parse(broken[i]) == XpmFileInvalid);
        CHECK(XpmWriteFileFromBuffer(path, (char *)broken[i]) == XpmSuccess);
        alarm(3);
        CHECK(XpmReadFileToXpmImage(path, &image, NULL) == XpmFileInvalid);
        alarm(0);
    }
    CHECK(XpmWriteFileFromBuffer(path, "") == XpmSuccess);
    CHECK(XpmReadFileToBuffer(path, &readback) == XpmSuccess);
    CHECK(readback != NULL && readback[0] == '\0');
    XpmFree(readback);
    CHECK(XpmWriteFileFromBuffer("/dev/full", "buffered write") == XpmOpenFailed);
    CHECK(XpmWriteFileFromBuffer(path, (char *)valid[0]) == XpmSuccess);
    CHECK(XpmReadFileToBuffer(path, &readback) == XpmSuccess);
    CHECK(strcmp(readback, valid[0]) == 0);
    XpmFree(readback);
    display = XOpenDisplay(NULL);
    CHECK(display != NULL);
    for (i = 0; i < sizeof(valid)/sizeof(valid[0]); ++i) {
        XImage *image = NULL, *mask = NULL;
        XpmAttributes attributes;
        memset(&attributes, 0, sizeof(attributes));
        CHECK(XpmCreateImageFromBuffer(display, (char *)valid[i], &image,
                                       &mask, &attributes) == XpmSuccess);
        CHECK(image && image->width == 2 && image->height == 2);
        CHECK(XGetPixel(image, 0, 0) == XGetPixel(image, 1, 1));
        CHECK(XGetPixel(image, 1, 0) == XGetPixel(image, 0, 1));
        CHECK(XGetPixel(image, 0, 0) != XGetPixel(image, 1, 0));
        XDestroyImage(image);
        if (mask) XDestroyImage(mask);
        XpmFreeAttributes(&attributes);
    }
    XCloseDisplay(display);
    CHECK(unlink(path) == 0);
    printf("PASS XPM input, comments, pixels and stream errors: %lu checks\n", checks);
    return 0;
}
