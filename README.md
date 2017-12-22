# Dolphin Trainer
A trainer for use with Super Smash Bros. Melee Running in Dolphin Emulator

Currently only supports Windows, though Linux support is planned in the future. Most functions have been ported to Linux at this point, except enumerateProcesses(), which can be avoided by removing some lines of code and manually entering a PID. **However, Linux support is not tested at all, not even sure if compiles. Expect it to crash and burn horribly.**

A scripting engine, GUI interface, and in-game overlay are planned after various technical challenges are solved. Some of these include:
  + Hooking into functions of the emulated system
  + Hooking into the OpenGL/DirectX draw calls to draw elements to the Dolphin window (the GUI overlay); This functionality is very likely to become a separate library if one does not already exist.
