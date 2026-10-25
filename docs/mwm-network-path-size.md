---
nav_exclude: true
---

# MWM network-file path sizing

MWM expands `host:~/path` and local `~/path` configuration references using the
user's home directory. Both expansions now use checked `size_t` arithmetic and
verify the Xt allocation before copying. An unrepresentable network expansion
returns to the ordinary fallback resolver; an unrepresentable local expansion
returns `NULL` instead of wrapping an `int` length.

Normal network and local path resolution is unchanged. The focused regression
check is `tests/automated/check-mwm-network-path-size.py`.
