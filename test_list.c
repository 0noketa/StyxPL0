#include <stdio.h>
#include <stdint.h>

#include <list.h>
#include <standard.h>


void pMember(Abs_T member) {
    printf("%d", (intptr_t)member);
}

void freeMember(Abs_T member) {}

int main(int argc, char *argv[]) {
    List(intprt_t) l = cons(1, cons(8, NULL));

    l = cons(length(l), l);

    /* param1 means indentation level */
    pList(l, 0, &pMember);

    freeList(l, &freeMember);

    return 0;
}