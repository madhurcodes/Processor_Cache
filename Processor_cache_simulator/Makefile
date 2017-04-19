processor_cache_simulator: main.c tokenizer2.l parser2.y Processor.c tokenizer.l parser.y
	flex -P akashM tokenizer2.l
	flex tokenizer.l
	bison -p akashM -d parser2.y
	bison -d parser.y
	gcc -g -pthread -o processor_cache_simulator cache.h main.h parser.tab.c lex.yy.c parser2.tab.c lex.akashM.c main.c cache.c -ll -ly -lm
