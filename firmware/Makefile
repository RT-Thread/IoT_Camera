scons := python ${SCONS}\scons.py

all:
	@$(scons)

clean:
	@$(scons) -c

buildua:
	@$(scons) --target=ua -s

buildlib:
	@$(scons) --buildlib=WICED -s

cleanlib:
	@$(scons) --cleanlib -s
