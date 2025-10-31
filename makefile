default: build

CPPCOMPILE = clang++
COMPILEFLAGS = -O0
LINKLIBS = -loqs -lssl -lcrypto -lpqxx -lpq
LINKFLAGS = $(LINKLIBS)

CPPDIR = ./backend
SOURCES = $(CPPDIR)/backend.cpp $(CPPDIR)/auth.cpp $(CPPDIR)/b64.cpp $(CPPDIR)/crypto.cpp $(CPPDIR)/session.cpp $(CPPDIR)/comments.cpp

OBJECTDIR = ./build/part
OBJECTS = $(SOURCES:$(CPPDIR)/%.cpp=$(OBJECTDIR)/%.o)

TARGET = ./build/backend/BlogCore

$(OBJECTDIR)%.o: $(CPPDIR)%.cpp
	$(CPPCOMPILE) $(COMPILEFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPCOMPILE) $(LINKFLAGS) $(OBJECTS) -o $(TARGET)

build: $(TARGET)

clean:
	rm -rf $(OBJECTDIR)/*.o
	rm -rf $(TARGET)
