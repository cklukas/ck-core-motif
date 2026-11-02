/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 * 
 */
/*
 * HISTORY
 */
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: earth.c /main/5 1995/07/14 09:41:16 drk $"
#endif
#endif
/***************************************************************************
 *  earth.
 *   This demo illustrates timer-driven bitmap animation and Motif.
 *   Author: Daniel Dardailler. 
 *
 *   Effects by: Andrew deBlois
 ***************************************************************************/
#define APP_CLASS "XmdEarth"   

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <X11/keysym.h>
#include <Xm/Appearance.h>
#include "globe.h"
#include <Xm/XmAll.h>
#include <X11/Xutil.h>
#include "terre.xbm"               /* can cause compilation problem with
				      some gcc version, due of the size of 
				      the bitmap array */
/*** Application default stuff */
typedef struct {
  int          speed; /* tenths of a revolution per minute */
  int          fps;
  XtIntervalId timeoutID;
  Pixel        foreground ;
  Pixel        background ;
} ApplicationData, *ApplicationDataPtr;
ApplicationData AppData;

#define XmNspeed  "speed"
#define XmCSpeed  "Speed"
static XtResource resources[] = {
    { "fps", "Fps", XmRInt, sizeof(int),
      XtOffsetOf(ApplicationData, fps), XmRImmediate, (XtPointer)30 },
    { 
	XmNspeed, XmCSpeed, XmRInt, sizeof(int),
	XtOffsetOf (ApplicationData, speed), 
	XmRImmediate, (caddr_t) 5 },

    /* overwrite the default colors (doing that will lead to weird
       behavior if you specify -fg or -bg on the command line, but
       I really don't care, it's easier to use the standard names) */
    {
	XmNforeground, XmCForeground, XmRPixel, sizeof (Pixel),
	XtOffsetOf (ApplicationData, foreground),
	XmRString, "brown"},
    {
	XmNbackground, XmCBackground, XmRPixel, sizeof (Pixel),
	XtOffsetOf (ApplicationData, background),
	XmRString, "turquoise"},

};

static XrmOptionDescRec options[] = {
    {"-speed", "*speed", XrmoptionSepArg, NULL},
    {"-fps", "*fps", XrmoptionSepArg, NULL},
} ;
/*** end of Application default stuff */

static XtAppContext app_con;
static Widget       toplevel, draw, layout, speed_scale ;
static GC           gc ;
static int          n ;
static Boolean modern;
static double angle = 0, last_tick, next_frame;
static unsigned long frame_count;
static unsigned char *texture;
static int texture_width, texture_height;
static XImage *frame;
static GlobePoint *projection;
static int frame_width, frame_height;
static double zoom = 1.0, frame_zoom;
static Boolean dragging;
static int drag_x, drag_saved_speed;
static double drag_radius, drag_phase, drag_distance, drag_start_angle;
static struct { uint32_t time; double phase; } drag_samples[16];
static int drag_sample_count;
#ifndef EARTH_MAP_FILE
#define EARTH_MAP_FILE "world-map.xpm"
#endif
#ifndef EARTH_SOURCE_MAP_FILE
#define EARTH_SOURCE_MAP_FILE EARTH_MAP_FILE
#endif

static double monotonic_seconds(void)
{
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC, &now);
  return now.tv_sec + now.tv_nsec / 1e9;
}

static unsigned channel(unsigned long pixel, unsigned long mask)
{
  if (!mask) return 0;
  while (!(mask & 1)) { mask >>= 1; pixel >>= 1; }
  return (unsigned)((pixel & mask) * 255 / mask);
}

static unsigned long pack_channel(unsigned value, unsigned long mask)
{
  unsigned shift = 0;
  if (!mask) return 0;
  while (!(mask & 1)) { mask >>= 1; ++shift; }
  return ((value * mask + 127) / 255) << shift;
}

static Boolean load_texture(void)
{
  Display *display = XtDisplay(draw);
  Screen *screen = XtScreen(draw);
  Pixmap map;
  XImage *source;
  Window root;
  int x, y;
  unsigned w, h, border, depth;
  if (DefaultVisualOfScreen(screen)->class != TrueColor) return False;
  map = XmGetPixmapByDepth(screen, EARTH_MAP_FILE, 0, 0,
                          DefaultDepthOfScreen(screen));
  if (map == XmUNSPECIFIED_PIXMAP)
    map = XmGetPixmapByDepth(screen, EARTH_SOURCE_MAP_FILE, 0, 0,
                            DefaultDepthOfScreen(screen));
  if (map == XmUNSPECIFIED_PIXMAP) return False;
  XGetGeometry(display, map, &root, &x, &y, &w, &h, &border, &depth);
  if (w < 2 || h < 2 || w > 8192 || h > 4096) {
    XmDestroyPixmap(screen, map); return False;
  }
  source = XGetImage(display, map, 0, 0, w, h, AllPlanes, ZPixmap);
  XmDestroyPixmap(screen, map);
  if (!source) return False;
  texture = malloc((size_t)w * h * 3);
  if (!texture) { XDestroyImage(source); return False; }
  texture_width = w; texture_height = h;
  for (y = 0; y < (int)h; ++y) for (x = 0; x < (int)w; ++x) {
    unsigned long pixel = XGetPixel(source, x, y);
    unsigned char *rgb = texture + ((size_t)y*w+x)*3;
    rgb[0] = channel(pixel, DefaultVisualOfScreen(screen)->red_mask);
    rgb[1] = channel(pixel, DefaultVisualOfScreen(screen)->green_mask);
    rgb[2] = channel(pixel, DefaultVisualOfScreen(screen)->blue_mask);
  }
  XDestroyImage(source);
  return True;
}


static void destroy_callback(Widget widget, XtPointer closure, XtPointer call)
{
  (void)closure; (void)call;
  if (dragging) XtUngrabPointer(widget, CurrentTime);
  dragging = False;
  if (AppData.timeoutID) XtRemoveTimeOut(AppData.timeoutID);
  AppData.timeoutID = 0;
  if (gc) XtReleaseGC(widget, gc);
  gc = NULL;
  if (frame) XDestroyImage(frame);
  frame = NULL;
  free(projection); projection = NULL;
  free(texture); texture = NULL;
  draw = NULL;
  XtAppSetExitFlag(app_con);
}

static void    Syntax() ;
static void    input_callback();
static void    expose_callback();
static void    speed_callback() ;
static void    NextBitmap ();

/* Speed is tenths of RPM; presentation cadence is independently set in FPS. */
static int normalized_speed(int speed)
{
  return speed < -200 ? -200 : speed > 200 ? 200 : speed;
}

static void advance_animation(double now)
{
  angle = globe_advance(angle, fmax(0, now - last_tick), AppData.speed);
  last_tick = now;
  n = (int)(angle * 30 / EARTH_TAU) % 30;
}

static void schedule_animation(void)
{
  double now = monotonic_seconds();
  if (AppData.timeoutID) XtRemoveTimeOut(AppData.timeoutID);
  AppData.timeoutID = 0;
  if (AppData.speed && !dragging) {
    double remaining = next_frame - now;
    unsigned long milliseconds = remaining > 0 ? (unsigned long)ceil(remaining*1000) : 1;
    AppData.timeoutID = XtAppAddTimeOut(app_con, milliseconds, NextBitmap, NULL);
  }
}

static void render_modern(void)
{
  Dimension width, height;
  Pixel background;
  XColor bg;
  int x, y, c;
  double bg_rgb[3], offset = angle / EARTH_TAU - 0.08;
  Display *display = XtDisplay(draw);
  Screen *screen = XtScreen(draw);
  XtVaGetValues(draw, XmNwidth, &width, XmNheight, &height,
                XmNbackground, &background, NULL);
  if (!frame || width != frame_width || height != frame_height || frame_zoom != zoom) {
    double radius = fmax(1, ((width < height ? width : height)/2.0 - 16) * zoom);
    if (frame) XDestroyImage(frame);
    free(projection); projection = NULL;
    frame = XCreateImage(display, DefaultVisualOfScreen(screen),
        DefaultDepthOfScreen(screen), ZPixmap, 0, NULL, width, height, 32, 0);
    if (!frame) return;
    frame->data = calloc(height, frame->bytes_per_line);
    projection = calloc((size_t)width * height, sizeof(*projection));
    if (!frame->data || !projection) {
      XDestroyImage(frame); frame = NULL; free(projection); projection = NULL; return;
    }
    frame_width = width; frame_height = height; frame_zoom = zoom;
    for (y = 0; y < height; ++y) for (x = 0; x < width; ++x)
      projection[(size_t)y*width+x] = globe_project(
          x + 0.5 - width/2.0, y + 0.5 - height/2.0, radius);
  }
  bg.pixel = background;
  XQueryColor(display, DefaultColormapOfScreen(screen), &bg);
  bg_rgb[0] = bg.red/257.0; bg_rgb[1] = bg.green/257.0; bg_rgb[2] = bg.blue/257.0;
  for (y = 0; y < height; ++y) for (x = 0; x < width; ++x) {
    GlobePoint *p = &projection[(size_t)y*width+x];
    unsigned long pixel = background;
    if (p->coverage > 0) {
      double rgb[3];
      unsigned color[3];
      globe_sample(texture, texture_width, texture_height, p->u + offset, p->v, rgb);
      for (c = 0; c < 3; ++c)
        color[c] = (unsigned)(rgb[c]*p->light*p->coverage + bg_rgb[c]*(1-p->coverage) + 0.5);
      pixel = pack_channel(color[0], frame->red_mask) |
              pack_channel(color[1], frame->green_mask) |
              pack_channel(color[2], frame->blue_mask);
    }
    XPutPixel(frame, x, y, pixel);
  }
  /* One complete image transfer: never clear between animation frames. */
  XPutImage(display, XtWindow(draw), gc, frame, 0, 0, 0, 0, width, height);
}

/* Scale the original bitmap uniformly and keep the globe centered. The shell
 * remains rectangular so the window manager owns movement and resizing. */
static void render_earth(void)
{
  Dimension width, height;
  Pixel background;
  XImage *image;
  int x, y, size, left, top;
  if (!draw || !XtIsRealized(draw) || !gc) return;
  if (modern) { render_modern(); return; }
  XtVaGetValues(draw, XmNwidth, &width, XmNheight, &height,
                XmNbackground, &background, NULL);
  size = (int)(((width < height ? width : height) - 32) * zoom);
  if (size < 1) return;
  left = (width - size) / 2;
  top = (height - size) / 2;
  image = XCreateImage(XtDisplay(draw),
      DefaultVisualOfScreen(XtScreen(draw)),
      DefaultDepthOfScreen(XtScreen(draw)), ZPixmap, 0, NULL,
      size, size, 32, 0);
  if (!image) return;
  image->data = calloc(size, image->bytes_per_line);
  if (!image->data) { XDestroyImage(image); return; }
  for (y = 0; y < size; ++y) {
    int sy = y * 64 / size;
    for (x = 0; x < size; ++x) {
      int sx = x * 64 / size;
      int dx = 2 * sx - 63, dy = 2 * sy - 63;
      Pixel pixel = background;
      if (dx * dx + dy * dy <= 64 * 64)
        pixel = (terre_bits[(n * 64 + sy) * 8 + sx / 8] &
                 (1 << (sx % 8))) ? AppData.foreground : AppData.background;
      XPutPixel(image, x, y, pixel);
    }
  }
  XClearWindow(XtDisplay(draw), XtWindow(draw));
  XPutImage(XtDisplay(draw), XtWindow(draw), gc, image,
            0, 0, left, top, size, size);
  XDestroyImage(image);
}

/* Direct manipulation uses the same continuous angle as timed animation.
 * Retain only recent motion, so release velocity follows the last gesture
 * rather than averaging a long drag or treating a held globe as a flick. */
static void record_drag(uint32_t time)
{
  if (drag_sample_count && drag_samples[drag_sample_count-1].time == time) {
    drag_samples[drag_sample_count-1].phase = drag_phase;
    return;
  }
  if (drag_sample_count == 16) {
    memmove(drag_samples, drag_samples + 1, 15 * sizeof(*drag_samples));
    --drag_sample_count;
  }
  drag_samples[drag_sample_count].time = time;
  drag_samples[drag_sample_count++].phase = drag_phase;
  while (drag_sample_count > 2 &&
         (uint32_t)(time - drag_samples[1].time) >= 120) {
    memmove(drag_samples, drag_samples + 1,
            (--drag_sample_count) * sizeof(*drag_samples));
  }
}

static void finish_drag(uint32_t time, Boolean cancel)
{
  int speed = 0;
  if (cancel) { angle = drag_start_angle; speed = drag_saved_speed; }
  else if (drag_distance >= 3) {
    uint32_t elapsed = time - drag_samples[0].time;
    uint32_t idle = time - drag_samples[drag_sample_count-1].time;
    speed = 0;
    if (idle < 150 && elapsed >= 10 && drag_sample_count > 1)
      speed = normalized_speed((int)lround(
          (drag_phase - drag_samples[0].phase) * 600000.0 / (EARTH_TAU * elapsed)));
  }
  dragging = False;
  XtUngrabPointer(draw, CurrentTime);
  AppData.speed = speed;
  if (speed_scale) XmScaleSetValue(speed_scale, speed);
  last_tick = monotonic_seconds();
  next_frame = last_tick + 1.0 / AppData.fps;
  schedule_animation();
  render_earth();
}

static void spin_handler(Widget widget, XtPointer closure, XEvent *event,
                         Boolean *dispatch)
{
  (void)closure; (void)dispatch;
  if (event->type == ButtonPress &&
      (event->xbutton.button == Button4 || event->xbutton.button == Button5)) {
    if (!dragging) {
      double requested = zoom * (event->xbutton.button == Button4 ? 1.02 : 1.0/1.02);
      requested = fmax(0.05, fmin(2.0, requested));
      if (requested != zoom) { zoom = requested; render_earth(); }
    }
    return;
  }
  if (event->type == ButtonPress && event->xbutton.button == Button1) {
    Dimension width, height;
    double x, y;
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    drag_radius = fmax(1, ((width < height ? width : height)/2.0 - 16) * zoom);
    x = event->xbutton.x - width/2.0; y = event->xbutton.y - height/2.0;
    if (x*x + y*y > drag_radius*drag_radius) return;
    if (XtGrabPointer(widget, False, PointerMotionMask | ButtonReleaseMask,
        GrabModeAsync, GrabModeAsync, None, None, event->xbutton.time) != GrabSuccess)
      return;
    /* Freeze the last displayed orientation immediately; do not advance
     * once more on press, which would visibly jump under the pointer. */
    dragging = True;
    drag_x = event->xbutton.x;
    drag_phase = drag_distance = 0; drag_start_angle = angle;
    drag_saved_speed = AppData.speed;
    AppData.speed = 0;
    if (speed_scale) XmScaleSetValue(speed_scale, 0);
    drag_sample_count = 0;
    record_drag((uint32_t)event->xbutton.time);
    schedule_animation(); /* Removes the timer until release. */
  } else if (event->type == MotionNotify && dragging) {
    drag_distance += abs(event->xmotion.x - drag_x);
    drag_phase -= (event->xmotion.x - drag_x) / drag_radius;
    drag_x = event->xmotion.x;
    angle = globe_advance(drag_start_angle + drag_phase, 0, 0);
    n = (int)(angle * 30 / EARTH_TAU) % 30;
    record_drag((uint32_t)event->xmotion.time);
    render_earth();
  } else if (event->type == ButtonRelease && event->xbutton.button == Button1 && dragging) {
    finish_drag((uint32_t)event->xbutton.time, False);
  } else if (event->type == KeyPress && dragging &&
             XLookupKeysym(&event->xkey, 0) == XK_Escape) {
    finish_drag((uint32_t)event->xkey.time, True);
  }
}

char *fallback[] = {
  "XmdEarth.title: Earth",
  "XmdEarth.width: 480",
  "XmdEarth.height: 480",
  NULL
};


int main(argc, argv) int argc; char **argv ;
/**************************************/
{
    Arg args[10] ;
    Cardinal n;

    XGCValues values;
    XtGCMask  valueMask;
    /* Initialize, using my options */
    toplevel = XtAppInitialize(&app_con, APP_CLASS, 
			       options, XtNumber(options),
			       &argc, argv, fallback, NULL, 0);

    /* if a bad option was reached, XtAppInitialize should let it */
    if (argc > 1) Syntax(argv[0]) ;

    /* read the programmer app default */
    XtGetApplicationResources(toplevel,
			      (XtPointer)&AppData,
			      resources,
			      XtNumber(resources),
			      NULL,
			      0);

    AppData.speed = normalized_speed(AppData.speed);
    AppData.fps = AppData.fps < 1 ? 1 : AppData.fps > 120 ? 120 : AppData.fps;
    {
      XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
      XmGetAppearanceProfile(XtDisplay(toplevel), &profile);
      modern = profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK;
    }
    XtVaSetValues(toplevel, XmNminWidth, 240, XmNminHeight, 240, NULL);

    /* Resizable drawing area and right-click speed dialog. */
    n = 0 ;
    layout = XtVaCreateManagedWidget("layout", xmFormWidgetClass, toplevel,
        XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
    draw = XtVaCreateWidget("draw", xmDrawingAreaWidgetClass, layout,
        XmNbackground, BlackPixelOfScreen(XtScreen(toplevel)),
        XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
        NULL);
    XtManageChild(draw);
    XtAddCallback(draw, XmNdestroyCallback, destroy_callback, NULL);
    XtAddEventHandler(draw, ButtonPressMask | ButtonReleaseMask |
        PointerMotionMask | KeyPressMask, False, spin_handler, NULL);
    XtAddCallback(draw,XmNinputCallback,(XtCallbackProc)input_callback,NULL);
    XtAddCallback(draw,XmNexposeCallback,(XtCallbackProc)expose_callback,NULL);

    XtAddCallback(draw,XmNresizeCallback,(XtCallbackProc)expose_callback,NULL);
    XtRealizeWidget(toplevel);

    /* create a GC for the earth colors */
    valueMask = GCForeground | GCBackground ;
    values.foreground = AppData.foreground ;
    values.background = AppData.background ;
    gc = XtGetGC ((Widget) toplevel, valueMask , &values);

    if (modern && !load_texture()) {
      fputs("earth: cannot load world-map.xpm on a TrueColor visual\n", stderr);
      return EXIT_FAILURE;
    }
    render_earth();
    last_tick = monotonic_seconds();
    next_frame = last_tick + 1.0 / AppData.fps;
    schedule_animation();

    XtAppMainLoop(app_con);

    return 0;    /* make compiler happy */
}

static void NextBitmap (client_data, id)
/****************************************************/
XtPointer client_data;
XtIntervalId *id;
{


  AppData.timeoutID = 0;

  if (dragging) return; /* Never let a late timeout fight pointer motion. */
  advance_animation(monotonic_seconds());
  render_earth();
  ++frame_count;
  next_frame += 1.0 / AppData.fps;
  /* Skip missed presentation deadlines instead of bursting through old frames. */
  if (next_frame <= monotonic_seconds()) next_frame = monotonic_seconds() + 1.0 / AppData.fps;
  schedule_animation();
}


static void expose_callback(widget, tag, callback_data)
/****************************************************/
Widget widget ;
XtPointer tag ;
XtPointer callback_data ;
{
    render_earth();
}

static void input_callback(widget, tag, callback_data)
/****************************************************/
Widget widget ;
XtPointer tag ;
XtPointer callback_data ;
{
  XmDrawingAreaCallbackStruct * dacb = (XmDrawingAreaCallbackStruct *) callback_data ;
  if (!dragging && dacb->event && dacb->event->type == ButtonPress &&
      dacb->event->xbutton.button == Button3) {
    if (speed_scale && XtIsManaged(speed_scale)) {
      XtUnmanageChild(speed_scale);
      XtVaSetValues(draw, XmNbottomAttachment, XmATTACH_FORM,
                    XmNbottomOffset, 0, NULL);
    } else {
      if (!speed_scale) {
        double ui_scale = 1.0;
        XmString title = XmStringCreateLocalized("Rotation (RPM; 0 pauses)");
        XmGetWidgetScale(draw, &ui_scale);
        speed_scale = XtVaCreateWidget("speed_scale", xmScaleWidgetClass, layout,
            XmNtitleString, title, XmNshowValue, True, XmNdecimalPoints, 1,
            XmNvalue, AppData.speed, XmNminimum, -200, XmNmaximum, 200,
            XmNorientation, XmHORIZONTAL,
            XmNscaleHeight, (int)ceil(32 * ui_scale),
            XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 16,
            XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 16,
            XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 12, NULL);
        XmStringFree(title);
        XtAddCallback(speed_scale, XmNdragCallback, speed_callback, NULL);
        XtAddCallback(speed_scale, XmNvalueChangedCallback, speed_callback, NULL);
      }
      XtManageChild(speed_scale);
      {
        XtWidgetGeometry preferred;
        XtQueryGeometry(speed_scale, NULL, &preferred);
        if (preferred.request_mode & CWHeight)
          XtVaSetValues(speed_scale, XmNheight, preferred.height, NULL);
      }
      XtVaSetValues(draw, XmNbottomAttachment, XmATTACH_WIDGET,
                    XmNbottomWidget, speed_scale, XmNbottomOffset, 8, NULL);
    }
  }
}


static void speed_callback(widget, tag, callback_data)
/******************************************************/
Widget widget ;
XtPointer tag ;
XtPointer callback_data ;
{
  XmScaleCallbackStruct * scb = 
    (XmScaleCallbackStruct *) callback_data ;


  advance_animation(monotonic_seconds());
  AppData.speed = normalized_speed(scb->value);
  next_frame = last_tick + 1.0 / AppData.fps;
  schedule_animation();

}


static void Syntax(com) char * com ;
/**********************************/
{
    fprintf(stderr, "%s understands all standard Xt options, plus:\n",com);
    fprintf(stderr, "       -fps:      1..120 (default 30 frames/second)\n");
    fprintf(stderr, "       -speed:    -200..200 tenths RPM (default 5 = 0.5 RPM; 0 pauses)\n");
}


