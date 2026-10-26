/* $XConsortium: ScalTics.c /main/7 1995/12/06 21:39:58 cde-sun $ */
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



#include <Xm/Xm.h>
#include <Xm/XmosP.h>  /* for allocate local */
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <limits.h>
#include <stdint.h>
#include "XmI.h"

/************************************************************************
 *
 *  XmScaleSetTicks
 *
 ************************************************************************/
void
XmScaleSetTicks(
        Widget scale,
        int big_every,
        Cardinal num_med,
        Cardinal num_small, 
	Dimension  size_big,
	Dimension  size_med,
	Dimension  size_small)
{
    Widget *sep ;
    Cardinal n, i, j, k, sep_num;
    int real_num_big, real_num_med, real_num_small;
    Arg args[5];
    int max, min ;
    unsigned char orient ;
    char * dim_res ;

    if (scale == (Widget) NULL)
        return;

    _XmWidgetToAppContext(scale);

    _XmAppLock(app);

    /* Some checking first.  A non-positive interval would either divide by
       zero or make the generated tick counts nonsensical. */
    if (size_big == 0 || big_every <= 0) {
        _XmAppUnlock(app);
        return ;
    }
    if (size_med == 0) num_med = 0 ;
    if (size_small == 0) num_small = 0 ;

    /* big_every is the number of values between big tics, while
       num_med and num_small are the number of tics between resp.
       big and med values */

    /* compute num_big first */
    n = 0 ;
    XtSetArg(args[n], XmNmaximum, &max); n++;
    XtSetArg(args[n], XmNminimum, &min); n++;
    XtSetArg(args[n], XmNorientation, &orient); n++;
    XtGetValues(scale, args, n);
    
    /* Promote before subtracting: applications may legitimately configure
       the scale with the full signed-int range. */
    if (max < min) { _XmAppUnlock(app); return ; }
    {
        intmax_t range = (intmax_t)max - (intmax_t)min;
        intmax_t big_count = (range / (intmax_t)big_every) + 1;
        if (big_count < 2 || big_count > INT_MAX) {
            _XmAppUnlock(app);
            return ;
        }
        real_num_big = (int)big_count;
    }
    if (real_num_big < 2) { _XmAppUnlock(app); return ; }

    if ((size_t)num_med > (size_t)INT_MAX / (size_t)(real_num_big - 1)) {
        _XmAppUnlock(app);
        return ;
    }
    real_num_med = (real_num_big - 1) * (int)num_med;
    if (real_num_med > INT_MAX - real_num_big + 1) {
        _XmAppUnlock(app);
        return ;
    }
    {
        size_t small_base = (size_t)real_num_big + (size_t)real_num_med - 1;
        if (small_base > (size_t)INT_MAX ||
            (size_t)num_small > (size_t)INT_MAX / small_base) {
            _XmAppUnlock(app);
            return ;
        }
        real_num_small = (int)(small_base * (size_t)num_small);
    }
    {
        size_t total = (size_t)real_num_big + (size_t)real_num_med +
                       (size_t)real_num_small;
        if (total > UINT_MAX || total > SIZE_MAX / sizeof(Widget)) {
            _XmAppUnlock(app);
            return ;
        }
        sep_num = (Cardinal)total;
    }

    sep = (Widget*) ALLOCATE_LOCAL(sep_num * sizeof(Widget));
    if (sep == NULL) {
	_XmAppUnlock(app);
	return;
    }

    if (orient == XmHORIZONTAL) {
	dim_res = XmNheight;
	orient = XmVERTICAL;
    } else {
	dim_res = XmNwidth ;
	orient = XmHORIZONTAL;
    }
    XtSetArg(args[0], XmNmargin, 0); 
    XtSetArg(args[1], XmNorientation, orient); 

    sep_num = 0 ;
    for (i=0; i < real_num_big; i++) {
	n = 2 ;
	XtSetArg(args[n], dim_res, size_big); n++ ;
	sep[sep_num] = XmCreateSeparatorGadget(scale, "BigTic", args, n); 
	if (sep[sep_num] != (Widget) NULL)
	    sep_num++;

	if (i == real_num_big - 1) break ;

	for (k=0; k < num_small; k++) {
	    n = 2 ;
	    XtSetArg(args[n], dim_res, size_small); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSINGLE_LINE); n++;
	    sep[sep_num] = XmCreateSeparatorGadget(scale, "SmallTic", 
						   args, n); 
	    if (sep[sep_num] != (Widget) NULL)
		sep_num++;
	}

	for (j=0; j < num_med; j++) {
	    n = 2 ;
	    XtSetArg(args[n], dim_res, size_med); n++;
	    sep[sep_num] = XmCreateSeparatorGadget(scale, "MedTic", args, n); 
	    if (sep[sep_num] != (Widget) NULL)
		sep_num++;
	    for (k=0; k < num_small; k++) {
		n = 2 ;
		XtSetArg(args[n], dim_res, size_small); n++;
		XtSetArg(args[n], XmNseparatorType, XmSINGLE_LINE); n++;
		sep[sep_num] = XmCreateSeparatorGadget(scale, "SmallTic", 
						       args, n); 
		if (sep[sep_num] != (Widget) NULL)
		    sep_num++;
	    }
	    
	}
    }

    XtManageChildren(sep, sep_num);			
    DEALLOCATE_LOCAL((char*)sep);

    _XmAppUnlock(app);
}


