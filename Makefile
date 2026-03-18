.PHONY: build_debug
build_debug:
	mkdir -p build
	clang -g -D DEBUG src/txt_to_89t.c -o build/txt_to_89t.debug

.PHONY: build_release
build_release:
	mkdir -p build
	clang -O3 src/txt_to_89t.c -o build/txt_to_89t.build_release
