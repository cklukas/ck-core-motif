/*
 * A buffered paint must not request a double-buffer back buffer for a window
 * whose attributes the server did not return: its visual is then unknown.
 *
 * The Xdbe calls and XGetWindowAttributes are replaced at link time.  The
 * attribute replacement reports failure and leaves a valid visual in the
 * structure, so a toolkit that ignores the status would find the visual
 * supported and request a back buffer.  Only that request is counted.
 */
#define _GNU_SOURCE
#include <Xm/XmAll.h>
#include <Xm/XmP.h>
#include "PaintI.h"
#include <X11/extensions/Xdbe.h>
#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail_attributes = 0;
static int attribute_replies = 0;
static int back_buffers = 0;
static int paints = 0;

Status XdbeQueryExtension(Display *display, int *major, int *minor)
{
    (void)display;
    *major = 1;
    *minor = 0;
    return 1;
}

XdbeScreenVisualInfo *XdbeGetVisualInfo(Display *display, Drawable *drawables,
                                        int *num_screens)
{
    XdbeScreenVisualInfo *info = malloc(sizeof(*info));
    XdbeVisualInfo *visinfo = malloc(sizeof(*visinfo));

    (void)drawables;
    if (info == NULL || visinfo == NULL) abort();
    visinfo->visual = DefaultVisualOfScreen(DefaultScreenOfDisplay(display))->visualid;
    visinfo->depth = DefaultDepth(display, DefaultScreen(display));
    visinfo->perflevel = 0;
    info->count = 1;
    info->visinfo = visinfo;
    *num_screens = 1;
    return info;
}

void XdbeFreeVisualInfo(XdbeScreenVisualInfo *info)
{
    if (info == NULL) return;
    free(info->visinfo);
    free(info);
}

XdbeBackBuffer XdbeAllocateBackBufferName(Display *display, Window window,
                                         XdbeSwapAction action)
{
    (void)display; (void)window; (void)action;
    ++back_buffers;
    return None;
}

Status XGetWindowAttributes(Display *display, Window window,
                            XWindowAttributes *attributes)
{
    static Status (*real)(Display *, Window, XWindowAttributes *);

    if (real == NULL)
        real = (Status (*)(Display *, Window, XWindowAttributes *))
               dlsym(RTLD_NEXT, "XGetWindowAttributes");
    if (real == NULL) abort();
    if (!fail_attributes)
        return real(display, window, attributes);

    ++attribute_replies;
    memset(attributes, 0, sizeof(*attributes));
    attributes->visual = DefaultVisualOfScreen(DefaultScreenOfDisplay(display));
    return 0;
}

static int unexpected_error(Display *display, XErrorEvent *event)
{
    (void)display;
    fprintf(stderr, "Unexpected X error: %u request %u\n",
            event->error_code, event->request_code);
    abort();
    return 0;
}

static void paint(Widget widget, XEvent *event, Region region)
{
    (void)widget; (void)event; (void)region;
    ++paints;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "PaintWindowAttributes", NULL, 0,
                                     &argc, argv, NULL, NULL);
    Widget widget = XtVaCreateManagedWidget("surface", xmDrawingAreaWidgetClass,
                                            shell, XmNwidth, 96, XmNheight, 64,
                                            NULL);
    XtRealizeWidget(shell);
    XMapRaised(XtDisplay(shell), XtWindow(shell));
    XSync(XtDisplay(shell), False);
    XSetErrorHandler(unexpected_error);

    fail_attributes = 1;
    _XmBufferedPaint(widget, paint, NULL, NULL);
    fail_attributes = 0;
    XSync(XtDisplay(shell), False);

    assert(attribute_replies > 0);
    assert(paints == 1);
    assert(back_buffers == 0);
    printf("PASS buffered paint with unavailable window attributes: "
           "no back buffer requested\n");
    return 0;
}
