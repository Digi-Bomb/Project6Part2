CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS =

# Boost - adjust BOOST_ROOT if needed
ifdef BOOST_ROOT
	CXXFLAGS += -I$(BOOST_ROOT)
endif

SRCS = Client.cpp FileReader.cpp
TARGET = client

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
