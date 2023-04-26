
all:
	gcc program0.c -lpthread -o program0
	gcc program1.c -lpthread -o program1
	gcc program2.c -lpthread -o program2
	gcc program3.c -lpthread -o program3

clean:
	rm program0
	rm program1
	rm program2
	rm program3

clean_data:
	rm shared1.dat
	rm shared2.dat
	rm output.data