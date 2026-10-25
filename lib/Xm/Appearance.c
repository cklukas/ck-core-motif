/*
 * Motif appearance-profile selection.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#include <Xm/Appearance.h>
#include <Xm/Xm.h>
#include "XmI.h"

#define APPEARANCE_RESOURCE "ckCoreMotif.appearanceProfile"
#define APPEARANCE_CLASS "CkCoreMotif.AppearanceProfile"
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
    "*background: #f6f8fb\n"
    "*foreground: #202124\n"
    "*topShadowColor: #ffffff\n"
    "*bottomShadowColor: #c4ccd6\n"
    "*highlightColor: #1a73e8\n"
    "*highlightThickness: 2\n"
    "*shadowThickness: 1\n"
    "*armColor: #dbeafe\n"
    "*selectColor: #dbeafe\n"
    "*XmLabel.traversalOn: True\n"
    "*XmLabel.highlightThickness: 2\n"
    "*XmPushButton.marginWidth: 10\n"
    "*XmPushButton.marginHeight: 6\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.marginWidth: 10\n"
    "*XmToggleButton.marginHeight: 6\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.marginWidth: 8\n"
    "*XmTextField.marginHeight: 5\n"
    "*XmTextField.traversalOn: True\n"
    "*XmList.listMarginWidth: 8\n"
    "*XmList.listMarginHeight: 6\n"
    "*XmList.listSpacing: 2\n"
    "*XmScrollBar.highlightThickness: 2\n";

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
    "*highlightThickness: 2\n"
    "*shadowThickness: 1\n"
    "*armColor: #3c4043\n"
    "*selectColor: #3c4043\n"
    "*insensitiveForeground: #9aa0a6\n"
    "*XmToggleButton.bottomShadowColor: #9aa0a6\n"
    "*XmLabel.traversalOn: True\n"
    "*XmPushButton.marginWidth: 10\n"
    "*XmPushButton.marginHeight: 6\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.marginWidth: 10\n"
    "*XmToggleButton.marginHeight: 6\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.marginWidth: 8\n"
    "*XmTextField.marginHeight: 5\n"
    "*XmTextField.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 2\n";

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
    AP_COUNT
} AppearanceProperty;

typedef struct _XmAppearanceWidgetState {
    Widget widget;
    Display *display;
    Boolean supported[AP_COUNT];
    Boolean explicit_arg[AP_COUNT];
    Boolean owned[AP_COUNT];
    XtArgVal original[AP_COUNT];
    XtArgVal last[AP_COUNT];
    unsigned int references;
    Boolean retired;
    Boolean initialized;
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
    struct _XmAppearanceDisplayState *next;
} XmAppearanceDisplayState;

static XmAppearanceWidgetState *appearance_widgets;
static XmAppearanceDisplayState *appearance_displays;

void
_XmDestroyAppearanceDisplay(Display *display)
{
    XmAppearanceDisplayState **link;
    XmAppearanceDisplayState *state;
    XrmDatabase application_database;
    XrmDatabase active_database;

    if (display == NULL)
        return;

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
        XmNlistSpacing
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
           property == AP_LIST_MARGIN_HEIGHT || property == AP_LIST_SPACING;
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

static Boolean
class_contains(Widget widget, const char *text)
{
    return strstr(widget_class_name(widget), text) != NULL;
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

static const char *
profile_property_string(Widget widget, XmAppearanceProfile profile,
                        AppearanceProperty property)
{
    Boolean button = class_contains(widget, "PushButton") ||
                     class_contains(widget, "ToggleButton");
    Boolean text_field = class_contains(widget, "TextField");
    Boolean list = class_contains(widget, "List");
    Boolean traversal = class_contains(widget, "Label") || button ||
                        text_field || class_contains(widget, "ScrollBar");

    if (profile == XmAPPEARANCE_CLASSIC)
        return NULL;
    if (property == AP_BACKGROUND) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "#f6f8fb";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#000000";
        return "#202124";
    }
    if (property == AP_FOREGROUND) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "#202124";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#ffffff";
        return "#f1f3f4";
    }
    if (property == AP_TOP_SHADOW)
        return profile == XmAPPEARANCE_MODERN_LIGHT ? "#ffffff" : "#ffffff";
    if (property == AP_BOTTOM_SHADOW) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "#c4ccd6";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#ffffff";
        if (class_contains(widget, "ToggleButton")) return "#9aa0a6";
        return "#000000";
    }
    if (property == AP_HIGHLIGHT) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "#1a73e8";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#ffff00";
        return "#8ab4f8";
    }
    if (property == AP_ARM || property == AP_SELECT) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "#dbeafe";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#0000ff";
        return "#3c4043";
    }
    if (property == AP_INSENSITIVE_FOREGROUND) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return NULL;
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "#b3b3b3";
        return "#9aa0a6";
    }
    if (property == AP_HIGHLIGHT_THICKNESS) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "2";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "3";
        return "2";
    }
    if (property == AP_SHADOW_THICKNESS) {
        if (profile == XmAPPEARANCE_MODERN_LIGHT) return "1";
        if (profile == XmAPPEARANCE_HIGH_CONTRAST) return "2";
        return "1";
    }
    if (property == AP_TRAVERSAL)
        return traversal ? "True" : NULL;
    if (profile == XmAPPEARANCE_MODERN_LIGHT ||
        profile == XmAPPEARANCE_MODERN_DARK) {
        if (property == AP_MARGIN_WIDTH && button) return "10";
        if (property == AP_MARGIN_HEIGHT && button) return "6";
        if (property == AP_MARGIN_WIDTH && text_field) return "8";
        if (property == AP_MARGIN_HEIGHT && text_field) return "5";
        if (property == AP_LIST_MARGIN_WIDTH && list) return "8";
        if (property == AP_LIST_MARGIN_HEIGHT && list) return "6";
        if (property == AP_LIST_SPACING && list) return "2";
    }
    return NULL;
}

static Boolean
profile_property_value(Widget widget, XmAppearanceProfile profile,
                       AppearanceProperty property, XtArgVal *value_return)
{
    const char *text = profile_property_string(widget, profile, property);
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

static Boolean
appearance_resource_supported(Widget widget, AppearanceProperty property)
{
    XtResourceList resources = NULL;
    Cardinal count = 0;
    const char *name = appearance_property_name(property);
    Boolean supported = False;

    XtGetResourceList(XtClass(widget), &resources, &count);
    for (Cardinal i = 0; i < count; ++i)
        if (strcmp(resources[i].resource_name, name) == 0) {
            supported = True;
            break;
        }
    if (resources != NULL)
        XtFree((char *)resources);
    return supported;
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
appearance_xrm_value(Widget widget, AppearanceProperty property,
                     XrmValue *value_return)
{
    Widget chain[64];
    XrmName names[66];
    XrmClass classes[66];
    Widget current = widget;
    int count = 0;
    XrmRepresentation type;
    XrmDatabase database = XrmGetDatabase(XtDisplay(widget));
    const char *resource_class = appearance_property_name(property);
    XtResourceList resources = NULL;
    Cardinal resource_count = 0;

    if (database == NULL)
        return False;
    XtGetResourceList(XtClass(widget), &resources, &resource_count);
    for (Cardinal i = 0; i < resource_count; ++i)
        if (strcmp(resources[i].resource_name,
                   appearance_property_name(property)) == 0) {
            resource_class = resources[i].resource_class;
            break;
        }
    while (current != NULL && count < 64) {
        chain[count++] = current;
        current = XtParent(current);
    }
    for (int i = 0; i < count; ++i) {
        Widget item = chain[count - i - 1];
        names[i] = XrmStringToQuark(XtName(item));
        classes[i] = XrmStringToQuark(widget_class_name(item));
    }
    names[count] = XrmStringToQuark(appearance_property_name(property));
    classes[count] = XrmStringToQuark(resource_class);
    names[count + 1] = NULLQUARK;
    classes[count + 1] = NULLQUARK;
    if (resources != NULL)
        XtFree((char *)resources);
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
                           XmAppearanceChangePlan *plan)
{
    XtArgVal current[AP_COUNT];

    plan->state = state;
    memcpy(plan->owned, state->owned, sizeof(plan->owned));
    read_appearance_values(state, current);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        const char *profile_text;

        if (!state->supported[property])
            continue;
        /* A value changed outside the transaction is no longer toolkit-owned;
         * the prepared plan preserves that application value. */
        if (plan->owned[property] &&
            current[property] != state->last[property])
            plan->owned[property] = False;
        if (!plan->owned[property])
            continue;
        profile_text = profile_property_string(state->widget, new_profile,
                                               property);
        if (profile_text == NULL)
            plan->values[property] = state->original[property];
        else if (!profile_property_value(state->widget, new_profile, property,
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
            state->last[property] = plan->values[property];
        }
    if (count != 0)
        XtSetValues(state->widget, set_args, count);
    if (state->widget != NULL) {
        read_appearance_values(state, state->last);
        if (XtIsRealized(state->widget))
            XClearArea(XtDisplay(state->widget), XtWindow(state->widget),
                       0, 0, 0, 0, True);
    }
}

static void
initialize_appearance_state(XmAppearanceWidgetState *state,
                            XmAppearanceProfile current_profile)
{
    read_appearance_values(state, state->original);
    memcpy(state->last, state->original, sizeof(state->last));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        XrmValue xrm_value;
        const char *profile_text;
        if (!state->supported[property] || state->explicit_arg[property])
            continue;
        profile_text = profile_property_string(state->widget, current_profile,
                                               property);
        if (appearance_xrm_value(state->widget, property, &xrm_value)) {
            if (profile_text != NULL &&
                strcmp((const char *)xrm_value.addr, profile_text) == 0)
                state->owned[property] = True;
        } else {
            state->owned[property] = True;
        }
    }
    state->initialized = True;
}

Boolean
_XmRegisterAppearanceChange(Widget widget, ArgList args, Cardinal num_args)
{
    XmAppearanceWidgetState *state;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    XmAppearanceWidgetState *existing;

    if (widget == NULL || !XtIsWidget(widget))
        return False;
    state = (XmAppearanceWidgetState *)XtCalloc(1, sizeof(*state));
    if (state == NULL)
        return False;
    state->widget = widget;
    state->display = XtDisplayOfObject(widget);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->supported[property] = appearance_resource_supported(widget,
                                                                     property);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->explicit_arg[property] = appearance_explicit_arg(args, num_args,
                                                                 property);
    if (!XtIsShell(widget)) {
        (void)XmGetAppearanceProfile(state->display, &profile);
        initialize_appearance_state(state, profile);
    }
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
    XtAddCallback(widget, XmNdestroyCallback, appearance_widget_destroyed,
                  (XtPointer)state);
    return True;
}

Boolean
_XmApplyAppearanceChange(Display *display, XmAppearanceProfile old_profile,
                         XmAppearanceProfile new_profile)
{
    XmAppearanceWidgetState **snapshot = NULL;
    XmAppearanceChangePlan *plans = NULL;
    XmAppearanceWidgetState *state;
    Cardinal count = 0;
    Boolean success = True;

    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display)
            ++count;
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
            if (!appearance_widget_prepare(state, new_profile, &plans[i]))
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
    for (Cardinal i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL)
            appearance_widget_commit(&plans[i]);
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
profile_resource_database(XmAppearanceProfile profile, XrmDatabase *database)
{
    XrmDatabase defaults;

    if (profile == XmAPPEARANCE_MODERN_LIGHT)
        defaults = XrmGetStringDatabase(modern_light_resources);
    else if (profile == XmAPPEARANCE_HIGH_CONTRAST)
        defaults = XrmGetStringDatabase(high_contrast_resources);
    else if (profile == XmAPPEARANCE_MODERN_DARK)
        defaults = XrmGetStringDatabase(modern_dark_resources);
    else
        return True;
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
    const char *name;

    if (display == NULL)
        return False;

    if (appearance_transaction_is_active(display))
        return False;

    name = profile_name(profile);
    if (name == NULL)
        return False;

    database = XrmGetDatabase(display);
    if (database != NULL &&
        XrmGetResource(database, APPEARANCE_RESOURCE, APPEARANCE_CLASS,
                       &type, &value)) {
        if (!XmGetAppearanceProfile(display, &current) || current != profile)
            return False;
        return profile_resource_database(profile, &database);
    }
    if (database == NULL) {
        XrmInitialize();
        database = XrmGetStringDatabase("");
        if (database == NULL)
            return False;
    }

    if (ensure_display_state(display) == NULL)
        return False;

    if (!profile_resource_database(profile, &database))
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

Boolean
XmApplyAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XmAppearanceDisplayState *state;
    XmAppearanceProfile current;
    XrmDatabase database;
    XrmDatabase old_database;
    const char *name;

    if (display == NULL || profile_name(profile) == NULL)
        return False;
    if (!XmGetAppearanceProfile(display, &current))
        return False;
    if (current == profile)
        return True;
    state = ensure_display_state(display);
    if (state == NULL)
        return False;
    database = clone_database(state->application_database);
    if (database == NULL || !profile_resource_database(profile, &database)) {
        if (database != NULL)
            XrmDestroyDatabase(database);
        return False;
    }
    if (!appearance_transaction_begin(state)) {
        XrmDestroyDatabase(database);
        return False;
    }
    name = profile_name(profile);
    XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    old_database = XrmGetDatabase(display);
    if (!_XmApplyAppearanceChange(display, current, profile)) {
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
