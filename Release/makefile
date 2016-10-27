################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := --cmd_file="C:/ti/CC3200SDK_1.2.0/cc3200-sdk/ti_rtos/ti_rtos_config/ccs/Default/configPkg/compiler.opt"
GEN_CMDS__FLAG := -l"C:/ti/CC3200SDK_1.2.0/cc3200-sdk/ti_rtos/ti_rtos_config/ccs/Default/configPkg/linker.cmd"

ORDERED_OBJS += \
"./APA102.obj" \
"./AudioInput.obj" \
"./BeatDetect.obj" \
"./Console.obj" \
"./Effect.obj" \
"./MSGEQ7.obj" \
"./Matrix.obj" \
"./Spectrum.obj" \
"./main.obj" \
"./network_common.obj" \
"./rom_pin_mux_config.obj" \
"./uart_if.obj" \
"../cc3200v1p32.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/CC3200SDK_1.2.0/cc3200-sdk/oslib/ccs/ti_rtos/ti_rtos.a" \
-l"C:/ti/CC3200SDK_1.2.0/cc3200-sdk/simplelink/ccs/OS/simplelink.a" \
-l"C:/ti/CC3200SDK_1.2.0/cc3200-sdk/driverlib/ccs/Release/driverlib.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

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
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
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
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
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
EXE_OUTPUTS += \
Stickman_V2.out \

EXE_OUTPUTS__QUOTED += \
"Stickman_V2.out" \

BIN_OUTPUTS += \
Stickman_V2.bin \

BIN_OUTPUTS__QUOTED += \
"Stickman_V2.bin" \


# All Target
all: Stickman_V2.out

dependents:
	-cd C:/ti/CC3200SDK_1.2.0/cc3200-sdk/ti_rtos/ti_rtos_config/ccs/Default && $(MAKE) all

# Tool invocations
Stickman_V2.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) C:/ti/CC3200SDK_1.2.0/cc3200-sdk/ti_rtos/ti_rtos_config/ccs/Default/configPkg/linker.cmd
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -g --define=ccs --define=USE_TIRTOS --define=SL_PLATFORM_MULTI_THREADED --define=cc3200 --diag_wrap=off --diag_warning=225 --display_error_number -z -m"Stickman_V2.map" --heap_size=16384 --stack_size=16384 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --display_error_number --diag_wrap=off --warn_sections --xml_link_info="Stickman_V2_linkInfo.xml" --rom_model -o "Stickman_V2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

Stickman_V2.bin: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "Stickman_V2.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "APA102.d" "AudioInput.d" "BeatDetect.d" "Console.d" "Effect.d" "MSGEQ7.d" "Matrix.d" "Spectrum.d" "main.d" "network_common.d" "rom_pin_mux_config.d" "uart_if.d" 
	-$(RM) "APA102.obj" "AudioInput.obj" "BeatDetect.obj" "Console.obj" "Effect.obj" "MSGEQ7.obj" "Matrix.obj" "Spectrum.obj" "main.obj" "network_common.obj" "rom_pin_mux_config.obj" "uart_if.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv6/utils/tiobj2bin/tiobj2bin" "Stickman_V2.out" "Stickman_V2.bin" "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armofd" "C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex" "C:/ti/ccsv6/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build
C:/ti/CC3200SDK_1.2.0/cc3200-sdk/ti_rtos/ti_rtos_config/ccs/Default/configPkg/linker.cmd:

-include ../makefile.targets

