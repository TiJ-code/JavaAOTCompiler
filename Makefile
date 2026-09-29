BUILD_DIR ?= build

.PHONY: all configure build test clean run-example format

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug

build: configure
	cmake --build $(BUILD_DIR) --parallel

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run-example: build
	$(BUILD_DIR)/jaot0 tests/programs/hello.jaot -S -o $(BUILD_DIR)/hello.s

format:
	@find bootstrap runtime -type f \
		\( -name '*.cpp' -o -name '*.h' \) \
		-print0 | \
		xargs -0 -r clang-format -i

clean:
	rm -rf $(BUILD_DIR)

