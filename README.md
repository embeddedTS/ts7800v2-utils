# ts7800v2-utils

To build, download/unpack/clone and run:

./autogen.sh

./configure

make

System utilities for the TS-7800v2.  These are:<br>

    eth_phy_peekpoke, for accessing registers in the ethernet PHY.
    
    peekpoke, for accessing memory and memory-mapped registers.
    
    led7800, for controlling the status LEDs on the board.
    
    rtc7800, for accessing the RTC.
    
    ts7800ctl, for accessing ADCs, WDT, and sleep function.
    
    pwmctl, for controlling the PWMs.

    idleinject, for keeping the system from cooking itself.

    tshwinit, for setting startup conditions and TS-7800 backwards-compatibility
    
