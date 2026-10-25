---
nav_exclude: true
---

# MWM workspace-string allocation

Workspace names and workspace-presence titles are now copied through the
existing width-checked catalog-string helper. This removes raw `strlen`-based
Xt allocations from both paths, preserving the existing warning and default
title fallbacks when copying fails.
