/*
 * Drawing a tab stop into a window that is not a widget must not read the
 * screen of the window when the server does not return its attributes.
 *
 * XmStringDraw gives the drawable but no screen.  For a tab stop the toolkit
 * then asks the server for the window's screen.  XGetWindowAttributes is
 * replaced at link time: the replacement reports failure and leaves a value
 * that is not a screen in the screen field.  A toolkit that uses that field
 * would convert the tab position through a bad pointer and crash.
 */
#define _GNU_SOURCE
#include <Xm/XmAll.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fail_attributes = 0;
static int attribute_replies = 0;

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
    attributes->screen = (Screen *)(uintptr_t)0x1;	/* not a screen */
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

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "XmStringTabWindow", NULL, 0,
				     &argc, argv, NULL, NULL);
    Widget surface = XtVaCreateManagedWidget("surface", xmDrawingAreaWidgetClass,
					     shell, XmNwidth, 200, XmNheight, 40,
					     NULL);
    Display *display;
    Window window;
    XmTab tab;
    XmTabList tabs;
    XmRendition rendition;
    XmRenderTable table;
    XmString string;
    Arg args[3];
    GC gc;

    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    /* A top-level window of its own: no widget is registered for it. */
    window = XCreateSimpleWindow(display, RootWindowOfScreen(XtScreen(shell)),
				 0, 0, 200, 40, 0, 0, 0);
    XMapRaised(display, window);
    XSync(display, False);
    XSetErrorHandler(unexpected_error);
    gc = XCreateGC(display, window, 0, NULL);

    /* Inches convert through the screen's resolution; pixels do not. */
    tab = XmTabCreate(0.5, XmINCHES, XmABSOLUTE, XmALIGNMENT_BEGINNING, NULL);
    tabs = XmTabListInsertTabs(NULL, &tab, 1, 0);
    XtSetArg(args[0], XmNtabList, tabs);
    XtSetArg(args[1], XmNfontName, "fixed");
    XtSetArg(args[2], XmNfontType, XmFONT_IS_FONT);
    rendition = XmRenditionCreate(surface, "Tabs", args, 3);
    table = XmRenderTableAddRenditions(NULL, &rendition, 1, XmMERGE_NEW);
    XmRenditionFree(rendition);
    {
	/* "a", a tab component, then "b", as an application builds a tabbed string. */
	XmString first = XmStringCreate("a", "Tabs");
	XmString tab_component = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
	XmString second = XmStringCreate("b", "Tabs");
	XmString head = XmStringConcat(first, tab_component);

	string = XmStringConcat(head, second);
	XmStringFree(first);
	XmStringFree(tab_component);
	XmStringFree(second);
	XmStringFree(head);
    }

    fail_attributes = 1;
    XmStringDraw(display, window, table, string, gc, 4, 4, 190,
		 XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R, NULL);
    fail_attributes = 0;
    XSync(display, False);

    if (attribute_replies == 0) {
	fprintf(stderr, "the tab was drawn without asking for the window attributes\n");
	return 1;
    }
    XmStringDraw(display, window, table, string, gc, 4, 4, 190,
		 XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R, NULL);
    XSync(display, False);

    XmStringFree(string);
    XmRenderTableFree(table);
    XmTabListFree(tabs);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XtDestroyWidget(surface);
    XtDestroyWidget(shell);
    printf("PASS tab stop with unavailable window attributes: no crash\n");
    return 0;
}
