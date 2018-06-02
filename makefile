stage1exe: parser.c tree.c stack.c lexer.c driver.c 
	gcc parser.c tree.c stack.c lexer.c driver.c AST.c symtable.c -lm -o stage1exe -g 
