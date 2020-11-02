
CXXFLAGS=-std=c++11 -g -Wall
BIGBANG=bigbang.exe
BIGBANG_OBJS=src/bigbang.o src/bb_namegen.o src/world.o src/utility.o
REALMS=realms.exe
REALMS_OBJS=src/realms.o src/realms_map.o src/realms_list.o src/realms_stats.o src/world.o src/utility.o

all: $(BIGBANG) $(REALMS)

$(BIGBANG): $(BIGBANG_OBJS)
	$(CXX) $(BIGBANG_OBJS) -o $(BIGBANG)
$(REALMS): $(REALMS_OBJS)
	$(CXX) $(REALMS_OBJS) -o $(REALMS)

clean:
	$(RM) src/*.o $(BIGBANG) $(REALMS)

.PHONY: all
