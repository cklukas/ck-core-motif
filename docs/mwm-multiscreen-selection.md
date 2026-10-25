# Mwm multi-screen selection naming

Mwm owns one `WM_S<number>` selection for each managed screen. The selection
name is now formatted from `wmGD.Screens[scr].screen` inside the per-screen
loop, so multiple screens no longer reuse the display's default screen atom.
The buffer is sized and formatted with `snprintf`; single-screen behavior is
unchanged.

`tests/automated/check-mwm-multiscreen-selection.py` guards this mapping.
