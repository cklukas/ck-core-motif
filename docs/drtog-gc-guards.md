# Toggle drawing GC guards

The shared diamond and circle drawing helpers can be reached while a widget is
partially initialized or being destroyed. Their old paths passed missing
graphics contexts into Xlib: diamonds could dereference a null top or bottom
GC, while circles unconditionally queried both shadow GCs when a shadow was
requested.

`DrTog.c` now returns safely when a required GC is absent. A diamond still
allows its existing small-width top-only cases, and circles retain their
optional center GC behavior. The public drawing APIs and normal geometry are
unchanged. `tests/automated/check-drtog-gc-guards.py` covers the guards.
