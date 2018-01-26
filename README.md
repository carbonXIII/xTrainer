# xTrainer
A general purpose library for creating utilities or trainers for games and other applications. This grew into a separate project from my original trainer for the Dolphin Emulator. After this trainer is completely fleshed out with packet sniffing, DLL injection, and GUI overlay support, it will hopefully play a large role in creating the perfect scriptable trainer for Super Smash Bros. Melee running in Dolphin Emulator, allowing for custom code injection on the fly. Of course, feel free to adapt this library for your own personal use.
I AM NOT RESPONSIBLE FOR WHAT YOU DO WITH THIS LIBRARY.

Currently only supports Windows, though Linux support is planned in the future. Most functions have been ported to Linux at this point, except enumerateProcesses(), which can be avoided by removing some lines of code and manually entering a PID. **However, Linux support is not tested at all, not even sure if compiles. Expect it to crash and burn horribly.**

Planned features:
+Cross Platform support (see above)
+Interactive Scripting Support (perhaps using boost::python)
+DLL Injection
+Packet sniffing (perhaps useful for botting)
+GUI Overlays (perhaps through DLL injection)
