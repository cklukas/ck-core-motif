---
nav_exclude: true
---
# FontSelector child allocation

FontSelector now checks merged argument-list allocation, its button-box child,
and its scrolled text child before managing or registering callbacks. Partial
child construction returns cleanly without passing a null widget into Xt.
