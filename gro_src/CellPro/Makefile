#options
CSTD=c++17
CXXFLAGS=-c -Wall -Wextra $(INCLUDE) -std=$(CSTD) -Igch
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
SRC=src/cg
BUILD=build/$(OS_DIR)/$(MODE)/

OBJECTS=$(BUILD)/GeneticElement.o $(BUILD)/DivisionSplit.o $(BUILD)/Marker.o $(BUILD)/Randomness.o $(BUILD)/Function.o $(BUILD)/GateBase.o $(BUILD)/GateBoolean.o $(BUILD)/GateQuantitative.o $(BUILD)/Ode.o $(BUILD)/Historic.o
OBJECTS+=$(BUILD)/Molecule.o $(BUILD)/Regulation.o $(BUILD)/Operon.o
OBJECTS+=$(BUILD)/Pilus.o $(BUILD)/OriT.o $(BUILD)/PartitionSystem.o $(BUILD)/CopyControl.o $(BUILD)/OriV.o 
OBJECTS+=$(BUILD)/PlasmidBase.o $(BUILD)/PlasmidBoolean.o $(BUILD)/PlasmidQuantitative.o
OBJECTS+=$(BUILD)/Mutation.o $(BUILD)/MutationProcess.o $(BUILD)/Strain.o $(BUILD)/CellType.o $(BUILD)/Metabolite.o $(BUILD)/Flux.o $(BUILD)/CellColour.o
OBJECTS+=$(BUILD)/HorizontalCommon.o

OBJECTS+=$(BUILD)/GeneticCollection.o $(BUILD)/EventScheduler.o $(BUILD)/GeneticReport.o $(BUILD)/GeneticHandler.o $(BUILD)/RandomnessHandler.o $(BUILD)/LogicHandler.o 
OBJECTS+=$(BUILD)/MetabolismHandler.o $(BUILD)/ExpressionHandler.o $(BUILD)/GrowthHandler.o $(BUILD)/HorizontalHandler.o $(BUILD)/PlasmidsHandler.o $(BUILD)/ColourHandler.o
OBJECTS+=$(BUILD)/Genome.o

OUTPUT=$(BUILD)/libcellpro.a

PCH_SRC = include/cg/PRECOMPILED.hpp
PCH_HEADERS = include/cg/PRECOMPILED.hpp
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