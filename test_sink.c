#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sink.h>
#include <standard.h>

#define LINE_LIM 78


c_string loadSrc(char *fileName) {
    char buf[LINE_LIM + 2];
    FILE *fIn = NULL;
    Sink tmp = Sink_open();

    if (fileName != NULL) fIn = fopen(fileName, "r");
    if (fIn == NULL) fIn = stdin;

    while (!feof(fIn)) {
        buf[0] = 0;
        fgets(buf, LINE_LIM, fIn);
        Sink_puts(tmp, buf);
    }

    if (fIn != stdin) fclose(fIn);

    return Sink_close(tmp);
}


int main(int argc, char *argv[]) {
    c_string src = NULL;

    src = loadSrc(1 < argc ? argv[1] : (char*)NULL);

    puts(src);

    FreeMem(src);

    return 0;
}
