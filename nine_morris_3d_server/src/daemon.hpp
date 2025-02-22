// Signal systemd that we're up and running
void notify_ready();

// Signal systemd that we're about to stop
void notify_stopping();

// Signal systemd that we're about to stop with an error (status) message
void notify_stopping(const char* format, ...);

// Set an arbitrary status message
void notify_status(const char* format, ...);
