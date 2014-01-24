################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
acquire.obj: ../acquire.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="acquire.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

blinky.obj: ../blinky.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="blinky.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

buttons.obj: ../buttons.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="buttons.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

cfal96x64x16.obj: /media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232/drivers/cfal96x64x16.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="cfal96x64x16.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

fat_usbmsc.obj: /media/windows/pi19404/CCS/ti/flash/f/third_party/fatfs/port/fat_usbmsc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="fat_usbmsc.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

ff.obj: ../ff.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="ff.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

flashstore.obj: ../flashstore.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="flashstore.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

lm4f232h5qd_startup_ccs.obj: ../lm4f232h5qd_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="lm4f232h5qd_startup_ccs.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

mmc-ek-lm4f232h5qd.obj: ../mmc-ek-lm4f232h5qd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="mmc-ek-lm4f232h5qd.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

usbstick.obj: ../usbstick.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O4 --opt_for_speed=5 --include_path="/opt/ti/ccsv5/tools/compiler/arm_5.1.1/include" --include_path="/media/windows/pi19404/CCS/ti/flash/f/boards/ek-lm4f232" --include_path="/opt/ti/flash/f/third_party/" --include_path="/opt/ti/flash/f" --gcc --define=ccs="ccs" --define=TARGET_IS_BLIZZARD_RA1 --define=PART_LM4F232H5QD --diag_warning=225 --preproc_with_compile --preproc_dependency="usbstick.pp" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


