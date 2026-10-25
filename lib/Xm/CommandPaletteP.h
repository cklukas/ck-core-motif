#ifndef _XmCommandPaletteP_h
#define _XmCommandPaletteP_h
#include <Xm/CommandPalette.h>
#include <Xm/ManagerP.h>
typedef struct _XmCommandPaletteClassPart { XtPointer extension; } XmCommandPaletteClassPart;
typedef struct _XmCommandPaletteClassRec { CoreClassPart core_class; CompositeClassPart composite_class;
    ConstraintClassPart constraint_class; XmManagerClassPart manager_class;
    XmCommandPaletteClassPart palette_class; } XmCommandPaletteClassRec;
typedef struct _XmCommandPaletteStoredItem { char *id; XmString label, shortcut; XtPointer user_data; } XmCommandPaletteStoredItem;
typedef struct _XmCommandPalettePart { XmCommandPaletteStoredItem *items; Cardinal item_count, *visible;
    Cardinal visible_count; Widget search, list; Widget previous_focus; XtCallbackList invoke_callback, dismiss_callback;
    Boolean filtering, shown, laying_out, initializing; } XmCommandPalettePart;
typedef struct _XmCommandPaletteRec { CorePart core; CompositePart composite; ConstraintPart constraint;
    XmManagerPart manager; XmCommandPalettePart palette; } XmCommandPaletteRec;
extern XmCommandPaletteClassRec xmCommandPaletteClassRec;
#endif
