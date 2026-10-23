/* Keep Xt's private callback/layout types out of Motif implementation units.
 * The event dispatcher remains the sole authority for native grab admission.
 * LGPL-2.0-or-later. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "XtInputI.h"
#include <X11/IntrinsicI.h>
#include <X11/EventI.h>
#include <X11/InitialI.h>

Boolean _XmXtInputAllowed(Widget widget)
{
    XtGrabList grabs;
    if (!widget || widget->core.being_destroyed || !XtIsSensitive(widget))
        return False;
    grabs = *_XtGetGrabList(_XtGetPerDisplayInput(XtDisplayOfObject(widget)));
    return !grabs || _XtOnGrabList(widget, grabs);
}
