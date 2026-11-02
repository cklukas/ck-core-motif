/* $XConsortium: ScrollFrameT.c /main/5 1995/07/15 20:55:20 drk $ */
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
 */
/*
 * HISTORY
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>
#include <limits.h>

#include "XmI.h"
#include <Xm/TraitP.h>
#include <Xm/ScrollFrameT.h>
#include <stdint.h>
#include <Xm/NavigatorT.h>
#include "MessagesI.h"
#include "ScrollFramTI.h"

#define SWMessage3      _XmMMsgScrollFrameT_0000
#define SWMessage4      _XmMMsgScrollFrameT_0001



/************************************************************************
 *
 *  _XmSFAddNavigator convenience function
 *    Entering here, sf is an initialized scrollframe, and 
 *    scroll_frame_data is always valid (but the move_cb field can be 
 *    NULL, for pure APP_DEFINED case support).
 *
 *************************************<->***********************************/
void 
_XmSFAddNavigator(
    Widget sf,
    Widget nav,
    Mask dimMask,
    XmScrollFrameData scroll_frame_data)
{
    XmNavigatorTrait nav_trait ;
    XmNavigatorDataRec nav_data ;

    if ((sf == NULL) || (nav == NULL) || (scroll_frame_data == NULL))
	return;

    if ((nav_trait = (XmNavigatorTrait) 
	 XmeTraitGet((XtPointer) XtClass(nav), XmQTnavigator)) == NULL ||
	 nav_trait->setValue == NULL ||
	 (scroll_frame_data->move_cb != NULL &&
	  nav_trait->changeMoveCB == NULL)) {
	XmeWarning(sf, SWMessage3);
	return;
    }

	if (scroll_frame_data->move_cb)
	    nav_trait -> changeMoveCB(nav, scroll_frame_data->move_cb,
				      (XtPointer) scroll_frame_data->scrollable,
				      True) ;

	nav_data.valueMask = NavDimMask ;
	nav_data.dimMask = dimMask ;
	nav_trait -> setValue(nav, &nav_data, False);

	if (scroll_frame_data->num_nav_list == 
	    scroll_frame_data->num_nav_slots) {
	    Cardinal new_slots;
	    /* Allocate more space */
	    if (scroll_frame_data->num_nav_slots > (Cardinal)-1 - 2 ||
		(size_t)(scroll_frame_data->num_nav_slots + 2) >
		  SIZE_MAX / sizeof(Widget) ||
		(size_t)(scroll_frame_data->num_nav_slots + 2) >
		  (size_t)(Cardinal)-1 / sizeof(Widget))
		return;
	    new_slots = scroll_frame_data->num_nav_slots + 2;
	    WidgetList new_list =
		(WidgetList) XtRealloc((char*)scroll_frame_data->nav_list,
			new_slots * sizeof(Widget));
	    if (new_list == NULL)
		return;
	    scroll_frame_data->num_nav_slots = new_slots;
	    scroll_frame_data->nav_list = new_list;
	}

	scroll_frame_data->nav_list[scroll_frame_data->num_nav_list] = nav;
	scroll_frame_data->num_nav_list++;

}






/************************************************************************
 *
 *  _XmSFRemoveNavigator convenience function
 *
 *************************************<->***********************************/
void 
_XmSFRemoveNavigator(
    Widget sf,
    Widget nav,
    XmScrollFrameData scroll_frame_data)
{
    Cardinal	     position, i;
    XmNavigatorTrait nav_trait ;

    if ((scroll_frame_data == NULL) || (nav == NULL))
	return;

    if ((nav_trait = (XmNavigatorTrait) 
	 XmeTraitGet((XtPointer) XtClass(nav), XmQTnavigator)) != NULL) {

	/* remove the move callback */
	if (scroll_frame_data->move_cb && nav_trait->changeMoveCB != NULL)
	    nav_trait -> changeMoveCB(nav, scroll_frame_data->move_cb,
				(XtPointer) scroll_frame_data->scrollable,
				False) ;
    }  else {
	XmeWarning(sf, SWMessage4);
	return ;
    }

    if (scroll_frame_data->num_nav_list != 0 &&
	    scroll_frame_data->nav_list == NULL)
	return;

    for (position = 0; position<scroll_frame_data->num_nav_list; position++) {
        if (scroll_frame_data->nav_list[position] == nav) {
	    break;
	}
    }
    if (position == scroll_frame_data->num_nav_list) return;

    
    scroll_frame_data->num_nav_list--;
    for (i = position; i < scroll_frame_data->num_nav_list; i++) {
        scroll_frame_data->nav_list[i] = scroll_frame_data->nav_list[i+1];
    }

}


/************************************************************************
 *
 *  _XmSFUpdateNavigatorsValues convenience function
 *
 *************************************<->***********************************/
typedef struct {
    Widget widget;
    Boolean destroyed;
} NavigatorUpdateLifetime;

static void NavigatorUpdateDestroyed(Widget widget, XtPointer closure,
                                     XtPointer call_data)
{
    NavigatorUpdateLifetime *lifetime = (NavigatorUpdateLifetime *)closure;
    (void)widget;
    (void)call_data;
    lifetime->destroyed = True;
}

void
_XmSFUpdateNavigatorsValue(Widget sf, XmNavigatorData nav_data, Boolean notify)
{
    Cardinal count, current_count;
    Widget *nav_list, *current_list;
    XmScrollFrameTrait sf_trait;
    NavigatorUpdateLifetime owner = {sf, False};
    NavigatorUpdateLifetime *snapshot;

    if (sf == NULL)
        return;
    sf_trait = (XmScrollFrameTrait)
        XmeTraitGet((XtPointer)XtClass(sf), XmQTscrollFrame);
    if (sf_trait == NULL || sf_trait->getInfo == NULL ||
        !sf_trait->getInfo(sf, NULL, &nav_list, &count) ||
        nav_list == NULL || count == 0 ||
        (size_t)count > UINT_MAX / sizeof(*snapshot))
        return;
    snapshot = (NavigatorUpdateLifetime *)XtCalloc(count, sizeof(*snapshot));
    if (snapshot == NULL)
        return;
    XtAddCallback(sf, XmNdestroyCallback, NavigatorUpdateDestroyed, &owner);
    for (Cardinal i = 0; i < count; ++i) {
        snapshot[i].widget = nav_list[i];
        if (snapshot[i].widget != NULL)
            XtAddCallback(snapshot[i].widget, XmNdestroyCallback,
                          NavigatorUpdateDestroyed, &snapshot[i]);
    }

    /* A setValue notification can remove navigators, reallocate the borrowed
     * list, or destroy the entire frame. Watch the snapshot's lifetimes and
     * recheck current membership before invoking another navigator. */
    for (Cardinal i = 0; i < count; ++i) {
        Widget nav = snapshot[i].widget;
        XmNavigatorTrait nav_trait;
        Boolean present = False;
        if (owner.destroyed || sf->core.being_destroyed)
            break;
        if (nav == NULL || snapshot[i].destroyed || nav->core.being_destroyed)
            continue;
        if (!sf_trait->getInfo(sf, NULL, &current_list, &current_count) ||
            current_list == NULL)
            break;
        for (Cardinal j = 0; j < current_count; ++j)
            if (current_list[j] == nav) present = True;
        if (!present)
            continue;
        nav_trait = (XmNavigatorTrait)
            XmeTraitGet((XtPointer)XtClass(nav), XmQTnavigator);
        if (nav_trait != NULL && nav_trait->setValue != NULL)
            nav_trait->setValue(nav, nav_data, notify);
    }
    for (Cardinal i = 0; i < count; ++i)
        if (snapshot[i].widget != NULL && !snapshot[i].destroyed)
            XtRemoveCallback(snapshot[i].widget, XmNdestroyCallback,
                             NavigatorUpdateDestroyed, &snapshot[i]);
    if (!owner.destroyed)
        XtRemoveCallback(sf, XmNdestroyCallback, NavigatorUpdateDestroyed, &owner);
    XtFree((char *)snapshot);
}
