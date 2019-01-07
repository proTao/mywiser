CC = clang++
CFLAG = -std=c++11 -g -I. 
MYSQLFLAG = `mysql_config --cflags`

TARGET = main
LIBS = `mysql_config --libs`


OBJS = database.o \
	   tokenizer.o \
	   parser.o \
	   indexer.o \
	   utils.o \
	   main.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $(TARGET) $(OBJS) $(LIBS)

database.o: database.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<

utils.o: utils.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<

tokenizer.o: tokenizer.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<

parser.o: parser.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<

indexer.o: indexer.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<

main.o: main.cpp
	$(CC) $(CFLAG) $(MYSQLFLAG) -o $@ -c $<


all:	
	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
