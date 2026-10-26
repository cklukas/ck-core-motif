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

/* Density changes layout defaults in modern profiles. Classic retains its
 * traditional resource defaults. Choose startup settings before creating
 * controls; use Apply for an existing hierarchy. */
typedef enum {
    XmAPPEARANCE_DENSITY_COMFORTABLE = 0,
    XmAPPEARANCE_DENSITY_COMPACT = 1
} XmAppearanceDensity;

typedef enum {
    XmAPPEARANCE_SPACING_XS = 0,
    XmAPPEARANCE_SPACING_SMALL,
    XmAPPEARANCE_SPACING_MEDIUM,
    XmAPPEARANCE_SPACING_LARGE,
    XmAPPEARANCE_SPACING_XL
} XmAppearanceSpacing;

extern Boolean XmSetAppearanceDensity(
    Display *display,
    XmAppearanceDensity density);
extern Boolean XmGetAppearanceDensity(
    Display *display,
    XmAppearanceDensity *density_return);
extern Boolean XmApplyAppearanceDensity(
    Display *display,
    XmAppearanceDensity density);

/* Returns 4/8/12/16/24 logical pixels converted once to display pixels.
 * Invalid arguments return False without modifying the output. */
extern Boolean XmGetAppearanceSpacing(
    Display *display,
    XmAppearanceSpacing spacing,
    Dimension *pixels_return);

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
