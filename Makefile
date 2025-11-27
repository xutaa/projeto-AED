# make              # to compile files and create the executables
# make clean        # to cleanup object files and executables
# make cleanobj     # to cleanup object files only

CFLAGS = -Wall -Wextra -O2 -g

PROGS = imageRGBTest perf_test

# Default rule: make all programs
all: $(PROGS)

imageRGBTest: imageRGBTest.o imageRGB.o instrumentation.o error.o \
			  PixelCoords.o PixelCoordsQueue.o PixelCoordsStack.o

perf_test: perf_test.o imageRGB.o instrumentation.o error.o \
			 PixelCoords.o PixelCoordsQueue.o PixelCoordsStack.o

perf_test.o: imageRGB.h instrumentation.h error.h \
			 PixelCoords.h PixelCoordsQueue.h PixelCoordsStack.h

imageRGBTest.o: imageRGB.h instrumentation.h error.h \
                PixelCoords.h PixelCoordsQueue.h PixelCoordsStack.h

# Rule to make any .o file dependent upon corresponding .h file
%.o: %.h

# Make uses builtin rule to create .o from .c files.

cleanobj:
	rm -f *.o

cleanimg:
	rm -f *.ppm *.pbm

clean: cleanobj cleanimg
	rm -f $(PROGS)

