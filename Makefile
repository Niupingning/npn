TOPDIR := ../../../bsp/application/
include $(TOPDIR)build/core/config.mak

SDK_DIR:=../../../bsp/application/sdk

PATH_M:=../../../bsp/linux/sdk/prebuilt/common_rootfs/lib

PUBLIC_INCLUDE_PATH=../public/include 
PUBLIC_LIB_PATH=../public/libs

CXXFLAGS += -I$(SDK_DIR)/include -DYOCTO_LINUX
CXXFLAGS += -Wno-non-virtual-dtor --std=c++11 
CXXFLAGS += -I$(APPLICATION_DIR)/include -I$(PUBLIC_INCLUDE_PATH)

LDFLAGS += -L$(PUBLIC_LIB_PATH) -L$(PROJECT_DIR)/system/lib  -L$(SDK_DIR)/lib -L$(APPLICATION_DIR)/lib -L$(PATH_M)
LDFLAGS += -lbinder -lutils -llog -lpthread -lsppfc -lresourcemanager_client -laudio -lAudioCtrl -lgpioctrl -Wl,--rpath=.

# LDFLAGS += -lcutils -lcorkscrew -ldl
# LDFLAGS += -luevent_observer
LDFLAGS += -lPublicFGE -lAppBase -lgui -Wl,-rpath-link=$(PUBLIC_LIB_PATH) 
LDFLAGS += -Wl,-rpath-link=$(SDK_DIR)/lib -Wl,-rpath=$(PATH_M) -Wl,-rpath=.

LDFLAGS += -lplayback  -lxrpc -lxosal -lspvideoin -ldispap

CXXFLAGS += -I./src/include
CXXFLAGS += -I./src/audio_boost
BINDIR:=$(APPLICATION_OUT_DIR)/system/apps
GOAL = $(BINDIR)/IVIHomeApp
#GOAL = ./bins/IVIHomeApp

CPP_SOURCE = ${wildcard src/*.cpp} ${wildcard src/control/*.cpp} ${wildcard src/mode/*.cpp} ${wildcard src/view/*.cpp} ${wildcard src/viewcomp/*.cpp} ${wildcard src/audio_boost/*.cpp}

OBJS = ${patsubst %.cpp,%.o,$(CPP_SOURCE)}

OBJDIR = ./build
#OBJS=$(patsubst %.cpp,$(OBJDIR)/.o,$(CPP_SOURCE))


LIBS =

.PHONY: all
all: $(GOAL)
	$(MSG_SPLIT_LINE)
	
$(GOAL): $(OBJS)
	$(MSG_SPLIT_LINE)
	$(MSG_LINKING)
	$(TARGET_CXX) $(CXXFLAGS)$(LDFLAGS) $(OBJS) -o $(GOAL) $(LIBS)
	$(MSG_GOAL_OK)

$(OBJS): %.o : %.cpp
	$(MSG_SPLIT_LINE)
	$(MSG_LINKING)
	$(TARGET_CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	$(MSG_SPLIT_LINE)
	$(RM) -rf $(GOAL)
	$(RM) -rf src/*.o 
	$(RM) -rf src/control/*.o 
	$(RM) -rf src/mode/*.o 
	$(RM) -rf src/view/*.o
	$(RM) -rf src/viewcomp/*.o
	$(RM) -rf src/audio_boost/*.o

.PHONY: release
release:

-include $(OBJS:.o=.d)

