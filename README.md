# NuMaker mbed uVisor Example

This is a simple example to show how to build a secured application with uVisor on Nuvoton Mbed Enabled targets.
Please refer to [Arm uVisor](https://github.com/ARMmbed/uvisor) for its details.

This demo contains three secure boxes:

- Main Box (insecure box 0) does nothing.
- AES Box, which provides AES cipher functions through RPC, with cipher key closed.
- AES Client Box, which does cipher operations through about box exported RPC.
- Snoop Box, which attempts to snoop the cipher key triggered by pushing a button. As it happens, system will halt.

## Support platforms

- NuMaker-PFM-M487

## Building

### Release

For a release build, please enter:

```
$ mbed compile -m NUMAKER_PFM_M487 -t GCC_ARM --app-config mbed_app.json -c
```

You will find the resulting binary in `BUILD/NUMAKER_PFM_M487/GCC_ARM/mbed-os-example-uvisor.bin`. You can drag and drop it onto your board USB drive.

### Debug

For a debug build, please enter:

```
$ mbed compile -m NUMAKER_PFM_M487 -t GCC_ARM --app-config mbed_app.json --profile debug -c
```

#### uVisor core debug message
In debug version, uVisor core outputs debug message via semihosting. So it is necessary to connect to Debugger for running debug version.

#### Build with GCC; debug with Keil uVision
Currently only GCC_ARM toolchain is supported to build uVisor program. If you have Keil uVision 5 or afterwards installed, you could debug through the
[Build with GCC and debug with Keil uVision](BUILD_GCC_DEBUG_UVISION.md) guide.
