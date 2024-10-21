CC = g++

CFLAGS = -g -Wall -fpic -std=c++0x
#CFLAGS = -g -Wall -fpic -O3 -march=native -mfpmath=sse -msse2

#---- Compilation de la bibliotheque libAstroTools
SRCS = src/Image.cpp \
       src/Util.cpp \
       src/Zncc.cpp \
       src/Display.cpp \
       src/ObjectDetector.cpp

OBJS = Image.o \
       Util.o \
       Zncc.o \
       Display.o \
       ObjectDetector.o
       
       
MYLIB = libAstroTools.so

LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_flann \
       -ltiff -lraw -ljpeg
       
       
INCLUDE = -I ./include \
          -I $(OPENCV_INC) \
          -I $(TIFF_INC) \
          -I $(LIBRAW_INC) \
          -I $(BMP_INC)

LIB_DIR = -L $(OPENCV_LIB) \
          -L $(TIFF_LIB) \
          -L $(LIBRAW_LIB)


#---- Actions
all: $(MYLIB)

$(MYLIB):$(SRCS)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : Compilation de la bibliotheque libAstroTools"
	@echo "-------------------------------------------------------------"
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRCS)
	mkdir -p lib
	$(CC) -shared -Wl,-soname,$(MYLIB) -o $(MYLIB) $(OBJS) $(LIB_DIR) $(LIBS)
	mv *.o *.so lib


clean:
	rm -f lib/*

