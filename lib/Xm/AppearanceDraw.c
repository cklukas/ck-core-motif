/* Shared modern control drawing. Classic/high-contrast retain their renderers. */
#include "PaintI.h"
#include <Xm/XmP.h>
#include <Xm/Appearance.h>
#include "XmI.h"
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include <Xm/NotebookP.h>
#include <Xm/ScrolledWP.h>
#include <Xm/ScrollBarP.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "AppearanceDrawI.h"

Dimension
_XmAppearanceArrowExtent(Widget widget, XmAppearanceProfile profile,
                         XmAppearanceDensity density,
                         Dimension highlight, Dimension shadow)
{
    unsigned long margins = 2UL * ((unsigned long)highlight + shadow);
    unsigned long extent = 15UL + margins;
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        unsigned long target = _XmDpiDefaultDimension(widget,
            density == XmAPPEARANCE_DENSITY_COMPACT ? 28 : 36);
        extent = MAX(target, (unsigned long)_XmDpiDefaultDimension(widget,22) + margins);
    }
    return (Dimension)MIN(extent,USHRT_MAX);
}

XtGeometryResult
_XmArrowButtonQueryGeometry(Widget widget, XtWidgetGeometry *request,
                            XtWidgetGeometry *reply)
{
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    Dimension highlight,shadow;
    profile = _XmAppearanceGeometryProfile(XtDisplayOfObject(widget));
    density = _XmAppearanceGeometryDensity(XtDisplayOfObject(widget));
    XtVaGetValues(widget,XmNhighlightThickness,&highlight,XmNshadowThickness,&shadow,NULL);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = reply->height = _XmAppearanceArrowExtent(widget,profile,density,highlight,shadow);
    return XmeReplyToQueryGeometry(widget,request,reply);
}

/* Sample at pixel centers and mirror each horizontal span. XFillArc's
 * endpoint convention is not reflection-symmetric for these filled corners. */
typedef void (*RoundedSpanProc)(void *, int, int, int, int);
static void
RoundedRectangleSpans(int x, int y, int width, int height, int radius,
                      RoundedSpanProc span, void *closure)
{
    int row, inset;
    uint64_t diameter_squared;
    if (width <= 0 || height <= 0) return;
    radius = MAX(0, MIN(radius, MIN(width, height) / 2));
    if (radius == 0) {
        span(closure, x, y, width, height);
        return;
    }
    diameter_squared = 4ULL * (uint64_t)radius * (uint64_t)radius;
    inset = radius;
    for (row = 0; row < radius; ++row) {
        int64_t dy = 2LL * (radius - row) - 1;
        while (inset > 0) {
            int64_t dx = 2LL * (radius - inset + 1) - 1;
            if ((uint64_t)(dx * dx + dy * dy) > diameter_squared) break;
            --inset;
        }
        if (width > 2 * inset) {
            span(closure, x + inset, y + row,
                           width - 2 * inset, 1);
            span(closure, x + inset,
                           y + height - 1 - row, width - 2 * inset, 1);
        }
    }
    if (height > 2 * radius)
        span(closure, x, y + radius,
                       width, height - 2 * radius);
}

typedef struct { Display *display; Drawable drawable; GC gc; } RoundedFill;
static void
FillRoundedSpan(void *closure, int x, int y, int width, int height)
{
    RoundedFill *fill = closure;
    XFillRectangle(fill->display, fill->drawable, fill->gc, x, y, width, height);
}

void
_XmAppearanceFillRoundedRectangle(Display *display, Drawable drawable, GC gc,
                                  int x, int y, int width, int height, int radius)
{
    RoundedFill fill = {display, drawable, gc};
    RoundedRectangleSpans(x,y,width,height,radius,FillRoundedSpan,&fill);
}

static void
UnionRoundedSpan(void *closure, int x, int y, int width, int height)
{
    Region region = closure;
    XRectangle rectangle = {x,y,width,height};
    XUnionRectWithRegion(&rectangle,region,region);
}

Region
_XmAppearanceRoundedRectangleRegion(int x, int y, int width, int height, int radius)
{
    Region region = XCreateRegion();
    RoundedRectangleSpans(x,y,width,height,radius,UnionRoundedSpan,region);
    return region;
}

/* Fit the complete painted diameter inside the widget, reducing decorative
 * padding for undersized allocations instead of producing negative origins. */
void
_XmAppearanceRingBounds(int width, int height, int spacing, int highlight,
                        int maximum, int *x, int *y, int *diameter)
{
    int edge = MAX(0, MIN(width, height));
    int inset = MIN(MAX(0, MAX(spacing, highlight)), MAX(0, (edge - 1) / 2));
    *diameter = MIN(MAX(0, maximum), MAX(0, edge - 2 * inset));
    *x = inset;
    *y = (height - *diameter) / 2;
}

typedef struct {
    Display *display;
    Drawable drawable;
    GC gc;
    XRectangle rectangles[64];
    int count, x, y, diameter;
    double start_x, start_y, end_x, end_y;
    Boolean full, reflex;
} RingRaster;

static void
RingSpan(RingRaster *raster, int left, int right, int row)
{
    XRectangle *rectangle;
    if (left > right) return;
    rectangle = &raster->rectangles[raster->count++];
    *rectangle = (XRectangle){raster->x + left, raster->y + row, right - left + 1, 1};
    if (raster->count == XtNumber(raster->rectangles)) {
        XFillRectangles(raster->display, raster->drawable, raster->gc,
                        raster->rectangles, raster->count);
        raster->count = 0;
    }
}

/* Clip an integer interval to a pixel-center half-plane a*x+b >= 0.
 * Binary search avoids rounding the intersection differently at opposite
 * quadrant boundaries. No per-pixel trigonometry or image allocation. */
static void
RingHalfPlane(int *left, int *right, double a, double b)
{
    int low = *left, high = *right, middle;
    const double epsilon = 1e-9;
    if (a == 0) {
        if (b < -epsilon) *right = *left - 1;
    } else if (a > 0) {
        if (a * high + b < -epsilon) {*left = high + 1; return;}
        while (low < high) {
            middle = low + (high - low) / 2;
            if (a * middle + b >= -epsilon) high = middle;
            else low = middle + 1;
        }
        *left = low;
    } else {
        if (a * low + b < -epsilon) {*right = low - 1; return;}
        while (low < high) {
            middle = low + (high - low + 1) / 2;
            if (a * middle + b >= -epsilon) low = middle;
            else high = middle - 1;
        }
        *right = high;
    }
}

static void
RingSectorSpan(RingRaster *raster, int left, int right, int row)
{
    int a = left, b = right, c = left, d = right;
    double dy = raster->diameter - 1 - 2 * row;
    double origin = 1 - raster->diameter;
    if (left > right) return;
    if (raster->full) {RingSpan(raster, left, right, row); return;}
    RingHalfPlane(&a, &b, -2 * raster->start_y,
                  raster->start_x * dy - raster->start_y * origin);
    RingHalfPlane(&c, &d, 2 * raster->end_y,
                  raster->end_y * origin - raster->end_x * dy);
    if (!raster->reflex) RingSpan(raster, MAX(a,c), MIN(b,d), row);
    else if (a > b) RingSpan(raster,c,d,row);
    else if (c > d) RingSpan(raster,a,b,row);
    else if (MAX(a,c) <= MIN(b,d) + 1) RingSpan(raster,MIN(a,c),MAX(b,d),row);
    else {RingSpan(raster,a,b,row); RingSpan(raster,c,d,row);}
}

void
_XmAppearanceDrawRing(Display *display, Drawable drawable, GC gc,
                      int x, int y, int diameter, int thickness,
                      int start, int sweep)
{
    const int turn = 360 * 64;
    const double radians = 3.14159265358979323846 / (180.0 * 64.0);
    RingRaster raster = {0};
    int row, inner;
    int64_t outer_squared, inner_squared;
    if (diameter <= 0 || diameter > SHRT_MAX || thickness <= 0 || !sweep ||
        x < SHRT_MIN || y < SHRT_MIN || x > SHRT_MAX - diameter ||
        y > SHRT_MAX - diameter) return;
    thickness = MIN(thickness, (diameter + 1) / 2);
    inner = MAX(0, diameter - 2 * thickness);
    outer_squared = (int64_t)diameter * diameter;
    inner_squared = (int64_t)inner * inner;
    raster.display=display; raster.drawable=drawable; raster.gc=gc;
    raster.x=x; raster.y=y; raster.diameter=diameter;
    raster.full = sweep >= turn || sweep <= -turn;
    start %= turn;
    if (!raster.full && sweep < 0) {start = (start + sweep) % turn; sweep = -sweep;}
    raster.reflex = sweep > turn / 2;
    if (!raster.full) {
        raster.start_x=cos(start*radians); raster.start_y=sin(start*radians);
        raster.end_x=cos((start+sweep)*radians); raster.end_y=sin((start+sweep)*radians);
    }
    for (row=0; row<diameter; ++row) {
        int64_t dy=diameter-1-2*row;
        int extent=(int)sqrt((double)(outer_squared-dy*dy));
        int left=(diameter-extent)/2, right=diameter-1-left;
        int64_t hole=inner_squared-dy*dy-1;
        if (hole >= 0) {
            int hole_extent=(int)sqrt((double)hole);
            int hole_left=(diameter-hole_extent)/2;
            int hole_right=diameter-1-hole_left;
            RingSectorSpan(&raster,left,hole_left-1,row);
            RingSectorSpan(&raster,hole_right+1,right,row);
        } else RingSectorSpan(&raster,left,right,row);
    }
    if (raster.count) XFillRectangles(display,drawable,gc,raster.rectangles,raster.count);
}

/* Focus belongs to the reserved outer highlight band. Use the same mirrored
 * corner raster as the face without repainting application-drawn contents. */
Boolean
_XmAppearanceDrawButtonFocus(Widget widget, Boolean focused)
{
    XmAppearanceProfile profile;
    Dimension highlight;
    Pixel parent_background, accent;
    Display *display = XtDisplayOfObject(widget);
    Drawable drawable = (XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget)));
    int x = XtIsWidget(widget) ? 0 : XtX(widget);
    int y = XtIsWidget(widget) ? 0 : XtY(widget);
    int width = XtWidth(widget), height = XtHeight(widget);
    XGCValues values;
    XRectangle outer;
    int radius;
    Region clip, inside;
    GC gc;
    if (!XmGetAppearanceProfile(display, &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    if (XtParent(widget) && XmIsNotebook(XtParent(widget)) && widget->core.constraints) {
        XmNotebookConstraintPtr constraint = (XmNotebookConstraintPtr)widget->core.constraints;
        if (constraint->notebook.child_type == XmMAJOR_TAB ||
            constraint->notebook.child_type == XmMINOR_TAB) return False;
    }
    XtVaGetValues(widget, XmNhighlightThickness, &highlight,
        XmNhighlightColor, &accent, XmNbackground, &parent_background, NULL);
    if (!drawable || !highlight || !width || !height) return True;
    if (XtParent(widget)) XtVaGetValues(XtParent(widget), XmNbackground, &parent_background, NULL);
    radius = MIN((int)_XmDpiDefaultDimension(widget, 4) + highlight,
                 MIN(width,height)/2);
    outer = (XRectangle){x, y, width, height};
    clip = XCreateRegion(); XUnionRectWithRegion(&outer, clip, clip);
    if (width > 2*highlight && height > 2*highlight) {
        /* Preserve the rounded face, not its rectangular bounding box.
         * Concentric outer/inner corners keep the same stroke width on
         * straight edges and diagonals without touching face contents. */
        inside = _XmAppearanceRoundedRectangleRegion(x+highlight, y+highlight,
            width-2*highlight, height-2*highlight, MAX(0,radius-highlight));
        XSubtractRegion(clip, inside, clip); XDestroyRegion(inside);
    }
    values.foreground = parent_background;
    values.graphics_exposures = False;
    gc = XCreateGC(display, drawable, GCForeground | GCGraphicsExposures, &values);
    if (!gc) {
        XDestroyRegion(clip);
        return True;
    }
    XSetRegion(display, gc, clip); XDestroyRegion(clip);
    XFillRectangle(display, drawable, gc, x, y, width, height);
    if (focused) {
        XSetForeground(display, gc, accent);
        _XmAppearanceFillRoundedRectangle(display, drawable, gc, x, y, width, height, radius);
    }
    XFreeGC(display, gc);
    return True;
}

/* Ordinary buttons share their shape. Notebook tabs retain their joined-page
 * renderer; callers retain menu painting and application-drawn content. */
Boolean
_XmAppearanceDrawButton(Widget widget, Pixel fill, Boolean emphasized,
                        Boolean paint_background)
{
    XmAppearanceProfile profile;
    Dimension highlight,shadow;
    Pixel border,accent,parent_background;
    Display *display=XtDisplayOfObject(widget);
    Drawable drawable=(XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget)));
    int x,y,width,height,stroke,radius,band;
    XGCValues values;
    XRectangle outer,inner;
    Region clip,inside;
    GC gc;
    if (!XmGetAppearanceProfile(display,&profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    if (_XmContainerDrawDisclosure(widget)) return True;
    if (XtParent(widget) && XmIsNotebook(XtParent(widget)) && widget->core.constraints) {
        XmNotebookConstraintPtr constraint=(XmNotebookConstraintPtr)widget->core.constraints;
        if (constraint->notebook.child_type == XmMAJOR_TAB ||
            constraint->notebook.child_type == XmMINOR_TAB) return False;
    }
    if (!drawable) return True;
    XtVaGetValues(widget,XmNhighlightThickness,&highlight,XmNshadowThickness,&shadow,
        XmNbottomShadowColor,&border,XmNhighlightColor,&accent,NULL);
    parent_background=fill;
    if (XtParent(widget)) XtVaGetValues(XtParent(widget),XmNbackground,&parent_background,NULL);
    x=(XtIsWidget(widget)?0:XtX(widget))+highlight;
    y=(XtIsWidget(widget)?0:XtY(widget))+highlight;
    width=(int)XtWidth(widget)-2*highlight; height=(int)XtHeight(widget)-2*highlight;
    if (width<=0 || height<=0 || x<0 || y<0 || x+width>SHRT_MAX || y+height>SHRT_MAX)
        return True;
    emphasized=emphasized && XtIsSensitive(widget);
    stroke=MIN((int)shadow,MIN(width,height)/2);
    if (emphasized) stroke=MIN(MAX(stroke,(int)_XmDpiDefaultDimension(widget,2)),MIN(width,height)/2);
    radius=MIN((int)_XmDpiDefaultDimension(widget,4),MIN(width,height)/2);
    outer=(XRectangle){x,y,width,height};
    clip=XCreateRegion();XUnionRectWithRegion(&outer,clip,clip);
    if (!paint_background) {
        band=MAX(stroke,radius);
        if (width>2*band && height>2*band) {
            inner=(XRectangle){x+band,y+band,width-2*band,height-2*band};
            inside=XCreateRegion();XUnionRectWithRegion(&inner,inside,inside);
            XSubtractRegion(clip,inside,clip);XDestroyRegion(inside);
        }
    }
    values.foreground=parent_background;
    values.graphics_exposures=False;
    gc=XCreateGC(display,drawable,GCForeground|GCGraphicsExposures,&values);
    if (!gc) {
        XDestroyRegion(clip);
        return True;
    }
    XSetRegion(display,gc,clip);XDestroyRegion(clip);
    XFillRectangle(display,drawable,gc,x,y,width,height);
    XSetForeground(display,gc,emphasized?accent:border);
    _XmAppearanceFillRoundedRectangle(display,drawable,gc,x,y,width,height,radius);
    XSetForeground(display,gc,fill);
    _XmAppearanceFillRoundedRectangle(display,drawable,gc,x+stroke,y+stroke,width-2*stroke,height-2*stroke,MAX(0,radius-stroke));
    XFreeGC(display,gc);
    /* Face repaint clears the corner cutouts too; restore the complete
     * curved focus band after each face/frame update. */
    if ((XmIsPrimitive(widget) && ((XmPrimitiveWidget)widget)->primitive.highlighted) ||
        (XmIsGadget(widget) && ((XmGadget)widget)->gadget.highlighted))
        _XmAppearanceDrawButtonFocus(widget, True);
    return True;
}

/* Paint the full face on every state transition, including drag-out while
 * armed. Widget and gadget callers supply interaction state, never GC identity. */
Boolean
_XmAppearanceDrawArrowRegion(Widget widget, int x, int y, int width, int height,
                             Dimension shadow, unsigned char direction,
                             Boolean pressed, Boolean sensitive)
{
    XmAppearanceProfile profile;
    Display *display = XtDisplayOfObject(widget);
    Drawable drawable = (XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget)));
    Pixel foreground, background, border, fill, glyph;
    int stroke, radius, pad, side;
    XGCValues values;
    XRectangle clip;
    GC gc;
    if (!XmGetAppearanceProfile(display, &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    if (!drawable) return True;
    XtVaGetValues(widget, XmNforeground, &foreground, XmNbackground, &background,
        XmNbottomShadowColor, &border, NULL);
    sensitive = sensitive && XtIsSensitive(widget);
    if (width <= 0 || height <= 0 || x < 0 || y < 0 ||
        x+width > SHRT_MAX || y+height > SHRT_MAX) return True;
    pressed = pressed && sensitive;
    fill = pressed ? foreground : background;
    glyph = sensitive ? (pressed ? background : foreground) : border;
    stroke = MIN((int)shadow, MIN(width,height)/2);
    radius = MIN((int)_XmDpiDefaultDimension(widget,4),MIN(width,height)/2);
    values.foreground = background;
    values.graphics_exposures = False;
    gc = XCreateGC(display,drawable,GCForeground|GCGraphicsExposures,&values);
    if (!gc) return True;
    clip = (XRectangle){x,y,width,height};
    XSetClipRectangles(display,gc,0,0,&clip,1,Unsorted);
    XFillRectangle(display,drawable,gc,x,y,width,height);
    XSetForeground(display,gc,pressed ? foreground : border);
    _XmAppearanceFillRoundedRectangle(display,drawable,gc,x,y,width,height,radius);
    XSetForeground(display,gc,fill);
    _XmAppearanceFillRoundedRectangle(display,drawable,gc,x+stroke,y+stroke,width-2*stroke,
                     height-2*stroke,MAX(0,radius-stroke));
    /* Embedded steppers have half-height regions. Reduce decorative padding
     * there before reducing the glyph, while keeping a clear edge margin. */
    pad = stroke + MIN(MAX(1,(int)_XmDpiDefaultDimension(widget,4)),
                       MAX(1,(MIN(width,height)-2*stroke)/6));
    side = MIN((int)_XmDpiDefaultDimension(widget,14),MIN(width,height)-2*pad);
    if (side >= 5) {
        int line = MIN(MAX(1,(int)_XmDpiDefaultDimension(widget,2)),MAX(1,side/4));
        XSetForeground(display,gc,glyph);
        _XmAppearanceDrawChevron(display,drawable,gc,
            x+(width-side)/2,y+(height-side)/2,side,side,line,direction);
    }
    XFreeGC(display,gc);
    return True;
}

Boolean
_XmAppearanceDrawArrowButton(Widget widget, unsigned char direction, Boolean pressed)
{
    Dimension highlight, shadow;
    XtVaGetValues(widget, XmNhighlightThickness, &highlight,
                  XmNshadowThickness, &shadow, NULL);
    Boolean painted = _XmAppearanceDrawArrowRegion(widget,
        (XtIsWidget(widget) ? 0 : XtX(widget)) + highlight,
        (XtIsWidget(widget) ? 0 : XtY(widget)) + highlight,
        (int)XtWidth(widget) - 2*highlight,
        (int)XtHeight(widget) - 2*highlight,
        shadow, direction, pressed, XtIsSensitive(widget));
    /* Face painting clears the rectangular corner area inside the highlight
     * band. Restore the curved ring after every arm/leave/release paint too,
     * including borderless arrows; expose is not the only repaint path. */
    if (painted && ((XmIsPrimitive(widget) && ((XmPrimitiveWidget)widget)->primitive.highlighted) ||
                    (XmIsGadget(widget) && ((XmGadget)widget)->gadget.highlighted)))
        _XmAppearanceDrawButtonFocus(widget, True);
    return painted;
}

/* Fill nested shapes rather than stroking outside their allocated bounds. */
static void
ToggleShape(Display *display, Drawable drawable, GC gc, int x, int y,
            int edge, unsigned char type, int radius)
{
    if (edge <= 0) return;
    if (type == XmONE_OF_MANY_DIAMOND) {
        XPoint points[4] = {{x + edge/2, y}, {x + edge-1, y + edge/2},
                            {x + edge/2, y + edge-1}, {x, y + edge/2}};
        XFillPolygon(display, drawable, gc, points, 4, Convex, CoordModeOrigin);
    } else {
        _XmAppearanceFillRoundedRectangle(display, drawable, gc, x, y,
            edge, edge, type == XmN_OF_MANY ? radius : edge / 2);
    }
}

/* Use the application's actual colormap and colors, including private maps. */
static Pixel
ToggleGlyphPixel(Widget widget, Pixel fill, Pixel foreground, Pixel background)
{
    XColor colors[3];
    Colormap colormap;
    double light[3], ratio[2];
    Widget owner = XtIsWidget(widget) ? widget : XtParent(widget);
    XtVaGetValues(owner, XmNcolormap, &colormap, NULL);
    colors[0].pixel = fill; colors[1].pixel = foreground; colors[2].pixel = background;
    XQueryColors(XtDisplayOfObject(widget), colormap, colors, 3);
    for (int i = 0; i < 3; ++i)
        light[i] = (0.2126*colors[i].red + 0.7152*colors[i].green +
                    0.0722*colors[i].blue)/65535.0;
    for (int i = 0; i < 2; ++i)
        ratio[i] = (MAX(light[0], light[i+1]) + 0.05) /
                   (MIN(light[0], light[i+1]) + 0.05);
    return ratio[0] > ratio[1] ? foreground : background;
}

Boolean
_XmAppearanceDrawToggle(Widget widget, const XmAppearanceToggle *toggle)
{
    XmAppearanceProfile profile;
    Display *display = XtDisplayOfObject(widget);
    Drawable drawable = (XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget)));
    int left, top, right, bottom, x, y, edge, stroke, radius;
    Pixel border, fill, glyph;
    Boolean selected = toggle->state != XmUNSET;
    Boolean sensitive = XtIsSensitive(widget);
    Boolean box;
    unsigned char indicator = toggle->indicator;
    XGCValues values;
    XRectangle clip;
    GC gc;
    if (!XmGetAppearanceProfile(display, &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    if (indicator == XmINDICATOR_NONE || !drawable) return True;
    if (indicator == XmINDICATOR_FILL) indicator = XmINDICATOR_CHECK_BOX;
    else if (indicator == XmINDICATOR_BOX) indicator = XmINDICATOR_3D_BOX;
    box = (indicator & 0x0f) != 0 || toggle->type != XmN_OF_MANY;
    left = XtIsWidget(widget) ? 0 : XtX(widget);
    top = XtIsWidget(widget) ? 0 : XtY(widget);
    right = left + XtWidth(widget) - toggle->inset;
    bottom = top + XtHeight(widget) - toggle->inset;
    left += toggle->inset; top += toggle->inset;
    edge = MIN(toggle->edge, MIN(right-left, bottom-top));
    if (edge <= 0) return True;
    x = MAX(left, MIN(toggle->x, right-edge));
    y = MAX(top, MIN(toggle->y, bottom-edge));
    stroke = MAX(1, (int)_XmDpiDefaultDimension(widget, 1));
    stroke = MIN(stroke, MAX(1, edge/4));
    radius = MIN((int)_XmDpiDefaultDimension(widget, 3), edge/2);
    border = sensitive ? (selected ? toggle->select :
        (toggle->armed ? toggle->foreground : toggle->border)) : toggle->border;
    fill = selected && toggle->fill && toggle->type == XmN_OF_MANY ?
        border : toggle->unselect;
    if (toggle->unselect == (Pixel)-1 &&
        !(selected && toggle->fill && toggle->type == XmN_OF_MANY))
        fill = toggle->background;
    glyph = selected && toggle->fill && toggle->type == XmN_OF_MANY ?
        ToggleGlyphPixel(widget, fill, toggle->foreground, toggle->background) : border;
    values.foreground = toggle->background;
    values.graphics_exposures = False;
    gc = XCreateGC(display, drawable, GCForeground | GCGraphicsExposures, &values);
    if (!gc) return True;
    clip.x = x; clip.y = y; clip.width = clip.height = edge;
    XSetClipRectangles(display, gc, 0, 0, &clip, 1, Unsorted);
    XFillRectangle(display, drawable, gc, x, y, edge, edge);
    if (!toggle->visible && !selected) { XFreeGC(display, gc); return True; }
    if (box) {
        XSetForeground(display, gc, border);
        ToggleShape(display, drawable, gc, x, y, edge, toggle->type, radius);
        XSetForeground(display, gc, fill);
        ToggleShape(display, drawable, gc, x + stroke, y + stroke,
                    edge - 2*stroke, toggle->type, MAX(0, radius-stroke));
    }
    if (selected && edge >= 5) {
        int pad = MAX(stroke + 1, edge/4);
        int line = MAX(1, (int)_XmDpiDefaultDimension(widget, 2));
        line = MIN(line, MAX(1, edge/5));
        XSetForeground(display, gc, glyph);
        XSetLineAttributes(display, gc, line, LineSolid, CapRound, JoinRound);
        if (toggle->state == XmINDETERMINATE) {
            XDrawLine(display, drawable, gc, x + pad, y + edge/2,
                      x + edge-1-pad, y + edge/2);
        } else if (toggle->type != XmN_OF_MANY) {
            ToggleShape(display, drawable, gc, x + pad, y + pad,
                        edge - 2*pad, toggle->type, 0);
        } else if (indicator & XmINDICATOR_CROSS_GLYPH) {
            XDrawLine(display, drawable, gc, x+pad, y+pad, x+edge-1-pad, y+edge-1-pad);
            XDrawLine(display, drawable, gc, x+edge-1-pad, y+pad, x+pad, y+edge-1-pad);
        } else if (indicator & XmINDICATOR_CHECK_GLYPH) {
            XPoint points[3] = {{x + edge/5, y + edge/2},
                {x + 2*edge/5, y + 3*edge/4}, {x + 4*edge/5, y + edge/4}};
            XDrawLines(display, drawable, gc, points, 3, CoordModeOrigin);
        }
    }
    XFreeGC(display, gc);
    return True;
}


/* Flat frames use filled edge rectangles so every corner belongs to the
 * same bounded surface, including the bottom/right endpoint pixels. */
Boolean
_XmAppearanceDrawFrameRegion(Widget widget, int x, int y,
                             int width, int height, int stroke)
{
    XmAppearanceProfile profile;
    Pixel border;
    XGCValues values;
    GC gc;
    XRectangle edges[4];
    profile = _XmAppearanceGeometryProfile(XtDisplayOfObject(widget));
    if (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK)
        return False;
    if (!(XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget)))) return True;
    stroke = MIN(stroke, MIN(width, height)/2);
    if (stroke <= 0 || x < 0 || y < 0 || x+width > SHRT_MAX || y+height > SHRT_MAX)
        return True;
    XtVaGetValues(widget, XmNbottomShadowColor, &border, NULL);
    values.foreground = border;
    gc = XCreateGC(XtDisplayOfObject(widget), (XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget))), GCForeground, &values);
    if (!gc) return True;
    edges[0] = (XRectangle){x, y, width, stroke};
    edges[1] = (XRectangle){x, y+height-stroke, width, stroke};
    edges[2] = (XRectangle){x, y+stroke, stroke, height-2*stroke};
    edges[3] = (XRectangle){x+width-stroke, y+stroke, stroke, height-2*stroke};
    XFillRectangles(XtDisplayOfObject(widget), (XtIsWidget(widget) ? _XmPaintDrawable(widget) : _XmPaintDrawable(XtParent(widget))), gc, edges, 4);
    XFreeGC(XtDisplayOfObject(widget), gc);
    return True;
}

/* A scrolled surface owns its outer frame. Child windows paint only the
 * no separate border between content and navigators. */
Boolean
_XmAppearanceConnectedScrolledWindow(Widget widget)
{
    XmAppearanceProfile profile;
    if (!widget || !XmIsScrolledWindow(widget)) return False;
    profile = _XmAppearanceGeometryProfile(XtDisplay(widget));
    return (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) &&
        ((XmScrolledWindowWidget)widget)->swindow.pad == 0 &&
        ((XmScrolledWindowWidget)widget)->manager.shadow_thickness > 0;
}

Boolean
_XmAppearanceDrawScrolledChildFrame(Widget widget)
{
    Widget parent = XtParent(widget);
    XmScrolledWindowWidget sw;
    Widget content;
    if (!_XmAppearanceConnectedScrolledWindow(parent)) return False;
    sw=(XmScrolledWindowWidget)parent;
    content=sw->swindow.ScrollPolicy == XmAUTOMATIC ?
        (Widget)sw->swindow.ClipWindow : sw->swindow.WorkWindow;
    return widget == content || widget == (Widget)sw->swindow.hScrollBar ||
        widget == (Widget)sw->swindow.vScrollBar;
}

Boolean
_XmAppearanceDrawFieldFrame(Widget widget)
{
    XmPrimitiveWidget field = (XmPrimitiveWidget)widget;
    int inset = field->primitive.highlight_thickness;
    if (_XmAppearanceDrawScrolledChildFrame(widget)) return True;
    return _XmAppearanceDrawFrameRegion(widget, inset, inset,
        (int)field->core.width-2*inset, (int)field->core.height-2*inset,
        field->primitive.shadow_thickness);
}

/* Mirror the sampled coordinate before testing the distance to one 45-degree
 * arm. This gives identical pixels on both sides, including even stroke widths,
 * without relying on XDrawLines endpoint/cap rasterization. */
void
_XmAppearanceDrawChevron(Display *display, Drawable drawable, GC gc,
                         int x, int y, int width, int height, int stroke,
                         unsigned char direction)
{
    Boolean vertical = direction == XmARROW_UP || direction == XmARROW_DOWN;
    int side = MIN(width, height), arm, wide, narrow, gx, gy, gw, gh;
    XRectangle spans[64];
    int count = 0;
    if (side < 3) return;
    stroke = MAX(1, MIN(stroke, side / 3));
    arm = MAX(1, (side - stroke) / 2);
    wide = 2 * arm + stroke;
    narrow = arm + stroke;
    gw = vertical ? wide : narrow;
    gh = vertical ? narrow : wide;
    gx = x + (width - gw) / 2;
    gy = y + (height - gh) / 2;
    for (int row = 0; row < gh; ++row) {
        int run = -1;
        for (int col = 0; col <= gw; ++col) {
            Boolean inside = False;
            if (col < gw) {
                double u = vertical ? col + 0.5 : row + 0.5;
                double v = vertical ? row + 0.5 : col + 0.5;
                if (direction == XmARROW_UP || direction == XmARROW_LEFT)
                    v = narrow - v;
                double dx = fabs(u - wide / 2.0);
                double dy = v - (arm + stroke / 2.0);
                double t = (dx - dy) / (2.0 * arm);
                t = MAX(0.0, MIN(t, 1.0));
                dx -= t * arm;
                dy += t * arm;
                inside = dx * dx + dy * dy <= stroke * stroke / 4.0;
            }
            if (inside && run < 0) run = col;
            if (!inside && run >= 0) {
                spans[count++] = (XRectangle){gx + run, gy + row, col - run, 1};
                run = -1;
                if (count == XtNumber(spans)) {
                    XFillRectangles(display, drawable, gc, spans, count);
                    count = 0;
                }
            }
        }
    }
    if (count) XFillRectangles(display, drawable, gc, spans, count);
}

/* Submenu disclosure is a glyph on the menu surface, without a beveled tile. */
Boolean
_XmAppearanceDrawCascadeIndicator(Widget widget, int x, int y, int width,
                                  int height, unsigned char direction)
{
    XmAppearanceProfile profile;
    Pixel foreground;
    XGCValues values;
    GC gc;
    Display *display = XtDisplayOfObject(widget);
    Drawable drawable = XtIsWidget(widget) ? _XmPaintDrawable(widget) :
                                             _XmPaintDrawable(XtParent(widget));
    if (!XmGetAppearanceProfile(display, &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    if (!drawable || width < 1 || height < 1) return True;
    XtVaGetValues(widget, XtIsSensitive(widget) ? XmNforeground : XmNbottomShadowColor,
                  &foreground, NULL);
    int side = MIN((int)_XmDpiDefaultDimension(widget, 12), MIN(width,height));
    values.foreground = foreground;
    gc = XCreateGC(display, drawable, GCForeground, &values);
    if (!gc) return True;
    _XmAppearanceDrawChevron(display, drawable, gc, x+(width-side)/2,
        y+(height-side)/2, side, side,
        MIN(MAX(1,(int)_XmDpiDefaultDimension(widget,2)),MAX(1,side/4)), direction);
    XFreeGC(display, gc);
    return True;
}

/* Choose readable status/progress text using linear-light luminance. Xt caches
 * the black/white conversion in the widget's actual screen/colormap context. */
Pixel
_XmAppearanceTextOnFill(Widget widget, Pixel fill)
{
    Widget context = XtIsWidget(widget) ? widget : XtParent(widget);
    Colormap map;XColor color;double c[3];Pixel result;
    XtVaGetValues(context,XmNcolormap,&map,XmNforeground,&result,NULL);
    color.pixel=fill;XQueryColor(XtDisplayOfObject(widget),map,&color);
    c[0]=color.red/65535.0;c[1]=color.green/65535.0;c[2]=color.blue/65535.0;
    for(unsigned i=0;i<3;i++)c[i]=c[i]<=0.04045?c[i]/12.92:pow((c[i]+0.055)/1.055,2.4);
    const char *name=0.2126*c[0]+0.7152*c[1]+0.0722*c[2]<0.179?"white":"black";
    XrmValue from={(unsigned int)strlen(name)+1,(XPointer)name},to={sizeof(result),(XPointer)&result};
    (void)XtConvertAndStore(context,XmRString,&from,XmRPixel,&to);
    return result;
}

Boolean
_XmAppearancePickerSize(Widget widget,XmRenderTable table,const char *sample,Dimension *width,Dimension *height)
{
    XmAppearanceProfile profile=_XmAppearanceGeometryProfile(XtDisplay(widget));
    if(profile!=XmAPPEARANCE_MODERN_LIGHT&&profile!=XmAPPEARANCE_MODERN_DARK)return False;
    XmString label=XmStringCreateLocalized((char*)sample);Dimension w=0,h=0;
    if(label&&table)XmStringExtent(table,label,&w,&h);XmStringFree(label);
    *width=(Dimension)MIN(USHRT_MAX,MAX((unsigned long)_XmDpiDefaultDimension(widget,160),(unsigned long)w+_XmDpiDefaultDimension(widget,52)));
    *height=(Dimension)MIN(USHRT_MAX,MAX((unsigned long)_XmDpiDefaultDimension(widget,_XmAppearanceGeometryDensity(XtDisplay(widget))==XmAPPEARANCE_DENSITY_COMPACT?28:36),(unsigned long)h+_XmDpiDefaultDimension(widget,16)));
    return True;
}
Boolean
_XmAppearanceDrawPickerFrame(Widget widget,Boolean spin)
{
    if(!_XmAppearanceDrawFieldFrame(widget))return False;
    XmPrimitiveWidget field=(XmPrimitiveWidget)widget;
    Drawable drawable=_XmPaintDrawable(widget);if(!drawable)return True;
    int edge=MIN((int)_XmDpiDefaultDimension(widget,12),MIN((int)field->core.width/3,(int)field->core.height/3));
    if(edge<3)return True;
    int x=(int)field->core.width-_XmDpiDefaultDimension(widget,10)-edge;
    XGCValues values;values.foreground=XtIsSensitive(widget)?field->primitive.foreground:field->primitive.bottom_shadow_color;
    GC gc=XCreateGC(XtDisplay(widget),drawable,GCForeground,&values);if(!gc)return True;
    int stroke=MAX(1,_XmDpiDefaultDimension(widget,1));
    if(spin){_XmAppearanceDrawChevron(XtDisplay(widget),drawable,gc,x,field->core.height/4-edge/2,edge,edge,stroke,XmARROW_UP);_XmAppearanceDrawChevron(XtDisplay(widget),drawable,gc,x,3*field->core.height/4-edge/2,edge,edge,stroke,XmARROW_DOWN);}
    else _XmAppearanceDrawChevron(XtDisplay(widget),drawable,gc,x,((int)field->core.height-edge)/2,edge,edge,stroke,XmARROW_DOWN);
    XFreeGC(XtDisplay(widget),gc);return True;
}
