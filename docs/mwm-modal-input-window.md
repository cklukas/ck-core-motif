# MWM modal input-window failure handling

System-modal dialogs use a per-screen input-only window to block background
input. That helper is optional when X resource allocation fails. Modal setup,
sibling placement, and teardown now configure, map, or unmap it only when its
window ID is valid,
so dialogs do not issue requests against `None` after helper creation failure.
