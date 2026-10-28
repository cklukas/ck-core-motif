---
nav_exclude: true
---
# Completion callback allocation fallback

Transfer completion normally snapshots its callback list before invoking any
callback, which permits reentrant callback registration and avoids holding the
process lock during user code. If that snapshot allocation fails, completion
now falls back to fetching each callback pointer under the lock and invoking it
unlocked. Drop cleanup and user completion notifications therefore still run
under allocation pressure.

Regression coverage: `tests/automated/check-done-proc-allocation-fallback.py`.
