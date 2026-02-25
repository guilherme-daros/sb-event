# Antimony Event & System

A decoupled event notification system and POSIX signal management bridge.

## Features
- **Event System:** 
  - Decoupled Notifier/Listener pattern.
  - Static registration (no central manager required).
  - Support for both Synchronous and Asynchronous (Thread-pool based) notifications.
- **Signal System:** 
  - Bridges POSIX signals (SIGINT, SIGTERM, etc.) into the `sb::event` system.
  - Safe signal handling using a pipe-to-thread bridge to avoid async-signal-safety issues.

## Dependencies
- `antimony-types`
- `antimony-logger`

## Usage (CMake)
```cmake
add_subdirectory(extern/antimony-event-sys)
target_link_libraries(YourTarget PRIVATE sb::event sb::system)
```
