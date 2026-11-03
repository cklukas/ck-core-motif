/* $XConsortium: DrHiDash.c /main/5 1995/07/15 20:50:42 drk $ */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "XmI.h"
#include "DrawI.h"
#include <limits.h>

static short bounded_segment_coordinate(long long value)
{
   if (value < SHRT_MIN) return SHRT_MIN;
   if (value > SHRT_MAX) return SHRT_MAX;
   return (short)value;
}

static unsigned int bounded_line_width(int value)
{
   return value < 0 ? 0U : (unsigned int)value;
}

/****************************_XmDrawHighlight***************************
 *
 * This function modifies the given gc, which therefore needs to be created
 *   using XCreateGC or XtAllocateGC.
 *
 ***************************************************************************/

void _XmDrawHighlight(Display *display, Drawable d, 
			  GC gc, 
#if NeedWidePrototypes
                          int x, int y, 
			  int width, int height,
			  int highlight_thickness,
#else
                          Position x, Position y, 
			  Dimension width, Dimension height,
			  Dimension highlight_thickness,
#endif /* NeedWidePrototypes */
                   int line_style)
{
   XSegment seg[4];
   int draw_width = width;
   int draw_height = height;
   int thickness = highlight_thickness;
   int half_hl, cor;
   XGCValues gcvalues;

   if (!d || !highlight_thickness || !width || !height) return ;
   if (thickness > draw_width) thickness = draw_width;
   if (thickness > draw_height) thickness = draw_height;
   half_hl = thickness / 2;
   cor = thickness % 2;

   /* the XmList dash case relies on this particular order of X segments */
   
   seg[0].x1 = seg[2].x1 = bounded_segment_coordinate(x);
   seg[0].y1 = seg[0].y2 = bounded_segment_coordinate((long long)y + half_hl);
   seg[0].x2 = bounded_segment_coordinate((long long)x + draw_width - thickness);
   seg[1].x1 = seg[1].x2 = bounded_segment_coordinate((long long)x + draw_width - half_hl - cor);
   seg[1].y1 = seg[3].y1 = bounded_segment_coordinate(y);
   seg[3].y2 = bounded_segment_coordinate((long long)y + draw_height - half_hl);
   seg[2].y1 = seg[2].y2 = bounded_segment_coordinate((long long)y + draw_height - half_hl - cor);
   seg[3].x1 = seg[3].x2 = bounded_segment_coordinate((long long)x + half_hl);
   seg[2].x2 = bounded_segment_coordinate((long long)x + draw_width);
   seg[1].y2 = bounded_segment_coordinate((long long)y + draw_height);

   /* first save the current values we want to change */
   XGetGCValues(display, gc,
		GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle,
		&gcvalues);
   /* change them and draw the lines */
   XSetLineAttributes(display, gc, (unsigned int)thickness, line_style,
		      CapButt, JoinMiter);
   XDrawSegments (display, d, gc, seg, 4);

   /* put them back */
   XSetLineAttributes(display, gc,
		      bounded_line_width(gcvalues.line_width), gcvalues.line_style,
		      gcvalues.cap_style, gcvalues.join_style);
  
   /** note that the above is a hack, a read-only GC shoudl not 
     be modified, period */
}
