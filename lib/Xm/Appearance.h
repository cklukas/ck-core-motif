/*
 * Motif appearance-profile selection.
 *
 * XmSetAppearanceProfile selects the profile used by subsequently-created
 * widgets. XmApplyAppearanceProfile additionally applies a profile to an
 * existing widget hierarchy without replacing the Xt resource model.
 */
#ifndef _XmAppearance_h
#define _XmAppearance_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    XmAPPEARANCE_CLASSIC = 0,
    XmAPPEARANCE_MODERN_LIGHT = 1,
    XmAPPEARANCE_HIGH_CONTRAST = 2,
    XmAPPEARANCE_MODERN_DARK = 3
} XmAppearanceProfile;

extern Boolean XmSetAppearanceProfile(
    Display *display,
    XmAppearanceProfile profile);

extern Boolean XmGetAppearanceProfile(
    Display *display,
    XmAppearanceProfile *profile_return);

extern Boolean XmApplyAppearanceProfile(
    Display *display,
    XmAppearanceProfile profile);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"'. */
#endif

#endif /* _XmAppearance_h */
