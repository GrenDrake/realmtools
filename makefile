SDL_CXX=
SDL_LIBS=`sdl2-config --libs`

CXXFLAGS=-std=c++11 -g -Wall $(SDL_CXX)
BIGBANG=bigbang.exe
BIGBANG_OBJS=src/bigbang.o src/bb_generator.o src/world.o src/utility.o src/data.o
REALMS=realms.exe
REALMS_OBJS=src/realms.o src/realms_map.o src/realms_sql.o src/realms_gviz.o src/realms_list.o src/realms_stats.o src/world.o src/utility.o
VIEWER=viewer.exe
VIEWER_OBJS=src_viewer/viewer.o src_viewer/viewer_ui.o src_viewer/viewer_realms.o src_viewer/viewer_species.o src/world.o  src/utility.o

all: $(BIGBANG) $(REALMS) $(VIEWER)

$(BIGBANG): $(BIGBANG_OBJS)
	$(CXX) $(BIGBANG_OBJS) -o $(BIGBANG)
$(REALMS): $(REALMS_OBJS)
	$(CXX) $(REALMS_OBJS) -o $(REALMS)

$(VIEWER_OBJS): CXXFLAGS += `sdl2-config --cflags`
$(VIEWER): $(VIEWER_OBJS)
	$(CXX) $(VIEWER_OBJS) $(SDL_LIBS) -o $(VIEWER)

clean:
	$(RM) src/*.o $(BIGBANG) $(REALMS)

.PHONY: all
