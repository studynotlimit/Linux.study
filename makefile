all:write read

write:a.c
	gcc -g a.c -o write
read:b.c
	gcc -g b.c -o read
clean:
	rm write read text.txt