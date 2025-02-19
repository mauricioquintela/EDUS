CXX =  mpicxx
#CXXFLAGS = -ansi -Wall -W -g  -std=c++14 -march=skylake-avx512 ${CFLAGS} -ftree-vectorize -fopt-info-all=test_vec_printing
CXXFLAGS = -ansi -g  -std=c++14 -march=znver1 ${CFLAGS} -ftree-vectorize -fopt-info-all=test_vec_printing
OPTFLAGS = -O3 -fopenmp
RPTFLAGS = -unroll-aggressive

INCLUDES = -I./headers


LIBS = -larmadillo -lstdc++ -llapack -lblas -lgfortran ${LDFLAGS} ##-lmkl_rt ${LDFLAGS}

NAMEXE = EDUS_amd.x
#NAMEXE = EDUS_intel.x
#NAMEXE = svd2.x
#NAMEXE  = mpiamd.x
#OBJS  = svd2.o
OBJS  = main_MPI.o


.PHONY: exec clean



exec: $(OBJS)
	        $(CXX) $(CXXFLAGS) $(OPTFLAGS) $(RPTFLAGS) $(LFLAGS) $(OBJS) $(INCLUDES) $(LIBS) -o $(NAMEXE)
			        @echo Linked


%.o: %.cpp
	        $(CXX) $(CXXFLAGS) $(OPTFLAGS) $(RPTFLAGS) $(LFLAGS) $(INCLUDES) -c $<
			        @echo Compiled
					        
clean: 
	        $(RM) $(OBJS) $(POBJS) $(OOBJS) $(LOBJS)
	        $(RM) $(NAMEXE) $(PNAMEXE) $(ONAMEXE) $(LNAMEXE)
			$(RM) *.optrpt

