# Mwm builtin binding buffer lifetime

When default key or button bindings are expanded with the configured root-menu
name, `mwm` allocates a temporary parser string.  The legacy setup path never
released either string, so repeated per-screen initialization leaked memory; an
allocation failure also reached `sprintf` with a null pointer.

The setup now treats allocation failure as a fatal initialization error and
releases each temporary string immediately after parsing.  Binding text and
parser behavior are unchanged.

`tests/automated/check-mwm-binding-buffer-lifetime.py` protects this contract.
