/*
 * Motif appearance-profile selection.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <Xm/Appearance.h>
#include <Xm/Xm.h>
#include "XmI.h"
#include "RowColumnP.h"
#include "RCLayoutI.h"

#define APPEARANCE_RESOURCE "ckCoreMotif.appearanceProfile"
#define APPEARANCE_CLASS "CkCoreMotif.AppearanceProfile"
#define DENSITY_RESOURCE "ckCoreMotif.appearanceDensity"
#define DENSITY_CLASS "CkCoreMotif.AppearanceDensity"
#define CLASSIC_PROFILE "classic"
#define MODERN_LIGHT_PROFILE "modern-light"
#define HIGH_CONTRAST_PROFILE "high-contrast"
#define MODERN_DARK_PROFILE "modern-dark"

/*
 * These are deliberately ordinary Xt resources.  XrmCombineDatabase() with
 * override=False keeps an application's exact resource entries, while Xt's
 * normal specificity rules keep more-specific application entries ahead of
 * these wildcard defaults.
 */
static const char modern_light_resources[] =
    "*background: #f1f2f4\n"
    "*foreground: #202124\n"
    "*topShadowColor: #ffffff\n"
    "*bottomShadowColor: #9aa3ae\n"
    "*highlightColor: #1a73e8\n"
    "*highlightThickness: 1\n"
    "*shadowThickness: 2\n"
    "*armColor: #dbeafe\n"
    "*selectColor: #dbeafe\n"
    "*XmLabel.traversalOn: False\n"
    "*XmLabel.highlightThickness: 0\n"
    "*XmPushButton.background: #e2e6eb\n"
    "*XmPushButtonGadget.background: #e2e6eb\n"
    "*XmTextField.background: #ffffff\n"
    "*XmText.background: #ffffff\n"
    "*XmList.background: #ffffff\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 1\n"
    "*XmNotebook.frameShadowThickness: 0\n"
    "*XmNotebook.borderWidth: 0\n";

static const char high_contrast_resources[] =
    "*background: #000000\n"
    "*foreground: #ffffff\n"
    "*topShadowColor: #ffffff\n"
    "*bottomShadowColor: #ffffff\n"
    "*highlightColor: #ffff00\n"
    "*highlightThickness: 3\n"
    "*shadowThickness: 2\n"
    "*armColor: #0000ff\n"
    "*selectColor: #0000ff\n"
    "*insensitiveForeground: #b3b3b3\n"
    "*XmLabel.traversalOn: True\n"
    "*XmLabel.highlightThickness: 3\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 3\n";

static const char modern_dark_resources[] =
    "*background: #202124\n"
    "*foreground: #f1f3f4\n"
    "*topShadowColor: #5f6368\n"
    "*bottomShadowColor: #000000\n"
    "*highlightColor: #8ab4f8\n"
    "*highlightThickness: 1\n"
    "*shadowThickness: 1\n"
    "*armColor: #3c4043\n"
    "*selectColor: #3c4043\n"
    "*insensitiveForeground: #9aa0a6\n"
    "*XmToggleButton.bottomShadowColor: #9aa0a6\n"
    "*XmLabel.traversalOn: True\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 1\n"
    "*XmNotebook.frameShadowThickness: 0\n"
    "*XmNotebook.borderWidth: 0\n";

/* Layout defaults are shared by all modern palettes. Keep density independent
 * of palette selection, and resolve these same patterns for startup and live
 * changes. Natural control sizes remain the responsibility of each widget. */
typedef struct {
    const char *resource;
    Dimension comfortable;
    Dimension compact;
} AppearanceDensityResource;

static const AppearanceDensityResource density_resources[] = {
    {"*XmPushButton.marginWidth", 12, 8},
    {"*XmPushButton.marginHeight", 4, 2},
    {"*XmPushButtonGadget.marginWidth", 12, 8},
    {"*XmPushButtonGadget.marginHeight", 4, 2},
    {"*XmToggleButton.marginWidth", 12, 8},
    {"*XmToggleButton.marginHeight", 4, 2},
    {"*XmToggleButton.indicatorSize", 18, 16},
    {"*XmToggleButtonGadget.marginWidth", 12, 8},
    {"*XmToggleButtonGadget.marginHeight", 4, 2},
    {"*XmToggleButtonGadget.indicatorSize", 18, 16},
    {"*XmTextField.marginWidth", 8, 6},
    {"*XmTextField.marginHeight", 4, 2},
    {"*XmText.marginWidth", 8, 6},
    {"*XmText.marginHeight", 4, 2},
    {"*XmList.listMarginWidth", 8, 4},
    {"*XmList.listMarginHeight", 8, 4},
    {"*XmList.listSpacing", 4, 2}
};

typedef enum {
    AP_BACKGROUND,
    AP_FOREGROUND,
    AP_TOP_SHADOW,
    AP_BOTTOM_SHADOW,
    AP_HIGHLIGHT,
    AP_HIGHLIGHT_THICKNESS,
    AP_SHADOW_THICKNESS,
    AP_ARM,
    AP_SELECT,
    AP_INSENSITIVE_FOREGROUND,
    AP_TRAVERSAL,
    AP_MARGIN_WIDTH,
    AP_MARGIN_HEIGHT,
    AP_LIST_MARGIN_WIDTH,
    AP_LIST_MARGIN_HEIGHT,
    AP_LIST_SPACING,
    AP_FRAME_SHADOW_THICKNESS,
    AP_BORDER_WIDTH,
    AP_INDICATOR_SIZE,
    AP_COUNT
} AppearanceProperty;

typedef struct _XmAppearanceWidgetState {
    Widget widget;
    Widget parent;
    Display *display;
    Boolean supported[AP_COUNT];
    Boolean explicit_arg[AP_COUNT];
    Boolean owned[AP_COUNT];
    XtArgVal original[AP_COUNT];
    unsigned int references;
    Boolean retired;
    Boolean initialized;
    Boolean gadget;
    struct _XmAppearanceWidgetState *next;
} XmAppearanceWidgetState;

typedef struct {
    XmAppearanceWidgetState *state;
    XtArgVal values[AP_COUNT];
    Boolean owned[AP_COUNT];
    Boolean apply[AP_COUNT];
} XmAppearanceChangePlan;

typedef struct _XmAppearanceDisplayState {
    Display *display;
    XrmDatabase application_database;
    XrmDatabase active_database;
    Boolean owns_active_database;
    Boolean transaction_active;
    Boolean defer_layout;
    Widget hooks;
    struct _XmAppearanceDisplayState *next;
} XmAppearanceDisplayState;

static XmAppearanceWidgetState *appearance_widgets;
static XmAppearanceDisplayState *appearance_displays;

static void appearance_release_gadget_states(Display *display);
static void appearance_created(Widget, XtPointer, XtPointer);
static void appearance_changed(Widget, XtPointer, XtPointer);
static void appearance_hooks_destroyed(Widget, XtPointer, XtPointer);

void
_XmDestroyAppearanceDisplay(Display *display)
{
    XmAppearanceDisplayState **link;
    XmAppearanceDisplayState *state;
    XrmDatabase application_database;
    XrmDatabase active_database;

    if (display == NULL)
        return;

    appearance_release_gadget_states(display);

    _XmProcessLock();
    link = &appearance_displays;
    while (*link != NULL && (*link)->display != display)
        link = &(*link)->next;
    state = *link;
    if (state == NULL) {
        _XmProcessUnlock();
        return;
    }
    *link = state->next;
    application_database = state->application_database;
    active_database = state->active_database;
    state->application_database = NULL;
    state->active_database = NULL;
    if (state->hooks != NULL) {
        XtRemoveCallback(state->hooks, XtNcreateHook, appearance_created, state);
        XtRemoveCallback(state->hooks, XtNchangeHook, appearance_changed, state);
        XtRemoveCallback(state->hooks, XtNdestroyCallback,
                         appearance_hooks_destroyed, state);
    }
    XtFree((char *)state);
    _XmProcessUnlock();

    if (active_database != NULL && XrmGetDatabase(display) == active_database) {
        XrmSetDatabase(display, NULL);
        XrmDestroyDatabase(active_database);
    }
    if (application_database != NULL)
        XrmDestroyDatabase(application_database);
}

static const char *
appearance_property_name(AppearanceProperty property)
{
    static const char *const names[AP_COUNT] = {
        XmNbackground, XmNforeground, XmNtopShadowColor,
        XmNbottomShadowColor, XmNhighlightColor, XmNhighlightThickness,
        XmNshadowThickness, XmNarmColor, XmNselectColor,
        "insensitiveForeground", XmNtraversalOn, XmNmarginWidth,
        XmNmarginHeight, XmNlistMarginWidth, XmNlistMarginHeight,
        XmNlistSpacing, XmNframeShadowThickness, XmNborderWidth,
        XmNindicatorSize
    };
    return names[property];
}

static Boolean
appearance_property_is_dimension(AppearanceProperty property)
{
    return property == AP_HIGHLIGHT_THICKNESS ||
           property == AP_SHADOW_THICKNESS ||
           property == AP_MARGIN_WIDTH || property == AP_MARGIN_HEIGHT ||
           property == AP_LIST_MARGIN_WIDTH ||
           property == AP_LIST_MARGIN_HEIGHT || property == AP_LIST_SPACING ||
           property == AP_FRAME_SHADOW_THICKNESS || property == AP_BORDER_WIDTH ||
           property == AP_INDICATOR_SIZE;
}

static Boolean
appearance_property_is_boolean(AppearanceProperty property)
{
    return property == AP_TRAVERSAL;
}

static Boolean
appearance_property_is_color(AppearanceProperty property)
{
    return !appearance_property_is_dimension(property) &&
           !appearance_property_is_boolean(property);
}

static const char *
widget_class_name(Widget widget)
{
    WidgetClass class = XtClass(widget);
    return class != NULL ? class->core_class.class_name : "";
}

static const char *
profile_name(XmAppearanceProfile profile)
{
    switch (profile) {
    case XmAPPEARANCE_CLASSIC:
        return CLASSIC_PROFILE;
    case XmAPPEARANCE_MODERN_LIGHT:
        return MODERN_LIGHT_PROFILE;
    case XmAPPEARANCE_HIGH_CONTRAST:
        return HIGH_CONTRAST_PROFILE;
    case XmAPPEARANCE_MODERN_DARK:
        return MODERN_DARK_PROFILE;
    default:
        return NULL;
    }
}

/* Canonical profile databases are immutable and shared for the process
 * lifetime, like the static resource strings from which they are built. Both
 * Xt construction and live transactions resolve the same Xrm patterns. */
static XrmDatabase
profile_defaults(XmAppearanceProfile profile, XmAppearanceDensity density)
{
    static XrmDatabase databases[4][2];
    const char *resources;
    XrmDatabase result;
    if (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
        density != XmAPPEARANCE_DENSITY_COMPACT) return NULL;
    if (profile == XmAPPEARANCE_CLASSIC) return NULL;
    switch (profile) {
    case XmAPPEARANCE_MODERN_LIGHT: resources = modern_light_resources; break;
    case XmAPPEARANCE_HIGH_CONTRAST: resources = high_contrast_resources; break;
    case XmAPPEARANCE_MODERN_DARK: resources = modern_dark_resources; break;
    default: return NULL;
    }
    _XmProcessLock();
    if (databases[profile][density] == NULL) {
        XrmDatabase database = XrmGetStringDatabase(resources);
        if (database != NULL) {
            for (Cardinal i = 0; i < XtNumber(density_resources); ++i) {
                char value[24];
                const AppearanceDensityResource *entry = &density_resources[i];
                snprintf(value, sizeof(value), "%u", (unsigned int)
                    (density == XmAPPEARANCE_DENSITY_COMPACT
                        ? entry->compact : entry->comfortable));
                XrmPutStringResource(&database, entry->resource, value);
            }
            databases[profile][density] = database;
        }
    }
    result = databases[profile][density];
    _XmProcessUnlock();
    return result;
}

static Boolean appearance_database_value(Widget widget, XrmDatabase database,
    AppearanceProperty property, XrmValue *value_return);

static const char *
profile_property_string(Widget widget, XmAppearanceProfile profile,
                        XmAppearanceDensity density,
                        AppearanceProperty property)
{
    XrmValue value;
    if (appearance_database_value(widget, profile_defaults(profile, density), property,
                                  &value))
        return value.addr;
    return NULL;
}

static Boolean
profile_property_value(Widget widget, XmAppearanceProfile profile,
                       XmAppearanceDensity density,
                       AppearanceProperty property, XtArgVal *value_return)
{
    const char *text = profile_property_string(widget, profile, density, property);
    char *end = NULL;
    unsigned long number;

    if (text == NULL)
        return False;
    if (appearance_property_is_color(property)) {
        XColor color;
        Colormap colormap = DefaultColormapOfScreen(XtScreen(widget));
        if (!XParseColor(XtDisplay(widget), colormap, text, &color) ||
            !XAllocColor(XtDisplay(widget), colormap, &color))
            return False;
        *value_return = (XtArgVal)color.pixel;
        return True;
    }
    number = strtoul(text, &end, 10);
    if (appearance_property_is_boolean(property)) {
        if (strcmp(text, "True") == 0 || strcmp(text, "true") == 0) {
            *value_return = (XtArgVal)True;
            return True;
        }
        if (strcmp(text, "False") == 0 || strcmp(text, "false") == 0) {
            *value_return = (XtArgVal)False;
            return True;
        }
        return False;
    }
    if (*text == '\0' || end == text || *end != '\0' ||
        number > (unsigned long)~(Dimension)0)
        return False;
    *value_return = (XtArgVal)(Dimension)number;
    return True;
}

/* Cached gadget resources and text backend resources live in secondary
 * tables; XtGetResourceList alone does not describe the public widget API. */
static const char *
appearance_resource_class(Widget widget, AppearanceProperty property)
{
    XtResourceList resources = NULL;
    XmSecondaryResourceData *secondary = NULL;
    Cardinal count = 0, blocks;
    const char *name = appearance_property_name(property);
    const char *result = NULL;

    XtGetResourceList(XtClass(widget), &resources, &count);
    for (Cardinal i = 0; i < count; ++i)
        if (strcmp(resources[i].resource_name, name) == 0) {
            result = resources[i].resource_class;
            break;
        }
    XtFree((char *)resources);
    if (result != NULL) return result;
    blocks = XmGetSecondaryResourceData(XtClass(widget), &secondary);
    for (Cardinal i = 0; i < blocks; ++i) {
        for (Cardinal j = 0; j < secondary[i]->num_resources; ++j)
            if (strcmp(secondary[i]->resources[j].resource_name, name) == 0)
                result = secondary[i]->resources[j].resource_class;
        XtFree((char *)secondary[i]->resources);
        XtFree((char *)secondary[i]);
    }
    XtFree((char *)secondary);
    return result;
}

static Boolean
appearance_resource_supported(Widget widget, AppearanceProperty property)
{
    return appearance_resource_class(widget, property) != NULL;
}

static Boolean
appearance_explicit_arg(ArgList args, Cardinal num_args,
                        AppearanceProperty property)
{
    const char *name = appearance_property_name(property);
    for (Cardinal i = 0; i < num_args; ++i)
        if (strcmp(args[i].name, name) == 0)
            return True;
    return False;
}

static Boolean
appearance_database_value(Widget widget, XrmDatabase database,
                          AppearanceProperty property, XrmValue *value_return)
{
    Widget chain[64];
    XrmName names[66];
    XrmClass classes[66];
    Widget current = widget;
    int count = 0;
    XrmRepresentation type;
    const char *resource_class;

    if (database == NULL)
        return False;
    resource_class = appearance_resource_class(widget, property);
    if (resource_class == NULL)
        return False;
    while (current != NULL && count < 64) {
        chain[count++] = current;
        current = XtParent(current);
    }
    for (int i = 0; i < count; ++i) {
        Widget item = chain[count - i - 1];
        names[i] = XrmStringToQuark(XtName(item));
        classes[i] = XtIsApplicationShell(item)
            ? ((ApplicationShellWidget)item)->application.xrm_class
            : XrmStringToQuark(widget_class_name(item));
    }
    names[count] = XrmStringToQuark(appearance_property_name(property));
    classes[count] = XrmStringToQuark(resource_class);
    names[count + 1] = NULLQUARK;
    classes[count + 1] = NULLQUARK;
    return XrmQGetResource(database, names, classes, &type, value_return);
}

static Bool
clone_resource(XrmDatabase *database, XrmBindingList bindings,
               XrmQuarkList quarks, XrmRepresentation *type,
               XrmValue *value, XPointer closure)
{
    XrmDatabase *target = (XrmDatabase *)closure;
    (void)database;
    XrmQPutResource(target, bindings, quarks, *type, value);
    return False;
}

static XrmDatabase
clone_database(XrmDatabase source)
{
    XrmDatabase copy = XrmGetStringDatabase("");
    XrmName names[1] = {NULLQUARK};
    XrmClass classes[1] = {NULLQUARK};
    if (copy == NULL)
        return NULL;
    if (source != NULL && XrmEnumerateDatabase(source, names, classes,
                                               XrmEnumAllLevels,
                                               clone_resource,
                                               (XPointer)&copy)) {
        XrmDestroyDatabase(copy);
        return NULL;
    }
    return copy;
}

static XmAppearanceDisplayState *
find_display_state(Display *display)
{
    XmAppearanceDisplayState *state;
    for (state = appearance_displays; state != NULL; state = state->next)
        if (state->display == display)
            return state;
    return NULL;
}

/*
 * Appearance changes call XtSetValues() on application objects.  A class
 * SetValues hook may therefore call back into this API while the outer
 * transaction is still applying.  Resource-database replacement is not
 * composable in that situation: accepting the nested operation would let
 * the two snapshots observe and publish different profiles.  Reject the
 * nested operation explicitly; the caller remains responsible for deciding
 * whether to retry it after the outer transaction completes.
 */
static Boolean
appearance_transaction_begin(XmAppearanceDisplayState *state)
{
    Boolean available;

    _XmProcessLock();
    available = !state->transaction_active;
    if (available)
        state->transaction_active = True;
    _XmProcessUnlock();
    return available;
}

static void
appearance_transaction_end(XmAppearanceDisplayState *state)
{
    _XmProcessLock();
    state->transaction_active = False;
    _XmProcessUnlock();
}

static Boolean
appearance_transaction_is_active(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    Boolean active = False;

    if (state != NULL) {
        _XmProcessLock();
        active = state->transaction_active;
        _XmProcessUnlock();
    }
    return active;
}

/* Layout managers must not normalize siblings against a mixture of old and
 * new appearance values. The transaction holds this phase only while committing
 * resources, then performs cross-child layout with all new values present. */
Boolean
_XmAppearanceDefersLayout(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    return state != NULL && state->defer_layout;
}

static XmAppearanceWidgetState *
find_live_widget_state(Widget widget)
{
    XmAppearanceWidgetState *state;

    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->gadget && !state->retired && state->widget == widget)
            return state;
    return NULL;
}

static Boolean
appearance_gadget_is_live(XmAppearanceWidgetState *state)
{
    CompositeWidget parent;

    if (!state->gadget || state->widget == NULL || state->parent == NULL)
        return False;
    if (find_live_widget_state(state->parent) == NULL ||
        !XtIsComposite(state->parent))
        return False;
    parent = (CompositeWidget)state->parent;
    for (Cardinal i = 0; i < parent->composite.num_children; ++i)
        if (parent->composite.children[i] == state->widget)
            return True;
    return False;
}

static void
appearance_prune_gadgets(Display *display)
{
    XmAppearanceWidgetState **link;
    XmAppearanceWidgetState *state;

    _XmProcessLock();
    link = &appearance_widgets;
    while (*link != NULL) {
        state = *link;
        if (state->display == display && state->gadget &&
            !appearance_gadget_is_live(state)) {
            state->retired = True;
            *link = state->next;
            if (state->references == 0)
                XtFree((char *)state);
            continue;
        }
        link = &state->next;
    }
    _XmProcessUnlock();
}

static void
appearance_release_gadget_states(Display *display)
{
    XmAppearanceWidgetState **link;
    XmAppearanceWidgetState *state;

    _XmProcessLock();
    link = &appearance_widgets;
    while (*link != NULL) {
        state = *link;
        if (state->display == display && state->gadget) {
            state->retired = True;
            *link = state->next;
            if (state->references == 0)
                XtFree((char *)state);
            continue;
        }
        link = &state->next;
    }
    _XmProcessUnlock();
}

static int
appearance_display_closed(Display *display, XExtCodes *codes)
{
    (void)codes;
    _XmDestroyAppearanceDisplay(display);
    return 0;
}

static XmAppearanceDisplayState *
ensure_display_state(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    if (state != NULL)
        return state;
    state = (XmAppearanceDisplayState *)XtCalloc(1, sizeof(*state));
    if (state == NULL)
        return NULL;
    state->display = display;
    state->application_database = clone_database(XrmGetDatabase(display));
    if (state->application_database == NULL) {
        XtFree((char *)state);
        return NULL;
    }
    /* Selection is also valid on an XOpenDisplay connection before Xt is
     * initialized. Use Xlib lifetime tracking here; install Xt hooks only when
     * an actual Motif object registers on the initialized Xt display. */
    {
        XExtCodes *extension = XAddExtension(display);
        if (extension == NULL) {
            XrmDestroyDatabase(state->application_database);
            XtFree((char *)state);
            return NULL;
        }
        XESetCloseDisplay(display, extension->extension, appearance_display_closed);
    }
    state->next = appearance_displays;
    appearance_displays = state;
    return state;
}

static void
appearance_widget_destroyed(Widget widget, XtPointer closure,
                            XtPointer call_data)
{
    XmAppearanceWidgetState *state = (XmAppearanceWidgetState *)closure;
    XmAppearanceWidgetState **link;
    (void)widget;
    (void)call_data;
    _XmProcessLock();
    state->retired = True;
    state->widget = NULL;
    if (state->references == 0) {
        link = &appearance_widgets;
        while (*link != NULL && *link != state)
            link = &(*link)->next;
        if (*link == state)
            *link = state->next;
        XtFree((char *)state);
    }
    _XmProcessUnlock();
}

static void
read_appearance_values(XmAppearanceWidgetState *state, XtArgVal *values)
{
    Arg get_args[AP_COUNT];
    Cardinal count = 0;
    /* GetValues writes the resource's actual size, which can be smaller than
     * XtArgVal (Dimension/Boolean). Clear the remaining bytes before comparing. */
    memset(values, 0, AP_COUNT * sizeof(*values));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        if (state->supported[property])
        {
            XtSetArg(get_args[count],
                     (String)appearance_property_name(property),
                     &values[property]);
            ++count;
        }
    if (count != 0)
        XtGetValues(state->widget, get_args, count);
}

static void initialize_appearance_state(XmAppearanceWidgetState *state,
                                        XmAppearanceProfile current_profile);

static Boolean
appearance_widget_prepare(XmAppearanceWidgetState *state,
                           XmAppearanceProfile new_profile,
                           XmAppearanceDensity density,
                           XmAppearanceChangePlan *plan)
{
    plan->state = state;
    memcpy(plan->owned, state->owned, sizeof(plan->owned));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        const char *profile_text;

        if (!state->supported[property])
            continue;
        /* Ownership follows explicit arguments and Xt changes, not equality:
         * a parent's internal margin normalization is not an app override. */
        if (!plan->owned[property])
            continue;
        profile_text = profile_property_string(state->widget, new_profile,
                                               density, property);
        if (profile_text == NULL)
            plan->values[property] = state->original[property];
        else if (!profile_property_value(state->widget, new_profile, density, property,
                                         &plan->values[property]))
            return False;
        plan->apply[property] = True;
    }
    return True;
}

static void
appearance_widget_commit(XmAppearanceChangePlan *plan)
{
    XmAppearanceWidgetState *state = plan->state;
    Arg set_args[AP_COUNT];
    Cardinal count = 0;

    memcpy(state->owned, plan->owned, sizeof(state->owned));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        if (plan->apply[property]) {
            XtSetArg(set_args[count],
                     (String)appearance_property_name(property),
                     plan->values[property]);
            ++count;
        }
    if (count != 0)
        XtSetValues(state->widget, set_args, count);
    if (state->widget != NULL && XtIsWidget(state->widget)) {
        if (XtIsRealized(state->widget))
            XClearArea(XtDisplay(state->widget), XtWindow(state->widget),
                       0, 0, 0, 0, True);
    } else if (state->parent != NULL && XtIsRealized(state->parent)) {
        XClearArea(XtDisplay(state->parent), XtWindow(state->parent),
                   0, 0, 0, 0, True);
    }
}

static void
initialize_appearance_state(XmAppearanceWidgetState *state,
                            XmAppearanceProfile current_profile)
{
    (void)current_profile;
    read_appearance_values(state, state->original);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        XrmValue xrm_value;
        XmAppearanceDisplayState *display_state = find_display_state(state->display);
        XrmDatabase application = display_state != NULL
            ? display_state->application_database : XrmGetDatabase(state->display);
        if (!state->supported[property] || state->explicit_arg[property])
            continue;
        /* An application override remains authoritative even when its value
         * happens to equal the current theme default. */
        state->owned[property] = !appearance_database_value(state->widget,
            application, property, &xrm_value);
    }
    state->initialized = True;
}

static void
appearance_hooks_destroyed(Widget hooks, XtPointer closure, XtPointer data)
{
    XmAppearanceDisplayState *display_state = closure;
    (void)hooks;
    (void)data;
    display_state->hooks = NULL;
}

/* Xt's create hook runs after class initializers, initialize hooks and gadget
 * cache setup. Snapshot here rather than reading a partially initialized Text
 * backend or treating constructor normalization as an application override. */
static void
appearance_created(Widget hooks, XtPointer closure, XtPointer data)
{
    XtCreateHookData created = data;
    XmAppearanceDisplayState *display_state = closure;
    XmAppearanceWidgetState *state;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    (void)hooks;
    if (created == NULL) return;
    (void)XmGetAppearanceProfile(display_state->display, &profile);
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->widget == created->widget &&
            !state->initialized) {
            initialize_appearance_state(state, profile);
            break;
        }
}

/* Equal-valued XtSetValues calls are still explicit application choices.
 * Record arguments, not just a later comparison with the previous value. */
static void
appearance_changed(Widget hooks, XtPointer closure, XtPointer data)
{
    XtChangeHookData changed = data;
    XmAppearanceDisplayState *display_state = closure;
    XmAppearanceWidgetState *state;
    XtChangeHookSetValuesData values;
    (void)hooks;
    if (display_state->transaction_active || changed == NULL ||
        changed->type == NULL || strcmp(changed->type, XtHsetValues) != 0 ||
        changed->event_data == NULL) return;
    values = changed->event_data;
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->initialized &&
            state->widget == changed->widget) {
            for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
                if (appearance_explicit_arg(values->args, values->num_args, property)) {
                    state->explicit_arg[property] = True;
                    state->owned[property] = False;
                }
            break;
        }
}

Boolean
_XmRegisterAppearanceChange(Widget widget, ArgList args, Cardinal num_args)
{
    XmAppearanceWidgetState *state;
    XmAppearanceWidgetState *existing;
    XmAppearanceDisplayState *display_state;

    if (widget == NULL || !XtIsObject(widget))
        return False;
    display_state = ensure_display_state(XtDisplayOfObject(widget));
    if (display_state == NULL) return False;
    if (display_state->hooks == NULL) {
        display_state->hooks = XtHooksOfDisplay(display_state->display);
        XtAddCallback(display_state->hooks, XtNcreateHook,
                      appearance_created, display_state);
        XtAddCallback(display_state->hooks, XtNchangeHook,
                      appearance_changed, display_state);
        XtAddCallback(display_state->hooks, XtNdestroyCallback,
                      appearance_hooks_destroyed, display_state);
    }
    state = (XmAppearanceWidgetState *)XtCalloc(1, sizeof(*state));
    if (state == NULL)
        return False;
    state->widget = widget;
    state->gadget = !XtIsWidget(widget);
    state->parent = state->gadget ? XtParent(widget) : NULL;
    state->display = XtDisplayOfObject(widget);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->supported[property] = appearance_resource_supported(widget,
                                                                     property);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->explicit_arg[property] = appearance_explicit_arg(args, num_args,
                                                                 property);
    _XmProcessLock();
    for (existing = appearance_widgets; existing != NULL;
         existing = existing->next)
        if (existing->widget == widget) {
            _XmProcessUnlock();
            XtFree((char *)state);
            return True;
        }
    state->next = appearance_widgets;
    appearance_widgets = state;
    _XmProcessUnlock();
    if (!state->gadget)
        XtAddCallback(widget, XmNdestroyCallback, appearance_widget_destroyed,
                      (XtPointer)state);
    return True;
}

Boolean
_XmApplyAppearanceChange(Display *display, XmAppearanceProfile old_profile,
                         XmAppearanceProfile new_profile,
                         XmAppearanceDensity density)
{
    XmAppearanceWidgetState **snapshot = NULL;
    XmAppearanceChangePlan *plans = NULL;
    XmAppearanceWidgetState *state;
    XmAppearanceDisplayState *display_state = find_display_state(display);
    Cardinal count = 0;
    Boolean success = True;

    appearance_prune_gadgets(display);
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display)
            ++count;
    if (count != 0 &&
        ((size_t)count > SIZE_MAX / sizeof(*snapshot) ||
         (size_t)count > UINT_MAX / sizeof(*snapshot) ||
         (size_t)count > SIZE_MAX / sizeof(*plans))) {
        _XmProcessUnlock();
        return False;
    }
    if (count != 0)
        snapshot = (XmAppearanceWidgetState **)XtMalloc(count * sizeof(*snapshot));
    if (count != 0 && snapshot == NULL) {
        _XmProcessUnlock();
        return False;
    }
    if (count != 0)
        plans = (XmAppearanceChangePlan *)calloc(count, sizeof(*plans));
    if (count != 0 && plans == NULL) {
        XtFree((char *)snapshot);
        _XmProcessUnlock();
        return False;
    }
    count = 0;
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display) {
            ++state->references;
            snapshot[count++] = state;
        }
    _XmProcessUnlock();
    (void)old_profile;
    for (Cardinal i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL) {
            if (!state->initialized)
                initialize_appearance_state(state, old_profile);
            if (!appearance_widget_prepare(state, new_profile, density, &plans[i]))
                success = False;
        }
        if (!success)
            break;
    }
    if (!success) {
        for (Cardinal i = 0; i < count; ++i) {
            state = snapshot[i];
            _XmProcessLock();
            if (state->references != 0)
                --state->references;
            if (state->retired && state->references == 0)
                XtFree((char *)state);
            _XmProcessUnlock();
        }
        if (snapshot != NULL)
            XtFree((char *)snapshot);
        if (plans != NULL)
            free(plans);
        return False;
    }
    if (display_state != NULL)
        display_state->defer_layout = True;
    for (Cardinal i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL)
            appearance_widget_commit(&plans[i]);
    }
    if (display_state != NULL)
        display_state->defer_layout = False;
    /* Retain every snapshot reference through the layout pass: a SetValues or
     * geometry callback may retire another object. Normalize only after all
     * siblings have their new margins. */
    for (Cardinal i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL &&
            XmIsRowColumn(state->widget)) {
            _XmRCDoMarginAdjustment((XmRowColumnWidget)state->widget);
            _XmRCAdaptToSize((XmRowColumnWidget)state->widget, NULL, NULL);
        }
    }
    for (Cardinal i = 0; i < count; ++i) {
        state = snapshot[i];
        _XmProcessLock();
        if (state->references != 0)
            --state->references;
        if (state->retired && state->references == 0)
            XtFree((char *)state);
        _XmProcessUnlock();
    }
    if (snapshot != NULL)
        XtFree((char *)snapshot);
    if (plans != NULL)
        free(plans);
    return success;
}

static Boolean
profile_from_name(const char *name, XmAppearanceProfile *profile_return)
{
    if (name != NULL && strcmp(name, CLASSIC_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_CLASSIC;
        return True;
    }
    if (name != NULL && strcmp(name, MODERN_LIGHT_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_MODERN_LIGHT;
        return True;
    }
    if (name != NULL && strcmp(name, HIGH_CONTRAST_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_HIGH_CONTRAST;
        return True;
    }
    if (name != NULL && strcmp(name, MODERN_DARK_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_MODERN_DARK;
        return True;
    }
    return False;
}

static Boolean
profile_resource_database(XmAppearanceProfile profile,
                          XmAppearanceDensity density, XrmDatabase *database)
{
    XrmDatabase defaults;

    if (profile == XmAPPEARANCE_CLASSIC) return True;
    defaults = profile_defaults(profile, density);
    if (defaults == NULL) return False;
    defaults = clone_database(defaults);
    if (defaults == NULL)
        return False;
    XrmCombineDatabase(defaults, database, False);
    return True;
}

Boolean
XmSetAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XrmDatabase database;
    XrmValue value;
    char *type = NULL;
    XmAppearanceProfile current;
    XmAppearanceDensity density;
    const char *name;

    if (display == NULL)
        return False;

    if (appearance_transaction_is_active(display))
        return False;

    name = profile_name(profile);
    if (name == NULL || !XmGetAppearanceDensity(display, &density))
        return False;

    database = XrmGetDatabase(display);
    if (database != NULL &&
        XrmGetResource(database, APPEARANCE_RESOURCE, APPEARANCE_CLASS,
                       &type, &value)) {
        if (!XmGetAppearanceProfile(display, &current) || current != profile)
            return False;
        return profile_resource_database(profile, density, &database);
    }
    if (database == NULL) {
        XrmInitialize();
        database = XrmGetStringDatabase("");
        if (database == NULL)
            return False;
    }

    if (ensure_display_state(display) == NULL)
        return False;

    if (!profile_resource_database(profile, density, &database))
        return False;

    XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    XrmSetDatabase(display, database);
    return True;
}

Boolean
XmGetAppearanceProfile(Display *display, XmAppearanceProfile *profile_return)
{
    XrmDatabase database;
    XrmValue value;
    char *type = NULL;

    if (display == NULL || profile_return == NULL)
        return False;

    *profile_return = XmAPPEARANCE_CLASSIC;
    database = XrmGetDatabase(display);
    if (database == NULL ||
        !XrmGetResource(database, APPEARANCE_RESOURCE, APPEARANCE_CLASS,
                        &type, &value))
        return True;

    return profile_from_name(value.addr, profile_return);
}

static Boolean
apply_appearance_settings(Display *display, XmAppearanceProfile profile,
                          XmAppearanceDensity density, Boolean select_profile)
{
    XmAppearanceDisplayState *state;
    XmAppearanceProfile current;
    XmAppearanceDensity current_density;
    XrmDatabase database;
    XrmDatabase old_database;
    const char *name;

    if (display == NULL || profile_name(profile) == NULL ||
        (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
         density != XmAPPEARANCE_DENSITY_COMPACT) ||
        appearance_transaction_is_active(display))
        return False;
    if (!XmGetAppearanceProfile(display, &current) ||
        !XmGetAppearanceDensity(display, &current_density))
        return False;
    if (current == profile && current_density == density)
        return True;
    state = ensure_display_state(display);
    if (state == NULL)
        return False;
    database = clone_database(state->application_database);
    if (database == NULL || !profile_resource_database(profile, density, &database)) {
        if (database != NULL)
            XrmDestroyDatabase(database);
        return False;
    }
    if (!appearance_transaction_begin(state)) {
        XrmDestroyDatabase(database);
        return False;
    }
    name = profile_name(profile);
    /* Choosing density alone must not freeze the startup profile at classic. */
    if (select_profile || current != XmAPPEARANCE_CLASSIC) {
        XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    } else {
        XrmValue value;
        char *type = NULL;
        XrmDatabase previous = XrmGetDatabase(display);
        if (previous != NULL && XrmGetResource(previous, APPEARANCE_RESOURCE,
                                              APPEARANCE_CLASS, &type, &value))
            XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    }
    XrmPutStringResource(&database, DENSITY_RESOURCE,
        density == XmAPPEARANCE_DENSITY_COMPACT ? "compact" : "comfortable");
    old_database = XrmGetDatabase(display);
    if ((current != XmAPPEARANCE_CLASSIC || profile != XmAPPEARANCE_CLASSIC) &&
        !_XmApplyAppearanceChange(display, current, profile, density)) {
        appearance_transaction_end(state);
        XrmDestroyDatabase(database);
        return False;
    }
    XrmSetDatabase(display, database);
    if (state->owns_active_database && old_database != NULL)
        XrmDestroyDatabase(old_database);
    state->active_database = database;
    state->owns_active_database = True;
    appearance_transaction_end(state);
    return True;
}

Boolean
XmApplyAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XmAppearanceDensity density;
    return XmGetAppearanceDensity(display, &density) &&
           apply_appearance_settings(display, profile, density, True);
}

Boolean
XmGetAppearanceDensity(Display *display, XmAppearanceDensity *density_return)
{
    XrmValue value;
    char *type = NULL;
    XrmDatabase database;
    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    if (display == NULL || density_return == NULL) return False;
    database = XrmGetDatabase(display);
    if (database != NULL && XrmGetResource(database, DENSITY_RESOURCE,
                                          DENSITY_CLASS, &type, &value)) {
        if (value.addr == NULL) return False;
        if (strcmp(value.addr, "compact") == 0)
            density = XmAPPEARANCE_DENSITY_COMPACT;
        else if (strcmp(value.addr, "comfortable") != 0)
            return False;
    }
    *density_return = density;
    return True;
}

Boolean
XmApplyAppearanceDensity(Display *display, XmAppearanceDensity density)
{
    XmAppearanceProfile profile;
    return XmGetAppearanceProfile(display, &profile) &&
           apply_appearance_settings(display, profile, density, False);
}

Boolean
XmSetAppearanceDensity(Display *display, XmAppearanceDensity density)
{
    XmAppearanceDensity current;
    XmAppearanceWidgetState *state;
    Boolean controls_exist = False;
    if (display == NULL ||
        (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
         density != XmAPPEARANCE_DENSITY_COMPACT) ||
        appearance_transaction_is_active(display) ||
        !XmGetAppearanceDensity(display, &current)) return False;
    if (current == density) return True;
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            state->widget != NULL && !XtIsShell(state->widget)) {
            controls_exist = True;
            break;
        }
    _XmProcessUnlock();
    if (controls_exist) return False;
    return XmApplyAppearanceDensity(display, density);
}

Boolean
XmGetAppearanceSpacing(Display *display, XmAppearanceSpacing spacing,
                        Dimension *pixels_return)
{
    static const Dimension logical[] = {4, 8, 12, 16, 24};
    double scale = 1.0;
    if (display == NULL || pixels_return == NULL ||
        (unsigned int)spacing >= XtNumber(logical)) return False;
    (void)_XmGetDpiScale(display, &scale);
    *pixels_return = _XmDpiScaleDimension(scale, logical[spacing]);
    return True;
}
