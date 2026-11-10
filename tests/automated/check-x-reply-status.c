/*
 * Failed X requests must not leave their output parameters to be believed.
 *
 * The toolkit asks the server for the size of pixmaps, the attributes of
 * windows and the contents of images.  When such a request fails (the
 * resource was freed, the connection is shared with code that installed an
 * error handler) the call reports it by its return value and does not write
 * its outputs.  This program replaces XGetGeometry, XGetWindowAttributes and
 * XGetImage with versions that fail on request and, when they fail, fill the
 * output parameters with values that would do damage if they were used, and
 * then checks that the toolkit ignored them.
 *
 *   check-x-reply-status CASE
 *
 * Each case runs in its own process because some of the paths are taken only
 * the first time a widget class is used.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBP.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeBGP.h>
#include <Xm/DataF.h>
#include <Xm/DragCP.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

#include <Xm/IconButton.h>
#include <Xm/DragDrop.h>

extern Boolean _XmIsViewable(Widget);
extern Boolean _XmGetDragReceiverInfo(Display *, Window, XmDragReceiverInfoStruct *);
extern unsigned char _XmByteOrderChar;

/* ------------------------------------------------------------------ */
/* Failing X requests                                                  */
/* ------------------------------------------------------------------ */

static int fail_geometry, fail_attributes, fail_image;
static int geometry_failures, attribute_failures, image_failures;
static int select_input_calls;
static Window select_input_window;
static long select_input_mask;
static int watch_select_input;

Status XGetGeometry(Display * d, Drawable dr, Window * root, int * x, int * y,
		    unsigned int * w, unsigned int * h, unsigned int * bw,
		    unsigned int * depth)
{
    static Status (*real)(Display *, Drawable, Window *, int *, int *,
			  unsigned int *, unsigned int *, unsigned int *,
			  unsigned int *);
    if (fail_geometry) {
	geometry_failures++;
	/* what a caller must not believe */
	if (root) *root = 0x1;
	if (x) *x = -4000;
	if (y) *y = -4000;
	if (w) *w = 40000;
	if (h) *h = 40000;
	if (bw) *bw = 77;
	if (depth) *depth = 33;
	return 0;
    }
    if (!real) real = dlsym(RTLD_NEXT, "XGetGeometry");
    return real(d, dr, root, x, y, w, h, bw, depth);
}

Status XGetWindowAttributes(Display * d, Window w, XWindowAttributes * a)
{
    static Status (*real)(Display *, Window, XWindowAttributes *);
    if (fail_attributes) {
	attribute_failures++;
	memset(a, 0, sizeof(*a));
	a->map_state = IsViewable;
	a->x = a->y = 0;
	a->width = a->height = 30000;
	a->your_event_mask = 0x0F000000L;
	return 0;
    }
    if (!real) real = dlsym(RTLD_NEXT, "XGetWindowAttributes");
    return real(d, w, a);
}

XImage * XGetImage(Display * d, Drawable dr, int x, int y, unsigned int w,
		   unsigned int h, unsigned long mask, int format)
{
    static XImage * (*real)(Display *, Drawable, int, int, unsigned int,
			    unsigned int, unsigned long, int);
    if (fail_image) {
	image_failures++;
	return NULL;
    }
    if (!real) real = dlsym(RTLD_NEXT, "XGetImage");
    return real(d, dr, x, y, w, h, mask, format);
}

int XSelectInput(Display * d, Window w, long mask)
{
    static int (*real)(Display *, Window, long);
    if (watch_select_input) {
	select_input_calls++;
	select_input_window = w;
	select_input_mask = mask;
    }
    if (!real) real = dlsym(RTLD_NEXT, "XSelectInput");
    return real(d, w, mask);
}

static int ignore_x_error(Display * d, XErrorEvent * e) { return 0; }

/* ------------------------------------------------------------------ */

static int failures;
#define CHECK(cond, ...) \
    do { \
	if (!(cond)) { \
	    failures++; \
	    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
	    fprintf(stderr, __VA_ARGS__); \
	    fprintf(stderr, "\n"); \
	} \
    } while (0)

static Widget shell;
static Display * dpy;

static void start(void)
{
    int argc = 1;
    char * argv[] = { "x-reply-status", NULL };
    XtAppContext app;

    shell = XtVaAppInitialize(&app, "XReplyStatus", NULL, 0, &argc, argv, NULL,
			      XtNwidth, 300, XtNheight, 300, NULL);
    dpy = XtDisplay(shell);
    XSetErrorHandler(ignore_x_error);
}

static Pixmap make_pixmap(void)
{
    return XCreatePixmap(dpy, DefaultRootWindow(dpy), 16, 16,
			 DefaultDepth(dpy, DefaultScreen(dpy)));
}

static Widget make_pane(void)
{
    Widget row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    return XmCreatePulldownMenu(row, "pane", NULL, 0);
}

static void case_cascade(void)
{
    Widget pane = make_pane();
    Pixmap pm = make_pixmap();
    Widget cb;

    fail_geometry = 1;
    cb = XtVaCreateManagedWidget("cb", xmCascadeButtonWidgetClass, pane,
				 XmNcascadePixmap, pm, NULL);
    fail_geometry = 0;
    CHECK(geometry_failures > 0, "the failing XGetGeometry was not reached");
    CHECK(CB_Cascade_width(cb) == 0 && CB_Cascade_height(cb) == 0,
	  "cascade size after a failed XGetGeometry: %ux%u",
	  (unsigned) CB_Cascade_width(cb), (unsigned) CB_Cascade_height(cb));
}

static void case_cascade_gadget(void)
{
    Widget pane = make_pane();
    Pixmap pm = make_pixmap();
    Widget cb;

    fail_geometry = 1;
    cb = XtVaCreateManagedWidget("cbg", xmCascadeButtonGadgetClass, pane,
				 XmNcascadePixmap, pm, NULL);
    fail_geometry = 0;
    CHECK(geometry_failures > 0, "the failing XGetGeometry was not reached");
    CHECK(CBG_Cascade_width(cb) == 0 && CBG_Cascade_height(cb) == 0,
	  "cascade gadget size after a failed XGetGeometry: %ux%u",
	  (unsigned) CBG_Cascade_width(cb), (unsigned) CBG_Cascade_height(cb));
}

/* The add mode cursor of a data field is made the first time one is realized. */
static void case_datafield(int geometry, int image)
{
    Widget df = XtVaCreateManagedWidget("df", xmDataFieldWidgetClass, shell, NULL);

    fail_geometry = geometry;
    fail_image = image;
    XtRealizeWidget(shell);
    XSync(dpy, False);
    fail_geometry = fail_image = 0;
    CHECK(geometry ? geometry_failures > 0 : image_failures > 0,
	  "the failing request was not reached");
    XmDataFieldSetString(df, "text");
    CHECK(strcmp(XmDataFieldGetString(df), "text") == 0, "data field text");
}

static void case_icon_button(void)
{
    Widget bb = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, shell, NULL);
    Pixmap pm = make_pixmap();
    XmString s = XmStringCreateLocalized("x");
    Widget ib;
    Dimension w = 0, h = 0;

    fail_geometry = 1;
    ib = XtVaCreateManagedWidget("ib", xmIconButtonWidgetClass, bb,
				 XmNlabelString, s, XmNpixmap, pm,
				 XmNiconPlacement, XmIconLeft, NULL);
    fail_geometry = 0;
    CHECK(geometry_failures > 0, "the failing XGetGeometry was not reached");
    XtVaGetValues(ib, XmNwidth, &w, XmNheight, &h, NULL);
    CHECK(w < 2000 && h < 2000, "icon button size after a failed XGetGeometry: %ux%u",
	  (unsigned) w, (unsigned) h);
}

static void case_visibility(void)
{
    Widget bb = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, shell,
					XmNwidth, 300, XmNheight, 300, NULL);
    Widget a = XtVaCreateManagedWidget("a", xmPushButtonWidgetClass, bb,
				       XmNx, 10, XmNy, 10, NULL);
    Widget b = XtVaCreateManagedWidget("b", xmPushButtonWidgetClass, bb,
				       XmNx, 200, XmNy, 200, NULL);
    XmVisibility va, vb;

    XtRealizeWidget(shell);
    XSync(dpy, False);
    va = XmGetVisibility(a);
    vb = XmGetVisibility(b);
    CHECK(va == XmVISIBILITY_UNOBSCURED && vb == XmVISIBILITY_UNOBSCURED,
	  "visibility with siblings elsewhere: %d %d", (int) va, (int) vb);
    /* The attributes of a sibling cannot be read: it covers nothing.  One of
     * the two buttons is above the other in the stacking order. */
    fail_attributes = 1;
    va = XmGetVisibility(a);
    vb = XmGetVisibility(b);
    fail_attributes = 0;
    CHECK(attribute_failures > 0, "the failing XGetWindowAttributes was not reached");
    CHECK(va == XmVISIBILITY_UNOBSCURED && vb == XmVISIBILITY_UNOBSCURED,
	  "visibility after a failed XGetWindowAttributes: %d %d", (int) va, (int) vb);
}

static void case_viewable(void)
{
    Widget bb = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, shell,
					XmNwidth, 100, XmNheight, 100, NULL);
    Widget a = XtVaCreateManagedWidget("a", xmPushButtonWidgetClass, bb,
				       XmNmappedWhenManaged, False, NULL);
    Boolean viewable;

    XtRealizeWidget(shell);
    XSync(dpy, False);
    fail_attributes = 1;
    viewable = _XmIsViewable(a);
    fail_attributes = 0;
    CHECK(attribute_failures > 0, "the failing XGetWindowAttributes was not reached");
    CHECK(!viewable, "a widget whose attributes cannot be read must not be viewable");
}

static void put_receiver_property(Window win)
{
    unsigned char info[16];
    Atom atom = XInternAtom(dpy, "_MOTIF_DRAG_RECEIVER_INFO", False);

    memset(info, 0, sizeof(info));
    info[0] = _XmByteOrderChar;
    info[1] = 0;			/* protocol version */
    info[2] = XmDRAG_DYNAMIC;
    info[12 + (_XmByteOrderChar == 'l' ? 0 : 3)] = 16;	/* heap offset */
    XChangeProperty(dpy, win, atom, atom, 8, PropModeReplace, info, sizeof(info));
    XSync(dpy, False);
}

static void case_receiver_info(void)
{
    Window win;
    XmDragReceiverInfoStruct info;
    Boolean found;

    XtRealizeWidget(shell);
    win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 10, 10, 0, 0, 0);
    put_receiver_property(win);

    memset(&info, 0, sizeof(info));
    found = _XmGetDragReceiverInfo(dpy, win, &info);
    CHECK(found && info.dragProtocolStyle == XmDRAG_DYNAMIC && info.width == 10,
	  "receiver geometry with the server available: %d %u", (int) found, info.width);

    memset(&info, 0, sizeof(info));
    fail_geometry = 1;
    found = _XmGetDragReceiverInfo(dpy, win, &info);
    fail_geometry = 0;
    CHECK(geometry_failures > 0, "the failing XGetGeometry was not reached");
    CHECK(!found && info.dragProtocolStyle == XmDRAG_NONE,
	  "receiver whose geometry cannot be read: found=%d style=%d width=%u",
	  (int) found, (int) info.dragProtocolStyle, info.width);
}

/* Starting a drag adds ButtonMotionMask to the event mask of the root window. */
static void case_drag_start(void)
{
    Widget bb = XtVaCreateManagedWidget("bb", xmBulletinBoardWidgetClass, shell,
					XmNwidth, 100, XmNheight, 100, NULL);
    XEvent ev;
    Widget dc;
    Window root = DefaultRootWindow(dpy);

    XtRealizeWidget(shell);
    XSync(dpy, False);
    memset(&ev, 0, sizeof(ev));
    ev.xbutton.type = ButtonPress;
    ev.xbutton.display = dpy;
    ev.xbutton.window = XtWindow(bb);
    ev.xbutton.root = root;
    ev.xbutton.button = Button1;
    ev.xbutton.time = CurrentTime;
    ev.xbutton.x = ev.xbutton.y = 5;
    ev.xbutton.x_root = ev.xbutton.y_root = 5;

    watch_select_input = 1;
    fail_attributes = 1;
    dc = XmDragStart(bb, &ev, NULL, 0);
    fail_attributes = 0;
    watch_select_input = 0;
    CHECK(attribute_failures > 0, "the failing XGetWindowAttributes was not reached");
    CHECK(dc != NULL, "the drag did not start");
    CHECK(!(select_input_calls > 0 && select_input_window == root &&
	    (select_input_mask & 0x0F000000L)),
	  "the root window event mask was set from attributes that were not read: %lx",
	  select_input_mask);
}

int main(int argc, char ** argv)
{
    if (argc != 2) {
	fprintf(stderr, "usage: %s CASE\n", argv[0]);
	return 2;
    }
    start();
    if (!strcmp(argv[1], "cascade")) case_cascade();
    else if (!strcmp(argv[1], "cascade-gadget")) case_cascade_gadget();
    else if (!strcmp(argv[1], "datafield-geometry")) case_datafield(1, 0);
    else if (!strcmp(argv[1], "datafield-image")) case_datafield(0, 1);
    else if (!strcmp(argv[1], "drag-start")) case_drag_start();
    else if (!strcmp(argv[1], "icon-button")) case_icon_button();
    else if (!strcmp(argv[1], "visibility")) case_visibility();
    else if (!strcmp(argv[1], "viewable")) case_viewable();
    else if (!strcmp(argv[1], "receiver-info")) case_receiver_info();
    else {
	fprintf(stderr, "unknown case %s\n", argv[1]);
	return 2;
    }
    printf("%s: %s\n", argv[1], failures ? "FAILED" : "ok");
    return failures != 0;
}
