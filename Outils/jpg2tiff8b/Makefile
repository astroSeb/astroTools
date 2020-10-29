CC = g++
CFLAGS = -g -Wall -std=c++0x

#---- jpg2tiff8b
SRCS = jpg2tiff8b.cpp

PROG = jpg2tiff8b

LIBS = 	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_nonfree -lopencv_flann \
		-ltiff -lraw -ljpeg \
		-lAstroTools

INCLUDE = -I $(LIBASTROTOOLS_INC) \
		  -I $(OPENCV_INC) \
		  -I $(TIFF_INC) \
		  -I $(BMP_INC) \
		  -I $(LIBRAW_INC)

LIB_DIR = -L $(LIBASTROTOOLS_LIB) \
		  -L $(OPENCV_LIB) \
		  -L $(TIFF_LIB) \
		  -L $(LIBRAW_LIB)


all: $(PROG)

$(PROG):$(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROG) $(SRCS) $(LIB_DIR) $(LIBS) 


clean:
	rm -f $(PROG)

