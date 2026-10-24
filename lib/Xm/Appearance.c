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

static const char *
profile_name(XmAppearanceProfile profile)
{
    switch (profile) {
    case XmAPPEARANCE_CLASSIC:
        return CLASSIC_PROFILE;
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
    return False;
}

Boolean
XmSetAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XrmDatabase database;
    const char *name;

    if (display == NULL)
        return False;

    name = profile_name(profile);
    if (name == NULL)
        return False;

    database = XrmGetDatabase(display);
    if (database == NULL) {
        XrmInitialize();
        database = XrmGetStringDatabase("");
        if (database == NULL)
            return False;
    }

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
