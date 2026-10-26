# MWM icon teardown

Icon cleanup can run after partial workspace setup. `FreeIcon` now validates
the WSM workspace list before reading its first icon frame and checks icon
window IDs in both WSM and non-WSM builds. Destroyed IDs are cleared so a
second cleanup remains harmless.
