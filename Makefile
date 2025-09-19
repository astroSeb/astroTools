CC = g++
CFLAGS = -g -Wall -std=c++0x


#---- wildFieldCompositor
SRCS_WILD_COMPO = exe/wildFieldCompositor.cpp
PROG_WILD_COMPO = bin/wildFieldCompositor

#---- masterDark (compile avec libAstroTools)
SRCS_MASTER_DARK = exe/masterDark.cpp
PROG_MASTER_DARK = bin/masterDark

#---- firstPlanStacker (compile avec libAstroTools)
SRCS_FIRST_PLAN = exe/firstPlanStacker.cpp
PROG_FIRST_PLAN = bin/firstPlanStacker

#---- planetarySelector (compile avec libAstroTools)
SRCS_PLAN_SELECT = exe/planetarySelector.cpp
PROG_PLAN_SELECT = bin/planetarySelector


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


all: $(PROG_WILD_COMPO) $(PROG_MASTER_DARK) $(PROG_FIRST_PLAN) $(PROG_PLAN_SELECT)

dark: $(PROG_MASTER_DARK)

firstPlan: $(PROG_FIRST_PLAN)

wildField: $(PROG_WILD_COMPO)

planetarySelect: $(PROG_PLAN_SELECT)

 

$(PROG_WILD_COMPO):$(SRCS_WILD_COMPO)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_WILD_COMPO)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_WILD_COMPO) $(SRCS_WILD_COMPO) $(LIB_DIR_NEW) $(LIBS_NEW)  


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

$(PROG_PLAN_SELECT):$(SRCS_ASTRO_STACKER)
	@echo "-------------------------------------------------------------"
	@echo "INFO   : compilation de $(PROG_PLAN_SELECT)"
	@echo "-------------------------------------------------------------"
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDE_NEW) -o $(PROG_PLAN_SELECT) $(SRCS_PLAN_SELECT) $(LIB_DIR_NEW) $(LIBS_NEW)

clean:
	rm -f $(PROG_WILD_COMPO) $(PROG_MASTER_DARK) $(PROG_FIRST_PLAN) $(PROG_PLAN_SELECT)



