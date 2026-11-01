/* Display-backed acceptance of the real workspace client's button callbacks. */
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "wsm.h"
#include "wsm_ui.h"
#include "wsmStruct.h"
#include "xrmLib.h"
extern Display *dsp;
extern Widget shell;
extern WSM_UI *wsm_ui;
extern Space *space_list, *current_space;
extern Boolean connected, mwm_gone;
extern unsigned long wsm_connection_count;
extern char *file_name;
static Window first, second;
static unsigned phase, attempts;
static Boolean restart_test;
static int copied_x, copied_y, linked_x, linked_y;
static int viewable(Window window)
{
    XWindowAttributes attributes;
    return XGetWindowAttributes(dsp, window, &attributes) && attributes.map_state == IsViewable;
}
static void position(Window window, int *x, int *y)
{
    Window child;
    XTranslateCoordinates(dsp, window, XRootWindowOfScreen(XtScreen(shell)), 0, 0, x, y, &child);
}
static Window test_window(const char *name)
{
    Window window = XCreateSimpleWindow(dsp, XRootWindowOfScreen(XtScreen(shell)),
                                        40, 40, 180, 100, 0, 0, 0xffffff);
    XStoreName(dsp, window, name);
    XClassHint hint = {(char *)name, "WsmIntegration"};
    XSetClassHint(dsp, window, &hint);
    XMapWindow(dsp, window);
    return window;
}
static void step(XtPointer closure, XtIntervalId *id)
{
    (void)closure; (void)id;
    if (++attempts > 250) {
        int x=0,y=0;if(first)position(first,&x,&y);
        fprintf(stderr,"FAIL Wsm UI phase %u did not complete: room=%s first=%d,%d copied=%d,%d linked=%d,%d\n",
            phase,current_space?current_space->name:"none",x,y,copied_x,copied_y,linked_x,linked_y);
        SaveWsmToFile(file_name);
        exit(1);
    }
    switch (phase) {
    case 0:
        if (!connected) break;
        if (wsm_ui->num_space_buttons < 2) { fputs("FAIL need two workspaces\n",stderr); exit(1); }
        first = test_window("WsmFirst"); phase = 1; break;
    case 1:
        if (!GetWorkWindow(first) || !viewable(first)) break;
        XmToggleButtonSetState(wsm_ui->space_button[1], True, True);
        phase = 2; break;
    case 2:
        if (current_space != GetSpaceFromID(1) || viewable(first)) break;
        second = test_window("WsmSecond"); phase = 3; break;
    case 3:
        if (!GetWorkWindow(second) || !viewable(second)) break;
        if (restart_test) {
            puts("READY Wsm integration restart"); fflush(stdout);
            phase = 7; break;
        }
        XmToggleButtonSetState(wsm_ui->space_button[0], True, True);
        phase = 4; break;
    case 4:
        if (current_space != GetSpaceFromID(0) || !viewable(first) || viewable(second)) break;
        position(first, &copied_x, &copied_y);
        CopyWindow(GetWorkWindow(first),GetSpaceFromID(0),GetSpaceFromID(1));
        phase = 8; break;
    case 5: {
        struct stat info;
        if (stat(file_name, &info) || !info.st_size) break;
        puts("PASS Wsm UI handshake, new-window registration, workspace buttons, visibility and save");
        fflush(stdout);
        ManageAllWindowsAndExit();
        phase = 6; break;
    }
    case 8:
        if (!IsWorkWindowInSpace(GetWorkWindow(first),GetSpaceFromID(1))) break;
        XmToggleButtonSetState(wsm_ui->space_button[1],True,True); phase=9; break;
    case 9:
        if (current_space!=GetSpaceFromID(1) || !viewable(first) || !viewable(second)) break;
        XMoveWindow(dsp,first,360,240); phase=10; break;
    case 10:
        position(first,&linked_x,&linked_y);
        if (linked_x==copied_x && linked_y==copied_y) break;
        XmToggleButtonSetState(wsm_ui->space_button[0],True,True); phase=11; break;
    case 11: {
        int x,y; position(first,&x,&y);
        if (current_space!=GetSpaceFromID(0) || x!=copied_x || y!=copied_y) break;
        LinkWindow(GetWorkWindow(first),GetSpaceFromID(0),GetSpaceFromID(1)); phase=12; break;
    }
    case 12:
        if (!GetWorkWindow(first)->linked) break;
        XMoveWindow(dsp,first,420,280); phase=13; break;
    case 13:
        position(first,&linked_x,&linked_y);
        if (linked_x==copied_x && linked_y==copied_y) break;
        XmToggleButtonSetState(wsm_ui->space_button[1],True,True); phase=14; break;
    case 14: {
        int x,y;position(first,&x,&y);
        if (current_space!=GetSpaceFromID(1) || !viewable(first) || x!=linked_x || y!=linked_y) break;
        MoveWindow(GetWorkWindow(second),GetSpaceFromID(1),GetSpaceFromID(0)); phase=15; break;
    }
    case 15:
        if (viewable(second) || IsWorkWindowInSpace(GetWorkWindow(second),GetSpaceFromID(1))) break;
        XmToggleButtonSetState(wsm_ui->space_button[0],True,True); phase=16; break;
    case 16:
        if (current_space!=GetSpaceFromID(0) || !viewable(second)) break;
        puts("PASS Wsm copy geometry, linked geometry and moving windows between workspaces");
        SendSaveWsm(file_name); phase=5;break;
    case 7:
        if (wsm_connection_count < 2 || !connected || mwm_gone) break;
        if (current_space != GetSpaceFromID(1) || viewable(first) || !viewable(second)) break;
        puts("PASS Wsm reconnected after MWM restart with workspace membership preserved");
        XmToggleButtonSetState(wsm_ui->space_button[0], True, True);
        phase = 4; break;
    default: break;
    }
    XtAppAddTimeOut(XtWidgetToApplicationContext(shell), 100, step, NULL);
}
void WsmStartIntegration(Boolean restart)
{
    restart_test = restart;
    XtAppAddTimeOut(XtWidgetToApplicationContext(shell), 100, step, NULL);
}
