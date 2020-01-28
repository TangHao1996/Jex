SOURCE := $(wildcard *.cpp)
OBJS := $(patsubst %cpp, %o, $(SOURCE))

TARGET := run
FLAGS := -Wall -g -std=c++11
LIBS := -lpthread

.PHONY: all clean #创建伪文件

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ $(FLAFS) -o $@ $^ $(LIBS)

%o: %cpp
	g++ $(FLAGS) -c $@ $< $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
