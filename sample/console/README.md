console
=======

Sample application for ESM.

Target environments
-------------------

Windows, Linux, macOS.

console is written in ISO C99/C++11, and so probably works fine on other OS.

How to build
------------

Use make and Makefile. Target name is `all`.
For example, on Unix environment, `make -f build-unix-gcc.mk all`.

| Toolset                           | Makefile           |
|:----------------------------------|:-------------------|
| Linux                             | build-unix-gcc.mk  |
| macOS                             | build-mac-clang.mk |
| MinGW/TDM-GCC (with GNU make)     | build-win-gcc.mk   |
| Microsoft Visual C++ (with NMAKE) | build-win-vc.mak   |

Usage
-----

Simply execute `console`. console has no option.

Example
-------

<pre>
$ <kbd>console</kbd>
<samp>state_1->event_handler->on_init();</samp>
<kbd>help</kbd>
<samp>exit    (no option)     Exit program.
help    (no option)     Show help message.
kill-gtimer     id      Kill global timer.
kill-timer      id      Kill timer.
next-handler    (no option)     Set next event handler.
post-msg-inner  (no option)     Post message (from main-loop() thread)
post-msg-outer  (no option)     Post message (from main thread)
set-gtimer      id timeout-millis repeat-[off|on]       Start global timer.
set-timer       id timeout-millis repeat-[off|on]       Start timer.</samp>
<kbd>next-handler</kbd>
<samp>state_1->event_handler->on_event(0x00000000);
state_1->event_handler->on_destroy();
state_1->event_handler->release_user_data();
state_2->event_handler->on_init();</samp>
<kbd>next-handler</kbd>
<samp>state_2->event_handler->on_event(0x00000000);
state_2->event_handler->on_destroy();
state_2->event_handler->release_user_data();
state_3->event_handler->on_init();</samp>
<kbd>next-handler</kbd>
<samp>state_3->event_handler->on_event(0x00000000);
state_3->event_handler->on_destroy();
state_3->event_handler->release_user_data();
state_1->event_handler->on_init();</samp>
<kbd>exit</kbd>
<samp>state_1->event_handler->on_destroy();
state_1->event_handler->release_user_data();</samp>
$ _
</pre>
