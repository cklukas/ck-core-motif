/*
 * Motif appearance-profile selection.
 *
 * This interface deliberately selects a profile before widgets are created;
 * it does not mutate realized widgets or replace the Xt resource model.
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
    XmAPPEARANCE_HIGH_CONTRAST = 2
} XmAppearanceProfile;

extern Boolean XmSetAppearanceProfile(
    Display *display,
    XmAppearanceProfile profile);

extern Boolean XmGetAppearanceProfile(
    Display *display,
    XmAppearanceProfile *profile_return);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"'. */
#endif

#endif /* _XmAppearance_h */
