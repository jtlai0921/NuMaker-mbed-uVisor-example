# Build with GCC toolchain; debug with Keil uVision

This is a simple guide for how to build with GCC toolchain and debug with Keil uVision.

The *NuMaker-PFM-M487* board is taken as an example for explanation.

## Hardware setup
1. Switch the *NuMaker-PFM-M487* board to **Debug** mode (by pushing No.1/2/3 to ON and No.4 to non-ON of the 4-switch row near board's USB socket).
1. Connect the board to the host computer via USB. If switch above is correct, you **WON'T** see USB drive in the host computer.

## Software setup
- [Keil MDK 5](http://www2.keil.com/mdk5) or afterwards 
- [Nu-Link Driver (Keil)](http://www.nuvoton.com/hq/products/microcontrollers/arm-cortex-m4-mcus/Software/?__locale=en&resourcePage=Y)

## Mbed command line
1. Guide GCC to generate Keil uVision-compatible debug information by adding the `"-gdwarf-2"` flag in the JSON file `mbed-os/tools/profiles/debug.json`.
    
    <pre>
    "GCC_ARM": {
            "common": ["-c", "-Wall", "-Wextra",
                    "-Wno-unused-parameter", "-Wno-missing-field-initializers",
                    "-fmessage-length=0", "-fno-exceptions", "-fno-builtin",
                        "-ffunction-sections", "-fdata-sections", "-funsigned-char",
                    "-MMD", "-fno-delete-null-pointer-checks",
                    "-fomit-frame-pointer", "-O0", “-g3”, <b>"-gdwarf-2"</b>],
    </pre>
    
1. Build *your_program* through mbed-cli and you would get *your_program*.elf in the BUILD/NUMAKER_PFM_M487/GCC_ARM folder.
    ```
    mbed compile -m NUMAKER_PFM_M487 -t GCC_ARM --profile debug
    ```

## Keil uVision IDE
1. Create a dummy Keil uVision project, e.g. *Debug_Med* through the menu **Project** > **New Project**. Select *Nuvoton/M480/M487JIDAE* for target.
1. Select *Nuvoton Nu-Link Debugger/M480* or *NULink Debugger/M480* as ICE driver through the menu **Debug**.
1. Copy *your_program*.elf built above to the uVision project folder **Debug_Mbed/Objects**.
1. Rename *Debug_Mbed* to *your_program*.elf through the menu **Debug** > **Name of Executable**.
1. Download *your_program*.elf through the  menu **Flash** > **Erase** (optional) and then **Flash** > **Download**.
1. Now, you can enter debug session through the menu **Debug** > **Start Debug Session** if everything goes well.
