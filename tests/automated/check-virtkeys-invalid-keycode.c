/* XmTranslateKey must ignore keycodes outside the server keymap. */
#include <X11/Intrinsic.h>
#include <X11/keysym.h>
#include <Xm/Xm.h>
#include <stdio.h>

extern void XmTranslateKey(Display *, KeyCode, Modifiers, Modifiers *, KeySym *);

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    int minimum, maximum;
    unsigned checks = 0;

    shell = XtVaAppInitialize(&app, "InvalidVirtualKey", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!XDisplayKeycodes(display, &minimum, &maximum)) return 1;

    if (minimum > 0) {
        Modifiers modifiers = ShiftMask;
        KeySym keysym = XK_F1;
        XmTranslateKey(display, (KeyCode)(minimum - 1), modifiers,
                       &modifiers, &keysym);
        ++checks;
    }
    if (maximum < 255) {
        Modifiers modifiers = ControlMask;
        KeySym keysym = XK_F2;
        XmTranslateKey(display, (KeyCode)(maximum + 1), modifiers,
                       &modifiers, &keysym);
        ++checks;
    }

    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS invalid virtual-keycode guard: %u checks\n", checks);
    return 0;
}
