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
GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc-v1.91/bin
#GCC_TOOCHAIN	?= /opt/gcc-riscv/riscv-wch-embedded-gcc12-v1.91/bin

# riscv-none-embed- or riscv-none-elf- for gcc12
GCC_PREFIX		?= riscv-none-embed-

OPENOCD_PATH	?= /opt/openocd/wch-openocd-v1.91/bin

##### Paths ############

LDSCRIPT		= Libraries/Ld/Link.ld
OPENOCD_CFG 	= Misc/wch-riscv.cfg.v1.91
LIB_FLAGS		= 

# C source folders
CDIRS	:= Apps/TimerDemo/src \
		Libraries/RVMSIS \
		Libraries/StdPeriphDriver
# C source files (if there are any single ones)
CFILES := 

# ASM source folders
ADIRS	:= Apps/TimerDemo/src
# ASM single files
AFILES	:= Libraries/Startup/startup_CH583.S

LIBS 	:= Libraries/StdPeriphDriver/libISP583.a

# Include paths
INCLUDES	:= Apps/TimerDemo/src \
		Libraries/RVMSIS \
		Libraries/StdPeriphDriver/inc

##### Optional Libraries ############

include ../../rules.mk
