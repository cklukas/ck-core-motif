/* Native regression for the actual Earth demo, including its Xt timers. */
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>
#include <Xm/ScrollBarP.h>
static void check_loop(XtAppContext);
#define XtAppMainLoop check_loop
#define main earth_main
#include "../../demos/programs/earth/earth.c"
#undef main
#undef XtAppMainLoop

static int expected_speed = 5, expected_fps = 30;

static double seconds(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}
static void pump(double duration)
{
    double end = seconds() + duration;
    while (seconds() < end) {
        while (XtAppPending(app_con)) XtAppProcessEvent(app_con, XtIMAll);
        struct timespec pause = {0, 1000000};
        nanosleep(&pause, NULL);
    }
}
static void set_speed(int value)
{
    XmScaleCallbackStruct cb = {0};
    cb.value = value;
    speed_callback(NULL, NULL, &cb);
}
static void check_loop(XtAppContext context)
{
    Boolean override;
    Bool bounding, clip;
    int bx, by, cx, cy;
    unsigned bw, bh, cw, ch;
    Dimension width, height;
    XImage *image;
    (void)context;
    assert(AppData.speed == expected_speed && AppData.fps == expected_fps);
    if (expected_speed != 5 || expected_fps != 30) {
        assert((AppData.timeoutID != 0) == (expected_speed != 0));
        if (expected_fps == 60 && expected_speed == 5) {
            double start = seconds(), previous = angle;
            unsigned long frames = frame_count;
            pump(.5);
            assert(frame_count - frames >= 24 && frame_count - frames <= 32);
            assert(fabs(angle - previous - (seconds()-start)*EARTH_TAU/120) < .005);
        }
        puts("PASS: startup speed/FPS resources, timer ownership and independent cadence");
        XtDestroyWidget(toplevel);
        return;
    }
    assert(normalized_speed(INT_MIN) == -200 && normalized_speed(INT_MAX) == 200);
    assert(fabs(globe_advance(0, 120, 5)) < 1e-10);
    assert(fabs(globe_advance(0, 1, 5) - EARTH_TAU/120) < 1e-10);
    assert(fabs(globe_advance(0, 1, -5) - (EARTH_TAU-EARTH_TAU/120)) < 1e-10);
    assert(globe_advance(1, 60, 0) == 1);
    {
        const unsigned char samples[] = {0,0,0, 100,100,100, 200,200,200, 100,100,100};
        double rgb[3];
        globe_sample(samples, 2, 2, 0.25, 0.5, rgb);
        assert(fabs(rgb[0]-100) < 1e-10);
        globe_sample(samples, 2, 2, -0.25, 0, rgb);
        assert(fabs(rgb[0]-50) < 1e-10);
        assert(globe_project(0, 0, 100).coverage == 1);
        assert(globe_project(101, 0, 100).coverage == 0);
        assert(globe_project(100, 0, 100).coverage == 0.5);
    }
    XtVaGetValues(toplevel, XtNoverrideRedirect, &override, NULL);
    assert(!override);
    assert(XShapeQueryExtents(XtDisplay(draw), XtWindow(toplevel),
        &bounding, &bx, &by, &bw, &bh, &clip, &cx, &cy, &cw, &ch));
    assert(!bounding);
    {
        Widget scale;
        int minimum, maximum, value;
        {
            Position x, y;
            XtTranslateCoords(draw, 40, 40, &x, &y);
            XTestFakeMotionEvent(XtDisplay(draw), -1, x, y, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button3, True, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button3, False, CurrentTime);
            XFlush(XtDisplay(draw)); pump(.05);
        }
        scale = XtNameToWidget(toplevel, "*speed_scale");
        assert(scale);
        XtVaGetValues(scale, XmNminimum, &minimum, XmNmaximum, &maximum,
                      XmNvalue, &value, NULL);
        assert(minimum == -200 && maximum == 200 && value == 5);
        assert(XtParent(scale) == layout && XtParent(draw) == layout);
        assert(XtIsManaged(scale));
        pump(.05);
        if (modern) {
            Dimension w, h;
            XImage *shot;
            FILE *f;
            int x, y;
            Visual *visual = DefaultVisualOfScreen(XtScreen(draw));
            XtVaGetValues(layout, XmNwidth, &w, XmNheight, &h, NULL);
            shot = XGetImage(XtDisplay(layout), XtWindow(layout), 0, 0,
                             w, h, AllPlanes, ZPixmap);
            assert(shot);
            f = fopen("controls.ppm", "wb"); assert(f);
            fprintf(f, "P6\n%d %d\n255\n", w, h);
            for (y = 0; y < h; ++y) for (x = 0; x < w; ++x) {
                unsigned long pixel = XGetPixel(shot, x, y);
                fputc(channel(pixel, visual->red_mask), f);
                fputc(channel(pixel, visual->green_mask), f);
                fputc(channel(pixel, visual->blue_mask), f);
            }
            fclose(f); XDestroyImage(shot);
        }
        {
            Position scale_y;
            Dimension draw_height;
            XtVaGetValues(scale, XmNy, &scale_y, NULL);
            XtVaGetValues(draw, XmNheight, &draw_height, NULL);
            Dimension scale_height, layout_height;
            XtVaGetValues(scale, XmNheight, &scale_height, NULL);
            XtVaGetValues(layout, XmNheight, &layout_height, NULL);
            printf("inline geometry: draw=%u scale=%d+%u layout=%u\n", draw_height, scale_y, scale_height, layout_height);
            assert(scale_height > 40);
            assert(scale_y >= draw_height);
            assert(scale_y + scale_height <= layout_height);
        }
        {
            XmScrollBarWidget bar = (XmScrollBarWidget)XtNameToWidget(scale, "Scrollbar");
            Position x, y;
            int value;
            assert(bar && bar->scrollBar.slider_height >= 24);
            XtTranslateCoords((Widget)bar,
                bar->scrollBar.slider_x + bar->scrollBar.slider_width/2,
                bar->scrollBar.slider_y + bar->scrollBar.slider_height/2, &x, &y);
            XTestFakeMotionEvent(XtDisplay(draw), -1, x, y, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button1, True, CurrentTime);
            XFlush(XtDisplay(draw)); pump(.03);
            assert(!dragging); /* Slider input must not grab or spin the globe. */
            XTestFakeMotionEvent(XtDisplay(draw), -1, x + 60, y, CurrentTime);
            XFlush(XtDisplay(draw)); pump(.03);
            XTestFakeButtonEvent(XtDisplay(draw), Button1, False, CurrentTime);
            XFlush(XtDisplay(draw)); pump(.03);
            XmScaleGetValue(scale, &value);
            assert(value > 15 && AppData.speed == value);
            XmScaleSetValue(scale, 5); set_speed(5);
        }
        {
            Position x, y;
            XtTranslateCoords(draw, 40, 40, &x, &y);
            XTestFakeMotionEvent(XtDisplay(draw), -1, x, y, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button3, True, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button3, False, CurrentTime);
            XFlush(XtDisplay(draw)); pump(.05);
        }
        assert(!XtIsManaged(scale));
        pump(.05);
    }
    {
        double previous = angle, started = seconds(), elapsed;
        unsigned long frames = frame_count;
        pump(.5);
        elapsed = seconds() - started;
        assert(frame_count - frames >= 12 && frame_count - frames <= 17);
        assert(fabs(angle - previous - elapsed*EARTH_TAU/120) < .005);
        printf("cadence: %lu frames in %.3f seconds\n", frame_count - frames, elapsed);
        set_speed(0); previous = angle;
        frames = frame_count;
        assert(AppData.timeoutID == 0);
        pump(.2); assert(angle == previous && frame_count == frames);
        set_speed(-5); pump(.3); assert(angle < previous);
        set_speed(5); set_speed(0); set_speed(5); set_speed(0);
        previous = angle; frames = frame_count;
        pump(.2); assert(angle == previous && frame_count == frames && !AppData.timeoutID);
        /* A stalled loop advances by wall time, without rendering a backlog. */
        set_speed(5); previous = angle; frames = frame_count;
        { struct timespec pause = {0, 200000000}; nanosleep(&pause, NULL); }
        XtAppProcessEvent(app_con, XtIMTimer);
        assert(frame_count == frames + 1);
        assert(fabs(angle - previous - .2*EARTH_TAU/120) < .005);
        set_speed(0);
    }
    XtVaSetValues(toplevel, XtNwidth, 400, XtNheight, 240,
                  XtNx, 120, XtNy, 100, NULL);
    pump(.1);
    XtVaGetValues(draw, XmNwidth, &width, XmNheight, &height, NULL);
    assert(width == 400 && height == 240);
    image = XGetImage(XtDisplay(draw), XtWindow(draw), 0, 0,
                     width, height, AllPlanes, ZPixmap);
    assert(image);
    assert(XGetPixel(image, 0, 0) == BlackPixelOfScreen(XtScreen(draw)));
    /* The globe grows, remains centered and has clear rectangular margins. */
    assert(XGetPixel(image, 200, 20) != XGetPixel(image, 0, 0));
    assert(XGetPixel(image, 10, 120) == XGetPixel(image, 0, 0));
    if (modern) {
        unsigned long colors[1000];
        int count = 0, x, y, i;
        assert(texture_width == 2048 && texture_height == 1024);
        for (y = 20; y < 220 && count < 1000; y += 3)
            for (x = 100; x < 300 && count < 1000; x += 3) {
                unsigned long pixel = XGetPixel(image, x, y);
                for (i = 0; i < count && colors[i] != pixel; ++i) {}
                if (i == count) colors[count++] = pixel;
            }
        assert(count > 500); /* Detailed texture and continuous shading, not two colors. */
        {
            FILE *f = fopen("globe.ppm", "wb");
            assert(f); fprintf(f, "P6\n%d %d\n255\n", width, height);
            for (y = 0; y < height; ++y) for (x = 0; x < width; ++x) {
                unsigned long pixel = XGetPixel(image, x, y);
                fputc(channel(pixel, frame->red_mask), f);
                fputc(channel(pixel, frame->green_mask), f);
                fputc(channel(pixel, frame->blue_mask), f);
            }
            fclose(f);
        }
    }
    XDestroyImage(image);
    puts("PASS: Earth continuous projection, interpolation, 30 FPS timer, elapsed-time rotation, pause/resume/reverse, slider, drag-to-spin/release/hold, shell and resized rendering");
    {
        Position x, y;
        Dimension w, h;
        double previous_angle = angle;
        int i;
        XtTranslateCoords(draw, 200, 120, &x, &y);
        XTestFakeMotionEvent(XtDisplay(draw), -1, x, y, CurrentTime);
        for (i = 0; i < 5; ++i) {
            XTestFakeButtonEvent(XtDisplay(draw), Button5, True, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button5, False, CurrentTime);
        }
        XFlush(XtDisplay(draw)); pump(.08);
        assert(zoom > .905 && zoom < .907 && angle == previous_angle);
        XtVaGetValues(draw, XmNwidth, &w, XmNheight, &h, NULL);
        assert(w == 400 && h == 240);
        if (modern) {
            int left = -1, right = -1;
            for (i = 0; i < 400; ++i)
                if (projection[120*400+i].coverage > 0) {
                    if (left < 0) left = i;
                    right = i;
                }
            assert(left > 100 && right < 300 && left + right == 399);
        }
        for (i = 0; i < 5; ++i) {
            XTestFakeButtonEvent(XtDisplay(draw), Button4, True, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button4, False, CurrentTime);
        }
        XFlush(XtDisplay(draw)); pump(.08);
        assert(fabs(zoom-1.0) < .00001);
        for (i = 0; i < 160; ++i) {
            XTestFakeButtonEvent(XtDisplay(draw), Button5, True, CurrentTime);
            XTestFakeButtonEvent(XtDisplay(draw), Button5, False, CurrentTime);
        }
        XFlush(XtDisplay(draw)); pump(.15);
        assert(zoom == .05);
        if (modern) {
            int left = -1, right = -1;
            for (i = 0; i < 400; ++i)
                if (projection[120*400+i].coverage > 0) {
                    if (left < 0) left = i;
                    right = i;
                }
            assert(left + right == 399 && right - left < 12);
        }
        zoom = 1; render_earth();
    }
    {
        Position x, y;
        int value, direction;
        double previous;
        Display *display = XtDisplay(draw);
        XtTranslateCoords(draw, 200, 120, &x, &y);
        for (direction = -1; direction <= 1; direction += 2) {
            set_speed(0);
            XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
            XTestFakeButtonEvent(display, Button1, True, CurrentTime);
            XFlush(display); pump(.03);
            assert(dragging && !AppData.timeoutID);
            previous = angle;
            XTestFakeMotionEvent(display, -1, x + direction*8, y, CurrentTime);
            XFlush(display); pump(.04);
            assert(angle != previous);
            XTestFakeButtonEvent(display, Button1, False, CurrentTime);
            XFlush(display); pump(.03);
            assert(!dragging && AppData.timeoutID);
            assert(AppData.speed * direction < 0);
            XmScaleGetValue(speed_scale, &value);
            assert(value == AppData.speed);
        }
        /* Press stops automatic motion immediately, even a late callback.
         * A left click without a drag leaves the globe stopped. */
        set_speed(35);
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
        XTestFakeButtonEvent(display, Button1, True, CurrentTime);
        XFlush(display); pump(.03);
        assert(dragging && !AppData.timeoutID && !AppData.speed);
        previous = angle;
        { unsigned long frames = frame_count;
          NextBitmap(NULL, NULL); pump(.15);
          assert(angle == previous && frame_count == frames); }
        XTestFakeButtonEvent(display, Button1, False, CurrentTime);
        XFlush(display); pump(.08);
        assert(!dragging && !AppData.speed && !AppData.timeoutID && angle == previous);
        XmScaleGetValue(speed_scale, &value); assert(value == 0);
        /* Hold after moving, then release: stop instead of reusing old velocity. */
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
        XTestFakeButtonEvent(display, Button1, True, CurrentTime);
        XFlush(display); pump(.03);
        XTestFakeMotionEvent(display, -1, x+12, y, CurrentTime);
        XFlush(display); pump(.2);
        XTestFakeButtonEvent(display, Button1, False, CurrentTime);
        XFlush(display); pump(.03);
        assert(!dragging && !AppData.speed && !AppData.timeoutID);
        /* Pointer grab delivers release even beyond the drawing area. */
        XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
        XTestFakeButtonEvent(display, Button1, True, CurrentTime);
        XFlush(display); pump(.03);
        XTestFakeMotionEvent(display, -1, x-230, y, CurrentTime);
        XFlush(display); pump(.03);
        XTestFakeButtonEvent(display, Button1, False, CurrentTime);
        XFlush(display); pump(.03);
        assert(!dragging && AppData.speed > 0 && AppData.speed <= 200);
        set_speed(0);
    }
    set_speed(5);
    assert(AppData.timeoutID);
    XtDestroyWidget(toplevel);
    assert(!AppData.timeoutID && !texture && !frame && !projection && !draw);
}
int main(int argc, char **argv)
{
    int i;
    for (i = 1; i + 1 < argc; ++i) {
        if (!strcmp(argv[i], "-speed")) {
            long v = strtol(argv[++i], NULL, 10);
            expected_speed = v < -200 ? -200 : v > 200 ? 200 : (int)v;
        } else if (!strcmp(argv[i], "-fps")) {
            long v = strtol(argv[++i], NULL, 10);
            expected_fps = v < 1 ? 1 : v > 120 ? 120 : (int)v;
        }
    }
    return earth_main(argc, argv);
}
