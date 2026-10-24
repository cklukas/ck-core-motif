/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Explicit Xft point sizes, real resize/remap and native control state. */
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <Xm/TextP.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static unsigned int assertions, activations;
static const char input[] = "Alpha e\xcc\x81 \xce\xb2 \xe4\xb8\xad \xd8\xa8";
static const char paragraph[] =
    "Alpha beta gamma delta epsilon zeta eta theta iota kappa lambda. "
    "Unicode e\xcc\x81 \xce\xb2 \xe4\xb8\xad \xd8\xa8 remains unchanged. "
    "Alpha beta gamma delta epsilon zeta eta theta iota kappa lambda. "
    "Unicode e\xcc\x81 \xce\xb2 \xe4\xb8\xad \xd8\xa8 remains unchanged. "
    "Alpha beta gamma delta epsilon zeta eta theta iota kappa lambda. "
    "Unicode e\xcc\x81 \xce\xb2 \xe4\xb8\xad \xd8\xa8 remains unchanged.";
static void require(int condition, const char *message) {
  ++assertions;
  if (!condition) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void drain(XtAppContext app, Display *display) {
  unsigned int round;
  for (round = 0; round < 4; ++round) {
    struct timespec pause = {0, 5000000};
    XSync(display, False);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    nanosleep(&pause, NULL);
  }
}
static void activated(Widget widget, XtPointer data, XtPointer call) {
  XmPushButtonCallbackStruct *event = call;
  (void)widget; (void)data;
  require(event && event->reason == XmCR_ACTIVATE && event->event &&
              event->event->type == KeyPress,
          "button activation did not originate in the native key translation");
  ++activations;
}
static void contents(Widget field, Widget text) {
  char *value = XmTextFieldGetString(field);
  require(value && !strcmp(value, input), "font/resize/remap changed UTF-8 field bytes");
  XtFree(value);
  value = XmTextGetString(text);
  require(value && !strcmp(value, paragraph), "font/resize/remap changed UTF-8 text bytes");
  XtFree(value);
  require(XmTextFieldGetInsertionPosition(field) == 5 &&
              XmTextGetInsertionPosition(text) == 7,
          "font/resize/remap changed scalar insertion positions");
}
static void children(Widget form, Widget *expected, Cardinal count) {
  WidgetList actual = NULL;
  Cardinal actual_count = 0, index;
  XtVaGetValues(form, XmNchildren, &actual, XmNnumChildren, &actual_count, NULL);
  require(actual_count == count, "font/resize/remap changed owned native child count");
  for (index = 0; index < count; ++index)
    require(actual[index] == expected[index], "font/resize/remap replaced a native child");
}
static void geometry(Widget shell, Widget form, Widget *controls, Cardinal count,
                     int expected_width, int expected_height) {
  XWindowAttributes top, host;
  Cardinal index;
  require(XGetWindowAttributes(XtDisplay(shell), XtWindow(shell), &top) &&
              top.map_state == IsViewable && top.width == expected_width &&
              top.height == expected_height,
          "actual native shell did not reach requested resize geometry");
  require(XGetWindowAttributes(XtDisplay(form), XtWindow(form), &host) &&
              host.width == top.width && host.height == top.height,
          "Form did not follow actual shell resize");
  for (index = 0; index < count; ++index) {
    XWindowAttributes control;
    Window child;
    int x = 0, y = 0;
    require(XGetWindowAttributes(XtDisplay(form), XtWindow(controls[index]), &control) &&
                control.map_state == IsViewable && control.width > 0 && control.height > 0,
            "resized control has no viewable native area");
    require(XTranslateCoordinates(XtDisplay(form), XtWindow(controls[index]),
                XtWindow(form), 0, 0, &x, &y, &child), "control translation failed");
    require(x >= 0 && y >= 0 && x + control.width <= host.width &&
                y + control.height <= host.height,
            "native Form layout placed a control outside its viewport");
  }
}
static XmFontList fonts(Widget owner, int points, int *advance, int *height) {
  Arg args[3];
  XmRendition rendition;
  XmFontList result;
  XmFontContext context;
  XmFontListEntry entry;
  XmFontType type;
  XftFont *font;
  FcChar32 character = '0';
  XGlyphInfo extent;
  double actual_points = 0;
  XtSetArg(args[0], XmNfontType, XmFONT_IS_XFT);
  XtSetArg(args[1], XmNfontName, "DejaVu Sans");
  XtSetArg(args[2], XmNfontSize, points);
  rendition = XmRenditionCreate(owner, XmFONTLIST_DEFAULT_TAG, args, 3);
  require(rendition != NULL, "explicit Xft rendition unavailable");
  result = XmRenderTableAddRenditions(NULL, &rendition, 1, XmMERGE_REPLACE);
  XmRenditionFree(rendition);
  require(result && XmFontListInitFontContext(&context, result), "explicit render table unavailable");
  entry = XmFontListNextEntry(context);
  require(entry != NULL, "explicit Xft entry missing");
  font = (XftFont *)XmFontListEntryGetFont(entry, &type);
  require(font && type == XmFONT_IS_XFT, "explicit font is not an actual Xft font");
  require(FcPatternGetDouble(font->pattern, FC_SIZE, 0, &actual_points) == FcResultMatch &&
              fabs(actual_points - points) < .01,
          "actual Xft font does not have the requested point size");
  XftTextExtents32(XtDisplay(owner), font, &character, 1, &extent);
  *advance = extent.xOff; *height = font->ascent + font->descent;
  require(*advance > 0 && *height > 0, "explicit font has no positive metrics");
  XmFontListFreeFontContext(context);
  return result;
}
static void press_space(Widget button) {
  XEvent event;
  memset(&event, 0, sizeof(event));
  event.xkey.type = KeyPress;
  event.xkey.display = XtDisplay(button);
  event.xkey.window = XtWindow(button);
  event.xkey.root = DefaultRootWindow(XtDisplay(button));
  event.xkey.keycode = XKeysymToKeycode(XtDisplay(button), XK_space);
  event.xkey.same_screen = True;
  require(event.xkey.keycode != 0 && XSendEvent(XtDisplay(button), XtWindow(button),
              False, KeyPressMask, &event), "native Space event could not be delivered");
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, form, label, field, text, button, controls[4];
  Display *display;
  XmString caption;
  int sizes[] = {12, 18, 24}, previous_advance = 0, previous_height = 0;
  unsigned int profile;
  require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF-8 locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "FontResize", NULL, 0, &argc, argv, NULL,
      XmNtitle, "Native font resize and remap", XmNallowShellResize, True,
      XmNwidth, 640, XmNheight, 480, NULL);
  display = XtDisplay(shell);
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell,
      XmNmarginWidth, 8, XmNmarginHeight, 8, NULL);
  caption = XmStringCreateLocalized("Font and geometry");
  label = XtVaCreateManagedWidget("caption", xmLabelWidgetClass, form,
      XmNlabelString, caption, XmNtopAttachment, XmATTACH_FORM,
      XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
  XmStringFree(caption);
  field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, form,
      XmNcolumns, 12, XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, label,
      XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
  caption = XmStringCreateLocalized("Activate");
  button = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, form,
      XmNlabelString, caption, XmNbottomAttachment, XmATTACH_FORM,
      XmNleftAttachment, XmATTACH_FORM, NULL);
  XmStringFree(caption);
  text = XtVaCreateManagedWidget("wrapped", xmTextWidgetClass, form,
      XmNeditMode, XmMULTI_LINE_EDIT, XmNwordWrap, True, XmNscrollHorizontal, False,
      XmNresizeWidth, False, XmNresizeHeight, False, XmNrows, 4, XmNcolumns, 12,
      XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, field,
      XmNbottomAttachment, XmATTACH_WIDGET, XmNbottomWidget, button,
      XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
  controls[0] = label; controls[1] = field; controls[2] = button; controls[3] = text;
  XtAddCallback(button, XmNactivateCallback, activated, NULL);
  XmTextFieldSetString(field, (char *)input);
  XmTextSetString(text, (char *)paragraph);
  XmTextFieldSetInsertionPosition(field, 5);
  XmTextSetInsertionPosition(text, 7);
  XtRealizeWidget(shell);
  drain(app, display);
  XSetInputFocus(display, XtWindow(shell), RevertToParent, CurrentTime);
  require(XmProcessTraversal(field, XmTRAVERSE_CURRENT), "initial field focus failed");
  drain(app, display);
  for (profile = 0; profile < XtNumber(sizes); ++profile) {
    int advance, height, narrow, wide, shell_height, narrow_lines, wide_lines;
    Cardinal index;
    XmFontList table = fonts(shell, sizes[profile], &advance, &height);
    require(advance > previous_advance && height > previous_height,
            "increasing point sizes did not change actual font metrics");
    for (index = 0; index < XtNumber(controls); ++index)
      XtVaSetValues(controls[index], XmNfontList, table, NULL);
    XmFontListFree(table);
    drain(app, display);
    require(XmGetFocusWidget(shell) == field, "font change lost field focus");
    narrow = advance * 18 + 48; wide = advance * 36 + 48;
    shell_height = height * 12 + 80;
    XtVaSetValues(shell, XmNwidth, narrow, XmNheight, shell_height, NULL);
    drain(app, display);
    geometry(shell, form, controls, XtNumber(controls), narrow, shell_height);
    contents(field, text); children(form, controls, XtNumber(controls));
    require(XmGetFocusWidget(shell) == field, "narrow resize lost field focus");
    narrow_lines = ((XmTextWidget)text)->text.total_lines;
    XtVaSetValues(shell, XmNwidth, wide, NULL);
    drain(app, display);
    geometry(shell, form, controls, XtNumber(controls), wide, shell_height);
    contents(field, text); children(form, controls, XtNumber(controls));
    require(XmGetFocusWidget(shell) == field, "wide resize lost field focus");
    wide_lines = ((XmTextWidget)text)->text.total_lines;
    require(narrow_lines > wide_lines && wide_lines > 1,
            "actual native wrap layout did not respond to narrow/wide resize");
    XtUnmapWidget(shell);
    drain(app, display);
    {
      XWindowAttributes attributes;
      require(XGetWindowAttributes(display, XtWindow(field), &attributes) &&
                  attributes.map_state != IsViewable,
              "native unmap did not hide the existing control");
    }
    XtMapWidget(shell);
    drain(app, display);
    XSetInputFocus(display, XtWindow(shell), RevertToParent, CurrentTime);
    drain(app, display);
    require(XmGetFocusWidget(shell) == field, "remap did not retain field focus");
    geometry(shell, form, controls, XtNumber(controls), wide, shell_height);
    contents(field, text); children(form, controls, XtNumber(controls));
    require(XmProcessTraversal(button, XmTRAVERSE_CURRENT), "button focus after remap failed");
    drain(app, display);
    require(XmGetFocusWidget(shell) == button, "native keyboard target is not focused");
    press_space(button);
    drain(app, display);
    require(activations == profile + 1, "native Space activated button other than exactly once");
    contents(field, text);
    require(XmProcessTraversal(field, XmTRAVERSE_CURRENT), "restore field focus failed");
    drain(app, display);
    printf("{\"points\":%d,\"zero_advance\":%d,\"font_height\":%d,"
           "\"narrow_width\":%d,\"wide_width\":%d,\"height\":%d,"
           "\"narrow_lines\":%d,\"wide_lines\":%d,\"owned_children\":4}\n",
           sizes[profile], advance, height, narrow, wide, shell_height,
           narrow_lines, wide_lines);
    previous_advance = advance; previous_height = height;
  }
  printf("{\"assertions\":%u,\"keyboard_activations\":%u}\n", assertions, activations);
  XtDestroyWidget(shell);
  XtCloseDisplay(display);
  XtDestroyApplicationContext(app);
  return 0;
}
