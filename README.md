# Minecraft Java Edition Ports to C
This repository contains ports of early Minecraft versions to the C programming language.

## Version: rd-132328 (RubyDung)
A faithful port of the early "rd-132328" version, now featuring:
- **Android 16 Support:** Compatible with modern Android devices using OpenGL ES 1.1.
- **Mobile UI:** Integrated virtual joystick and touch controls (Jump, Break, Place).
- **FPS Counter:** Always-on performance monitor.

### Building
For desktop:
```bash
make
./rubydung
```

For Android:
Use the provided `CMakeLists.txt` with the Android NDK.
