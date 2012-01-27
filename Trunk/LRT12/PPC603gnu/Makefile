# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = PPC603gnu
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := C:/WindRiver/workspace
PRJ_ROOT_DIR := $(WS_ROOT_DIR)/LRT12



#Global Build Macros
PROJECT_TYPE = DKM
DEFINES = 
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = ppc
CPU = PPC603
TOOL_FAMILY = gnu
TOOL = gnu
TOOL_PATH = 
CC_ARCH_SPEC = -mcpu=603 -mstrict-align -mno-implicit-fp -mlongcall
LIBPATH = 
LIBS = 

IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -I$(WIND_BASE)/target/h/WPILib -I$(WIND_BASE)/target/h 

IDE_LIBRARIES = $(WIND_BASE)/target/lib/WPILib.a 

IDE_DEFINES = 



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = -g
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
DEBUGFLAGS_C-Compiler = -g
else
DEBUGFLAGS_C++-Compiler = -O2 -fstrength-reduce -fno-builtin
DEBUGFLAGS_Linker = -O2 -fstrength-reduce -fno-builtin
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -O2 -fstrength-reduce -fno-builtin
DEBUGFLAGS_C-Compiler = -O2 -fstrength-reduce -fno-builtin
endif


#Project Targets
PROJECT_TARGETS = LRT12/$(MODE_DIR)/LRT12.out \
	 LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage.o


#Rules

# LRT12
ifeq ($(DEBUG_MODE),1)
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
else
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -O2 -fstrength-reduce -fno-builtin
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -O2 -fstrength-reduce -fno-builtin
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -O2 -fstrength-reduce -fno-builtin
LRT12/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -O2 -fstrength-reduce -fno-builtin
endif
LRT12/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -I$(WIND_BASE)/target/h/WPILib -I$(WIND_BASE)/target/h 
LRT12/$(MODE_DIR)/% : IDE_LIBRARIES = $(WIND_BASE)/target/lib/WPILib.a 
LRT12/$(MODE_DIR)/% : IDE_DEFINES = 
LRT12/$(MODE_DIR)/% : PROJECT_TYPE = DKM
LRT12/$(MODE_DIR)/% : DEFINES = 
LRT12/$(MODE_DIR)/% : EXPAND_DBG = 0
LRT12/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
LRT12/$(MODE_DIR)/% : CPU = PPC603
LRT12/$(MODE_DIR)/% : TOOL_FAMILY = gnu
LRT12/$(MODE_DIR)/% : TOOL = gnu
LRT12/$(MODE_DIR)/% : TOOL_PATH = 
LRT12/$(MODE_DIR)/% : CC_ARCH_SPEC = -mcpu=603 -mstrict-align -mno-implicit-fp -mlongcall
LRT12/$(MODE_DIR)/% : LIBPATH = 
LRT12/$(MODE_DIR)/% : LIBS = 
LRT12/$(MODE_DIR)/% : OBJ_DIR := LRT12/$(MODE_DIR)

OBJECTS_LRT12 = LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage.o

LRT12/$(MODE_DIR)/LRT12.out : $(OBJECTS_LRT12)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@";rm -f "$@";nmppc $(OBJECTS_LRT12) $(IDE_LIBRARIES) | tclsh $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c ppc > $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)ccppc $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -fdollars-in-identifiers -Wall $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES)  -o $(OBJ_DIR)/ctdt.o -c $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)ccppc -r -nostdlib -Wl,-X -T $(WIND_BASE)/target/h/tool/gnu/ldscripts/link.OUT -o "$@" $(OBJ_DIR)/ctdt.o $(OBJECTS_LRT12) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS)  $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

LRT12/$(MODE_DIR)/LRT12_compile_file : $(FILE) ;

_clean :: LRT12/$(MODE_DIR)/LRT12_clean

LRT12/$(MODE_DIR)/LRT12_clean : 
	$(TRACE_FLAG)if [ -d "LRT12" ]; then cd "LRT12"; rm -rf $(MODE_DIR); fi


# LRT12_partialImage
ifeq ($(DEBUG_MODE),1)
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
else
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -O2 -fstrength-reduce -fno-builtin
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -O2 -fstrength-reduce -fno-builtin
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -O2 -fstrength-reduce -fno-builtin
LRT12_partialImage/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -O2 -fstrength-reduce -fno-builtin
endif
LRT12_partialImage/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h/wrn/coreip -I$(WIND_BASE)/target/h/WPILib -I$(WIND_BASE)/target/h 
LRT12_partialImage/$(MODE_DIR)/% : IDE_LIBRARIES = $(WIND_BASE)/target/lib/WPILib.a 
LRT12_partialImage/$(MODE_DIR)/% : IDE_DEFINES = 
LRT12_partialImage/$(MODE_DIR)/% : PROJECT_TYPE = DKM
LRT12_partialImage/$(MODE_DIR)/% : DEFINES = 
LRT12_partialImage/$(MODE_DIR)/% : EXPAND_DBG = 0
LRT12_partialImage/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
LRT12_partialImage/$(MODE_DIR)/% : CPU = PPC603
LRT12_partialImage/$(MODE_DIR)/% : TOOL_FAMILY = gnu
LRT12_partialImage/$(MODE_DIR)/% : TOOL = gnu
LRT12_partialImage/$(MODE_DIR)/% : TOOL_PATH = 
LRT12_partialImage/$(MODE_DIR)/% : CC_ARCH_SPEC = -mcpu=603 -mstrict-align -mno-implicit-fp -mlongcall
LRT12_partialImage/$(MODE_DIR)/% : LIBPATH = 
LRT12_partialImage/$(MODE_DIR)/% : LIBS = 
LRT12_partialImage/$(MODE_DIR)/% : OBJ_DIR := LRT12_partialImage/$(MODE_DIR)

LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/ActionData/ActionData.o : $(PRJ_ROOT_DIR)/ActionData/ActionData.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Brain/Brain.o : $(PRJ_ROOT_DIR)/Brain/Brain.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Component.o : $(PRJ_ROOT_DIR)/Components/Component.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/ConfigLoader.o : $(PRJ_ROOT_DIR)/Components/ConfigLoader.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/CLDriveTrain.o : $(PRJ_ROOT_DIR)/Components/DriveTrain/CLDriveTrain.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/DitheredBrakeTrain.o : $(PRJ_ROOT_DIR)/Components/DriveTrain/DitheredBrakeTrain.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Drivetrain.o : $(PRJ_ROOT_DIR)/Components/Drivetrain.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/LRTServo.o : $(PRJ_ROOT_DIR)/Components/LRTServo.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Launcher.o : $(PRJ_ROOT_DIR)/Components/Launcher.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Shifter.o : $(PRJ_ROOT_DIR)/Components/Shifter.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Build.o : $(PRJ_ROOT_DIR)/Config/Build.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Config.o : $(PRJ_ROOT_DIR)/Config/Config.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Configurable.o : $(PRJ_ROOT_DIR)/Config/Configurable.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/DriverStationButtons.o : $(PRJ_ROOT_DIR)/Config/DriverStationButtons.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/DriverInputs/DebouncedJoystick.o : $(PRJ_ROOT_DIR)/DriverInputs/DebouncedJoystick.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/AsyncCANJaguar.o : $(PRJ_ROOT_DIR)/Jaguar/AsyncCANJaguar.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/CANJaguarBrake.o : $(PRJ_ROOT_DIR)/Jaguar/CANJaguarBrake.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/Esc.o : $(PRJ_ROOT_DIR)/Jaguar/Esc.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobot12.o : $(PRJ_ROOT_DIR)/LRTRobot12.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobotBase.o : $(PRJ_ROOT_DIR)/LRTRobotBase.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Log.o : $(PRJ_ROOT_DIR)/Log/Log.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Loggable.o : $(PRJ_ROOT_DIR)/Log/Loggable.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/DriveEncoders.o : $(PRJ_ROOT_DIR)/Sensors/DriveEncoders.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/LRTEncoder.o : $(PRJ_ROOT_DIR)/Sensors/LRTEncoder.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/AsyncPrinter.o : $(PRJ_ROOT_DIR)/Util/AsyncPrinter.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Console.o : $(PRJ_ROOT_DIR)/Util/Console.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/LCD.o : $(PRJ_ROOT_DIR)/Util/LCD.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PID.o : $(PRJ_ROOT_DIR)/Util/PID.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PrintInConstructor.o : $(PRJ_ROOT_DIR)/Util/PrintInConstructor.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Profiler.o : $(PRJ_ROOT_DIR)/Util/Profiler.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Util.o : $(PRJ_ROOT_DIR)/Util/Util.cpp $(FORCE_FILE_BUILD)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc $(DEBUGFLAGS_C++-Compiler) $(CC_ARCH_SPEC) -ansi -Wall -MD -MP -mlongcall -frtti -Wunused -fimplicit-templates $(ADDED_C++FLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -DCPU=$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL   $(DEFINES) -o "$@" -c "$<"


OBJECTS_LRT12_partialImage = LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/ActionData/ActionData.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Brain/Brain.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Component.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/ConfigLoader.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/CLDriveTrain.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/DitheredBrakeTrain.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Drivetrain.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/LRTServo.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Launcher.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Shifter.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Build.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Config.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Configurable.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/DriverStationButtons.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/DriverInputs/DebouncedJoystick.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/AsyncCANJaguar.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/CANJaguarBrake.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/Esc.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobot12.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobotBase.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Log.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Loggable.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/DriveEncoders.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/LRTEncoder.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/AsyncPrinter.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Console.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/LCD.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PID.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PrintInConstructor.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Profiler.o \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Util.o

LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage.o : $(OBJECTS_LRT12_partialImage)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)ccppc -r -nostdlib -Wl,-X  -o "$@" $(OBJECTS_LRT12_partialImage) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS)  && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage_compile_file : $(FILE) ;

_clean :: LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage_clean

LRT12_partialImage/$(MODE_DIR)/LRT12_partialImage_clean : 
	$(TRACE_FLAG)if [ -d "LRT12_partialImage" ]; then cd "LRT12_partialImage"; rm -rf $(MODE_DIR); fi

force : 

DEP_FILES := LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/ActionData/ActionData.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Brain/Brain.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Component.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/ConfigLoader.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/CLDriveTrain.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/DriveTrain/DitheredBrakeTrain.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Drivetrain.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/LRTServo.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Launcher.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Components/Shifter.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Build.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Config.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/Configurable.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Config/DriverStationButtons.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/DriverInputs/DebouncedJoystick.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/AsyncCANJaguar.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/CANJaguarBrake.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Jaguar/Esc.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobot12.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/LRTRobotBase.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Log.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Log/Loggable.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/DriveEncoders.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Sensors/LRTEncoder.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/AsyncPrinter.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Console.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/LCD.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PID.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/PrintInConstructor.d LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Profiler.d \
	 LRT12_partialImage/$(MODE_DIR)/Objects/LRT12/Util/Util.d
-include $(DEP_FILES)



-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

main_all : external_build $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
