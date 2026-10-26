# MWM bitmap-cache installation

The WSM bitmap-cache helper now rejects missing names, `None` pixmaps, and
zero dimensions, and checks `XGetImage` before calling `XmInstallImage`. This
keeps built-in icon-cache setup safe when an X resource is unavailable.
