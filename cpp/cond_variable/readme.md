# C++ condition_variable wrapper

- C++
- C++ 17
- condition_variable

Project contains four `condition_variable` wrappers. Visual Studio 2022 solution is used, but any C++ 17 compiler and C++ IDE are OK.

All wrappers contain the following methods:
- `set` - post notification or message with parameters.
- `wait` - wait for a message/notification.

Wrappers:

- `notification`. Works like Windows API `PostMessage` without parameters. Several `set` calls - the same number of successful `wait` calls.
- `notification_single`. Works like Windows API auto-reset event. Several `set` calls - at least one successful wait call.
- `notification_data`. Template, works like message queue for a given type `T`.
- `notification_data_v`. Template, works like message queue for a given type `T`. Internally uses `std::vector` to store items, and returns `std::vector` from `wait` function. Several `set` calls - at least one successful wait call.
