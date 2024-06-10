program for drawing simple block diagrams in the terminal and exporting them to files

compilation:
```
git clone https://github.com/sarvl/terminalBlockDiagramDrawer
cd terminalBlockDiagramDrawer
mkdir out
make program
```

usage:
either mouse or keyboard can be used for controling place of cursor
to move an object using mouse, press it, drag then release

```
COMMAND LINE OPTIONS:
	-h --help   	prints this help
	-c --config 	prints help about config file 
IN PROGRAM CONTROL:
	arrows      - move cursor and choosed part (if any)
	space/enter - (un)selects part (first searches for box, then for line then for text in cursor pos)
	              	box and text are selected by top left corner while line is selected wherever its points are
	              	`space` doesnt work for unselecting text, enter has to be used  
	TAB         - removes selected part
	b           - create box
	l           - create line
	t           - create text
	p           - add point to a line and switch to it
	r           - remove point from a line
	a           - change line to an arrow and vice versa
	[ ]         - change box size on y axis
	{ }         - change box size on x axis
	
	c '         - copy selected object
	v \         - paste copied object

	`           - add currently choosed object to list to move

	:           - enter command mode
	if text is selected, characters are added/removed via regular typing/using backspace
		keep in mind that cursor is not changing its position while chars are added/removed

COMMANDS:
	Q           - exits program, UPPERCASE, not lowercase
	CLEAR       - deletes screen contents

	sFILENAME   - saves current project to FILENAME so it can be loaded later
	lFILENAME   - loads current project from FILENAME, discarding what is on the screen
	wFILENAME   - write contents of screen to FILENAME

	MMV         - move list (see "`") to cursor position 
	MCL         - clear list of to move
	MAL         - move all to cursor position

	enter       - execute current command
	escape      - discard current command
```

diagram in program:

![image](https://user-images.githubusercontent.com/95301979/154369259-f9de3f5c-b6c6-44a1-92d1-399d3ae50b9b.png)


other diagram exported to file: 
![image](https://user-images.githubusercontent.com/95301979/154369436-b7996eac-7793-4158-891e-b8c7029f5c2c.png)
