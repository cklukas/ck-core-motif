# Flyout destroy-state callbacks

Flyout owner and popup destroy callbacks now stop when the Flyout itself is
already being destroyed. This prevents late teardown notifications from
mutating popup, label, or visibility fields after widget destruction has begun.
