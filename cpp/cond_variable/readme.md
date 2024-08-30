# C++ condition_variable wrapper

- C++
- C++ 17
- condition_variable

Project contains three `condition_variable` wrappers. Visual Studio 2022 solution is used, but any C++ 17 compiler and C++ IDE are OK.

All wrappers contain the following methods:
- `set` - post notification or message with parameters.
- `wait` - wait for a message/notification.

Wrappers:

- `notification`. Works like Windows API `PostMessage` without parameters. Several `set` calls - the same number of successful `wait` calls.
- `notification_single`. Works like Windows API auto-reset event. Several `set` calls - at least one successful wait call.
- `notification_data`. Template, works like message queue for a given type `T`.
