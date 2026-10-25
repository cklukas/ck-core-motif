/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Rendition bodies own Xft references; scratch projections only borrow them. */
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/Xft/Xft.h>
#include <fontconfig/fontconfig.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int assertions, x_errors;
static void require(int condition, const char *message) {
  ++assertions;
  if (!condition) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void display_retired(Widget widget, XtPointer data, XtPointer call) {
  Boolean *live = data;
  (void)widget; (void)call;
  require(*live, "native display owner was destroyed more than once");
  *live = False;
}
static int x_error(Display *display, XErrorEvent *event) {
  char message[256];
  XGetErrorText(display, event->error_code, message, sizeof(message));
  fprintf(stderr, "native X error: %s (request %u)\n", message, event->request_code);
  ++x_errors;
  return 0;
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
  XSync(display, False);
  require(!x_errors, "font lifetime produced an actual X protocol error");
}
typedef struct {
  XtPointer core;
  XftFont *xft;
  XmFontType type;
} FontResources;
static FontResources resources(XmRendition value) {
  FontResources result = {NULL, NULL, XmFONT_IS_FONT};
  Arg args[3];
  XtSetArg(args[0], XmNfont, &result.core);
  XtSetArg(args[1], XmNxftFont, &result.xft);
  XtSetArg(args[2], XmNfontType, &result.type);
  XmRenditionRetrieve(value, args, 3);
  return result;
}
static XmRendition shared_copy(XmRendition value, XmStringTag tag) {
  XmRenderTable table = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  XmRendition result = XmRenderTableGetRendition(table, tag);
  XmRenderTableFree(table);
  require(result != NULL, "public rendition lookup did not return a copy");
  return result;
}
static XmRendition merged_copy(Widget owner, XmRendition value, XmMergeMode mode) {
  XmRenderTable table = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  XmStringTag tags[] = {XmFONTLIST_DEFAULT_TAG};
  XmRenderTable copy = XmRenderTableCopy(table, tags, 1);
  Arg arg;
  XmRendition decoration, result;
  XtSetArg(arg, XmNunderlineType, XmSINGLE_LINE);
  decoration = XmRenditionCreate(owner, XmFONTLIST_DEFAULT_TAG, &arg, 1);
  require(decoration && copy, "public table merge inputs unavailable");
  copy = XmRenderTableAddRenditions(copy, &decoration, 1, mode);
  result = XmRenderTableGetRendition(copy, XmFONTLIST_DEFAULT_TAG);
  require(result != NULL, "merged public rendition unavailable");
  XmRenditionFree(decoration);
  XmRenderTableFree(copy);
  XmRenderTableFree(table);
  return result;
}
static void table_count(XmRenderTable table, int expected) {
  XmStringTag *tags = NULL;
  int count = XmRenderTableGetTags(table, &tags), index;
  require(count == expected, "render-table copy/mutation changed the wrong table's entries");
  for (index = 0; index < count; ++index) XtFree(tags[index]);
  XtFree((char *)tags);
}
static void table_lifetime(Widget owner, XmRendition value) {
  XmRenderTable original = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  XmRenderTable copy = XmRenderTableCopy(original, NULL, 0), filtered;
  XmRenderTable unique;
  XmStringTag wanted[] = {"missing", XmFONTLIST_DEFAULT_TAG};
  XmStringTag remove[] = {XmFONTLIST_DEFAULT_TAG};
  XmRendition decoration, unique_decoration, retained;
  Arg arg;
  XtSetArg(arg, XmNunderlineType, XmSINGLE_LINE);
  decoration = XmRenditionCreate(owner, "decoration", &arg, 1);
  require(decoration && copy, "shared table lifecycle inputs unavailable");
  copy = XmRenderTableAddRenditions(copy, &decoration, 1, XmMERGE_REPLACE);
  XmRenditionFree(decoration);
  table_count(original, 1); table_count(copy, 2);
  filtered = XmRenderTableCopy(copy, wanted, 2);
  table_count(filtered, 1); /* Missing tags must not leave holes in entry storage. */
  retained = XmRenderTableGetRendition(filtered, XmFONTLIST_DEFAULT_TAG);
  require(retained != NULL, "filtered public table lost its existing font");
  XmRenderTableFree(filtered);
  XmRenditionFree(retained);

  /* A unique table keeps its public handle while compaction publishes only
   * after the resized body is ready. */
  unique = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  unique_decoration = XmRenditionCreate(owner, "unique-decoration", &arg, 1);
  require(unique_decoration != NULL, "unique table rendition setup failed");
  unique = XmRenderTableAddRenditions(unique, &unique_decoration, 1, XmDUPLICATE);
  XmRenditionFree(unique_decoration);
  require(unique != NULL, "unique table setup failed");
  table_count(unique, 2);
  unique = XmRenderTableRemoveRenditions(unique, remove, 1);
  table_count(unique, 1);
  XmRenderTableFree(unique);

  /* A shared table receives an independent compacted body and leaves the
   * source table untouched. */
  filtered = XmRenderTableCopy(copy, NULL, 0);
  filtered = XmRenderTableRemoveRenditions(filtered, remove, 1);
  table_count(filtered, 1);
  table_count(copy, 2);
  XmRenderTableFree(filtered);

  filtered = XmRenderTableCopy(original, NULL, 0);
  filtered = XmRenderTableRemoveRenditions(filtered, remove, 1);
  require(filtered == NULL, "removing the only copied entry must empty that copy");
  table_count(original, 1);
  XmRenderTableFree(copy);
  XmRenderTableFree(original);
}
static XmRendition rendition(Widget owner, int points) {
  Arg args[3];
  XmRendition result;
  XtSetArg(args[0], XmNfontType, XmFONT_IS_XFT);
  XtSetArg(args[1], XmNfontName, "DejaVu Sans");
  XtSetArg(args[2], XmNfontSize, points);
  result = XmRenditionCreate(owner, XmFONTLIST_DEFAULT_TAG, args, 3);
  require(result && resources(result).xft && resources(result).type == XmFONT_IS_XFT,
          "explicit Xft rendition did not load its font");
  return result;
}
static void usable(Display *display, XmRendition value, int points) {
  double size = 0;
  XGlyphInfo extent;
  FcChar32 character = 'W';
  XftFont *font = resources(value).xft;
  require(font != NULL, "rendition Xft font is absent");
  require(FcPatternGetDouble(font->pattern, FC_SIZE, 0, &size) == FcResultMatch &&
              fabs(size - points) < .01, "live font has an unexpected point size");
  XftTextExtents32(display, font, &character, 1, &extent);
  require(extent.xOff > 0, "retained font is not usable after another owner released it");
}
static void font_type_transition(Widget owner) {
  Display *display = XtDisplay(owner);
  XFontStruct *core = XLoadQueryFont(display, "fixed");
  XmRendition changing, retained;
  Arg args[3];
  require(core != NULL, "borrowed core transition font unavailable");
  XtSetArg(args[0], XmNfontType, XmFONT_IS_FONT);
  XtSetArg(args[1], XmNfontName, "fixed");
  XtSetArg(args[2], XmNfont, core);
  changing = XmRenditionCreate(owner, "fontTypeTransition", args, 3);
  require(changing && resources(changing).core == core &&
              resources(changing).type == XmFONT_IS_FONT,
          "transition fixture did not retain its explicitly supplied core font");
  retained = shared_copy(changing, "fontTypeTransition");
  XtSetArg(args[0], XmNfontType, XmFONT_IS_XFT);
  XtSetArg(args[1], XmNfontName, "DejaVu Sans");
  XtSetArg(args[2], XmNfontSize, 18);
  /* No replacement XmNfont is supplied: the stale core pointer must not
   * prevent ValidateAndLoadFont from loading the requested Xft font. */
  XmRenditionUpdate(changing, args, 3);
  require(resources(changing).type == XmFONT_IS_XFT &&
              resources(changing).core == NULL && (resources(changing).xft != NULL),
          "FONT to XFT update retained a stale core font or failed to load Xft");
  usable(display, changing, 18);
  require(resources(retained).type == XmFONT_IS_FONT &&
              resources(retained).core == core && XTextWidth(core, "core", 4) > 0,
          "font-type copy-on-write changed the retained core rendition");
  XmRenditionFree(retained);
  /* An explicit replacement core font must survive the reverse transition. */
  XtSetArg(args[0], XmNfontType, XmFONT_IS_FONT);
  XtSetArg(args[1], XmNfontName, "fixed");
  XtSetArg(args[2], XmNfont, core);
  XmRenditionUpdate(changing, args, 3);
  require(resources(changing).type == XmFONT_IS_FONT &&
              resources(changing).core == core && !resources(changing).xft,
          "explicit replacement core font was cleared during type update");
  require(XTextWidth(core, "core", 4) > 0,
          "borrowed core font was prematurely released by rendition teardown");
  XmRenditionFree(changing);
  require(XTextWidth(core, "core", 4) > 0,
          "rendition consumed the caller's borrowed core font");
  XFreeFont(display, core);
}
static void draw(Widget widget, XmRendition value) {
  Display *display = XtDisplay(widget);
  XmRenderTable table = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  Arg arg;
  XmRendition decoration;
  XmString caption;
  GC gc;
  unsigned int round;
  XtSetArg(arg, XmNunderlineType, XmSINGLE_LINE);
  decoration = XmRenditionCreate(widget, "accent", &arg, 1);
  table = XmRenderTableAddRenditions(table, &decoration, 1, XmMERGE_REPLACE);
  XmRenditionFree(decoration);
  caption = XmStringGenerate("Retained font", XmFONTLIST_DEFAULT_TAG,
                             XmCHARSET_TEXT, "accent");
  gc = XCreateGC(display, XtWindow(widget), 0, NULL);
  require(table && caption && gc, "native compound-string drawing inputs unavailable");
  for (round = 0; round < 10; ++round)
    XmStringDraw(display, XtWindow(widget), table, caption, gc, 2, 2, 200,
                 XmALIGNMENT_BEGINNING, XmSTRING_DIRECTION_L_TO_R, NULL);
  XSync(display, False);
  require(!x_errors, "native compound-string rendering failed");
  XFreeGC(display, gc);
  XmStringFree(caption);
  XmRenderTableFree(table);
}
static Widget label(Widget shell, XmRendition value) {
  XmFontList table = XmRenderTableAddRenditions(NULL, &value, 1, XmMERGE_REPLACE);
  XmString text = XmStringCreateLocalized("Retained font");
  Widget result = XtVaCreateManagedWidget("label", xmLabelWidgetClass, shell,
      XmNfontList, table, XmNlabelString, text, NULL);
  XmStringFree(text);
  XmFontListFree(table);
  return result;
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, second_shell, first_label, second_label;
  Widget first_display_widget, second_display_widget;
  Boolean first_display_live = True, second_display_live = True;
  Display *first, *second;
  XmRendition original, shared, clone, merged, raw, other;
  Arg arg, raw_args[2];
  XftFont *caller_font;
  require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF-8 locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "FontLifetime", NULL, 0, &argc, argv,
                            NULL, XmNtitle, "First font owner", NULL);
  {
    int height = -1, ascent = -1, descent = -1;
    XmRenderTableGetDefaultFontExtents(NULL, &height, &ascent, &descent);
    require(height == 0 && ascent == 0 && descent == 0,
            "null render-table extents did not initialize outputs");
  }
  first = XtDisplay(shell);
  first_display_widget = XmGetXmDisplay(first);
  require(first_display_widget != NULL, "first native display owner unavailable");
  XtAddCallback(first_display_widget, XmNdestroyCallback, display_retired, &first_display_live);
  XSetErrorHandler(x_error);
  font_type_transition(shell);
  original = rendition(shell, 12);
  table_lifetime(shell, original);
  shared = shared_copy(original, XmFONTLIST_DEFAULT_TAG);
  clone = merged_copy(shell, original, XmMERGE_OLD);
  merged = merged_copy(shell, original, XmMERGE_NEW);
  XtSetArg(arg, XmNfontSize, 18);
  XmRenditionUpdate(original, &arg, 1);
  usable(first, original, 18); usable(first, shared, 12);
  usable(first, clone, 12); usable(first, merged, 12);
  XmRenditionFree(shared);
  usable(first, clone, 12);
  XmRenditionFree(clone);
  usable(first, merged, 12);
  XmRenditionFree(merged);
  usable(first, original, 18);

  shared = shared_copy(original, XmFONTLIST_DEFAULT_TAG);
  XtSetArg(arg, XmNunderlineType, XmSINGLE_LINE);
  XmRenditionUpdate(original, &arg, 1); /* Non-font copy-on-write retains font refs. */
  XmRenditionFree(shared);
  usable(first, original, 18);

  /* A caller retains its reference when supplying the XmNxftFont resource. */
  caller_font = XftFontOpenName(first, DefaultScreen(first), "DejaVu Sans:size=24");
  require(caller_font != NULL, "caller Xft font unavailable");
  XtSetArg(raw_args[0], XmNxftFont, caller_font);
  XtSetArg(raw_args[1], XmNfontType, XmFONT_IS_XFT);
  raw = XmRenditionCreate(shell, "caller", raw_args, 2);
  require(raw && resources(raw).xft == caller_font,
          "raw Xft resource did not preserve the supplied font");
  XftFontClose(first, caller_font);
  usable(first, raw, 24);
  XtSetArg(arg, XmNxftFont, resources(original).xft);
  XmRenditionUpdate(raw, &arg, 1);
  usable(first, raw, 18);
  XmRenditionFree(raw);

  second = XtOpenDisplay(app, DisplayString(first), "second", "FontLifetime",
                          NULL, 0, &argc, argv);
  require(second && second != first, "second independent display connection unavailable");
  second_shell = XtVaAppCreateShell("second", "FontLifetime",
      applicationShellWidgetClass, second, XmNtitle, "Second font owner", NULL);
  second_display_widget = XmGetXmDisplay(second);
  require(second_display_widget && second_display_widget != first_display_widget,
          "independent connections must have independent native display owners");
  XtAddCallback(second_display_widget, XmNdestroyCallback, display_retired, &second_display_live);
  other = rendition(second_shell, 18);
  require(resources(original).xft != resources(other).xft,
          "font cache aliased independent display connections");
  first_label = label(shell, original);
  second_label = label(second_shell, other);
  XtRealizeWidget(shell); XtRealizeWidget(second_shell);
  drain(app, first); drain(app, second);
  draw(first_label, original); draw(second_label, other);
  XmRenditionFree(original);
  XtDestroyWidget(shell);
  /* The last VendorShell normally retires XmDisplay. Its destroy callback
   * records that event without reading the retired widget. Clean up a still
   * live independent owner before closing the connection, exactly once. */
  if (first_display_live) XtDestroyWidget(first_display_widget);
  require(!first_display_live, "first native display owner was not retired");
  XtCloseDisplay(first);
  usable(second, other, 18);
  draw(second_label, other);
  XClearArea(second, XtWindow(second_shell), 0, 0, 0, 0, True);
  drain(app, second);
  XmRenditionFree(other);
  XtDestroyWidget(second_shell);
  if (second_display_live) XtDestroyWidget(second_display_widget);
  require(!second_display_live, "second native display owner was not retired");
  XtCloseDisplay(second);
  XtDestroyApplicationContext(app);
  /* This standalone process has now released every widget, font and display.
   * FcInit/FcFini controls establish terminal configuration cleanup; a held
   * client pattern remains detectable, so this does not replace owner teardown. */
  FcFini();
  printf("{\"assertions\":%u,\"independent_displays\":2,\"x_errors\":%u}\n",
         assertions, x_errors);
  return 0;
}
