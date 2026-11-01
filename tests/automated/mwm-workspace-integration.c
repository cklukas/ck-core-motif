#define _POSIX_C_SOURCE 200809L
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Transfer.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "wsm_proto.h"
#include "utm_send.h"
static XtAppContext app;
static Widget widget;
static int complete, failure;
static WSMReply last;
static Display *display;
/* Standalone protocol library's timestamp provider. */
Time GetTimestamp(Display *dpy)
{
    Window helper = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0,0,1,1,0,0,0);
    XSelectInput(dpy, helper, PropertyChangeMask);
    Atom atom = XInternAtom(dpy, "WORKSPACE_TEST_TIME", False);
    unsigned char byte = 0;
    XChangeProperty(dpy, helper, atom, atom, 8, PropModeReplace, &byte, 1);
    XEvent event; XWindowEvent(dpy, helper, PropertyChangeMask, &event);
    XDestroyWindow(dpy, helper);
    return event.xproperty.time;
}
static void received(Widget w, XtPointer closure, WSMReply *reply, WSMErrorCode error)
{
    (void)w; (void)closure;
    if (error != WSM_SUCCESS || !reply) { failure = 1; complete = 1; return; }
    last = *reply;
    /* Values needed by assertions are copied before transport frees the reply. */
    if (reply->any.type == WSM_GET_STATE && reply->get_state.num_win_info_list) {
        last.get_state.win_info_list = calloc(reply->get_state.num_win_info_list, sizeof(WSMWinInfo));
        for (int i = 0; i < reply->get_state.num_win_info_list; ++i) {
            WSMWinInfo *to = &last.get_state.win_info_list[i], *from = &reply->get_state.win_info_list[i];
            *to = *from;
            to->data_list = calloc(from->num_data_list, sizeof(WSMWinData));
            memcpy(to->data_list, from->data_list, from->num_data_list * sizeof(WSMWinData));
        }
    }
    complete = 1;
}
static void tick(XtPointer closure, XtIntervalId *id) { (void)closure; (void)id; }
static void pump(unsigned milliseconds)
{
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        XtAppAddTimeOut(app, 10, tick, NULL);
        XtAppProcessEvent(app, XtIMAll);
        clock_gettime(CLOCK_MONOTONIC, &now);
    } while ((now.tv_sec-start.tv_sec)*1000+(now.tv_nsec-start.tv_nsec)/1000000 < (long)milliseconds);
}
static void exchange(WSMRequest *request)
{
    complete = failure = 0;
    if (!WSMSendMessage(widget, WSM_WINDOW_MANAGER, request, received, NULL)) {
        fprintf(stderr,"could not send request %d\n", request->any.type); exit(1);
    }
    for (int i = 0; !complete && i < 500; ++i) pump(10);
    if (!complete || failure) { fprintf(stderr,"request %d failed\n",request->any.type); exit(1); }
}
#define CHECK(value, message) do { if (!(value)) { fprintf(stderr,"FAIL %s\n",message); exit(1); } } while(0)
static long value(Window window, const char *name)
{
    WSMRequest request = {0}; request.get_state.type = WSM_GET_STATE; request.get_state.window = window;
    exchange(&request);
    CHECK(last.get_state.num_win_info_list == 1, "one requested managed window");
    WSMWinInfo *info = last.get_state.win_info_list;
    long result = -999;
    for (int i = 0; i < info->num_data_list; ++i)
        if (info->data_list[i].nameq == XrmStringToQuark(name)) result = info->data_list[i].data.value;
    for (int i = 0; i < last.get_state.num_win_info_list; ++i) free(last.get_state.win_info_list[i].data_list);
    free(last.get_state.win_info_list);
    return result;
}
static void set(Window window, const char *name, long value)
{
    WSMWinData data = {0}; data.nameq = XrmStringToQuark(name); data.type = WSM_VALUE_DATA; data.data.value = value; data.data_len = -1; /* unused for scalars */
    WSMWinInfo info = {window, &data, 1};
    WSMRequest request = {0}; request.set_state.type = WSM_SET_STATE;
    request.set_state.win_info_list = &info; request.set_state.num_win_info_list = 1;
    exchange(&request); pump(100);
}
static int visible(Window window)
{
    XWindowAttributes attrs;
    XSync(display, False);
    CHECK(XGetWindowAttributes(display, window, &attrs), "live managed window");
    return attrs.map_state == IsViewable;
}
int main(int argc, char **argv)
{
    Widget shell = XtVaAppInitialize(&app, "WorkspaceProtocolTest", NULL, 0, &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    widget = XtVaCreateManagedWidget("utm", xmDrawingAreaWidgetClass, shell, XmNwidth, 160, XmNheight, 100, NULL);
    XtAddCallback(widget, XmNdestinationCallback, UTMDestinationProc, NULL);
    XtRealizeWidget(shell); pump(300);
    Window window = XtWindow(shell);
    WSMRequest request = {0}; short versions[] = {3,2,1};
    request.connect.type = WSM_CONNECT; request.connect.known_versions = versions; request.connect.num_versions = 3;
    exchange(&request); CHECK(last.connect.version == 3, "version negotiation");
    request.any.type = WSM_EXTENSIONS; request.extensions.num_extensions = 0; request.extensions.extension_suggestions = NULL;
    exchange(&request); CHECK(last.extensions.num_extensions == 0, "no unimplemented extensions");
    request.any.type = WSM_CONFIG_FMT; exchange(&request);
    CHECK(last.config_format.num_window_formats == 7 && last.config_format.num_icon_formats == 2, "configuration attributes");
    CHECK(value(window,"hidden") == 0 && visible(window), "initial visible state");
    set(window,"hidden",1); CHECK(value(window,"hidden") == 1 && !visible(window), "hide without withdrawal");
    set(window,"x",137); CHECK(value(window,"x") == 137 && !visible(window), "configure while hidden");
    set(window,"hidden",0); CHECK(visible(window), "restore hidden window");
    { Window child; int x,y;
      XTranslateCoordinates(display,window,DefaultRootWindow(display),0,0,&x,&y,&child);
      CHECK(x==137 && y==value(window,"y"),"snapshot matches actual client-root geometry");
    }
    set(window,"width",321); CHECK(value(window,"width") == 321, "workspace geometry");
    set(window,"iconic",1); CHECK(value(window,"iconic") == 1 && !visible(window), "iconify");
    set(window,"hidden",1); CHECK(value(window,"hidden") == 1, "hide iconic window");
    set(window,"hidden",0); set(window,"iconic",0); CHECK(visible(window), "restore iconic window");
    set(window,"maximized",1); CHECK(value(window,"maximized")==1,"maximize state");
    set(window,"hidden",1); CHECK(!visible(window),"hide maximized window");
    set(window,"hidden",0); CHECK(visible(window) && value(window,"maximized")==1,"restore maximized window");
    set(window,"maximized",0); CHECK(value(window,"maximized")==0,"restore normal state");
    request.any.type = WSM_REG_WINDOW; request.register_window.window = window; exchange(&request);
    CHECK(last.register_window.num_window_data == 7, "register window snapshot");
    /* A transient follows its parent's visibility without changing its own workspace flag. */
    Widget transient = XtVaCreatePopupShell("Transient", transientShellWidgetClass, shell,
        XtNwidth, 80, XtNheight, 60, NULL);
    XtVaCreateManagedWidget("child", xmDrawingAreaWidgetClass, transient, NULL);
    XtPopup(transient, XtGrabNone); pump(300);
    CHECK(visible(XtWindow(transient)), "transient visible");
    set(window,"hidden",1); CHECK(!visible(XtWindow(transient)), "transient hidden with parent");
    set(window,"hidden",0); CHECK(visible(XtWindow(transient)), "transient restored with parent");
    set(window,"width",0); CHECK(value(window,"width") == 321, "reject invalid dimensions");
    /* Losing the workspace selection owner must never strand hidden clients. */
    Window owner = XCreateSimpleWindow(display, DefaultRootWindow(display),0,0,1,1,0,0,0);
    Atom selection = _WSMGetSelectionAtom(display, DefaultScreen(display), WSM_WORKSPACE_MANAGER);
    XSetSelectionOwner(display, selection, owner, CurrentTime);
    _WSMClearConfigScreenInfo(display, DefaultScreen(display));
    memset(&request,0,sizeof(request));
    request.connect.type=WSM_CONNECT; request.connect.known_versions=versions; request.connect.num_versions=3;
    exchange(&request);
    request.any.type=WSM_EXTENSIONS; request.extensions.num_extensions=0; request.extensions.extension_suggestions=NULL;
    exchange(&request);
    request.any.type=WSM_CONFIG_FMT; exchange(&request);
    set(window,"hidden",1); CHECK(!visible(window),"owner-tracked hiding");
    XSetSelectionOwner(display,selection,None,CurrentTime); XFlush(display); pump(800);
    CHECK(value(window,"hidden")==0 && visible(window),"selection owner loss restores clients");
    XDestroyWindow(display,owner);
    /* Queued events for a withdrawn frame must not find freed ClientData. */
    for (int iteration=0; iteration<4; ++iteration) {
        Widget victim = XtVaAppCreateShell("Withdrawn", "WorkspaceProtocolTest",
            topLevelShellWidgetClass, display, XtNwidth, 90, XtNheight, 70, NULL);
        XtVaCreateManagedWidget("child",xmDrawingAreaWidgetClass,victim,NULL);
        XtRealizeWidget(victim); pump(200);
        Window root, base, frame, *children=NULL; unsigned count;
        CHECK(XQueryTree(display,XtWindow(victim),&root,&base,&children,&count),"victim base");
        if(children)XFree(children);
        CHECK(base!=root,"victim reparented");
        CHECK(XQueryTree(display,base,&root,&frame,&children,&count),"victim frame");
        if(children)XFree(children);
        XtDestroyWidget(victim); pump(200);
        XEvent stale; memset(&stale,0,sizeof(stale));
        stale.xfocus.type=FocusIn; stale.xfocus.display=display;
        stale.xfocus.mode=NotifyNormal; stale.xfocus.detail=NotifyNonlinear;
        stale.xfocus.window=base;
        XSendEvent(display,root,False,FocusChangeMask,&stale);
        stale.xfocus.window=frame;
        XSendEvent(display,root,False,FocusChangeMask,&stale);
        XFlush(display);pump(100);
        set(window,"width",341+iteration);
        CHECK(value(window,"width")==341+iteration,"manager responsive after stale frame focus");
    }
    puts("PASS withdrawn frame contexts reject queued focus events and retain responsive stacking");
    XtDestroyWidget(shell);
    puts("PASS MWM workspace handshake, snapshots, visibility, geometry, icons, registration transient lifecycle and selection-owner recovery");
    return 0;
}
