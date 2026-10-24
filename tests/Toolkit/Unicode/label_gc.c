/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Label owns every cached GC reference across color/font replacement. */
#include <Xm/LabelP.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef FIX_1381
#error "This fixture requires the fork's insensitive label shadow support"
#endif

static unsigned int assertions;
static void require(int condition, const char *message)
{
  ++assertions;
  if (!condition) {
    fprintf(stderr, "%s\n", message);
    exit(1);
  }
}

static void shadow_color(Widget label, Pixel expected)
{
  XGCValues values;
  XmLabelWidget record = (XmLabelWidget)label;
  require(XGetGCValues(XtDisplay(label), record->label.shadow_GC,
                       GCForeground, &values), "read actual label shadow GC");
  require(values.foreground == expected,
          "insensitive label shadow did not follow topShadowColor");
}

int main(int argc, char **argv)
{
  XtAppContext app;
  Widget shell, label;
  Display *display;
  XFontStruct *font;
  XmFontList fonts;
  XmString text;
  Pixel dark, light;
  unsigned int iteration;

  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "LabelGCLifetime", NULL, 0, &argc, argv,
                           NULL, NULL);
  require(shell != NULL, "create label GC fixture shell");
  display = XtDisplay(shell);
  dark = BlackPixelOfScreen(XtScreen(shell));
  light = WhitePixelOfScreen(XtScreen(shell));
  require(dark != light, "label GC fixture requires distinct black/white pixels");
  font = XLoadQueryFont(display, "fixed");
  require(font != NULL, "load borrowed core font");
  fonts = XmFontListCreate(font, XmFONTLIST_DEFAULT_TAG);
  require(fonts != NULL, "create label render table");
  text = XmStringCreateLocalized("Insensitive shadow ownership");
  label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, shell,
      XmNlabelString, text, XmNfontList, fonts, XmNsensitive, False,
      XmNforeground, dark, XmNbackground, light, XmNtopShadowColor, dark,
      NULL);
  XmStringFree(text);
  require(label != NULL, "create label fixture");
  XtRealizeWidget(shell);
  shadow_color(label, dark);

  /* First change only this dependency. The original implementation retained
   * the old shadow color because its GC rebuild condition omitted it. */
  XtVaSetValues(label, XmNtopShadowColor, light, NULL);
  shadow_color(label, light);
  for (iteration = 0; iteration < 32; ++iteration) {
    Pixel foreground = iteration & 1 ? dark : light;
    Pixel shadow = iteration & 1 ? light : dark;
    XmFontList replacement = XmFontListCreate(font, XmFONTLIST_DEFAULT_TAG);
    require(replacement != NULL, "create independent replacement render table");
    XtVaSetValues(label, XmNforeground, foreground, XmNfontList, replacement,
                  NULL);
    XmFontListFree(replacement);
    shadow_color(label, light);
    XtVaSetValues(label, XmNtopShadowColor, shadow, NULL);
    shadow_color(label, shadow);
    XtVaSetValues(label, XmNtopShadowColor, light, NULL);
    shadow_color(label, light);
  }

  /* Strict leak checking must see all shadow-GC references balanced. No text
   * editing/selection service is created by this focused ownership fixture. */
  XSync(display, False);
  XtDestroyWidget(shell);
  XmFontListFree(fonts);
  XFreeFont(display, font);
  XtCloseDisplay(display);
  XtDestroyApplicationContext(app);
  printf("{\"assertions\":%u,\"replacement_rounds\":32}\n", assertions);
  return 0;
}
