Simple-menu varargs count arithmetic
=====================================

The six simple-menu convenience constructors now check the addition of their
fixed internal argument count to the caller-derived count before storing it in
the legacy `int` `num_args` variable.  An unrepresentable sum returns `NULL`
after releasing the application lock, avoiding signed wraparound before the
allocation checks run.
