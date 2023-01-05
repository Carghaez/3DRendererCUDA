SRCS := main.cpp Matrix.cpp Point.cpp Renderer.cu
OBJS := $(patsubst %.cu,%.o,$(patsubst %.cpp,%.o,$(SRCS)))

%.o: %.cu
	nvcc -c -o $@ $< $(CFLAGS)

3dRenderer: $(OBJS)
	gcc -o $@ $(OBJS) -lGLU -lGL -lglut -lcudart -lm -lstdc++

.PHONY: clean
clean:
	$(RM) $(OBJS) 3dRenderer

.PHONY: all
all: 3dRenderer
