/* WP-47 regression test: Xrm semantic palette tokens and WCAG contrast guardrails. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Link.h>
#include <X11/Xresource.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "FAIL WP-47 semantic palette line %d: %s\n", __LINE__, #cond); \
      exit(1); \
    } \
  } while (0)

static unsigned int warning_count = 0;
static char last_warning[256];

static void
capture_warning(String message)
{
  ++warning_count;
  if (message != NULL)
    snprintf(last_warning, sizeof(last_warning), "%s", message);
}

static double
srgb_linear(unsigned short ch16)
{
  double c = (double)(ch16 >> 8) / 255.0;
  return (c <= 0.04045) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
}

static double
pixel_luminance(Display *dpy, Colormap cmap, Pixel p)
{
  XColor xc;
  memset(&xc, 0, sizeof(xc));
  xc.pixel = p;
  XQueryColor(dpy, cmap, &xc);
  return 0.2126 * srgb_linear(xc.red) +
         0.7152 * srgb_linear(xc.green) +
         0.0722 * srgb_linear(xc.blue);
}

static double
pixel_contrast(Display *dpy, Colormap cmap, Pixel a, Pixel b)
{
  double la = pixel_luminance(dpy, cmap, a);
  double lb = pixel_luminance(dpy, cmap, b);
  double lighter = (la > lb) ? la : lb;
  double darker  = (la > lb) ? lb : la;
  return (lighter + 0.05) / (darker + 0.05);
}

static void
query_rgb8(Display *dpy, Colormap cmap, Pixel p,
           unsigned int *r, unsigned int *g, unsigned int *b)
{
  XColor xc;
  memset(&xc, 0, sizeof(xc));
  xc.pixel = p;
  XQueryColor(dpy, cmap, &xc);
  *r = (unsigned int)(xc.red >> 8);
  *g = (unsigned int)(xc.green >> 8);
  *b = (unsigned int)(xc.blue >> 8);
}

int
main(int argc, char **argv)
{
  XtAppContext app;
  Widget shell = XtVaAppInitialize(&app, "SemanticPaletteTest", NULL, 0,
                                   &argc, argv, NULL, NULL);
  Display *dpy = XtDisplay(shell);
  Colormap cmap = DefaultColormap(dpy, DefaultScreen(dpy));
  XtAppSetWarningHandler(app, capture_warning);

  /* 1. Startup with custom valid tokens in modern-light */
  XrmDatabase db = XrmGetDatabase(dpy);
  if (db == NULL) {
    XrmInitialize();
    db = XrmGetStringDatabase("");
  }
  XrmPutStringResource(&db, "*appearanceProfile", "modern_light");
  XrmPutStringResource(&db, "*accentColor", "#0b6e4f");
  XrmPutStringResource(&db, "*accentForeground", "#ffffff");
  XrmPutStringResource(&db, "*dangerColor", "#9e1b32");
  XrmPutStringResource(&db, "*surfaceBackground", "#f5f0e6");
  XrmSetDatabase(dpy, db);

  Widget board = XtVaCreateManagedWidget("board", xmBulletinBoardWidgetClass,
                                         shell, XmNwidth, 400, XmNheight, 300,
                                         NULL);
  Widget primary_btn = XtVaCreateManagedWidget(
      "primaryBtn", xmPushButtonWidgetClass, board,
      XmNappearanceRole, XmAPPEARANCE_ROLE_PRIMARY, NULL);
  Widget danger_btn = XtVaCreateManagedWidget(
      "dangerBtn", xmPushButtonWidgetClass, board,
      XmNappearanceRole, XmAPPEARANCE_ROLE_DESTRUCTIVE, NULL);
  Widget normal_btn = XtVaCreateManagedWidget(
      "normalBtn", xmPushButtonWidgetClass, board, NULL);
  Widget toggle_btn = XtVaCreateManagedWidget(
      "toggleBtn", xmToggleButtonWidgetClass, board, NULL);
  Widget field = XtVaCreateManagedWidget(
      "field", xmTextFieldWidgetClass, board, NULL);
  Widget link = XtVaCreateManagedWidget(
      "link", xmLinkWidgetClass, board, NULL);

  XtRealizeWidget(shell);
  XSync(dpy, False);

  CHECK(warning_count == 0);

  Pixel surf_bg = 0, text_fg = 0;
  Pixel prim_bg = 0, prim_fg = 0, prim_arm = 0, prim_hl = 0;
  Pixel dng_bg = 0, dng_fg = 0, dng_arm = 0;
  Pixel norm_bg = 0, norm_arm = 0, tog_sel = 0, fld_bg = 0, lnk_fg = 0;

  XtVaGetValues(board, XmNbackground, &surf_bg, XmNforeground, &text_fg, NULL);
  XtVaGetValues(primary_btn, XmNbackground, &prim_bg, XmNforeground, &prim_fg,
                XmNarmColor, &prim_arm, XmNhighlightColor, &prim_hl, NULL);
  XtVaGetValues(danger_btn, XmNbackground, &dng_bg, XmNforeground, &dng_fg,
                XmNarmColor, &dng_arm, NULL);
  XtVaGetValues(normal_btn, XmNbackground, &norm_bg, XmNarmColor, &norm_arm, NULL);
  XtVaGetValues(toggle_btn, XmNselectColor, &tog_sel, NULL);
  XtVaGetValues(field, XmNbackground, &fld_bg, NULL);
  XtVaGetValues(link, XmNforeground, &lnk_fg, NULL);

  unsigned int r = 0, g = 0, b = 0;
  query_rgb8(dpy, cmap, prim_bg, &r, &g, &b);
  /* Verify green-teal accent (#0b6e4f) was applied on primary button & toggle */
  CHECK(g > r + 40 && b > r + 20);
  CHECK(tog_sel == prim_bg);
  CHECK(prim_hl == prim_bg);
  CHECK(prim_arm != prim_bg);
  CHECK(dng_arm != dng_bg);
  CHECK(norm_bg != surf_bg);
  CHECK(fld_bg != surf_bg);

  /* Verify WCAG contrast ratios on valid custom tokens */
  CHECK(pixel_contrast(dpy, cmap, surf_bg, text_fg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, prim_bg, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, prim_fg, prim_bg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, dng_bg, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, dng_fg, dng_bg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, lnk_fg, surf_bg) >= 4.45);

  /* 2. Low-contrast tokens in modern-light must clamp and emit XmeWarning */
  warning_count = 0;
  XrmPutStringResource(&db, "*surfaceBackground", "#353535");
  XrmPutStringResource(&db, "*accentColor", "#e8eaed");
  XrmPutStringResource(&db, "*accentForeground", "#404040");
  XrmPutStringResource(&db, "*dangerColor", "#f0f0f0");
  XrmSetDatabase(dpy, db);
  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
  CHECK(warning_count >= 3);

  XtVaGetValues(board, XmNbackground, &surf_bg, XmNforeground, &text_fg, NULL);
  XtVaGetValues(primary_btn, XmNbackground, &prim_bg, XmNforeground, &prim_fg,
                XmNhighlightColor, &prim_hl, NULL);
  XtVaGetValues(danger_btn, XmNbackground, &dng_bg, XmNforeground, &dng_fg, NULL);

  CHECK(pixel_contrast(dpy, cmap, surf_bg, text_fg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, prim_hl, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, prim_fg, prim_bg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, dng_bg, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, dng_fg, dng_bg) >= 4.45);

  /* 3. Transition to modern-dark with low-contrast dark tokens and invalid color */
  warning_count = 0;
  XrmPutStringResource(&db, "*surfaceBackground", "#e5e5e5");
  XrmPutStringResource(&db, "*accentColor", "#252629");
  XrmPutStringResource(&db, "*accentForeground", "not_a_color_12345");
  XrmPutStringResource(&db, "*dangerColor", "also_invalid_color");
  XrmSetDatabase(dpy, db);
  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));
  CHECK(warning_count >= 3);

  XtVaGetValues(board, XmNbackground, &surf_bg, XmNforeground, &text_fg, NULL);
  XtVaGetValues(primary_btn, XmNbackground, &prim_bg, XmNforeground, &prim_fg,
                XmNhighlightColor, &prim_hl, NULL);
  XtVaGetValues(danger_btn, XmNbackground, &dng_bg, XmNforeground, &dng_fg, NULL);

  CHECK(pixel_contrast(dpy, cmap, surf_bg, text_fg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, prim_hl, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, prim_fg, prim_bg) >= 4.45);
  CHECK(pixel_contrast(dpy, cmap, dng_bg, surf_bg) >= 2.95);
  CHECK(pixel_contrast(dpy, cmap, dng_fg, dng_bg) >= 4.45);

  /* 4. Explicit widget resource still overrides semantic tokens */
  Widget explicit_btn = XtVaCreateManagedWidget(
      "explicitBtn", xmPushButtonWidgetClass, board,
      XmNappearanceRole, XmAPPEARANCE_ROLE_PRIMARY,
      XmNbackground, BlackPixelOfScreen(XtScreen(shell)), NULL);
  Pixel exp_bg = 0;
  XtVaGetValues(explicit_btn, XmNbackground, &exp_bg, NULL);
  CHECK(exp_bg == BlackPixelOfScreen(XtScreen(shell)));

  XtDestroyWidget(shell);
  XtCloseDisplay(dpy);
  XtDestroyApplicationContext(app);
  printf("PASS WP-47 semantic palette tokens and WCAG contrast guardrails\n");
  return 0;
}
