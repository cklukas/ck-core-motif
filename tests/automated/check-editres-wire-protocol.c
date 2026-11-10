/*
 * Behavioural test of the Editres wire protocol handler.
 *
 * The production file lib/Xm/EditresCom.c is compiled into this program
 * (under AddressSanitizer and UndefinedBehaviorSanitizer when run by
 * run-editres-wire-protocol.py) and driven with requests built here, byte by
 * byte, from the protocol description.  The requests are handed over in
 * heap blocks of exactly the length that was "received", so a parser that
 * reads past the end of a request is caught.  The final cases run the whole
 * path with a second X connection acting as the resource editor.
 *
 * Define EDITRES_PREFIX_CONTROL to build this test against the earlier
 * implementation of the file, which relied on libXmu's protocol helpers.
 */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "EditresCom.c"

/* ------------------------------------------------------------------ */
/* Test bookkeeping                                                    */
/* ------------------------------------------------------------------ */

static int failures;
static int checks;

#define CHECK(cond, ...) \
    do { \
	checks++; \
	if (!(cond)) { \
	    failures++; \
	    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
	    fprintf(stderr, __VA_ARGS__); \
	    fprintf(stderr, "\n"); \
	} \
    } while (0)

/* ------------------------------------------------------------------ */
/* A widget class whose String resource keeps the pointer it is given */
/* ------------------------------------------------------------------ */

typedef struct { CorePart core; String label; } KeeperRec;
typedef struct _KeeperClassRec { CoreClassPart core_class; } KeeperClassRec;

static XtResource keeper_resources[] = {
    { "label", "Label", XtRString, sizeof(String),
      XtOffsetOf(KeeperRec, label), XtRImmediate, (XtPointer) NULL }
};

static KeeperClassRec keeperClassRec = {
    {
	(WidgetClass) &widgetClassRec, "Keeper", sizeof(KeeperRec),
	NULL, NULL, FALSE, NULL, NULL, XtInheritRealize, NULL, 0,
	keeper_resources, XtNumber(keeper_resources), NULLQUARK,
	TRUE, TRUE, TRUE, FALSE, NULL, XtInheritResize, XtInheritExpose,
	NULL, NULL, XtInheritSetValuesAlmost, NULL, XtInheritAcceptFocus,
	XtVersion, NULL, XtInheritTranslations, XtInheritQueryGeometry,
	XtInheritDisplayAccelerator, NULL
    }
};
static WidgetClass keeperWidgetClass = (WidgetClass) &keeperClassRec;

/* A composite widget that, unlike the generic one, accepts non-widget
 * children, so that a rectangle object can be part of the tree. */
typedef struct _HolderClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
} HolderClassRec;

static CompositeClassExtensionRec holder_extension = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), TRUE, FALSE
};

static HolderClassRec holderClassRec = {
    {
	(WidgetClass) &compositeClassRec, "Holder", sizeof(CompositeRec),
	NULL, NULL, FALSE, NULL, NULL, XtInheritRealize, NULL, 0,
	NULL, 0, NULLQUARK,
	TRUE, TRUE, TRUE, FALSE, NULL, XtInheritResize, XtInheritExpose,
	NULL, NULL, XtInheritSetValuesAlmost, NULL, XtInheritAcceptFocus,
	XtVersion, NULL, XtInheritTranslations, XtInheritQueryGeometry,
	XtInheritDisplayAccelerator, NULL
    },
    {
	NULL, NULL, XtInheritInsertChild, XtInheritDeleteChild,
	(XtPointer) &holder_extension
    }
};
static WidgetClass holderWidgetClass = (WidgetClass) &holderClassRec;

/* ------------------------------------------------------------------ */
/* Request building and reply reading, from the protocol description   */
/* ------------------------------------------------------------------ */

typedef struct { uint8_t * p; size_t n, cap; } Buf;

static void put8(Buf * b, unsigned v)
{
    if (b->n == b->cap) {
	b->cap = b->cap ? b->cap * 2 : 64;
	b->p = realloc(b->p, b->cap);
    }
    b->p[b->n++] = (uint8_t) v;
}
static void put16(Buf * b, unsigned v) { put8(b, v >> 8); put8(b, v); }
static void put32(Buf * b, uint32_t v) { put16(b, v >> 16); put16(b, v & 0xFFFF); }
static void putstr(Buf * b, const char * s)
{
    size_t i, n = strlen(s);
    put16(b, (unsigned) n);
    for (i = 0; i < n; i++) put8(b, (unsigned char) s[i]);
}
static void putpath(Buf * b, Widget w)
{
    Widget chain[64];
    int n = 0, i;
    for (; w != NULL; w = XtParent(w)) chain[n++] = w;
    put16(b, (unsigned) n);
    for (i = n - 1; i >= 0; i--) put32(b, (uint32_t) ((uintptr_t) chain[i] & 0xFFFFFFFFu));
}

typedef struct { const uint8_t * p; const uint8_t * end; int bad; } Rd;

static unsigned r8(Rd * r)
{
    if (r->p >= r->end) { r->bad = 1; return 0; }
    return *r->p++;
}
static unsigned r16(Rd * r) { unsigned h = r8(r); return (h << 8) | r8(r); }
static uint32_t r32(Rd * r) { uint32_t h = r16(r); return (h << 16) | r16(r); }
static int rs16(Rd * r) { unsigned v = r16(r); return (v & 0x8000) ? (int) v - 0x10000 : (int) v; }
static void rstr(Rd * r, char * out, size_t cap)
{
    unsigned n = r16(r), i;
    for (i = 0; i < n; i++) {
	unsigned c = r8(r);
	if (i + 1 < cap) out[i] = (char) c;
    }
    out[n < cap ? n : cap - 1] = '\0';
}

typedef struct {
    int got;
    unsigned ident, type;
    uint8_t * payload;
    size_t size;
} Reply;

static Display * dpy;
static Widget shell;
static Atom a_comm, a_proto, a_clientval, a_command, a_editres;

static void free_reply(Reply * r) { free(r->payload); memset(r, 0, sizeof(*r)); }

/* Fetches the reply the handler currently offers as the selection value. */
static void fetch_reply(Reply * r)
{
    Atom sel = a_comm, target = a_clientval, type = 0;
    XtPointer value = NULL;
    unsigned long len = 0;
    int format = 0;

    memset(r, 0, sizeof(*r));
    if (!ConvertReturnCommand(shell, &sel, &target, &type, &value, &len, &format))
	return;
    CHECK(type == a_proto && format == 8, "reply type/format");
    CHECK(len >= HEADER_SIZE, "reply shorter than its header");
    if (len >= HEADER_SIZE) {
	const uint8_t * v = value;
	uint32_t size = ((uint32_t) v[2] << 24) | (v[3] << 16) | (v[4] << 8) | v[5];
	r->got = 1;
	r->ident = v[0];
	r->type = v[1];
	r->size = size;
	CHECK(size == len - HEADER_SIZE, "reply length field %u != payload %lu",
	      (unsigned) size, (unsigned long) (len - HEADER_SIZE));
	r->payload = malloc(len - HEADER_SIZE + 1);
	memcpy(r->payload, v + HEADER_SIZE, len - HEADER_SIZE);
    }
#ifndef EDITRES_PREFIX_CONTROL
    XtFree(value);
#endif
}

/*
 * Hands a request to the handler.  The block holding it is exactly `length'
 * bytes long when `slack' is 0.  With slack, `slack' further bytes follow it
 * in the same block; the handler has been told they were not received.
 */
static void send_request(Buf * req, size_t length, size_t slack, unsigned ident, Reply * r)
{
    uint8_t * block = (uint8_t *) XtMalloc((Cardinal) (length + slack + 1));
    Atom sel = a_comm, type = a_proto;
    unsigned long len = length;
    int format = 8;

    memcpy(block, req->p, length + slack <= req->n ? length + slack : req->n);
    /* The selection value belongs to the handler from here on. */
    GetCommand(shell, (XtPointer) (uintptr_t) ident, &sel, &type, block, &len, &format);
#ifdef EDITRES_PREFIX_CONTROL
    XtFree((char *) block);
#endif
    fetch_reply(r);
}

/* Wraps a body in a request header. */
static void make_request(Buf * out, unsigned ident, unsigned command, const Buf * body, uint32_t declared)
{
    memset(out, 0, sizeof(*out));
    put8(out, ident);
    put8(out, command);
    put32(out, declared);
    if (body != NULL) {
	size_t i;
	for (i = 0; i < body->n; i++) put8(out, body->p[i]);
    }
}

static void request(unsigned ident, unsigned command, Buf * body, Reply * r)
{
    Buf req;
    make_request(&req, ident, command, body, (uint32_t) (body ? body->n : 0));
    send_request(&req, req.n, 0, ident, r);
    free(req.p);
}

static int is_failure(const Reply * r, const char * contains)
{
    Rd rd = { r->payload, r->payload + r->size, 0 };
    char msg[256];
    if (!r->got || r->type != Failure) return 0;
    rstr(&rd, msg, sizeof(msg));
    return contains == NULL || strstr(msg, contains) != NULL;
}

/* ------------------------------------------------------------------ */
/* The widget tree under test                                          */
/* ------------------------------------------------------------------ */

static Widget box, child0, child1, keeper, rect, popup, popup_child;

static Widget make_core(const char * name, Widget parent, int x, int y, int w, int h)
{
    return XtVaCreateManagedWidget(name, coreWidgetClass, parent,
				   XtNx, x, XtNy, y, XtNwidth, w, XtNheight, h,
				   XtNborderWidth, 0, NULL);
}

static void build_tree(XtAppContext * app)
{
    int argc = 1;
    char * argv[] = { "editres-test", NULL };

    shell = XtVaAppInitialize(app, "EditresTest", NULL, 0, &argc, argv, NULL,
			      XtNwidth, 400, XtNheight, 300, NULL);
    box = XtVaCreateManagedWidget("box", holderWidgetClass, shell,
				  XtNx, 50, XtNy, 50, XtNwidth, 100,
				  XtNheight, 100, NULL);
    child0 = make_core("child0", box, 0, 0, 10, 10);
    child1 = make_core("child1", box, 40, 40, 30, 30);
    keeper = XtVaCreateManagedWidget("keeper", keeperWidgetClass, box,
				     XtNx, 80, XtNy, 0, XtNwidth, 5,
				     XtNheight, 5, XtNborderWidth, 0, NULL);
    rect = XtVaCreateManagedWidget("rect", rectObjClass, box,
				   XtNx, 10, XtNy, 60, XtNwidth, 25,
				   XtNheight, 15, XtNborderWidth, 0, NULL);
    popup = XtVaCreatePopupShell("popup", transientShellWidgetClass, shell, NULL);
    popup_child = XtVaCreateManagedWidget("popchild", holderWidgetClass,
					  popup, XtNwidth, 20, XtNheight, 20, NULL);
    dpy = XtDisplay(shell);
    a_comm = XInternAtom(dpy, "EditresComm", False);
    a_proto = XInternAtom(dpy, "EditresProtocol", False);
    a_clientval = XInternAtom(dpy, "EditresClientVal", False);
    a_command = XInternAtom(dpy, "EditresCommand", False);
    a_editres = XInternAtom(dpy, "Editres", False);
}

/* Lets the handler initialise itself, the way the first client message does. */
static void prime_handler(void)
{
    XClientMessageEvent ev;
    Boolean cont = True;
    Reply r;

    memset(&ev, 0, sizeof(ev));
    ev.type = ClientMessage;
    ev.display = dpy;
    ev.window = 0;
    ev.message_type = a_editres;
    ev.format = 32;
    ev.data.l[0] = CurrentTime;
    ev.data.l[1] = (long) a_comm;
    ev.data.l[2] = 1;
    ev.data.l[3] = 4;		/* not the protocol version in use */
    _XmEditResCheckMessages(shell, NULL, (XEvent *) &ev, &cont);
    fetch_reply(&r);
    CHECK(r.got && r.type == ProtocolMismatch && r.size == 1 &&
	  r.payload[0] == 5, "protocol mismatch reply");
    free_reply(&r);
}

/* ------------------------------------------------------------------ */
/* Reading reply contents                                              */
/* ------------------------------------------------------------------ */

static Widget path_widget(Rd * r)
{
    unsigned n = r16(r), i;
    uint32_t last = 0;
    Widget found = NULL;
    for (i = 0; i < n; i++) last = r32(r);
    if (last == (uint32_t) ((uintptr_t) box & 0xFFFFFFFFu)) found = box;
    else if (last == (uint32_t) ((uintptr_t) child0 & 0xFFFFFFFFu)) found = child0;
    else if (last == (uint32_t) ((uintptr_t) child1 & 0xFFFFFFFFu)) found = child1;
    else if (last == (uint32_t) ((uintptr_t) shell & 0xFFFFFFFFu)) found = shell;
    else if (last == (uint32_t) ((uintptr_t) keeper & 0xFFFFFFFFu)) found = keeper;
    else if (last == (uint32_t) ((uintptr_t) rect & 0xFFFFFFFFu)) found = rect;
    return found;
}

/* ------------------------------------------------------------------ */
/* Cases                                                               */
/* ------------------------------------------------------------------ */

static void test_widget_tree(void)
{
    Buf body;
    Reply r;
    Rd rd;
    unsigned count, i;
    uint32_t window;
    int seen_popchild = 0, seen_child1 = 0, seen_rect = 0;
    char name[64], cls[64], toolkit[16];

    (void) body;
    request(7, SendWidgetTree, NULL, &r);
    CHECK(r.got && r.ident == 7 && r.type == PartialSuccess, "widget tree reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    count = r16(&rd);
    CHECK(count == 8, "widget tree has %u entries, expected 8", count);
    for (i = 0; i < count; i++) {
	unsigned n = r16(&rd), k;
	for (k = 0; k < n; k++) (void) r32(&rd);
	rstr(&rd, name, sizeof(name));
	rstr(&rd, cls, sizeof(cls));
	window = r32(&rd);
	if (strcmp(name, "popchild") == 0) seen_popchild = (n == 3);
	if (strcmp(name, "child1") == 0)
	    seen_child1 = (n == 3 && strcmp(cls, "Core") == 0 && window != 0 && window != 2);
	if (strcmp(name, "rect") == 0) seen_rect = (n == 3 && window == 2);
    }
    rstr(&rd, toolkit, sizeof(toolkit));
    CHECK(!rd.bad && rd.p == rd.end && strcmp(toolkit, "Xt") == 0,
	  "widget tree trailer");
    CHECK(seen_popchild && seen_child1 && seen_rect,
	  "popup child (%d), child1 (%d) and rect (%d) entries",
	  seen_popchild, seen_child1, seen_rect);
    free_reply(&r);
}

/* Widget addresses are 64 bits wide here; the lookup must not depend on
 * their upper half. */
static void test_resources_and_geometry(void)
{
    Buf body;
    Reply r;
    Rd rd;
    unsigned entries, res;

    memset(&body, 0, sizeof(body));
    put16(&body, 3);
    putpath(&body, shell);
    putpath(&body, child1);
    putpath(&body, popup_child);
    request(9, GetResources, &body, &r);
    CHECK(r.got && r.type == PartialSuccess, "get resources reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    entries = r16(&rd);
    CHECK(entries == 3, "get resources entry count %u", entries);
    {
	int e;
	for (e = 0; e < 3; e++) {
	    unsigned n = r16(&rd), k, errflag;
	    for (k = 0; k < n; k++) (void) r32(&rd);
	    errflag = r8(&rd);
	    CHECK(errflag == 0, "entry %d reported an error (widget lookup)", e);
	    if (errflag) { char m[128]; rstr(&rd, m, sizeof(m)); break; }
	    res = r16(&rd);
	    CHECK(res > 5 && res < 200, "entry %d lists %u resources", e, res);
	    for (k = 0; k < res; k++) {
		char s[96];
		(void) r8(&rd);
		rstr(&rd, s, sizeof(s)); rstr(&rd, s, sizeof(s)); rstr(&rd, s, sizeof(s));
	    }
	}
    }
    CHECK(!rd.bad && rd.p == rd.end, "get resources reply fully consumed");
    free_reply(&r);
    free(body.p);

    /* Geometry of a rectangle (not a windowed widget), of a mapped widget,
     * of a widget that is not mapped and of one that is not realized. */
    memset(&body, 0, sizeof(body));
    put16(&body, 4);
    putpath(&body, rect);
    putpath(&body, child1);
    putpath(&body, child0);
    putpath(&body, popup_child);
    XtSetMappedWhenManaged(child0, False);
    XSync(dpy, False);
    request(10, GetGeometry, &body, &r);
    XtSetMappedWhenManaged(child0, True);
    CHECK(r.got && r.type == PartialSuccess, "get geometry reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    CHECK(r16(&rd) == 4, "geometry entry count");
    {
	static const struct { int visible; int w, h; Widget widget; } want[] = {
	    { 1, 25, 15, NULL }, { 1, 30, 30, NULL }, { 0, 0, 0, NULL }, { 0, 0, 0, NULL }
	};
	Widget order[4];
	int e, k;
	order[0] = rect; order[1] = child1; order[2] = child0; order[3] = popup_child;
	for (e = 0; e < 4; e++) {
	    unsigned n = r16(&rd);
	    Position ex, ey;
	    for (k = 0; k < (int) n; k++) (void) r32(&rd);
	    CHECK(r8(&rd) == 0, "geometry %d error flag", e);
	    CHECK((int) r8(&rd) == want[e].visible, "geometry %d visibility", e);
	    if (want[e].visible) {
		XtTranslateCoords(order[e], 0, 0, &ex, &ey);
		CHECK(rs16(&rd) == ex && rs16(&rd) == ey, "geometry %d x/y", e);
		CHECK((int) r16(&rd) == want[e].w && (int) r16(&rd) == want[e].h &&
		      r16(&rd) == 0, "geometry %d size", e);
	    } else {
		for (k = 0; k < 5; k++) CHECK(r16(&rd) == 0, "geometry %d zero", e);
	    }
	}
    }
    CHECK(!rd.bad && rd.p == rd.end, "get geometry reply fully consumed");
    free_reply(&r);
    free(body.p);
}

/* FindChild coordinates are signed. */
static void test_find_child_signed(void)
{
    static const struct { int x, y; int absolute; int expect; } pts[] = {
	{ 5, 5, 0, 1 },		/* inside child0 */
	{ -5, -5, 0, 0 },	/* above and left of every child */
	{ -1, -1, 0, 0 },
	{ -1, 5, 0, 0 },
	{ 5, -1, 0, 0 },
	{ -32768, -32768, 1, 0 },
	{ 32767, 32767, 1, 0 },
	{ 50, 50, 0, 2 },	/* inside child1 (40..70) */
    };
    size_t i;
    Position bx, by;

    XtTranslateCoords(box, 0, 0, &bx, &by);
    for (i = 0; i < sizeof(pts) / sizeof(pts[0]); i++) {
	Buf body;
	Reply r;
	Rd rd;
	Widget found;
	int x = pts[i].absolute ? pts[i].x : bx + pts[i].x;
	int y = pts[i].absolute ? pts[i].y : by + pts[i].y;

	memset(&body, 0, sizeof(body));
	putpath(&body, box);
	put16(&body, (unsigned) x & 0xFFFF);
	put16(&body, (unsigned) y & 0xFFFF);
	request(11, FindChild, &body, &r);
	CHECK(r.got && r.type == PartialSuccess, "find child reply (%d,%d)",
	      pts[i].x, pts[i].y);
	rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
	found = path_widget(&rd);
	if (pts[i].expect == 0)
	    CHECK(found == box, "point (%d,%d) must name the box, got %p",
		  pts[i].x, pts[i].y, (void *) found);
	else if (pts[i].expect == 1)
	    CHECK(found == child0, "point (%d,%d) must name child0", pts[i].x, pts[i].y);
	else
	    CHECK(found == child1, "point (%d,%d) must name child1", pts[i].x, pts[i].y);
	free_reply(&r);
	free(body.p);
    }
}

/* A request may not claim more data than was received. */
static void test_request_bounds(void)
{
    Buf body, req;
    Reply r;

    /* GetResources, one entry, whose widget id is cut in the middle.  The
     * missing byte is physically present after the received data. */
    memset(&body, 0, sizeof(body));
    put16(&body, 1);
    putpath(&body, shell);	/* 2 + 4 bytes */
    make_request(&req, 21, GetResources, &body, 0xFFFFFFF0u);
    send_request(&req, req.n - 1, 1, 21, &r);
    CHECK(is_failure(&r, "Improperly formatted"),
	  "request cut inside a field with a huge declared length");
    free_reply(&r);
    free(req.p);

    /* The declared length is exact but the data ends one byte early. */
    make_request(&req, 22, GetResources, &body, (uint32_t) body.n);
    send_request(&req, req.n - 1, 1, 22, &r);
    CHECK(is_failure(&r, "Improperly formatted"),
	  "declared length larger than the received data");
    free_reply(&r);
    free(req.p);

    /* The declared length is smaller than the fields it must hold. */
    make_request(&req, 23, GetResources, &body, (uint32_t) body.n - 1);
    send_request(&req, req.n, 0, 23, &r);
    CHECK(is_failure(&r, "Improperly formatted"),
	  "fields extend past the declared length");
    free_reply(&r);
    free(req.p);

    /* A header alone, and less than a header. */
    memset(&req, 0, sizeof(req));
    put8(&req, 24); put8(&req, GetResources); put32(&req, 4);
    send_request(&req, req.n, 0, 24, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "header declaring missing data");
    free_reply(&r);
    send_request(&req, 3, 0, 24, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "request shorter than a header");
    free_reply(&r);
    free(req.p);
    free(body.p);

    /* A request for another identifier is ignored without an answer. */
    memset(&body, 0, sizeof(body));
    put16(&body, 0);
    make_request(&req, 25, GetResources, &body, 2);
    {
	Reply before, after;
	uint8_t * block = (uint8_t *) XtMalloc((Cardinal) req.n);
	Atom sel = a_comm, type = a_proto;
	unsigned long len = req.n;
	int format = 8;
	fetch_reply(&before);
	memcpy(block, req.p, req.n);
	GetCommand(shell, (XtPointer) (uintptr_t) 26, &sel, &type, block, &len, &format);
#ifdef EDITRES_PREFIX_CONTROL
	XtFree((char *) block);
#endif
	fetch_reply(&after);
	CHECK(before.got && after.got && before.ident == after.ident &&
	      before.size == after.size, "mismatched identifier must not be answered");
	free_reply(&before);
	free_reply(&after);
    }
    free(req.p);
    free(body.p);
}

/* Counts that cannot be satisfied are refused without trusting them. */
static void test_counts(void)
{
    Buf body;
    Reply r;
    unsigned i;

    /* 65535 entries announced, none present. */
    memset(&body, 0, sizeof(body));
    put16(&body, 65535);
    request(31, GetResources, &body, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "65535 announced entries");
    free_reply(&r);
    free(body.p);

    /* A path with no widgets, and a path with more ids than bytes. */
    memset(&body, 0, sizeof(body));
    put16(&body, 1);
    put16(&body, 0);
    request(32, GetResources, &body, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "empty widget path");
    free_reply(&r);
    free(body.p);

    memset(&body, 0, sizeof(body));
    put16(&body, 1);
    put16(&body, 65535);
    put32(&body, 1);
    request(33, GetResources, &body, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "path longer than its data");
    free_reply(&r);
    free(body.p);

    /* Several entries followed by garbage in the last one: nothing leaks
     * (checked by LeakSanitizer when the program exits). */
    memset(&body, 0, sizeof(body));
    put16(&body, 40);
    for (i = 0; i < 39; i++) putpath(&body, child0);
    put16(&body, 3);
    put32(&body, 5);
    request(34, GetResources, &body, &r);
    CHECK(is_failure(&r, "Improperly formatted"), "truncated last entry");
    free_reply(&r);
    free(body.p);

    /* Many valid entries are all answered (and all released). */
    memset(&body, 0, sizeof(body));
    put16(&body, 300);
    for (i = 0; i < 300; i++) putpath(&body, i % 2 ? child0 : child1);
    request(35, GetGeometry, &body, &r);
    CHECK(r.got && r.type == PartialSuccess, "300 geometry entries");
    free_reply(&r);
    free(body.p);

    /* An unknown command. */
    request(36, 77, NULL, &r);
    CHECK(is_failure(&r, "Unknown Protocol request 77"), "unknown command");
    free_reply(&r);
}

/* A widget id that names nothing is reported, not followed. */
static void test_missing_widgets(void)
{
    Buf body;
    Reply r;
    Rd rd;

    memset(&body, 0, sizeof(body));
    put16(&body, 3);
    put16(&body, 2);		/* right root, wrong child */
    put32((&body), (uint32_t) ((uintptr_t) shell & 0xFFFFFFFFu));
    put32(&body, 0x12345678u);
    put16(&body, 1);		/* a forged root */
    put32(&body, 0xDEADBEEFu);
    putpath(&body, child0);	/* a good one after the bad ones */
    request(41, GetResources, &body, &r);
    CHECK(r.got && r.type == PartialSuccess, "missing widgets reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    CHECK(r16(&rd) == 3, "missing widgets count");
    {
	int e;
	for (e = 0; e < 3; e++) {
	    unsigned n = r16(&rd), k, flag;
	    char m[128];
	    for (k = 0; k < n; k++) (void) r32(&rd);
	    flag = r8(&rd);
	    if (e < 2) {
		CHECK(flag == 1, "entry %d must be an error", e);
		rstr(&rd, m, sizeof(m));
		CHECK(strstr(m, "no longer exists") != NULL, "entry %d message", e);
	    } else {
		unsigned res, q;
		CHECK(flag == 0, "good entry after bad ones");
		res = r16(&rd);
		for (q = 0; q < res; q++) {
		    char s[96];
		    (void) r8(&rd);
		    rstr(&rd, s, sizeof(s)); rstr(&rd, s, sizeof(s)); rstr(&rd, s, sizeof(s));
		}
	    }
	}
    }
    CHECK(!rd.bad && rd.p == rd.end, "missing widgets reply consumed");
    free_reply(&r);
    free(body.p);

    /* FindChild and GetValues with a forged path fail cleanly. */
    memset(&body, 0, sizeof(body));
    put16(&body, 1);
    put32(&body, 0xCAFEBABEu);
    put16(&body, 0);
    put16(&body, 0);
    request(42, FindChild, &body, &r);
    CHECK(is_failure(&r, "no longer exists"), "FindChild on a forged widget");
    free_reply(&r);
    free(body.p);
}

static String keeper_label(void) { return ((KeeperRec *) keeper)->label; }

static void test_set_values(void)
{
    Buf body;
    Reply r;
    Rd rd;
    char msg[256];
    String before;
    unsigned n, k;

    /* A String resource keeps the pointer it was given; it must stay valid
     * after the request is gone. */
    memset(&body, 0, sizeof(body));
    putstr(&body, "label");
    putstr(&body, "String");
    putstr(&body, "kept value");
    put16(&body, 1);
    putpath(&body, keeper);
    request(51, SetValues, &body, &r);
    CHECK(r.got && r.type == PartialSuccess, "set values reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    CHECK(r16(&rd) == 0, "set values on a String resource reported errors");
    free_reply(&r);
    free(body.p);
    CHECK(keeper_label() != NULL && strcmp(keeper_label(), "kept value") == 0,
	  "String value must stay valid after the request");
    before = keeper_label();

    /* Conversion failures are reported with the offending string. */
    memset(&body, 0, sizeof(body));
    putstr(&body, "width");
    putstr(&body, "String");
    putstr(&body, "abc%s%n");
    put16(&body, 1);
    putpath(&body, child0);
    request(52, SetValues, &body, &r);
    CHECK(r.got && r.type == PartialSuccess, "set values conversion reply");
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    n = r16(&rd);
    CHECK(n >= 1, "conversion failure not reported");
    for (k = 0; k < n; k++) {
	unsigned ids = r16(&rd), q;
	for (q = 0; q < ids; q++) (void) r32(&rd);
	rstr(&rd, msg, sizeof(msg));
	if (k == 0)
	    CHECK(strstr(msg, "abc%s%n") != NULL && strstr(msg, "`width'") != NULL,
		  "conversion message: %s", msg);
    }
    CHECK(!rd.bad, "conversion reply parse");
    free_reply(&r);
    free(body.p);

    /* An unknown resource name. */
    memset(&body, 0, sizeof(body));
    putstr(&body, "nosuchresource");
    putstr(&body, "String");
    putstr(&body, "1");
    put16(&body, 1);
    putpath(&body, child0);
    request(53, SetValues, &body, &r);
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    CHECK(r.got && r16(&rd) == 1, "unknown resource count");
    {
	unsigned ids = r16(&rd), q;
	for (q = 0; q < ids; q++) (void) r32(&rd);
	rstr(&rd, msg, sizeof(msg));
	CHECK(strstr(msg, "`nosuchresource' resource is not used") != NULL,
	      "unknown resource message: %s", msg);
    }
    free_reply(&r);
    free(body.p);

    /* A value that is a valid Dimension is applied. */
    memset(&body, 0, sizeof(body));
    putstr(&body, "width");
    putstr(&body, "String");
    putstr(&body, "17");
    put16(&body, 1);
    putpath(&body, child0);
    request(54, SetValues, &body, &r);
    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
    CHECK(r.got && r16(&rd) == 0, "valid value reported an error");
    CHECK(child0->core.width == 17, "width must be 17, is %u", (unsigned) child0->core.width);
    free_reply(&r);
    free(body.p);

    /* Only String values can travel in this protocol: other types would be
     * read from the request's own memory. */
    memset(&body, 0, sizeof(body));
    putstr(&body, "width");
    putstr(&body, "Dimension");
    putstr(&body, "\x01\x02");
    put16(&body, 1);
    putpath(&body, child0);
    request(55, SetValues, &body, &r);
    CHECK(is_failure(&r, "String"), "non-String SetValues must be refused");
    CHECK(child0->core.width == 17, "refused SetValues must not change the widget");
    free_reply(&r);
    free(body.p);

    /* The retained strings are bounded. */
    {
	int refused = 0, i;
	char big[4096];
	memset(big, 'x', sizeof(big) - 1);
	big[sizeof(big) - 1] = '\0';
	for (i = 0; i < 400 && !refused; i++) {
	    memset(&body, 0, sizeof(body));
	    putstr(&body, "label");
	    putstr(&body, "String");
	    putstr(&body, big);
	    put16(&body, 1);
	    putpath(&body, keeper);
	    request(56, SetValues, &body, &r);
	    rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
	    if (r.got && r.type == PartialSuccess && r16(&rd) == 1) refused = 1;
	    free_reply(&r);
	    free(body.p);
	}
	CHECK(refused, "retained String values must be limited");
	CHECK(before != NULL && strcmp(before, "kept value") == 0,
	      "earlier retained value must stay valid");
    }
}

static void get_value(unsigned ident, Widget w, const char * resource, char * out, size_t cap)
{
    Buf body;
    Reply r;
    Rd rd;

    memset(&body, 0, sizeof(body));
    putstr(&body, resource);
    put16(&body, 1);
    putpath(&body, w);
    request(ident, GetValues, &body, &r);
    out[0] = '\0';
    if (r.got && r.type == PartialSuccess) {
	rd.p = r.payload; rd.end = r.payload + r.size; rd.bad = 0;
	CHECK(r16(&rd) == 1, "get values count");
	rstr(&rd, out, cap);
	CHECK(!rd.bad && rd.p == rd.end, "get values reply consumed");
    } else {
	CHECK(0, "get values reply for %s", resource);
    }
    free_reply(&r);
    free(body.p);
}

static void test_get_values(void)
{
    char v[256];

    get_value(61, child1, "width", v, sizeof(v));
    CHECK(strcmp(v, "30") == 0 || strncmp(v, "30", 2) == 0, "width value '%s'", v);
    get_value(62, child1, "x", v, sizeof(v));
    CHECK(strncmp(v, "40", 2) == 0, "x value '%s'", v);
    get_value(63, child1, "mappedWhenManaged", v, sizeof(v));
    CHECK(v[0] != '\0', "mappedWhenManaged value '%s'", v);
    get_value(64, keeper, "label", v, sizeof(v));
    CHECK(strlen(v) > 0, "label value '%s'", v);
    get_value(65, child1, "background", v, sizeof(v));
    CHECK(v[0] != '\0', "background value '%s'", v);
    get_value(66, child1, "nonexistent", v, sizeof(v));
    CHECK(strcmp(v, "NoValue") == 0, "unknown resource '%s'", v);
    /* a constraint-free parent, and a request for the shell itself */
    get_value(67, shell, "title", v, sizeof(v));
    CHECK(v[0] != '\0', "title value '%s'", v);
}

static void test_block(void)
{
    Buf body;
    Reply r;
    EditresBlock saved = globals.block;

    memset(&body, 0, sizeof(body));
    putstr(&body, "width");
    putstr(&body, "String");
    putstr(&body, "20");
    put16(&body, 1);
    putpath(&body, child0);
    globals.block = BlockSetValues;
    request(71, SetValues, &body, &r);
    CHECK(is_failure(&r, "blocked all SetValues"), "SetValues blocked");
    free_reply(&r);
    CHECK(child0->core.width == 17, "blocked SetValues changed the widget");
    globals.block = BlockAll;
    request(72, SendWidgetTree, NULL, &r);
    CHECK(is_failure(&r, "blocked all Editres"), "everything blocked");
    free_reply(&r);
    globals.block = saved;
    free(body.p);
}

#ifndef EDITRES_PREFIX_CONTROL
/* Internals added with the rewrite. */
static void test_internals(void)
{
    static const unsigned char two[] = { 0xFF, 0xFB, 0x7F, 0xFF, 0x80, 0x00, 0x00, 0x00 };
    WireReader rd;
    short s;
    char * text;
    String params[2] = { "one", "two" };
    ReplyStream st;
    size_t i;

    rd.cur = two; rd.end = two + sizeof(two);
    CHECK(WireGetSigned16(&rd, &s) && s == -5, "signed16 -5");
    CHECK(WireGetSigned16(&rd, &s) && s == 32767, "signed16 32767");
    CHECK(WireGetSigned16(&rd, &s) && s == -32768, "signed16 -32768");
    CHECK(WireGetSigned16(&rd, &s) && s == 0, "signed16 0");
    CHECK(!WireGetSigned16(&rd, &s), "signed16 at the end of the data");

    text = ExpandToolkitMessage("a %s b %s c %s d %n %d %%", params, 2);
    CHECK(text != NULL && strcmp(text, "a one b two c  d %n %d %") == 0,
	  "message expansion: '%s'", text ? text : "(null)");
    free(text);
    text = ExpandToolkitMessage("%s%s", NULL, 0);
    CHECK(text != NULL && text[0] == '\0', "expansion without parameters");
    free(text);

    memset(&st, 0, sizeof(st));
    for (i = 0; i < EDITRES_MAX_REPLY + 10; i++)
	ReplyPut8(&st, 1);
    CHECK(st.failed && st.size == EDITRES_MAX_REPLY, "reply limit: size %lu failed %d",
	  (unsigned long) st.size, st.failed);
    ReplyReset(&st);
    CHECK(!st.failed && st.size == 0 && st.buf == NULL, "reply reset releases a large buffer");

    /* A reply that no longer fits is replaced by a failure message. */
    {
	Reply r;
	BeginReply();
	globals.stream.failed = TRUE;
	SendCommand(shell, a_comm, 81, PartialSuccess);
	fetch_reply(&r);
	CHECK(is_failure(&r, "too large"), "oversized reply becomes a failure");
	CHECK(r.ident == 81, "failure keeps the request identifier");
	free_reply(&r);
    }
}
#endif

/* ------------------------------------------------------------------ */
/* The whole path, with a second connection acting as the editor       */
/* ------------------------------------------------------------------ */

static Display * cdpy;
static Window cwin;
static Buf pending_command;
static int served_command, got_clear;
static Atom a_primary;
static XSelectionEvent last_notify;
static int have_notify;

static void client_handle(XEvent * e)
{
    if (e->type == SelectionRequest) {
	XSelectionRequestEvent * q = &e->xselectionrequest;
	XSelectionEvent n;
	memset(&n, 0, sizeof(n));
	n.type = SelectionNotify;
	n.display = q->display;
	n.requestor = q->requestor;
	n.selection = q->selection;
	n.target = q->target;
	n.time = q->time;
	n.property = None;
	if (q->target == a_command && pending_command.n > 0) {
	    XChangeProperty(cdpy, q->requestor, q->property, a_proto, 8,
			    PropModeReplace, pending_command.p, (int) pending_command.n);
	    n.property = q->property;
	    served_command++;
	}
	XSendEvent(cdpy, q->requestor, False, NoEventMask, (XEvent *) &n);
	XFlush(cdpy);
    } else if (e->type == SelectionClear) {
	got_clear++;
    } else if (e->type == SelectionNotify) {
	last_notify = e->xselection;
	have_notify++;
    }
}

static void pump(XtAppContext app, int rounds, int * stop_when)
{
    int i;
    for (i = 0; i < rounds; i++) {
	XSync(cdpy, False);
	while (XPending(cdpy)) {
	    XEvent e;
	    XNextEvent(cdpy, &e);
	    client_handle(&e);
	}
	while (XtAppPending(app))
	    XtAppProcessEvent(app, XtIMAll);
	if (stop_when != NULL && *stop_when) return;
	usleep(5000);
    }
}

static void send_client_message(Atom comm, long version, unsigned ident)
{
    XEvent ev;
    memset(&ev, 0, sizeof(ev));
    ev.xclient.type = ClientMessage;
    ev.xclient.display = cdpy;
    ev.xclient.window = XtWindow(shell);
    ev.xclient.message_type = a_editres;
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = CurrentTime;
    ev.xclient.data.l[1] = (long) comm;
    ev.xclient.data.l[2] = ident;
    ev.xclient.data.l[3] = version;
    XSendEvent(cdpy, XtWindow(shell), False, NoEventMask, &ev);
    XFlush(cdpy);
}

static int read_reply_via_selection(XtAppContext app, Reply * r)
{
    Atom prop = XInternAtom(cdpy, "EDITRES_TEST_REPLY", False);
    Atom type;
    int format;
    unsigned long n, after;
    unsigned char * data = NULL;

    memset(r, 0, sizeof(*r));
    XDeleteProperty(cdpy, cwin, prop);
    have_notify = 0;
    XConvertSelection(cdpy, a_comm, a_clientval, prop, cwin, CurrentTime);
    XFlush(cdpy);
    pump(app, 400, &have_notify);
    if (!have_notify || last_notify.property == None)
	return 0;
    if (XGetWindowProperty(cdpy, cwin, prop, 0, 1 << 20, True, AnyPropertyType,
			   &type, &format, &n, &after, &data) != Success || data == NULL)
	return 0;
    if (type == a_proto && format == 8 && n >= HEADER_SIZE) {
	r->got = 1;
	r->ident = data[0];
	r->type = data[1];
	r->size = ((size_t) data[2] << 24) | (data[3] << 16) | (data[4] << 8) | data[5];
	r->payload = malloc(n);
	memcpy(r->payload, data + HEADER_SIZE, n - HEADER_SIZE);
	XFree(data);
	return 1;
    }
    XFree(data);
    return 0;
}

static void test_end_to_end(XtAppContext app)
{
    Buf body;
    Reply r;
    int ok;

    cwin = XCreateSimpleWindow(cdpy, DefaultRootWindow(cdpy), 0, 0, 1, 1, 0, 0, 0);
    XSelectInput(cdpy, cwin, PropertyChangeMask);

    /* A legitimate exchange: the editor owns the communication selection,
     * announces a command, serves it and reads the answer. */
    memset(&body, 0, sizeof(body));
    put16(&body, 1);
    putpath(&body, child1);
    make_request(&pending_command, 91, GetResources, &body, (uint32_t) body.n);
    free(body.p);
    XSetSelectionOwner(cdpy, a_comm, cwin, CurrentTime);
    served_command = 0;
    send_client_message(a_comm, 5, 91);
    pump(app, 60, &served_command);
    CHECK(served_command == 1, "the application must fetch the command (%d)", served_command);
    pump(app, 20, NULL);
    ok = read_reply_via_selection(app, &r);
    CHECK(ok && r.got && r.ident == 91 && r.type == PartialSuccess,
	  "end-to-end reply (ok=%d type=%u)", ok, r.type);
    free_reply(&r);

    /* A client message that names another selection must not make the
     * application fetch commands from it or take it over. */
    XSetSelectionOwner(cdpy, a_primary, cwin, CurrentTime);
    served_command = 0;
    got_clear = 0;
    send_client_message(a_primary, 5, 91);
    pump(app, 60, NULL);
    CHECK(got_clear == 0, "the application took over PRIMARY");
    CHECK(XGetSelectionOwner(cdpy, a_primary) == cwin, "PRIMARY changed owner");
    CHECK(served_command == 0, "the application asked PRIMARY for a command");

    /* The same with a version mismatch. */
    got_clear = 0;
    send_client_message(a_primary, 3, 92);
    pump(app, 30, NULL);
    CHECK(got_clear == 0 && XGetSelectionOwner(cdpy, a_primary) == cwin,
	  "version mismatch on another selection took it over");
    XSetSelectionOwner(cdpy, a_primary, None, CurrentTime);
    XDestroyWindow(cdpy, cwin);
    XFlush(cdpy);
}

int main(void)
{
    XtAppContext app = NULL;

    build_tree(&app);
    cdpy = XOpenDisplay(NULL);
    if (cdpy == NULL) {
	fprintf(stderr, "cannot open a second connection\n");
	return 2;
    }
    a_primary = XA_PRIMARY;
    XtAddEventHandler(shell, (EventMask) 0, True,
		      (XtEventHandler) _XmEditResCheckMessages, NULL);

    XtRealizeWidget(shell);
    XSync(dpy, False);

    prime_handler();
    test_widget_tree();
    test_resources_and_geometry();
    test_find_child_signed();
    test_request_bounds();
    test_counts();
    test_missing_widgets();
    test_set_values();
    test_get_values();
    test_block();
#ifndef EDITRES_PREFIX_CONTROL
    test_internals();
#endif
    test_end_to_end(app);

    /* The toolkit is not torn down: what it still holds stays reachable, so
     * that LeakSanitizer reports only memory this file lost. */
    XCloseDisplay(cdpy);

    printf("%d checks, %d failures\n", checks, failures);
    fflush(stdout);
    return failures != 0;
}
