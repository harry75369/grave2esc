# grave2esc

Transforming the GRAVE/TILDE key(\`/~) to the ESC key, and vice-versa, for some 60%/70% layout keyboards.

NOTE: this tool is only for Linux, as a plugin for [Interception Tools][interception-tools] framework.

There are alternatives for other operating systems, for example:
- macOS: [Karabiner][]
- Windows: [Interception library][interception]

## What is it?

Depending which physical key you have on your keyboard

- If you only have ESC key
  - You already have physical ESC key, nice!
  - Turn Shift+ESC into TILDE
  - Turn Alt+ESC into GRAVE
- If you only have GRAVE key
  - Turn GRAVE into ESC, and at the same time:
  - Turn Shift+GRAVE into TILDE
  - Turn Alt+GRAVE into GRAVE

## Why?!

Because for smaller keyboards, esp. of 60%/70% layouts, the GRAVE key and the ESC key
may not co-exist. And some keyboard firmware has ridiculous settings about how to
switch between them, let alone some firmware that is not customizable at all. This
bothers particularly Vim users a lot!

## Dependencies

- [Interception Tools][interception-tools]

## Building

```text
$ git clone https://github.com/harry75369/grave2esc.git
$ cd grave2esc
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
```

## Installation

```text
$ sudo cmake --install build
```

## Execution

```text
grave2esc - transforming the GRAVE key to the ESC key

usage: grave2esc [-h]

options:
    -h        show this message and exit
    -m mode   0: (default) the physical key is GRAVE
              1: the physical key is ESC
```

`grave2esc` is an [_Interception Tools_][interception-tools] plugin. A suggested
`udevmon` job configuration (check the [_Interception Tools_
README][interception-tools] for alternatives) is:

(e.g. `/etc/udevmon.yaml`)

```yaml
- JOB: intercept -g $DEVNODE | grave2esc | uinput -d $DEVNODE
  DEVICE:
    EVENTS:
      EV_KEY: [KEY_GRAVE, KEY_ESC, KEY_LEFTSHIFT, KEY_RIGHTSHIFT]
```

Then enable and start `udevmon` service, for example, by creating systemd service file
`/etc/systemd/system/udevmon.service`

```
[Unit]
Description=udevmon
Wants=systemd-udev-settle.service
After=systemd-udev-settle.service

[Service]
ExecStart=/usr/bin/nice -n -20 /usr/bin/udevmon -c /etc/udevmon.yaml

[Install]
WantedBy=multi-user.target
```

and then start it with `sudo systemctl enable --now udevmon`

## History

This library is forked from and based on [`caps2esc`][caps2esc] project.

## License

<a href="https://github.com/harry75369/grave2esc/blob/master/LICENSE.md">
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0b/License_icon-mit-2.svg/120px-License_icon-mit-2.svg.png" alt="MIT">
</a>

- Copyright © 2022 Chaoya Li
- Copyright © 2017 Francisco Lopes da Silva

[interception-tools]: https://gitlab.com/interception/linux/tools
[caps2esc]: https://gitlab.com/interception/linux/plugins/caps2esc
[karabiner]: https://pqrs.org/osx/karabiner/
[interception]: https://github.com/oblitum/Interception
