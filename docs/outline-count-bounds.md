# Outline count bounds

Outline constraint argument scanning and recursive node-child traversal now
use Xt `Cardinal` counters matching their `num_args` and hierarchy child
counts. Constraint updates and hierarchy layout behavior are unchanged while
large valid lists avoid signed-counter wraparound.
