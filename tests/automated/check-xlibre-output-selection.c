/* Verify the deterministic XLibre shell-boundary output policy. */
#include <stdio.h>
#include <stdlib.h>

#include "XlibreDpiI.h"

static int failed;

static void
expect_output(const char *name,
              const _XmXlibreDpiOutput *outputs,
              size_t output_count,
              const _XmXlibreDpiRect *client_area,
              RROutput expected)
{
    RROutput actual = _XmXlibreDpiSelectOutput(outputs, output_count,
                                               client_area);

    if (actual != expected) {
        fprintf(stderr, "%s: selected=%lu expected=%lu\n", name,
                (unsigned long)actual, (unsigned long)expected);
        failed = 1;
    }
}

int
main(void)
{
    const _XmXlibreDpiOutput outputs[] = {
        { 20, { 0, 0, 1920, 1080 } },
        { 10, { 1920, 0, 1920, 1080 } },
        { 30, { 0, 1080, 1920, 1080 } },
    };
    const _XmXlibreDpiOutput overlapping[] = {
        { 20, { 0, 0, 1920, 1080 } },
        { 5, { 0, 0, 1920, 1080 } },
    };
    const _XmXlibreDpiRect left = { 100, 100, 800, 600 };
    const _XmXlibreDpiRect right = { 2100, 100, 800, 600 };
    const _XmXlibreDpiRect lower = { 100, 1200, 800, 600 };
    const _XmXlibreDpiRect seam = { 1800, 100, 400, 600 };
    const _XmXlibreDpiRect gap = { 1870, 1030, 100, 100 };
    const _XmXlibreDpiRect invalid = { 0, 0, 0, 100 };

    expect_output("center containment", outputs, 3, &left, 20);
    expect_output("overlap tie", overlapping, 2, &left, 5);
    expect_output("second output", outputs, 3, &right, 10);
    expect_output("third output", outputs, 3, &lower, 30);
    expect_output("boundary center", outputs, 3, &seam, 10);
    expect_output("nearest tie", outputs, 3, &gap, 10);
    expect_output("zero-sized shell", outputs, 3, &invalid, None);
    expect_output("empty set", outputs, 0, &left, None);
    expect_output("null area", outputs, 3, NULL, None);
    if (failed)
        return EXIT_FAILURE;
    puts("PASS XLibre shell output selection policy");
    return EXIT_SUCCESS;
}
