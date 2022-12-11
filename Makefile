download: download.c
	gcc -o target/download src/download.c src/dyn_string.c -lm