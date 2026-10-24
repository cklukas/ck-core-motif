/* Generic Motif accessibility providers. LGPL-2.0-or-later. */
#ifndef _XmAccessible_h
#define _XmAccessible_h
#include <Xm/Xm.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XmAccessibility XmAccessibility;
typedef uint32_t XmAccessibleId;
#define XmACCESSIBLE_NONE ((XmAccessibleId)0)
/* Role and state numbers are the AT-SPI protocol values; no ATK/GTK dependency.
 */
enum {
  XmACCESSIBLE_APPLICATION = 75,
  XmACCESSIBLE_FRAME = 23,
  XmACCESSIBLE_PANEL = 39,
  XmACCESSIBLE_LABEL = 29,
  XmACCESSIBLE_BUTTON = 43,
  XmACCESSIBLE_CHECK_BOX = 7,
  XmACCESSIBLE_TEXT = 61,
  XmACCESSIBLE_PASSWORD_TEXT = 40,
  XmACCESSIBLE_SCROLL_BAR = 48,
  XmACCESSIBLE_LIST = 31,
  XmACCESSIBLE_LIST_ITEM = 32,
  XmACCESSIBLE_MENU = 33,
  XmACCESSIBLE_MENU_BAR = 34,
  XmACCESSIBLE_MENU_ITEM = 35,
  XmACCESSIBLE_CHECK_MENU_ITEM = 8,
  XmACCESSIBLE_RADIO_MENU_ITEM = 45,
  XmACCESSIBLE_TABLE = 55,
  XmACCESSIBLE_TABLE_CELL = 56,
  XmACCESSIBLE_PROGRESS_BAR = 41,
  XmACCESSIBLE_TREE = 46,
  XmACCESSIBLE_CALENDAR = 22
};
#define XmACCESSIBLE_STATE(n) (UINT64_C(1) << (n))
typedef struct {
  unsigned int role;
  uint64_t states;
  /* Owned UTF-8 strings, allocated with XtMalloc/XtNewString. Query caller
     releases with XmAccessibleInfoClear. Text is read separately through
     bounded range callbacks. */
  char *name, *description;
  Boolean has_text;
  int text_length; /* Unicode scalar count, independent of UTF-8 bytes. */
  int caret;
  Boolean has_extents;
  int screen_x, screen_y, width, height; /* Screen-pixel coordinates. */
  int rows, columns;                     /* -1 means no table interface. */
  unsigned int action_count;
  /* Scalar Value interface; finite bounds/current/increment, increment >= 0.
     Native ScrollBar maximum excludes sliderSize. value_text is optional owned
     UTF-8, freed by XmAccessibleInfoClear. */
  Boolean has_selection;
  Boolean has_value;
  double minimum_value, maximum_value, current_value, minimum_increment;
  char *value_text;
} XmAccessibleInfo;
typedef Boolean (*XmAccessibleQueryProc)(XtPointer, XmAccessibleInfo *);
typedef char *(*XmAccessibleTextProc)(XtPointer, int start, int end);
typedef Boolean (*XmAccessibleActionProc)(XtPointer, unsigned int);
typedef char *(*XmAccessibleActionNameProc)(XtPointer, unsigned int);
typedef Boolean (*XmAccessibleFocusProc)(XtPointer);
typedef XmAccessibleId (*XmAccessibleCellProc)(XtPointer, int, int);
typedef Boolean (*XmAccessibleSetValueProc)(XtPointer, double);
/* Visual lines use Unicode scalar offsets, with exclusive end. direction is
   -1 (previous), 0 (containing), or +1 (next). A hard newline belongs to the
   preceding line; offset == text_length selects the final line, including an
   empty line after a trailing newline. Missing neighbors are empty ranges at
   0 or text_length. Callbacks must return actual layout lines, including soft
   wrapping, and never substitute logical paragraphs for unavailable layout. */
typedef Boolean (*XmAccessibleVisualLineProc)(XtPointer, int offset,
                                             int direction, int *first,
                                             int *last);
typedef enum {
  XmACCESSIBLE_SELECT_CHILD,
  XmACCESSIBLE_DESELECT_CHILD,
  XmACCESSIBLE_SELECT_ALL,
  XmACCESSIBLE_CLEAR_SELECTION
} XmAccessibleSelectionOperation;
/* Child IDs refer to live direct children, never model row numbers. */
typedef int (*XmAccessibleSelectionCountProc)(XtPointer);
typedef XmAccessibleId (*XmAccessibleSelectedChildProc)(XtPointer, int);
typedef Boolean (*XmAccessibleChildSelectedProc)(XtPointer, XmAccessibleId);
typedef Boolean (*XmAccessibleChangeSelectionProc)(XtPointer,
    XmAccessibleSelectionOperation, XmAccessibleId);
typedef struct {
  XmAccessibleQueryProc query;
  XmAccessibleTextProc text;
  XmAccessibleActionProc action;
  XmAccessibleActionNameProc action_name;
  XmAccessibleFocusProc focus;
  XmAccessibleCellProc cell;
  XmAccessibleSetValueProc set_value;
  XmAccessibleVisualLineProc visual_line;
  XmAccessibleSelectionCountProc selection_count;
  XmAccessibleSelectedChildProc selected_child;
  XmAccessibleChildSelectedProc child_selected;
  XmAccessibleChangeSelectionProc change_selection;
} XmAccessibleProvider;
typedef enum {
  XmACCESSIBILITY_NOT_BUILT,
  XmACCESSIBILITY_UNAVAILABLE,
  XmACCESSIBILITY_CONNECTING,
  XmACCESSIBILITY_CONNECTED,
  XmACCESSIBILITY_DISCONNECTED
} XmAccessibilityStatus;
/* All calls, provider callbacks and dispatch run on the owning Xt UI thread.
   Destroy bridge before root widgets. No background calls into Xt are made. */
XmAccessibility *XmAccessibilityCreate(Widget root,
                                       const char *application_name);
void XmAccessibilityDestroy(XmAccessibility *);
/* One application bridge can own several native top-level shells. The shell
   must share the creating Xt application context. The bridge remains alive when
   an individual shell closes; destroy it before destroying the application. */
Boolean XmAccessibilityAttachShell(XmAccessibility *, Widget shell);
void XmAccessibilityRefresh(XmAccessibility *);
XmAccessibleId XmAccessibilityRoot(XmAccessibility *);
XmAccessibleId XmAccessibilityWidget(XmAccessibility *, Widget);
/* Register engine-projected semantic children beneath a widget or semantic
   node. Mutation targets must have a live native widget ancestor identifying
   their owning window; application-root-only semantics remain readable but
   cannot receive actions, focus, value or selection mutations. Providers own
   data until Remove; IDs are never reused in a bridge lifetime. */
XmAccessibleId XmAccessibilityAdd(XmAccessibility *, XmAccessibleId parent,
                                  const XmAccessibleProvider *, XtPointer);
void XmAccessibilityRemove(XmAccessibility *, XmAccessibleId);
Boolean XmAccessibilityQuery(XmAccessibility *, XmAccessibleId,
                             XmAccessibleInfo *);
void XmAccessibleInfoClear(XmAccessibleInfo *);
/* Owned UTF-8 range, end exclusive; NULL on invalid/stale/unavailable ranges.
 */
char *XmAccessibilityText(XmAccessibility *, XmAccessibleId, int start,
                          int end);
/* False means stale, invalid, or unavailable visual layout. */
Boolean XmAccessibilityVisualLine(XmAccessibility *, XmAccessibleId, int offset,
                                   int direction, int *first, int *last);
/* Mutations honor native sensitivity and the current Xt/Motif modal scope,
   including provider/list-item fast paths. A primary dialog blocks its parent
   branch while independent windows remain usable. Read-only queries remain
   available. Admission is rechecked after provider reads that may reenter Xt. */
Boolean XmAccessibilityAction(XmAccessibility *, XmAccessibleId, unsigned int);
/* Owned UTF-8 native/provider action name; NULL for invalid action. */
char *XmAccessibilityActionName(XmAccessibility *, XmAccessibleId,
                                unsigned int);
/* Rejects nonfinite/out-of-range values and unavailable, insensitive or
   read-only targets. Native ScrollBars require integral values and notify their
   normal valueChanged callbacks. Providers own any further
   quantization/validation and must call Changed when their state changes. */
Boolean XmAccessibilitySetValue(XmAccessibility *, XmAccessibleId, double);
Boolean XmAccessibilityFocus(XmAccessibility *, XmAccessibleId);
/* Selection indexes enumerate selected direct children; -1/0/False denote
 * unsupported, stale or invalid queries. Mutations reject insensitive owners.
 * Providers must admit changes atomically and return False without side effects
 * on failure. The bridge emits accepted selection deltas, not failed attempts.
 * Read callbacks must describe one stable selection of unique direct-child IDs
 * and call Changed for any semantic mutation. Hierarchy/Changed mutations during
 * a read reject that read; no mutation callback is then invoked. Selection change
 * callbacks must not modify the owner's child hierarchy. Removal/destruction is
 * allowed, but the operation then fails closed without an accepted delta. */
int XmAccessibilitySelectionCount(XmAccessibility *, XmAccessibleId);
XmAccessibleId XmAccessibilitySelectedChild(XmAccessibility *, XmAccessibleId, int);
Boolean XmAccessibilityChildSelected(XmAccessibility *, XmAccessibleId, XmAccessibleId);
Boolean XmAccessibilityChangeSelection(XmAccessibility *, XmAccessibleId,
    XmAccessibleSelectionOperation, XmAccessibleId);
/* Changed sends standard AT-SPI PropertyChange/StateChanged signals. */
void XmAccessibilityChanged(XmAccessibility *, XmAccessibleId);
/* Explicitly optional transport. Failure never prevents normal widget
   operation. A build without --enable-atspi returns NOT_BUILT. Session
   bus/AT-SPI registry are external services; CONNECTED requires the registry
   Embed handshake. */
XmAccessibilityStatus XmAccessibilityConnect(XmAccessibility *);
XmAccessibilityStatus XmAccessibilityGetStatus(XmAccessibility *);
const char *XmAccessibilityBusName(XmAccessibility *);
#ifdef __cplusplus
}
#endif
#endif
