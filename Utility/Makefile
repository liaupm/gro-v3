#options
CSTD=c++17
CXXFLAGS=-c -Wall -Wextra -I$(INCLUDE) -std=$(CSTD) -Igch
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
	CXXFLAGS+= -g -Og
endif

#paths
INCLUDE=include
SRC=src/ut
BUILD=build/$(OS_DIR)/$(MODE)/
OBJECTS=$(BUILD)/StaticOperators.o $(BUILD)/Stat.o $(BUILD)/StatPack.o 
OBJECTS+=$(BUILD)/ExtendedVector.o $(BUILD)/SortedVector.o $(BUILD)/PairVector.o $(BUILD)/ThresholdVector.o $(BUILD)/ElementCounter.o $(BUILD)/Bitset.o $(BUILD)/MultiBitset.o
OBJECTS+=$(BUILD)/Metadata.o $(BUILD)/TypeCollection.o $(BUILD)/PolyVector.o $(BUILD)/NameMap.o $(BUILD)/SubtypeRegister.o $(BUILD)/Factory.o $(BUILD)/CollectionManager.o 
OBJECTS+=$(BUILD)/DistributionInterface.o $(BUILD)/DistributionCombi.o $(BUILD)/SeedGenerator.o $(BUILD)/Param.o
OBJECTS+=$(BUILD)/Identifiable.o $(BUILD)/ElementIndex.o $(BUILD)/LinkedElement.o $(BUILD)/Time.o $(BUILD)/Event.o $(BUILD)/Routine.o $(BUILD)/Report.o $(BUILD)/Handler.o
OBJECTS+=$(BUILD)/AgentState.o $(BUILD)/Agent.o $(BUILD)/MathFunction.o $(BUILD)/GateBoolean.o $(BUILD)/GateQuantitative.o 
OUTPUT=$(BUILD)/libutility.a

PCH_SRC = $(INCLUDE)/ut/PRECOMPILED.hpp
PCH_HEADERS = $(INCLUDE)/ut/PRECOMPILED.hpp
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