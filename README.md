program that allows you to draw simple block diagrams in the terminal and export them to files, there isnt much to add 

before compilation you need to have out/ and src/ directory and move .cpp and .h files to src/
then execute: make program

usage:
```
COMMAND LINE OPTIONS:
	-h --help 	prints this help
IN PROGRAM CONTROL:
	arrows      - move cursor and choosed part (if any)
	space/enter - (un)selects part (first searches for box, then for line then for text in cursor pos)
	              	box and text is selected by top left corner while line is selected wherever its points are
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

	:           - enter command mode
	if text is selected you can type normally to add characters and remove them using backspace
		keep in mind that cursor is not changing its position when you are typing
COMMANDS:
	Q           - exits program, UPPERCASE, not lowercase
	sFILENAME   - saves current project to FILENAME so it can be loaded later
	lFILENAME   - loads current project from FILENAME, discarding what is on the screen
	wFILENAME   - write contents of screen to FILENAME
	enter       - execute current command
	escape      - discard current command
```

diagram in program:

![image](https://user-images.githubusercontent.com/95301979/154369259-f9de3f5c-b6c6-44a1-92d1-399d3ae50b9b.png)


other diagram exported to file: (green is here because my terminal is green)
![image](https://user-images.githubusercontent.com/95301979/154369436-b7996eac-7793-4158-891e-b8c7029f5c2c.png)

video:
https://www.youtube.com/watch?v=VwXoJ5pzZig
