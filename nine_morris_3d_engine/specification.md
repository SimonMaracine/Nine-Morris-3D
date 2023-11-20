# Engine Specification

## Entry point

Don't declare the main function, but include `engine/entry_point.hpp` in your main compilation unit and declare
there `int application_main() {}`.

## Application

An instance of an application or game is created with an `Application` instance on the stack.

Only one instance of the application should be running at a time.

Application parameters are defined with `ApplicationBuilder`.

`Application::initialize_applications()` must be called at the very beginning.
