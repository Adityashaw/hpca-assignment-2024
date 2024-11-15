# Variables
NVCC = nvcc
GCC = g++
CUDA_ARCH = -gencode arch=compute_70,code=compute_70
CUDART = --cudart shared
CFLAGS = -I ./header
SIM_FLAGS = $(CUDART) $(CUDA_ARCH)
GENERATE = ./data/generate
INPUT_SIZE ?= 128
INPUT_FILE = data/input_$(INPUT_SIZE).in
OUTPUT_SIM = cmm_sim
OUTPUT_SERVER = cmm_server
HEADERS = header/gpu_thread.h
SR_NO ?= 11111
ZIP_NAME = ${SR_NO}.zip

# Targets
all: $(OUTPUT_SIM) $(OUTPUT_SERVER)

sim: $(INPUT_FILE) $(OUTPUT_SIM)

server: $(INPUT_FILE) $(OUTPUT_SERVER)

$(OUTPUT_SERVER): main.cu $(HEADERS)
	$(NVCC) main.cu -o $(OUTPUT_SERVER) $(CFLAGS)

$(OUTPUT_SIM): main.cu $(HEADERS)
	$(NVCC) main.cu -o $(OUTPUT_SIM) $(CFLAGS) $(SIM_FLAGS)

data/generate: data/generate.cpp
	$(GCC) ./data/generate.cpp -o ./data/generate

$(INPUT_FILE): data/generate
	@if [ ! -f $(INPUT_FILE) ]; then \
	    echo "Generating input file $(INPUT_FILE)"; \
	    $(GENERATE) $(INPUT_SIZE); \
	fi

run_sim: $(INPUT_FILE) $(OUTPUT_SIM)
	./$(OUTPUT_SIM) $(INPUT_FILE)

run_server: $(INPUT_FILE) $(OUTPUT_SERVER)
	./$(OUTPUT_SERVER) $(INPUT_FILE)

clean:
	rm -f $(OUTPUT_SIM) $(OUTPUT_SERVER)

zip:
	zip -r $(ZIP_NAME) header report