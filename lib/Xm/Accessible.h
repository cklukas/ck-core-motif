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
  XmACCESSIBLE_SCROLL_BAR = 48,
  XmACCESSIBLE_TABLE = 55,
  XmACCESSIBLE_TABLE_CELL = 56
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
typedef struct {
  XmAccessibleQueryProc query;
  XmAccessibleTextProc text;
  XmAccessibleActionProc action;
  XmAccessibleActionNameProc action_name;
  XmAccessibleFocusProc focus;
  XmAccessibleCellProc cell;
  XmAccessibleSetValueProc set_value;
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
   node. Providers own data until Remove; IDs are never reused in a bridge
   lifetime. */
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
