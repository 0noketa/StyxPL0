#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sink.h>
#include <hmap.h>
#include <symbols.h>
#include <scn_base.h>
#include <scn_abs.h>
#include <scn_io.h>
#include <prs_dfn.h>
#include <prs_abs.h>
#include <ptm.h>
#include <ptm_gen.h>

#include "PL0_int.h"
#include "PL0_lim.h"
#include "PL0_pim.h"

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

    MAP_init();
    initSymbols();
    PL0_initSymbols();

    { Scn_T scn;
        Scn_get_PL0(&scn);

        { PLR_Tab plr = PLR_get_PL0();
            { PT_Term trm;
                { Scn_Stream strm = Stream_string(scn, src);
                    { PT_Cfg cfg = PT_init(plr, strm);       
                        trm = PT_PARSE(cfg, "Prog");

                        PT_quit(cfg);
                    }
                    Stream_close(strm);
                    Stream_free(strm);
                }

                { PL0Prog prog;
                    GLS_Tok tok_id;
                    GLS_Lst(PL0Def) lst;
                    PL0Stms stms;

                    if (!PL0_Start_Prog(trm, &prog))
                        puts("error");
                    else if (PL0Prog_prog(prog, &tok_id, &lst, &stms)) {
                        c_string s_id = GLS_Tok_string(tok_id);
                        printf("program %s", s_id);

                        if (GLS_LENGTH(lst) == 0) {
                            printf(" without defs");
                        } else {
                            GLS_Lst(PL0Def) p;
                            int i;
                            printf(" with defs");

                            GLS_FORALL(p, lst) {
                                PL0Def def = GLS_FIRST(PL0Def, p);
                                GLS_Lst(GLS_tok) lst_toks, q;
                                GLS_Lst(PL0Def) lst_defs;
                                GLS_Tok tok;
                                PL0_Opt3 opt3;
                                PL0Stms stms;
                                PL0Ids ids;

                                if (PL0Def_proc(def, &tok, &opt3, &lst_defs, &stms)) {
                                    printf(" proc(%s)", GLS_Tok_string(tok));
                                } else if (PL0Def_vars(def, &ids) &&
                                        PL0Ids_ids(ids, &tok, &lst_toks)) {

                                    printf(" var(%s)", GLS_Tok_string(tok));

                                    GLS_FORALL(q, lst_toks) {
                                        printf(" var(%s)", GLS_Tok_string(GLS_FIRST(GLS_Tok, q)));
                                    }
                                }
                            }
                        }
                    } else
                        puts("error2");
                }
            
                PT_delT(trm);
            }

            PLR_delTab(plr);            
        }

        Scn_free(scn);
    }

    FreeMem(src);

    PL0_quitSymbols();
    freeSymbols();
    MAP_quit();

    return 0;
}
