DEBUG_FOLDER=build/debug
RELEASE_FOLDER=build/release

.PHONY: default
default: debug

.PHONY: all
all: clean debug release

.PHONY: debug
debug:
	@mkdir -p $(DEBUG_FOLDER)
	@cd $(DEBUG_FOLDER) && cmake ../../ -DCMAKE_BUILD_TYPE=Debug && cmake --build .

.PHONY: test
test:
	@$(DEBUG_FOLDER)/bin/OptionParserUnitTest --gtest_filter=* --gtest_color=yes

.PHONY: run-debugger
run-debugger:
	@$(eval ARGS:="")
	@cd src && nemiver ../$(DEBUG_FOLDER)/bin/OptionParserTestExe $(ARGS) 2&> /dev/null

.PHONY: release
release:
	@mkdir -p $(RELEASE_FOLDER)
	@cd $(RELEASE_FOLDER) && cmake ../../ -DCMAKE_BUILD_TYPE=Release && cmake --build .

.PHONY: install
install:
	@cd $(RELEASE_FOLDER) && make install

.PHONY: uninstall
uninstall:
	@xargs rm -v < $(RELEASE_FOLDER)/install_manifest.txt

.PHONY: clean
clean:
	@rm -rf $(RELEASE_FOLDER)/*
	@rm -rf $(DEBUG_FOLDER)/*
