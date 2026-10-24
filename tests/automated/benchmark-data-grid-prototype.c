#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

typedef struct {
    size_t rows, columns, top, selected_row, selected_column;
    uint64_t *ids;
    char *values;
} BenchModel;

static uint64_t now_ns(void) {
    struct timespec value;
    clock_gettime(CLOCK_MONOTONIC, &value);
    return (uint64_t)value.tv_sec * 1000000000ULL + (uint64_t)value.tv_nsec;
}

static size_t cell_size(void) { return 32; }

static int model_init(BenchModel *model, size_t rows, size_t columns) {
    size_t count = rows * columns;
    model->rows = rows;
    model->columns = columns;
    model->top = model->selected_row = model->selected_column = 0;
    model->ids = calloc(rows, sizeof(*model->ids));
    model->values = calloc(count, cell_size());
    if (!model->ids || !model->values) {
        free(model->ids);
        free(model->values);
        model->ids = NULL;
        model->values = NULL;
        return 0;
    }
    for (size_t row = 0; row < rows; ++row) {
        model->ids[row] = 1000000 + row;
        for (size_t column = 0; column < columns; ++column)
            snprintf(model->values + (row * columns + column) * cell_size(),
                     cell_size(), "r%u-c%u", (unsigned)row,
                     (unsigned)column);
    }
    return 1;
}

static void model_destroy(BenchModel *model) {
    free(model->ids);
    free(model->values);
}

static size_t layout(BenchModel *model, size_t height, size_t row_height) {
    size_t visible = height > 24 ? (height - 24) / row_height : 0;
    size_t last = model->rows > visible ? model->rows - visible : 0;
    if (model->top > last) model->top = last;
    return visible;
}

int main(void) {
    const struct { size_t rows, columns; const char *name; } cases[] = {
        {64, 8, "small"}, {512, 16, "typical"}, {4096, 32, "maximum"}
    };
    size_t index;
    puts("case,rows,columns,model_bytes,init_us,operations_us,visible_rows,maxrss_kib");
    for (index = 0; index < sizeof(cases) / sizeof(cases[0]); ++index) {
        BenchModel model = {0};
        uint64_t begin, init_end, operation_end;
        size_t visible, iteration;
        volatile size_t sink = 0;
        begin = now_ns();
        if (!model_init(&model, cases[index].rows, cases[index].columns))
            return EXIT_FAILURE;
        init_end = now_ns();
        visible = layout(&model, 640, 22);
        for (iteration = 0; iteration < 10000; ++iteration) {
            model.selected_row = (iteration * 17) % model.rows;
            model.selected_column = (iteration * 7) % model.columns;
            model.top = model.selected_row > visible ?
                model.selected_row - visible + 1 : 0;
            layout(&model, 640, 22);
            sink += model.top + model.selected_column;
            for (size_t row = 0; row < visible && model.top + row < model.rows;
                 ++row)
                for (size_t column = 0; column < model.columns; ++column)
                    sink += model.values[(model.top + row) * model.columns +
                                         column];
            if ((iteration % 1000) == 0)
                snprintf(model.values + (model.selected_row * model.columns +
                                         model.selected_column) * cell_size(),
                         cell_size(), "edit-%zu", iteration);
        }
        operation_end = now_ns();
        {
            struct rusage usage;
            getrusage(RUSAGE_SELF, &usage);
            printf("%s,%zu,%zu,%zu,%llu,%llu,%zu,%ld\n", cases[index].name,
                   model.rows, model.columns,
                   model.rows * sizeof(*model.ids) +
                   model.rows * model.columns * cell_size(),
                   (unsigned long long)((init_end - begin) / 1000),
                   (unsigned long long)((operation_end - init_end) / 1000),
                   visible, usage.ru_maxrss);
        }
        if (sink == SIZE_MAX) fputs("", stdout);
        model_destroy(&model);
    }
    return 0;
}
