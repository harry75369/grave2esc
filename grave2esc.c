#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
syn            = {.type = EV_SYN , .code = SYN_REPORT   , .value = 0},
grave_up       = {.type = EV_KEY , .code = KEY_GRAVE    , .value = 0},
left_alt_up    = {.type = EV_KEY , .code = KEY_LEFTALT  , .value = 0},
right_alt_up   = {.type = EV_KEY , .code = KEY_RIGHTALT , .value = 0};
// clang-format on

void print_usage(FILE *stream, const char *program) {
    // clang-format off
    fprintf(stream,
            "grave2esc - transforming the GRAVE key to the ESC key\n"
            "\n"
            "usage: %s [-h]\n"
            "\n"
            "options:\n"
            "    -h        show this message and exit\n"
            "    -m mode   0: (default) the physical key is GRAVE\n"
            "              1: the physical key is ESC\n",
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

void write_event_with_mode(struct input_event *event, int mode) {
    enum {
        MOD_NONE     = 0x0,
        MOD_SHIFT    = 0x1,
        MOD_LEFTALT  = 0x2,
        MOD_RIGHTALT = 0x4,
    };
    static int mod_key = 0;

    // save mod state
    if (event->type == EV_KEY && event->value != 0) {
        if (event->code == KEY_LEFTSHIFT || event->code == KEY_RIGHTSHIFT) {
            mod_key |= MOD_SHIFT;
        } else if (event->code == KEY_LEFTALT) {
            mod_key |= MOD_LEFTALT;
        } else if (event->code == KEY_RIGHTALT) {
            mod_key |= MOD_RIGHTALT;
        }
    } else if (event->type == EV_KEY && event->value == 0) {
        if (event->code == KEY_LEFTSHIFT || event->code == KEY_RIGHTSHIFT) {
            mod_key &= ~MOD_SHIFT;
        } else if (event->code == KEY_LEFTALT) {
            mod_key &= ~MOD_LEFTALT;
        } else if (event->code == KEY_RIGHTALT) {
            mod_key &= ~MOD_RIGHTALT;
        }
    }

    // modify key
    if (event->type == EV_KEY) {
        if (mode == 0) {
            if (event->code == KEY_GRAVE) {
                if (mod_key & MOD_SHIFT) {
                    // shift + grave => tilde
                } else if (mod_key & MOD_LEFTALT) {
                    // alt + grave => grave
                    write_event(&left_alt_up);
                    write_event(&syn);
                } else if (mod_key & MOD_RIGHTALT) {
                    // alt + grave => grave
                    write_event(&right_alt_up);
                    write_event(&syn);
                } else {
                    // grave => esc
                    event->code = KEY_ESC;
                    if (event->value == 0) {
                        write_event(&grave_up);
                        write_event(&syn);
                    }
                }
            }
        } else if (mode == 1) {
            if (event->code == KEY_ESC) {
                if (mod_key & MOD_SHIFT) {
                    // shift + esc => tilde
                    event->code = KEY_GRAVE;
                } else if (mod_key & MOD_LEFTALT) {
                    // alt + esc => grave
                    write_event(&left_alt_up);
                    write_event(&syn);
                    event->code = KEY_GRAVE;
                } else if (mod_key & MOD_RIGHTALT) {
                    // alt + esc => grave
                    write_event(&right_alt_up);
                    write_event(&syn);
                    event->code = KEY_GRAVE;
                }
            }
        }
    }

    // write event
    write_event(event);
}

int main(int argc, char *argv[]) {
    int mode = 0;

    for (int opt; (opt = getopt(argc, argv, "hm:")) != -1;) {
        switch (opt) {
            case 'h':
                return print_usage(stdout, argv[0]), EXIT_SUCCESS;
            case 'm':
                mode = atoi(optarg);
                continue;
        }

        return print_usage(stderr, argv[0]), EXIT_FAILURE;
    }

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    struct input_event event;
    while (read_event(&event)) {
        if (event.type == EV_MSC && event.code == MSC_SCAN)
            continue;

        if (event.type != EV_KEY && event.type != EV_REL &&
            event.type != EV_ABS) {
            write_event(&event);
            continue;
        }

        write_event_with_mode(&event, mode);
    }
}
