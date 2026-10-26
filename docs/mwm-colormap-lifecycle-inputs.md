# MWM colormap lifecycle inputs

Colormap focus, installation, and reset helpers now fail closed for incomplete
inputs:

- Focus and install helpers return when screen data is missing.
- `ResetColormapData` returns for a negative count or for a positive count
  without a window array.
- `ProcessColormapList` requires both screen and client records.

Valid focus changes, colormap installation, and list resets retain their
existing behavior.
