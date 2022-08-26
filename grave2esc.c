#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

void print_usage(FILE *stream, const char *program) {
    // clang-format off
    fprintf(stream,
            "grave2esc - transforming the GRAVE key to the ESC key\n"
            "\n"
            "usage: %s [-h]\n"
            "\n"
            "options:\n"
            "    -h         show this message and exit\n",
            program);
    // clang-format on
}

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1) {
        exit(EXIT_FAILURE);
    }
}

void write_event_with_shift(struct input_event *event, int shift_held) {
    if (event->type == EV_KEY) {
        if (!shift_held) {
            switch (event->code) {
                case KEY_GRAVE:
                    event->code = KEY_ESC;
                    break;
                case KEY_ESC:
                    event->code = KEY_GRAVE;
                    break;
            }
        }
    }
    write_event(event);
}

int main(int argc, char *argv[]) {
    for (int opt; (opt = getopt(argc, argv, "h")) != -1;) {
        switch (opt) {
            case 'h':
                return print_usage(stdout, argv[0]), EXIT_SUCCESS;
        }

        return print_usage(stderr, argv[0]), EXIT_FAILURE;
    }

    struct input_event input;
    enum { START, SHIFT_HELD } state = START;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        if (input.type != EV_KEY && input.type != EV_REL &&
            input.type != EV_ABS) {
            write_event(&input);
            continue;
        }

        switch (state) {
            case START:
                if (input.type == EV_KEY &&
                    (input.code == KEY_LEFTSHIFT ||
                     input.code == KEY_RIGHTSHIFT) &&
                    input.value != 0) {
                    state = SHIFT_HELD;
                }
                break;
            case SHIFT_HELD:
                if (input.type == EV_KEY &&
                    (input.code == KEY_LEFTSHIFT ||
                     input.code == KEY_RIGHTSHIFT) &&
                    input.value == 0) {
                    state = START;
                }
                break;
        }
        write_event_with_shift(&input, state == SHIFT_HELD);
    }
}
