---
nav_exclude: true
---

# MWM client-message initialization

X11 client-message events always carry five 32-bit payload words, even where a
particular Motif protocol uses only one, two, or three. MWM now clears its
outbound `XClientMessageEvent` records before assigning the defined fields.
This preserves the protocol values while making unused words deterministic and
preventing stack contents from being sent to clients.
