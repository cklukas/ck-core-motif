#ifndef PERIODIC_ACTIONS_H
#define PERIODIC_ACTIONS_H
#include <X11/Intrinsic.h>
typedef struct PeriodicActions PeriodicActions;
/* Keep the UIL act* values synchronized; native tests exercise each binding. */
typedef enum {
    PA_NEW=1,PA_OPEN,PA_SAVE,PA_SAVE_AS,PA_PRINT,PA_EXIT,
    PA_UNDO=10,PA_CUT,PA_COPY,PA_PASTE,PA_CLEAR,PA_DELETE,
    PA_STACK=20,PA_TILE,PA_DAY,PA_WEEK,PA_MONTH,PA_YEAR,
    PA_UP=30,PA_DOWN,PA_LEFT,PA_RIGHT,PA_CASE,PA_WRAP,
    PA_CONTEXT=40,PA_WINDOW_HELP,PA_TUTORIAL,PA_VERSION,
    PA_PREVIEW=50,PA_INCLUDE,PA_DRAFT,PA_SPELL,PA_TEMPLATE_CLOSE,
    PA_AUDIO=60,PA_GRAPHICS,PA_TEXT,PA_FILE_MERGE,PA_FILE_CLEAR
} PeriodicAction;
PeriodicActions *PeriodicInstallActions(Widget root);
void PeriodicDispatchAction(PeriodicActions *actions,Widget source,int action,XtPointer call);
void PeriodicPrepareSampleDialog(PeriodicActions *actions,Widget dialog);
#endif
