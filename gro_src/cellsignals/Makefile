#options
CSTD=c++17
CXXFLAGS=-c -Wall -Wextra $(INCLUDE) -std=$(CSTD)
CC = $(COMP) $(CXXFLAGS)

#select OS
ifeq ($(OS),Windows_NT)
		OS_DIR=win
        RM=del
		COMP = g++
		
else
        RM=rm -f
		UNAME_S := $(shell uname -s)
		
		ifeq ($(UNAME_S),Linux)
            OS_DIR=linux
			COMP = g++
        endif
        ifeq ($(UNAME_S),Darwin)
            OS_DIR=mac
			COMP = clang++
			CXXFLAGS+= -stdlib=libc++
        endif
endif

#select mode
ifeq ($(MODE),release)
	CXXFLAGS+= -O2
else
	CXXFLAGS+= -g
endif

#paths
INCLUDE=-I include -I ../Utility/include
SRC=src/mg
BUILD=build/$(OS_DIR)/$(MODE)/
OBJECTS=$(BUILD)/MediumElement.o $(BUILD)/DiffusionMatrix.o $(BUILD)/Signal.o $(BUILD)/Grid.o $(BUILD)/MediumCollection.o $(BUILD)/DynamicBorder.o $(BUILD)/SignalReport.o $(BUILD)/GridUnit.o $(BUILD)/GridReport.o $(BUILD)/Diffuser.o $(BUILD)/Medium.o 
OUTPUT=$(BUILD)/libsignals.a

PCH_SRC = include/mg/PRECOMPILED.hpp
PCH_HEADERS = include/mg/PRECOMPILED.hpp
PCH_OUT = $(BUILD)/PRECOMPILED.hpp.gch

#build
AR = ar -rs
all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(AR) $(OUTPUT) $(OBJECTS)

#uncomment to compile the PRECOMPILED file
$(PCH_OUT): $(PCH_SRC) $(PCH_HEADERS)
	$(CC) -o $@ $<
	
$(BUILD)/%.o: $(SRC)/%.cpp $(PCH_OUT)
	$(CC) -include $(PCH_SRC) -o $@ $<

.PHONY: clean
clean:
	RM -f $(BUILD)/*.o $(OUTPUT)