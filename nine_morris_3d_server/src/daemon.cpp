#include "daemon.hpp"

#include <cstdio>
#include <cstdarg>

// https://www.freedesktop.org/software/systemd/man/latest/systemd.service.html

#ifndef SM_SERVER_NO_SYSTEMD

#include <systemd/sd-daemon.h>

void notify_ready() {
    sd_notify(0, "READY=1\nSTATUS=Running");
}

void notify_stopping() {
    sd_notify(0, "STOPPING=1\nSTATUS=Stopped");
}

void notify_stopping(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256u] {};
    std::vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    sd_notifyf(0, "STOPPING=1\nSTATUS=%s", buffer);
}

void notify_status(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256u] {};
    std::vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    sd_notifyf(0, "STATUS=%s", buffer);
}

#else

void notify_ready() {}
void notify_stopping() {}
void notify_stopping(const char*, ...) {}
void notify_status(const char*, ...) {}

#endif  // SM_NO_SYSTEMD
