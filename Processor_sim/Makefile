processor_simulator: Processor.c tokenizer.l parser.y
	flex tokenizer.l
	bison -d parser.y
	gcc -g -pthread -o processor_simulator parser.tab.c lex.yy.c -ll -ly -lm
