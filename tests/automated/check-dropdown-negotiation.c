/* Refused geometry and partial query replies must not become X requests. */
#include <X11/IntrinsicP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/XmAll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr, "FAIL line %d\n", __LINE__); exit(1); } } while (0)
static unsigned refusals;
static XtGeometryResult deny(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{ (void)w; (void)request; (void)reply; ++refusals; return XtGeometryNo; }
static XtGeometryResult width_only(Widget w, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{ (void)request; memset(reply, 0xa5, sizeof(*reply)); reply->request_mode = CWWidth; reply->width = w->core.width; return XtGeometryAlmost; }
static void settle(XtAppContext app, Display *display)
{ unsigned i; for (i = 0; i < 10000; ++i) { XSync(display, False); if (!XtAppPending(app)) return; XtAppProcessEvent(app, XtIMAll); } CHECK(0); }

int main(int argc, char **argv)
{
    XtAppContext app; Display *display; unsigned cases = 0, profile, scale;
    XmAppearanceProfile profiles[] = {XmAPPEARANCE_CLASSIC, XmAPPEARANCE_MODERN_LIGHT, XmAPPEARANCE_MODERN_DARK};
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    display = XtOpenDisplay(app, NULL, "dropdownGeometry", "DropdownGeometry", NULL, 0, &argc, argv); CHECK(display);
    for (profile = 0; profile < 3; ++profile) for (scale = 0; scale < 2; ++scale) {
        Widget shell, board, drop, label; CompositeWidgetClass parent;
        XtGeometryHandler old_query, old_manager;
        CHECK(XmSetDisplayScaleOverride(display, scale ? 1.5 : 1)); CHECK(XmApplyAppearanceProfile(display, profiles[profile]));
        shell = XtAppCreateShell("dropdownGeometry", "DropdownGeometry", applicationShellWidgetClass, display, NULL, 0);
        board = XtVaCreateManagedWidget("board", xmDrawingAreaWidgetClass, shell, XmNwidth, 600, XmNheight, 300, NULL);
        drop = XtVaCreateManagedWidget("drop", xmDropDownWidgetClass, board, XmNshowLabel, True, NULL);
        XtRealizeWidget(shell); settle(app, display); label = XmDropDownGetLabel(drop); CHECK(label);
        old_query = XtClass(label)->core_class.query_geometry; XtClass(label)->core_class.query_geometry = width_only;
        { XtWidgetGeometry preferred = {0}; XtQueryGeometry(drop, NULL, &preferred); CHECK(preferred.height > 0 && preferred.height < 300); CHECK(preferred.width > 0 && preferred.width < 600); }
        XtClass(label)->core_class.query_geometry = old_query; parent = (CompositeWidgetClass)XtClass(board);
        old_manager = parent->composite_class.geometry_manager; parent->composite_class.geometry_manager = deny;
        { Dimension width = label->core.width, height = label->core.height; unsigned query, both;
          for (query = 0; query < 2; ++query) for (both = 0; both < 2; ++both) {
              XtWidgetGeometry request = {0}, reply; request.request_mode = CWHeight | (both ? CWWidth : 0) | (query ? XtCWQueryOnly : 0); request.height = 1234; request.width = width + 100; memset(&reply, 0xa5, sizeof(reply));
              CHECK(((CompositeWidgetClass)XtClass(drop))->composite_class.geometry_manager(label, &request, &reply) == XtGeometryNo); CHECK(label->core.width == width && label->core.height == height); CHECK(reply.request_mode == 0); ++cases;
          } }
        parent->composite_class.geometry_manager = old_manager; XtDestroyWidget(shell); settle(app, display);
    }
    CHECK(refusals == cases);
    for (unsigned cycle = 0; cycle < 4; ++cycle) { Widget shell = XtAppCreateShell("fontLifecycle", "FontLifecycle", applicationShellWidgetClass, display, NULL, 0); XtVaCreateManagedWidget("selector", xmFontSelectorWidgetClass, shell, NULL); XtRealizeWidget(shell); settle(app, display); CHECK(XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT)); CHECK(XmApplyAppearanceDensity(display, XmAPPEARANCE_DENSITY_COMPACT)); CHECK(XmSetDisplayScaleOverride(display, 1.5)); settle(app, display); XtDestroyWidget(shell); settle(app, display); }
    XtCloseDisplay(display); XtDestroyApplicationContext(app); printf("PASS DropDown negotiation: %u refusals\n", cases); return 0;
}
