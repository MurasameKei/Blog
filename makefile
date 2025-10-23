# Define the first target 
all: frontend backend

# Frontend compilation
TS_COMPILE = npx esbuild
TSFLAGS = --bundle --platform=node --target=ES2020 
#TSFLAGS = --bundle --platform=node --target=ES2020 --minify

TSDIR = ./frontend/scripts
TS = $(TSDIR)/homepage.ts $(TSDIR)/comments.ts

JSDIR = ./build/frontend/scripts
JS = $(TS:$(TSDIR)/%.ts=$(JSDIR)/%.js)

$(JSDIR)/%.js: $(TSDIR)/%.ts
	$(TS_COMPILE) $(TSFLAGS) $< --outfile=$@

# Backend compilation
CPPCOMPILE = clang++
COMPILEFLAGS = -O0
LINKLIBS = -loqs -lssl -lcrypto
LINKFLAGS = $(LINKLIBS)

CPPDIR = ./backend
SOURCES = $(CPPDIR)/backend.cpp $(CPPDIR)/auth.cpp $(CPPDIR)/b64.cpp $(CPPDIR)/crypto.cpp $(CPPDIR)/session.cpp

OBJECTDIR = ./build/part
OBJECTS = $(SOURCES:$(CPPDIR)/%.cpp=$(OBJECTDIR)/%.o)

TARGET = ./build/backend/BlogCore

$(OBJECTDIR)%.o: $(CPPDIR)%.cpp
	$(CPPCOMPILE) $(COMPILEFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPCOMPILE) $(LINKFLAGS) $(OBJECTS) -o $(TARGET)

# Define CLI rules
frontend: $(JS)

backend: $(TARGET)

clean: clean-frontend clean-backend

clean-frontend:
	rm -rf $(JSDIR)/*.js

clean-backend:
	rm -rf $(OBJECTDIR)/*.o
	rm -rf $(TARGET)
