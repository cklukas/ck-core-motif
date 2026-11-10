/*
 * A UID file is untrusted input for the resource manager.  This program takes
 * a UID file that the UIL compiler produced from a real interface description,
 * damages it in a reproducible way, and drives the loader with every damaged
 * copy: it opens the hierarchy and fetches every widget, literal, colour, icon
 * and bitmap that the interface defines, then closes it.  Run under
 * AddressSanitizer and UndefinedBehaviorSanitizer, any read or write outside
 * the file's data, any overflowing size and any invalid pointer is a failure.
 *
 *   check-mrm-uid-mutations BASE.uid NAMES WORKFILE FIRST COUNT LOGDIR
 *
 * NAMES lists the objects ("o:name") and values ("v:name") of the interface,
 * one per line.  The
 * copies FIRST .. FIRST+COUNT-1 are generated from a fixed pseudo-random
 * sequence, so a failure names the copy that reproduces it.
 *
 * A damaged file can also give a widget resource a value of the wrong type
 * (for example an integer where the widget expects a string): the loader
 * trusts the compiler to have matched them, and the widget then misuses the
 * value.  Every copy therefore runs in a child process, and the sanitizer
 * report decides whom the failure belongs to: a report whose first frame of
 * toolkit code is in the resource manager fails the program, one that first
 * enters widget code is counted and reported but does not.
 */

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/XmAll.h>
#include <Mrm/MrmPublic.h>

static uint64_t rng_state;

static uint64_t next_random(void)
{
    rng_state ^= rng_state >> 12;
    rng_state ^= rng_state << 25;
    rng_state ^= rng_state >> 27;
    return rng_state * UINT64_C(2685821657736338717);
}

static unsigned char * base;
static size_t base_size;
static char ** names;
static size_t name_count;

static void mutate(unsigned char * buf, size_t * size, unsigned long copy)
{
    unsigned kind;
    unsigned n, i;

    rng_state = UINT64_C(0x9E3779B97F4A7C15) ^ ((uint64_t) copy * UINT64_C(0xD1B54A32D192ED03));
    if (rng_state == 0) rng_state = 1;
    for (i = 0; i < 4; i++) (void) next_random();
    memcpy(buf, base, base_size);
    *size = base_size;
    kind = (unsigned) (next_random() % 8);
    switch (kind) {
    case 0:			/* a few random bytes */
	n = 1 + (unsigned) (next_random() % 8);
	for (i = 0; i < n; i++)
	    buf[next_random() % base_size] = (unsigned char) next_random();
	break;
    case 1:			/* a 32 bit field set to an extreme value */
    case 2: {
	static const uint32_t extreme[] = { 0, 1, 0x7fffffffu, 0x80000000u, 0xffffffffu, 0xffff, 0x10000, 0xfffffff0u, 0x7fff };
	n = 1 + (unsigned) (next_random() % 3);
	for (i = 0; i < n; i++) {
	    size_t at = next_random() % (base_size - 4);
	    uint32_t v = extreme[next_random() % (sizeof(extreme) / sizeof(extreme[0]))];
	    memcpy(buf + at, &v, 4);
	}
	break;
    }
    case 3: {			/* a 16 bit field */
	n = 1 + (unsigned) (next_random() % 4);
	for (i = 0; i < n; i++) {
	    size_t at = next_random() % (base_size - 2);
	    uint16_t v = (uint16_t) ((next_random() & 1) ? 0xffff : 0);
	    memcpy(buf + at, &v, 2);
	}
	break;
    }
    case 4:			/* truncation */
	*size = 1 + next_random() % base_size;
	break;
    case 5: {			/* a block copied over another */
	size_t len = 1 + next_random() % 256;
	size_t from = next_random() % base_size, to = next_random() % base_size;
	if (from + len > base_size) len = base_size - from;
	if (to + len > base_size) len = base_size - to;
	memmove(buf + to, buf + from, len);
	break;
    }
    case 6: {			/* the header and the first records */
	n = 1 + (unsigned) (next_random() % 6);
	for (i = 0; i < n; i++)
	    buf[next_random() % (base_size < 512 ? base_size : 512)] = (unsigned char) next_random();
	break;
    }
    default: {			/* a run of one byte */
	size_t at = next_random() % base_size;
	size_t len = 1 + next_random() % 64;
	if (at + len > base_size) len = base_size - at;
	memset(buf + at, (int) (next_random() & 1 ? 0xff : 0x00), len);
	break;
    }
    }
}

static unsigned long fetched_widgets, fetched_literals;

static void exercise(Widget shell, const char * path)
{
    MrmHierarchy hierarchy = NULL;
    String files[1];
    size_t i;
    Display * display = XtDisplay(shell);
    Screen * screen = XtScreen(shell);

    files[0] = (String) path;
    if (MrmOpenHierarchyPerDisplay(display, 1, files, NULL, &hierarchy) != MrmSUCCESS)
	return;
    for (i = 0; i < name_count; i++) {
	Widget w = NULL;
	MrmType class_return;
	XtPointer value = NULL;
	MrmCode type = 0;
	Pixel pixel;
	Pixmap pixmap;
	Dimension width, height;

	/* "o:name" is a widget, "v:name" a value */
	const char * name = names[i] + 2;
	if (names[i][0] == 'o') {
	    if (MrmFetchWidget(hierarchy, (String) name, shell, &w, &class_return) == MrmSUCCESS && w != NULL) {
		fetched_widgets++;
		XtDestroyWidget(w);
	    }
	    continue;
	}
	if (MrmFetchLiteral(hierarchy, (String) name, display, &value, &type) == MrmSUCCESS && value != NULL) {
	    fetched_literals++;
	    if (type == MrmRtypeChar8 || type == MrmRtypeCString)
		XtFree(value);
	}
	(void) MrmFetchColorLiteral(hierarchy, (String) name, display, DefaultColormapOfScreen(screen), &pixel);
	(void) MrmFetchIconLiteral(hierarchy, (String) name, screen, display,
				   BlackPixelOfScreen(screen), WhitePixelOfScreen(screen), &pixmap);
	(void) MrmFetchBitmapLiteral(hierarchy, (String) name, screen, display, &pixmap, &width, &height);
    }
    {
	Widget w = NULL;
	(void) MrmFetchInterfaceModule(hierarchy, "module", shell, &w);
	if (w != NULL) XtDestroyWidget(w);
    }
    MrmCloseHierarchy(hierarchy);
}

enum { OWNER_LOADER, OWNER_WIDGET, OWNER_UNKNOWN };
static const char * logdir;

/* The child's standard error: the sanitizers report there. */
static void redirect_report(void)
{
    char path[1024];
    int fd;

    snprintf(path, sizeof(path), "%s/report.%d", logdir, (int) getpid());
    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) { dup2(fd, 2); close(fd); }
}

/* Reads the report of a child and decides who owns the failure. */
static int classify(pid_t pid, unsigned long copy, int status)
{
    char path[1024], line[2048];
    int owner = OWNER_UNKNOWN, found = 0;
    FILE * f;

    snprintf(path, sizeof(path), "%s/report.%d", logdir, (int) pid);
    f = fopen(path, "r");
    if (f) {
	found = 1;
	while (owner == OWNER_UNKNOWN && fgets(line, sizeof(line), f)) {
	    /* the first frame (or UBSan location) inside this tree decides */
	    const char * mrm = strstr(line, "/lib/Mrm/");
	    const char * xm = strstr(line, "/lib/Xm/");
	    if (mrm && (!xm || mrm < xm)) owner = OWNER_LOADER;
	    else if (xm) owner = OWNER_WIDGET;
	}
	fclose(f);
    }
    fprintf(stderr, "copy %lu: %s", copy,
	    WIFSIGNALED(status) ? "killed by signal" : "failed");
    if (WIFSIGNALED(status)) fprintf(stderr, " %d", WTERMSIG(status));
    else if (WIFEXITED(status)) fprintf(stderr, " (exit status %d)", WEXITSTATUS(status));
    fprintf(stderr, ", %s (report %s)\n",
	    owner == OWNER_LOADER ? "in the loader" :
	    owner == OWNER_WIDGET ? "in widget code" :
	    found ? "location unknown" : "no report", path);
    return owner == OWNER_WIDGET ? OWNER_WIDGET : OWNER_LOADER;
}

static void log_quietly(String msg) { (void) msg; }
static void log_quietly_msg(String a, String b, String c, String d, String * e, Cardinal * f)
{ (void) a; (void) b; (void) c; (void) d; (void) e; (void) f; }

/* A lost X server is not the loader's failure: the child reports it. */
static int display_lost(Display * d) { (void) d; _exit(77); }
static void error_lost(String msg) __attribute__((noreturn));
static void error_lost(String msg) { (void) msg; _exit(77); }

/* Every child has its own connection and toolkit state. */
static void run_in_child(const char * path)
{
    XtAppContext app;
    Display * display;
    Widget shell;
    int xargc = 1;
    char * xargv[] = { "mrm-uid-mutations", NULL };

    XtSetLanguageProc(NULL, NULL, NULL);
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    XtAppSetErrorHandler(app, error_lost);
    display = XtOpenDisplay(app, NULL, "mrm-uid-mutations", "MrmUidMutations",
			    NULL, 0, &xargc, xargv);
    if (display == NULL)
	_exit(77);
    XSetIOErrorHandler(display_lost);
    MrmInitialize();
    shell = XtVaAppCreateShell("mrm-uid-mutations", "MrmUidMutations",
			       applicationShellWidgetClass, display, NULL);
    XtSetWarningHandler(log_quietly);
    XtAppSetWarningMsgHandler(app, log_quietly_msg);
    exercise(shell, path);
}

static unsigned char * read_all(const char * path, size_t * size)
{
    FILE * f = fopen(path, "rb");
    unsigned char * data;
    long n;
    if (!f) { perror(path); exit(2); }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    data = malloc((size_t) n + 1);
    if (fread(data, 1, (size_t) n, f) != (size_t) n) exit(2);
    fclose(f);
    *size = (size_t) n;
    return data;
}


int main(int argc, char ** argv)
{
    unsigned long first, count, k, loader_failures = 0, widget_failures = 0;
    unsigned char * buf;
    size_t size, name_size, i;
    unsigned char * name_text;
    if (argc != 7) {
	fprintf(stderr, "usage: %s BASE.uid NAMES WORKFILE FIRST COUNT LOGDIR\n", argv[0]);
	return 2;
    }
    logdir = argv[6];
    base = read_all(argv[1], &base_size);
    name_text = read_all(argv[2], &name_size);
    first = strtoul(argv[4], NULL, 10);
    count = strtoul(argv[5], NULL, 10);
    for (i = 0; i < name_size; i++)
	if (name_text[i] == '\n') name_count++;
    names = calloc(name_count + 1, sizeof(char *));
    {
	size_t at = 0, start = 0;
	for (i = 0; i < name_size; i++)
	    if (name_text[i] == '\n') {
		name_text[i] = 0;
		names[at++] = (char *) name_text + start;
		start = i + 1;
	    }
    }
    buf = malloc(base_size);

    /* the file as the compiler wrote it must load completely */
    {
	pid_t pid;
	int status;
	FILE * f = fopen(argv[3], "wb");
	if (!f || fwrite(base, 1, base_size, f) != base_size) { perror(argv[3]); return 2; }
	fclose(f);
	fflush(NULL);
	pid = fork();
	if (pid == 0) {
	    alarm(120);
	    redirect_report();
	    run_in_child(argv[3]);
	    printf("unchanged file: %lu widgets and %lu literals fetched\n", fetched_widgets, fetched_literals);
	    fflush(NULL);
	    {
		int want_widgets = 0, want_literals = 0;
		size_t n;
		for (n = 0; n < name_count; n++) {
		    if (names[n][0] == 'o') want_widgets = 1;
		    else want_literals = 1;
		}
		_exit((want_widgets && fetched_widgets == 0) ||
		      (want_literals && fetched_literals == 0) ? 3 : 0);
	    }
	}
	if (pid > 0 && waitpid(pid, &status, 0) >= 0 && WIFEXITED(status) && WEXITSTATUS(status) == 77) {
	    printf("server lost at copy baseline\n");
	    fflush(NULL);
	    return 77;
	}
	if (pid < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
	    fprintf(stderr, "FAIL the unchanged file does not load completely (status %d)\n", status);
	    classify(pid, 0, status);
	    return 1;
	}
    }

    for (k = first; k < first + count; k++) {
	FILE * f;
	pid_t pid;
	int status;

	mutate(buf, &size, k);
	f = fopen(argv[3], "wb");
	if (!f || fwrite(buf, 1, size, f) != size) { perror(argv[3]); return 2; }
	fclose(f);
	fflush(NULL);
	pid = fork();
	if (pid == 0) {
	    alarm(60);
	    redirect_report();
	    run_in_child(argv[3]);
	    _exit(0);
	}
	if (pid < 0 || waitpid(pid, &status, 0) < 0) { perror("fork"); return 2; }
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
	    continue;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 77) {
	    printf("server lost at copy %lu\n", k);
	    fflush(NULL);
	    return 77;
	}
	{
	    int owner = classify(pid, k, status);
	    if (owner == OWNER_LOADER) loader_failures++;
	    else if (owner == OWNER_WIDGET) widget_failures++;
	    else loader_failures++;
	}
    }
    if (widget_failures)
	printf("note: %lu damaged files made widget code misuse a resource value\n", widget_failures);
    if (loader_failures) {
	printf("mrm uid mutations: %lu of %lu copies failed in the loader\n", loader_failures, count);
	return 1;
    }
    printf("mrm uid mutations: %lu copies of %zu bytes, %zu names: ok\n", count, base_size, name_count);
    return 0;
}
