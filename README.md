# NuMaker mbed uVisor Example

This is a simple example to show how to use several uVisor APIs to build a secured application.

This demo contains three secure boxes:

- AES Box, which provides AES cipher functions through RPC, with cipher key closed.
- AES Client Box, which is client of above and cannot touch the cipher key.
- Snoop Box, which attempts to snoop the cipher key via IRQ.

The insecure box 0 (main box) does nothing.

## Building

The example currently only works on NUMAKER_PFM_M487 with the GCC_ARM toolchain.

### Release

For a release build, please enter:

```bash
$ mbed compile -m NUMAKER_PFM_M487 -t GCC_ARM --app-config mbed_app.json -c
```

You will find the resulting binary in `BUILD/NUMAKER_PFM_M487/GCC_ARM/mbed-os-example-uvisor.bin`. You can drag and drop it onto your board USB drive.

### Debug

On most targets, uVisor debug message is output through semihosting. When a debugger is connected, you can observe debug output from uVisor.
Please note that these messages are sent through semihosting, which halts the program execution if a debugger is not connected. For more information please read the [Debugging uVisor on mbed OS](https://github.com/ARMmbed/uvisor/blob/master/docs/api/DEBUGGING.md) guide.
On NUMAKER_PFM_M487 target, uVisor debug message is output through USB VCOM. You can observe debug output from uVisor as usual program.
To build a debug version of the program:

```bash
$ mbed compile -m NUMAKER_PFM_M487 -t GCC_ARM --app-config mbed_app.json --profile mbed-os/tools/profiles/debug.json -c
```
