---
nav_exclude: true
---
# Mwm temporary configuration cleanup

WSM preprocessing creates temporary configuration files.  The legacy cleanup
path assembled `/bin/rm <path>` in a fixed `MAXWMPATH+1` buffer and passed it to
the shell, so long names could overflow and shell metacharacters could change
the cleanup command.

Temporary files are now removed directly with `unlink()`.  Cleanup ordering and
ownership release remain unchanged, while path length and shell parsing no
longer affect the operation.

`tests/automated/check-mwm-temp-cleanup.py` protects this contract.
