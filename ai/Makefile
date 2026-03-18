include ../build/Makefile.param

commLib_obj = rv1126
LIBPATH := ../libs/$(commLib_obj)

INC := ../inc
ROCKCHIP_INC := -I$(commLib_inc)/rockchip

RK_INC := -I${SYSROOT}//usr/include/rkaiq/uAPI/ \
			-I${SYSROOT}/usr/include/rkaiq/xcore/ \
			-I${SYSROOT}//usr/include/rkaiq/common \
			-I${SYSROOT}//usr/include/rkaiq/algos/adebayer \
			-I${SYSROOT}/usr/include/rkaiq/algos/ahdr \
			-I${SYSROOT}/usr/include/rkaiq/algos \
			-I${SYSROOT}/usr/include/rkaiq/iq_parser \
			-I${SYSROOT}/usr/include/easymedia \
			-I${SYSROOT}/usr/include/freetype2 \

OBJPATH = ./obj_rv1126

SRCS-C := $(wildcard *.c)
OBJS-COMM := $(patsubst %.c,$(OBJPATH)/%.o,$(SRCS-C))

SRCS-CXX := $(wildcard *.cpp)
OBJS-COMM-CXX := $(patsubst %.cpp,$(OBJPATH)/%.o,$(SRCS-CXX))
OBJS := $(OBJS-COMM) $(OBJS-COMM-CXX)

TARGETLIB :=  $(LIBPATH)/libAI.a
all: $(TARGETLIB)	

$(OBJS-COMM):$(OBJPATH)/%.o:%.c
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	$(CC) $(CFLAGS) -o $@ -c $< -I$(INC)  $(RK_INC) -I$(commLib_inc) -I$(commLib_extra_inc) $(MPP_INC) $(CODEC_RKMEDIA_INC)

$(OBJS-COMM-CXX):$(OBJPATH)/%.o:%.cpp
	@[ -e $(OBJPATH) ] || mkdir $(OBJPATH)
	$(CXX) $(CXXFLAGS) -o $@ -c $< -I$(INC)  $(RK_INC) -I$(commLib_inc) -I$(commLib_extra_inc) $(MPP_INC) $(CODEC_RKMEDIA_INC)

$(TARGETLIB):$(OBJS)
	@[ -e $(LIBPATH) ] || mkdir $(LIBPATH)
	$(AR) rcv $@ $^
	
clean:
	-rm -rf  $(OBJS) $(TARGETLIB)
	
