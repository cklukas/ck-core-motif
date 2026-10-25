---
nav_exclude: true
---

# MWM preprocessor command bounds

When MWM preprocesses a configuration file, the configured preprocessor and
two generated paths form one shell command. The command is now measured before
writing into the fixed command buffer. An overlong command is rejected and its
temporary output name is removed, avoiding truncation or an out-of-bounds
write; normal commands are formatted exactly as before.

The focused regression check is
`tests/automated/check-mwm-preprocess-command-bounds.py`.
