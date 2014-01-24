################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv5/tools/compiler/arm_5.1.1

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./usbstick.obj" \
"./mmc-ek-lm4f232h5qd.obj" \
"./lm4f232h5qd_startup_ccs.obj" \
"./flashstore.obj" \
"./ff.obj" \
"./fat_usbmsc.obj" \
"./cfal96x64x16.obj" \
"./buttons.obj" \
"./blinky.obj" \
"./acquire.obj" \
"../blinky_ccs.cmd" \
-l"/media/windows/pi19404/CCS/ti/flash/f/usblib/ccs-cm4f/Debug/usblib-cm4f.lib" \
-l"/media/windows/pi19404/CCS/ti/flash/f/grlib/ccs-cm4f/Debug/grlib-cm4f.lib" \
-l"/media/windows/pi19404/CCS/ti/flash/f/driverlib/ccs-cm4f/Debug/driverlib-cm4f.lib" \
-l"rtsv7M4_T_le_v4SPD16_eabi.lib" \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: blinky.out

# Tool invocations
blinky.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 -z -m"blinky_ccs.map" --heap_size=4096 --stack_size=512 -i"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/lib" -i"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --reread_libs --warn_sections --display_error_number --xml_link_info="blinky_linkInfo.xml" --rom_model -o "blinky.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(TMS470_EXECUTABLE_OUTPUTS__QUOTED) "blinky.out"
	-$(RM) "acquire.pp" "blinky.pp" "buttons.pp" "cfal96x64x16.pp" "fat_usbmsc.pp" "ff.pp" "flashstore.pp" "lm4f232h5qd_startup_ccs.pp" "mmc-ek-lm4f232h5qd.pp" "usbstick.pp" 
	-$(RM) "acquire.obj" "blinky.obj" "buttons.obj" "cfal96x64x16.obj" "fat_usbmsc.obj" "ff.obj" "flashstore.obj" "lm4f232h5qd_startup_ccs.obj" "mmc-ek-lm4f232h5qd.obj" "usbstick.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"/opt/ti/ccsv5/utils/tiobj2bin/tiobj2bin" "blinky.out" "blinky.bin" "/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armofd" "/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armhex" "/opt/ti/ccsv5/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

