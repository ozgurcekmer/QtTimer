# QtTimer v1

A work stopwatch with responsive digits, explicit Ready/Running/Paused status,
and a navy blue theme with light blue digits and blue controls. Elapsed time is measured independently of display refreshes.

## Build and run

Requires CMake, a C++20 compiler, and Qt 6.3 or later with Widgets and Test.

```sh
cmake -S v1 -B v1/build
cmake --build v1/build
./v1/build/QtTimer
ctest --test-dir v1/build --output-on-failure
```

To build without tests or the Qt Test dependency, configure with `-DBUILD_TESTING=OFF`.

The interface uses larger 14-point labels, 15-point buttons, and a 22-point
in-window heading for laptop readability. The desktop controls the native title-bar font.

## Controls

- Space: start, pause, or resume.
- Ctrl+R: reset. Sessions of one minute or more require confirmation.
- Ctrl+M: toggle compact mode; leaving compact mode restores the previous window geometry.
- Always on top: ask the window manager to keep the timer above other windows.
- Hundredths: show fractional seconds (off by default).

Window geometry and display preferences are saved when the window closes.
Elapsed sessions are not saved across application restarts. The display refreshes
every 100 ms normally and every 30 ms with hundredths enabled; it does not promise
to render every hundredth. Always-on-top behaviour depends on the window manager.
