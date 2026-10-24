/* SPDX-License-Identifier: LGPL-2.0-or-later */
/*
 * WP-13 investigation prototype.  Worker threads own Result allocations and
 * publish only pointers through a POSIX pipe.  XtAppAddInput is the sole
 * hand-off: all result validation, freeing and state changes happen on the
 * owning Xt thread.  This is deliberately a consumer fixture, not a libXm
 * API or a claim that widgets are thread-safe.
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WORKERS 4U
#define PHASE_ONE_POSTS 80U
#define PHASE_TWO_ATTEMPTS 2000U
#define RESULT_MAGIC UINT32_C(0x434b5253)

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
    ++checks; \
} while (0)

typedef struct {
    uint32_t magic;
    uint32_t worker;
    uint32_t sequence;
} Result;

typedef struct {
    int write_fd;
    unsigned worker;
    unsigned attempts;
    atomic_bool *cancelled;
    atomic_uint *workers_done;
    atomic_ulong *post_failures;
} WorkerArgs;

typedef struct {
    XtAppContext app;
    int read_fd;
    XtInputId input_id;
    unsigned expected;
    unsigned delivered;
    unsigned freed_after_cancel;
    unsigned invalid_results;
    unsigned callbacks;
    unsigned workers;
    unsigned workers_done;
    unsigned phase;
    Boolean phase_done;
    Boolean failed;
    pthread_t ui_thread;
    atomic_bool cancelled;
    atomic_uint workers_finished;
    unsigned char pending[sizeof(Result *)];
    size_t pending_size;
    XtIntervalId poll_timer;
} State;

static unsigned long checks;

static int
set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
        return 0;
    return 1;
}

static int
write_pointer(int fd, Result *result, atomic_bool *cancelled)
{
    unsigned char *bytes = (unsigned char *)&result;
    size_t offset = 0;
    while (offset < sizeof(result)) {
        ssize_t written = write(fd, bytes + offset, sizeof(result) - offset);
        if (written > 0) {
            offset += (size_t)written;
            continue;
        }
        if (written < 0 && (errno == EINTR))
            continue;
        if (written < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            if (atomic_load_explicit(cancelled, memory_order_acquire))
                return 0;
            sched_yield();
            continue;
        }
        return 0;
    }
    return 1;
}

static void *
worker_main(void *opaque)
{
    WorkerArgs *args = (WorkerArgs *)opaque;
    unsigned sequence;
    for (sequence = 0; sequence < args->attempts; ++sequence) {
        Result *result;
        if (atomic_load_explicit(args->cancelled, memory_order_acquire))
            break;
        result = (Result *)malloc(sizeof(*result));
        if (!result) {
            atomic_fetch_add_explicit(args->post_failures, 1UL,
                                      memory_order_relaxed);
            break;
        }
        result->magic = RESULT_MAGIC;
        result->worker = args->worker;
        result->sequence = sequence;
        if (!write_pointer(args->write_fd, result, args->cancelled)) {
            free(result);
            break;
        }
    }
    atomic_fetch_add_explicit(args->workers_done, 1U, memory_order_release);
    return NULL;
}

static void
finish_result(State *state, Result *result)
{
    if (!result || result->magic != RESULT_MAGIC ||
        result->worker >= state->workers) {
        ++state->invalid_results;
        free(result);
        return;
    }
    if (state->phase == 1U) {
        ++state->delivered;
    } else {
        ++state->freed_after_cancel;
    }
    free(result);
}

static void
consume_bytes(State *state, const unsigned char *bytes, size_t count)
{
    while (count != 0) {
        size_t needed = sizeof(Result *) - state->pending_size;
        size_t take = count < needed ? count : needed;
        memcpy(state->pending + state->pending_size, bytes, take);
        state->pending_size += take;
        bytes += take;
        count -= take;
        if (state->pending_size == sizeof(Result *)) {
            Result *result = NULL;
            memcpy(&result, state->pending, sizeof(result));
            state->pending_size = 0;
            finish_result(state, result);
        }
    }
}

static void
input_ready(XtPointer client_data, int *source, XtInputId *id)
{
    State *state = (State *)client_data;
    unsigned char buffer[256];
    ssize_t count;
    (void)source;
    (void)id;
    ++state->callbacks;
    if (!pthread_equal(pthread_self(), state->ui_thread)) {
        state->failed = True;
        return;
    }
    ++checks;
    for (;;) {
        count = read(state->read_fd, buffer, sizeof(buffer));
        if (count > 0) {
            consume_bytes(state, buffer, (size_t)count);
            continue;
        }
        if (count < 0 && errno == EINTR)
            continue;
        if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            break;
        if (count == 0)
            break;
        state->failed = True;
        break;
    }
    if (state->phase == 1U && state->delivered == state->expected &&
        state->pending_size == 0)
        state->phase_done = True;
}

static void
poll_phase(XtPointer client_data, XtIntervalId *id)
{
    State *state = (State *)client_data;
    int source = state->read_fd;
    XtInputId input_id = state->input_id;
    (void)id;
    if (!pthread_equal(pthread_self(), state->ui_thread)) {
        state->failed = True;
        return;
    }
    ++checks;
    /* A timer can win the Xt dispatch tie with the input source.  Drain once
     * here before declaring shutdown complete, so no queued Result is lost
     * when the input source is removed. */
    input_ready(state, &source, &input_id);
    if (state->phase == 2U &&
        atomic_load_explicit(&state->workers_finished, memory_order_acquire) ==
            state->workers && state->pending_size == 0) {
        state->phase_done = True;
        return;
    }
    state->poll_timer = XtAppAddTimeOut(state->app, 5, poll_phase, state);
}

static int
start_workers(State *state, unsigned attempts, pthread_t *threads,
              WorkerArgs *arguments, int write_fd, atomic_ulong *failures)
{
    unsigned worker;
    atomic_store_explicit(&state->workers_finished, 0U, memory_order_release);
    state->workers = WORKERS;
    for (worker = 0; worker < WORKERS; ++worker) {
        arguments[worker].write_fd = write_fd;
        arguments[worker].worker = worker;
        arguments[worker].attempts = attempts;
        arguments[worker].cancelled = &state->cancelled;
        arguments[worker].workers_done = &state->workers_finished;
        arguments[worker].post_failures = failures;
        if (pthread_create(&threads[worker], NULL, worker_main,
                           &arguments[worker]) != 0)
            return 0;
    }
    return 1;
}

static void
join_workers(pthread_t *threads)
{
    unsigned worker;
    for (worker = 0; worker < WORKERS; ++worker)
        (void)pthread_join(threads[worker], NULL);
}

static void
run_events_until_done(State *state)
{
    while (!state->phase_done && !state->failed)
        XtAppProcessEvent(state->app, XtIMAll);
}

static void
remove_input(State *state)
{
    if (state->input_id != (XtInputId)0) {
        XtRemoveInput(state->input_id);
        state->input_id = (XtInputId)0;
    }
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    State state;
    int phase_one[2], phase_two[2];
    pthread_t threads[WORKERS];
    WorkerArgs arguments[WORKERS];
    atomic_ulong post_failures;
    unsigned phase_one_expected = WORKERS * PHASE_ONE_POSTS;
    (void)argv;

    shell = XtVaAppInitialize(&app, "CkMotifCompletionPrototype", NULL, 0,
                              &argc, argv, NULL, NULL);
    memset(&state, 0, sizeof(state));
    state.app = app;
    state.ui_thread = pthread_self();
    state.phase = 1U;
    atomic_init(&state.cancelled, False);
    atomic_init(&state.workers_finished, 0U);
    atomic_init(&post_failures, 0UL);
    CHECK(pipe(phase_one) == 0);
    CHECK(set_nonblocking(phase_one[0]));
    CHECK(set_nonblocking(phase_one[1]));
    state.read_fd = phase_one[0];
    state.expected = phase_one_expected;
    state.input_id = XtAppAddInput(app, phase_one[0], (XtPointer)XtInputReadMask,
                                   input_ready, &state);
    CHECK(state.input_id != (XtInputId)0);
    CHECK(start_workers(&state, PHASE_ONE_POSTS, threads, arguments,
                        phase_one[1], &post_failures));
    run_events_until_done(&state);
    join_workers(threads);
    CHECK(!state.failed);
    CHECK(state.delivered == phase_one_expected);
    CHECK(state.invalid_results == 0);
    CHECK(atomic_load_explicit(&post_failures, memory_order_relaxed) == 0UL);
    remove_input(&state);
    close(phase_one[0]);
    close(phase_one[1]);

    memset(&state.pending, 0, sizeof(state.pending));
    state.pending_size = 0;
    state.phase = 2U;
    state.phase_done = False;
    state.failed = False;
    state.delivered = 0;
    state.freed_after_cancel = 0;
    state.invalid_results = 0;
    atomic_store_explicit(&post_failures, 0UL, memory_order_relaxed);
    atomic_store_explicit(&state.cancelled, False, memory_order_release);
    CHECK(pipe(phase_two) == 0);
    CHECK(set_nonblocking(phase_two[0]));
    CHECK(set_nonblocking(phase_two[1]));
    state.read_fd = phase_two[0];
    state.input_id = XtAppAddInput(app, phase_two[0], (XtPointer)XtInputReadMask,
                                   input_ready, &state);
    CHECK(state.input_id != (XtInputId)0);
    CHECK(start_workers(&state, PHASE_TWO_ATTEMPTS, threads, arguments,
                        phase_two[1], &post_failures));
    state.poll_timer = XtAppAddTimeOut(app, 5, poll_phase, &state);
    XtAppAddTimeOut(app, 20, poll_phase, &state);
    while (!state.phase_done && !state.failed) {
        XtAppProcessEvent(app, XtIMAll);
        if (state.callbacks > 0 && !atomic_load_explicit(&state.cancelled,
                                                          memory_order_acquire))
            atomic_store_explicit(&state.cancelled, True, memory_order_release);
    }
    join_workers(threads);
    CHECK(!state.failed);
    CHECK(atomic_load_explicit(&state.cancelled, memory_order_acquire));
    CHECK(state.freed_after_cancel > 0);
    CHECK(state.invalid_results == 0);
    remove_input(&state);
    close(phase_two[0]);
    close(phase_two[1]);
    CHECK(state.pending_size == 0);
    CHECK(atomic_load_explicit(&post_failures, memory_order_relaxed) == 0UL);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS UI completion prototype: %lu checks, phase1=%u phase2-freed=%u callbacks=%u\n",
           checks, phase_one_expected, state.freed_after_cancel,
           state.callbacks);
    return 0;
}
