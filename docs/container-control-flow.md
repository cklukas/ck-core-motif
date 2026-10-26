---
nav_exclude: true
---
# Container control-flow clarity

Two Container branches mixed deep indentation with single-statement `if` and
`else` bodies.  The outline-visibility and keyboard-traversal paths now use
explicit blocks, preserving behavior while making the executed scope clear to
humans and compilers.  This removes the corresponding misleading-indentation
warnings from strict builds.
