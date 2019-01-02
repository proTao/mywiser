CC = clang++
CFLAG = -std=c++11 -g

TARGET = main
LIBS = 
OBJS = indexer.o \
	   parser.o \
	   tokenizer.o \
	   main.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $(TARGET) $(OBJS) $(LIBS)

indexer.o: indexer.cpp
	$(CC) $(CFLAG) -I. -o $@ -c $<

parser.o: parser.cpp
	$(CC) $(CFLAG) -I. -o $@ -c $<

tokenizer.o: tokenizer.cpp
	$(CC) $(CFLAG) -I. -o $@ -c $<

main.o: main.cpp
	$(CC) $(CFLAG) -I. -o $@ -c $<


all:	
	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
