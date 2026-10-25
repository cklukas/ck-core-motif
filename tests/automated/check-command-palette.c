#include <Xm/Accessible.h>
#include <Xm/CommandPalette.h>
#include <Xm/List.h>
#include <Xm/RowColumn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return EXIT_FAILURE; } ++checks; } while (0)
static void invoke(Widget widget, XtPointer data, XtPointer call) { const char **seen = (const char **)data; XmCommandPaletteCallbackStruct *callback = (XmCommandPaletteCallbackStruct *)call; (void)widget; if (callback && callback->id) *seen = callback->id; }
static void dismissed(Widget widget, XtPointer data, XtPointer call) { unsigned *count = (unsigned *)data; (void)widget; (void)call; ++*count; }
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, row, palette; XmString one, two, three, shortcut; XmCommandPaletteItem items[3]; const char *seen = NULL; unsigned dismissals = 0; XmAccessibility *accessibility; XmAccessibleInfo info; XmAccessibleId id; int count;
    (void)argv;
    shell = XtVaAppInitialize(&app, "CommandPaletteTest", NULL, 0, &argc, argv, NULL, XmNwidth, 400, XmNheight, 240, NULL);
    row = XmCreateRowColumn(shell, (char *)"row", NULL, 0); XtManageChild(row);
    one = XmStringCreateLocalized((char *)"Open document"); two = XmStringCreateLocalized((char *)"Close document"); three = XmStringCreateLocalized((char *)"Preferences"); shortcut = XmStringCreateLocalized((char *)"Ctrl+O");
    items[0] = (XmCommandPaletteItem){"open", one, shortcut, (XtPointer)"payload-open"}; items[1] = (XmCommandPaletteItem){"close", two, NULL, (XtPointer)"payload-close"}; items[2] = (XmCommandPaletteItem){"prefs", three, NULL, (XtPointer)"payload-prefs"};
    palette = XmVaCreateCommandPalette(row, (char *)"palette", NULL);
    XtAddCallback(palette, XmNcommandPaletteInvokeCallback, invoke, &seen); XtAddCallback(palette, XmNcommandPaletteDismissCallback, dismissed, &dismissals); XmCommandPaletteSetItems(palette, items, 3); XmStringFree(one); XmStringFree(two); XmStringFree(three); XmStringFree(shortcut); CHECK(XmCommandPaletteGetItemCount(palette) == 3);
    XmCommandPaletteSetFilter(palette, "OPEN"); count = (int)XmCommandPaletteGetVisibleCount(palette); CHECK(count == 1);
    XtRealizeWidget(shell); XmCommandPaletteShow(palette); XSync(XtDisplay(shell), False); CHECK(XmCommandPaletteIsShown(palette));
    accessibility = XmAccessibilityCreate(shell, "CommandPaletteTest"); CHECK(accessibility != NULL); id = XmAccessibilityWidget(accessibility, palette); CHECK(XmAccessibilityQuery(accessibility, id, &info)); CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL && strcmp(info.name, "Command palette") == 0 && info.action_count == 1); XmAccessibleInfoClear(&info);
    XmCommandPaletteInvokeSelection(palette, NULL); CHECK(seen && strcmp(seen, "open") == 0); XmCommandPaletteSetFilter(palette, "missing"); count = (int)XmCommandPaletteGetVisibleCount(palette); CHECK(count == 0); XmCommandPaletteInvokeSelection(palette, NULL); CHECK(seen && strcmp(seen, "open") == 0); XmAccessibilityDestroy(accessibility);
    XmCommandPaletteDismiss(palette, NULL); CHECK(!XmCommandPaletteIsShown(palette)); CHECK(dismissals == 1); XtDestroyWidget(shell); XtCloseDisplay(XtDisplay(shell)); XtDestroyApplicationContext(app);
    printf("PASS command palette: %u checks\n", checks); return EXIT_SUCCESS;
}
