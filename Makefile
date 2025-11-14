CXX 		:= g++
CXXFLAGS	:= -g -O2 -MMD -MP
LIBS 		:= -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc

SRCS		:= camera_sample.cpp
OBJS     := $(SRCS:.cpp=.o)
DEPS     := $(OBJS:.o=.d)

EXE 		:= test_camera

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

-include $(DEPS)

clean:
	rm -f $(OBJS) $(DEPS) $(EXE)

.PHONY: all clean
