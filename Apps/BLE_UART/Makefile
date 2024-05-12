##### Project #####

PROJECT			?= app
# Top path of the template
TOP				= ../..
# The path for generated files
BUILD_DIR		= Build

##### Options #####



##### Toolchains #######

#GCC_TOOCHAIN	?= /opt/gcc-riscv/xpack-riscv-none-embed-gcc-10.2.0-1.2/bin
#GCC_TOOCHAIN	?= /opt/gcc-riscv/xpack-riscv-none-elf-gcc-12.2.0-3/bin
#GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc-v1.60/bin
#GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc-v1.70/bin
#GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc-v1.91/bin
GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc12-v170/bin

# GCC 12: riscv-none-elf-
# GCC 11 and below: riscv-none-embed-
GCC_PREFIX		?= riscv-none-elf-

OPENOCD_PATH	?= /opt/openocd/wch-openocd-v1.91/bin

##### Paths ############

LDSCRIPT		= Libraries/Ld/Link.ld
OPENOCD_CFG 	= Misc/wch-riscv.cfg.v1.91
# add INT_SOFT for non-wch gcc
LIB_FLAGS		= 

# C source folders
CDIRS	:= Apps/BLE_UART/User \
		Apps/BLE_UART/HAL \
		Apps/BLE_UART/Profile \
		Libraries/RVMSIS \
		Libraries/StdPeriphDriver
# C source files (if there are any single ones)
CFILES := Apps/BLE_UART/ble_uart_service/ble_uart_service_16bit.c

# ASM source folders
ADIRS	:= Apps/TimerDemo/src
# ASM single files
AFILES	:= Libraries/Startup/startup_CH583.S

LIBS 	:= Libraries/StdPeriphDriver/libISP583.a \
		Apps/BLE_UART/Lib/libCH58xBLE.a \

# Include paths
INCLUDES	:= Apps/BLE_UART/User \
		Apps/BLE_UART/ble_uart_service \
		Apps/BLE_UART/HAL \
		Apps/BLE_UART/Lib \
		Apps/BLE_UART/Profile \
		Libraries/RVMSIS \
		Libraries/StdPeriphDriver/inc

##### Optional Libraries ############

include ../../rules.mk
