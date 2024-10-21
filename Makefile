CC = g++
CFLAGS = -g -Wall -std=c++0x

#---- compositor
SRCS_COMPO =	exe/compositor.cpp \
				src/Image.cpp \
				src/Util.cpp

PROG_COMPO = bin/compositor

#---- wildFieldCompositor
#~ SRCS_WILD_COMPO =	exe/wildFieldCompositor.cpp \
#~ 				src/Image.cpp \
#~ 				src/Util.cpp \
#~ 				src/Zncc.cpp
SRCS_WILD_COMPO = exe/wildFieldCompositor.cpp
PROG_WILD_COMPO = bin/wildFieldCompositor

#---- manualCompositor
SRCS_MANUAL_COMPO =	exe/manualCompositor.cpp \
				src/Image.cpp \
				src/Util.cpp

PROG_MANUAL_COMPO = bin/manualCompositor

#---- masterDark (compile avec libAstroTools)
SRCS_MASTER_DARK = exe/masterDark.cpp
PROG_MASTER_DARK = bin/masterDark

#---- firstPlanStacker (compile avec libAstroTools)
SRCS_FIRST_PLAN = exe/firstPlanStacker.cpp
PROG_FIRST_PLAN = bin/firstPlanStacker

#---- astroStacker (compile avec libAstroTools)
SRCS_ASTRO_STACKER = exe/astroStacker.cpp
PROG_ASTRO_STACKER = bin/astroStacker


#---- Lib externes sans libAstroTools
#LIBS = 	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_nonfree -lopencv_flann -ltiff -lraw -ljpeg
LIBS = 	-lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_flann -ltiff -lraw -ljpeg

INCLUDE = -I include \
		  -I $(OPENCV_INC) \
		  -I $(TIFF_INC) \
		  -I $(BMP_INC) \
		  -I $(LIBRAW_INC)

LIB_DIR = -L $(OPENCV_LIB) \
		  -L $(TIFF_LIB) \
		  -L $(LIBRAW_LIB)


#---- Lib externes avec libAstroTools
LIBS_NEW = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_flann \
		   -ltiff -lraw -ljpeg -lAstroTools

INCLUDE_NEW = -I $(LIBASTROTOOLS_INC) \
			  -I $(OPENCV_INC) \
			  -I $(TIFF_INC) \
			  -I $(BMP_INC) \
			  -I $(LIBRAW_INC)

LIB_DIR_NEW = -L $(LIBASTROTOOLS_LIB) \
			  -L $(OPENCV_LIB) \
			  -L $(TIFF_LIB) \
			  -L $(LIBRAW_LIB)

#all: $(PROG_COMPO) $(PROG_WILD_COMPO) $(PROG_MANUAL_COMPO) $(PROG_MASTER_DARK) $(PROG_DEBRUITE_DARK) $(PROG_FIRST_PLAN)
all: $(PROG_WILD_COMPO) $(PROG_MASTER_DARK) $(PROG_FIRST_PLAN) $(PROG_ASTRO_STACKER)

dark: $(PROG_MASTER_DARK)

firstPlan: $(PROG_FIRST_PLAN)

wildField: $(PROG_WILD_COMPO)

astroStacker: $(PROG_ASTRO_STACKER)

$(PROG_COMPO):$(SRCS_COMPO)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_COMPO)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROG_COMPO) $(SRCS_COMPO) $(LIB_DIR) $(LIBS) 

$(PROG_WILD_COMPO):$(SRCS_WILD_COMPO)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_WILD_COMPO)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_WILD_COMPO) $(SRCS_WILD_COMPO) $(LIB_DIR_NEW) $(LIBS_NEW)  

$(PROG_MANUAL_COMPO):$(SRCS_MANUAL_COMPO)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_MANUAL_COMPO)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE) -o $(PROG_MANUAL_COMPO) $(SRCS_MANUAL_COMPO) $(LIB_DIR) $(LIBS) 

$(PROG_MASTER_DARK):$(SRCS_MASTER_DARK)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_MASTER_DARK)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_MASTER_DARK) $(SRCS_MASTER_DARK) $(LIB_DIR_NEW) $(LIBS_NEW) 

$(PROG_FIRST_PLAN):$(SRCS_FIRST_PLAN)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_FIRST_PLAN)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_FIRST_PLAN) $(SRCS_FIRST_PLAN) $(LIB_DIR_NEW) $(LIBS_NEW)

$(PROG_ASTRO_STACKER):$(SRCS_ASTRO_STACKER)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_ASTRO_STACKER)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_ASTRO_STACKER) $(SRCS_ASTRO_STACKER) $(LIB_DIR_NEW) $(LIBS_NEW)

clean:
	rm -f $(PROG_COMPO) $(PROG_WILD_COMPO) $(PROG_MANUAL_COMPO) $(PROG_MASTER_DARK) $(PROG_DEBRUITE_DARK) $(PROG_FIRST_PLAN) $(PROG_ASTRO_STACKER)



