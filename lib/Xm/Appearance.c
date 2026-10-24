/*
 * Motif appearance-profile selection.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <X11/Xresource.h>

#include <Xm/Appearance.h>

#define APPEARANCE_RESOURCE "ckCoreMotif.appearanceProfile"
#define APPEARANCE_CLASS "CkCoreMotif.AppearanceProfile"
#define CLASSIC_PROFILE "classic"
#define MODERN_LIGHT_PROFILE "modern-light"

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

static const char *
profile_name(XmAppearanceProfile profile)
{
    switch (profile) {
    case XmAPPEARANCE_CLASSIC:
        return CLASSIC_PROFILE;
    case XmAPPEARANCE_MODERN_LIGHT:
        return MODERN_LIGHT_PROFILE;
    default:
        return NULL;
    }
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
    return False;
}

static Boolean
profile_resource_database(XmAppearanceProfile profile, XrmDatabase *database)
{
    XrmDatabase defaults;

    if (profile != XmAPPEARANCE_MODERN_LIGHT)
        return True;
    defaults = XrmGetStringDatabase(modern_light_resources);
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
