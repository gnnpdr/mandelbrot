CC = g++

CFLAGS = -mavx2 -O3
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
OUT_O_DIR = build
COMMONINC = -I./include
SRC = ./source

override CFLAGS += $(COMMONINC)
CSRC = main.cpp source/draw.cpp source/control.cpp
COBJ = $(addprefix $(OUT_O_DIR)/,$(CSRC:.cpp=.o))
DEPS = $(COBJ:.o=.d)

TARGET = mandelbrot

.PHONY: all
all: $(OUT_O_DIR)/$(TARGET)

$(OUT_O_DIR)/$(TARGET):$(COBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(COBJ):$(OUT_O_DIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEPS):$(OUT_O_DIR)/%.d : %.cpp
	@mkdir -p $(@D)
	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: clean
clean:
	rm -rf $(COBJ) $(DEPS) $(OUT_O_DIR)/$(TARGET) $(OUT_O_DIR)/$(SRC) $(OUT_O_DIR)/*.log
	
NODEPS = clean
	ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
	include $(DEPS)
endif