# UIL resource-vector allocation

UIL semantic initialization now verifies every resource-compression vector
after allocation.  Invalid limits and allocation failures stop initialization
before the vectors are indexed, instead of allowing a zero-size or null vector
to be dereferenced.
