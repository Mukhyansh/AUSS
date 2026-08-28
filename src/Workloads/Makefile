CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm

# Binaries to build
BINARIES = compressor matrixMultiplication byteReader Activate_Workloads

# Default target
all: $(BINARIES)
	@echo ""
	@echo "✓ All workloads compiled successfully!"
	@echo ""
	@ls -lh $(BINARIES)

# ============================================================================
# Build individual workloads
# ============================================================================

compressor: comp.c Compressor/compress.c Compressor/decompress.c Compressor/trie.c
	$(CC) $(CFLAGS) -o compressor comp.c Compressor/compress.c Compressor/decompress.c Compressor/trie.c $(LDFLAGS)
	@echo "✓ Built: compressor (mixed workload - CPU + I/O)"

matrixMultiplication: matrixMultiplication.c
	$(CC) $(CFLAGS) -o matrixMultiplication matrixMultiplication.c $(LDFLAGS)
	@echo "✓ Built: matrixMultiplication (CPU workload)"

byteReader: byteReader.c
	$(CC) $(CFLAGS) -o byteReader byteReader.c $(LDFLAGS)
	@echo "✓ Built: byteReader (I/O workload)"

Activate_Workloads: Activate_Workloads.c
	$(CC) $(CFLAGS) -o Activate_Workloads Activate_Workloads.c $(LDFLAGS)
	@echo "✓ Built: Activate_Workloads"

# ============================================================================
# Utility targets
# ============================================================================

# Clean compiled binaries and temporary files
clean:
	rm -f $(BINARIES)
	rm -f Compressor/compressed.dat
	rm -f Compressor/output_file.dat
	rm -f /tmp/auss_*.log
	rm -f /tmp/auss_pid_queue.txt
	rm -f /tmp/auss_telemetry.csv
	@echo "✓ Cleaned all binaries and temporary files"

# Help
help:
	@echo "AUSS Workloads Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make all                  - Build all workloads (default)"
	@echo "  make compressor           - Build compressor only"
	@echo "  make matrixMultiplication - Build matrixMultiplication only"
	@echo "  make byteReader           - Build byteReader only"
	@echo "  make Activate_Workloads   - Build Activate_Workloads only"
	@echo "  make clean                - Remove all binaries and temp files"
	@echo "  make help                 - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  ./compressor 20           - Run compressor for 20 seconds"
	@echo "  ./matrixMultiplication 20 - Run matrix multiplication for 20 seconds"
	@echo "  ./byteReader 20           - Run byte reader for 20 seconds"

# Verify workloads
verify: all
	@echo ""
	@echo "Verifying workloads..."
	@echo ""
	@for binary in $(BINARIES); do \
		if [ -x ./$$binary ]; then \
			echo "✓ $$binary is executable"; \
		else \
			echo "✗ $$binary is NOT executable"; \
		fi; \
	done
	@echo ""

# Test individual workloads
test-compressor: compressor
	@echo "Testing compressor for 2 seconds..."
	./compressor 2

test-matrix: matrixMultiplication
	@echo "Testing matrixMultiplication for 2 seconds..."
	./matrixMultiplication 2

test-reader: byteReader
	@echo "Testing byteReader for 20 seconds..."
	./byteReader 20

test-all: Activate_Workloads
	@echo "Testing Activate_Workloads..."
	./Activate_Workloads > output_file.txt

# test-all: test-compressor test-matrix test-reader
# 	@echo "✓ All workloads tested successfully!"

.PHONY: all clean help verify test-compressor test-matrix test-reader test-all