CC = g++
CFLAGS = -g -Wall -std=c++0x

#---- readRaw
SRCS = readRaw.cpp
PROG = readRaw

LIBS = 	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_nonfree -lopencv_flann -lraw

INCLUDE = -I include \
		  -I $(OPENCV_INC) \
		  -I $(LIBRAW_INC)

LIB_DIR = -L $(OPENCV_LIB) \
		  -L $(LIBRAW_LIB)


all: $(PROG)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROG) $(SRCS) $(LIB_DIR) $(LIBS) 


clean:
	rm -f $(PROG)

