---
nav_exclude: true
---

# MWM property element-count bounds

Xlib's `XChangeProperty` accepts its element count as a signed `int`, while
several MWM helpers receive an unsigned count or derive one from a byte span.
Each writer now validates the final element count against `INT_MAX` before
casting.  An oversized request is ignored, matching the existing invalid-input
behavior, rather than being narrowed into a different property length.

This applies to embedded-client, workspace-info, workspace-list,
workspace-presence, and MWM client-list writes.  Valid and zero-length property
writes retain their existing behavior.
