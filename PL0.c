#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void print_escaped_name(const c_string s);

void print_PL0Ids(PL0Ids ids) {
	GLS_Tok tok;
	PL0Ids ids2;

	while (1) {
		if (PL0Ids_none(ids)) {
			break;
		} else if (PL0Ids_id(ids, &tok)) {
			print_escaped_name(GLS_Tok_string(tok));
			break;
		} else if (PL0Ids_ids(ids, &tok, &ids2)) {
			print_escaped_name(GLS_Tok_string(tok));
			printf(", ");

			ids = ids2;
			continue;
		} else {
			puts("error @ PL0Ids");
			break;
		}
	}
}

void print_escaped_name(const c_string s) {
	static const char *keys[18] = {
		"async",
		"await",
		"class",
		"const",
		"for",
		"function",
		"do",
		"if",
		"in",
		"let",
		"new",
		"of",
		"return",
		"undefined",
		"var",
		"void",
		"while",
		"yield",
	};

	int i;

	for (i = 0; i < 18; ++i)
		if (!strcmp(s, keys[i])) {
			printf("_");
			break;
		}

	printf("%s", s);
}

void print_PL0Qid(PL0Qid qid) {
	PL0Qid qid2;
	GLS_Tok tok;

	while (1) {
		if (PL0Qid_id(qid, &tok)) {
			print_escaped_name(GLS_Tok_string(tok));
			break;
		} else if (PL0Qid_qid(qid, &tok, &qid2)) {
			print_escaped_name(GLS_Tok_string(tok));
			printf(".");

			qid = qid2;
			continue;
		} else {
			puts("error @ PL0Qid");
			break;
		}
	}
}

int print_PL0Qid_key(PL0Qid qid) {
	int d = 0;
	PL0Qid qid2;
	GLS_Tok tok;

	while (1) {
		if (PL0Qid_id(qid, &tok)) {
			print_escaped_name(GLS_Tok_string(tok));

			break;
		} else if (PL0Qid_qid(qid, &tok, &qid2)) {
			print_escaped_name(GLS_Tok_string(tok));
			printf(":{");

			++d;
			qid = qid2;
			continue;
		}
		else {
			puts("error @ PL0Qid");
			break;
		}
	}

	return d;
}

void print_PL0Defs(PL0Defs defs);
void print_PL0Args(PL0Args args);
void print_PL0Expr(PL0Expr e);

void print_PL0KVPair(PL0KVPair kv) {
	GLS_Tok tok;
	PL0Qid qid;
	PL0Expr e;

	if (PL0KVPair_skey(kv, &tok, &e)) {
		printf("%s: ", GLS_Tok_string(tok));
		print_PL0Expr(e);
	} else if (PL0KVPair_ikey(kv, &qid, &e)) {
		int d = print_PL0Qid_key(qid);

		printf(": ");

		print_PL0Expr(e);

		for (int i = 0; i < d; ++i) {
			printf("}");
		}
	} else {
		puts("error @ PL0KVPair");
	}
}

void print_PL0KVPairs(PL0KVPairs kvs) {
	PL0KVPair kv;
	PL0KVPairs kvs2;

	while (1) {
		if (PL0KVPairs_pair(kvs, &kv)) {
			print_PL0KVPair(kv);
			break;
		} else if (PL0KVPairs_pairs(kvs, &kv, &kvs2)) {
			print_PL0KVPair(kv);
			puts(",");

			kvs = kvs2;
			continue;
		}
		else {
			puts("error @ PL0KVPairs");
			break;
		}
	}
}

void print_PL0Obj(PL0Obj obj) {
	PL0KVPairs kvs;

	if (PL0Obj_obj(obj, &kvs)) {
		puts("{");
		print_PL0KVPairs(kvs);
		puts("}");
	} else {
		puts("error @ PL0Obj");
	}
}

void print_PL0Proc(PL0Proc proc) {
	PL0OptPrms opt;
	PL0Ids ids;
	PL0Defs defs;

	if (PL0Proc_proc(proc, &opt, &defs)) {
		printf("function(");

		if (PL0OptPrms_none(opt)) {
		} else if (PL0OptPrms_prms(opt, &ids)) {
			print_PL0Ids(ids);
		} else {
			puts("error @ PL0OptParams(in PL0Proc)");
			return;
		}

		printf(") {");

		print_PL0Defs(defs);

		printf("}");
	}
}

void print_PL0Exprs(PL0Exprs es) {
	PL0Expr e;
	PL0Exprs es2;

	while (1) {
		if (PL0Exprs_none(es)) {
			break;
		} else if (PL0Exprs_xpr(es, &e)) {
			print_PL0Expr(e);
			break;
		} else if (PL0Exprs_xprs(es, &e, &es2)) {
			print_PL0Expr(e);
			printf(", ");

			es = es2;
			continue;
		} else {
			puts("error @ PL0Exprs");
			break;
		}
	}
}
void print_PL0Expr(PL0Expr e) {
	PL0Qid qid;
    GLS_Tok tok;
    PL0Expr el, er;
    PL0Exprs es;
	PL0Obj obj;
	PL0Args args;
	PL0Proc proc;
    char *o;

    if (PL0Expr_qid(e, &qid)) {
		print_PL0Qid(qid);
    } else if (PL0Expr_int(e, &tok)) {
        printf("%s", GLS_Tok_string(tok));
    } else if (PL0Expr_str(e, &tok)) {
        printf("%s", GLS_Tok_string(tok));
	} else if (PL0Expr_proc(e, &proc)) {
		print_PL0Proc(proc);
    } else if (PL0Expr_call(e, &qid, &args)) {
        print_PL0Qid(qid);
		printf("(");

        print_PL0Args(args);

        printf(")");
    } else if (PL0Expr_load(e, &qid, &el)) {
        print_PL0Qid(qid);
		printf("[");
		
        print_PL0Expr(el);

        printf("]");
    } else if (PL0Expr_lst(e, &es) || PL0Expr_lst2(e, &es)) {
        printf("[");

        print_PL0Exprs(es);

        printf("]");
	} else if (PL0Expr_obj(e, &obj)) {
		print_PL0Obj(obj);
    } else {
        o = PL0Expr_add(e, &el, &er) ? "+"
			: PL0Expr_sub(e, &el, &er) ? "-"
			: PL0Expr_mul(e, &el, &er) ? "*"
			: PL0Expr_div(e, &el, &er) ? "/"
			: PL0Expr_mod(e, &el, &er) ? "%"
			: PL0Expr_equ(e, &el, &er) ? "=="
            : PL0Expr_neq(e, &el, &er) ? "!="
            : NULL;
        
        if (o != NULL) {
            printf("(");

            print_PL0Expr(el);

            printf(") %s (", o);

            print_PL0Expr(er);

            printf(")");
        } else {
            puts("error @ PL0Expr");
        }
    }
}

void print_PL0Args(PL0Args args) {
	PL0Exprs args2;

	if (PL0Args_none(args)) {
	} else if (PL0Args_args(args, &args2)) {
		print_PL0Exprs(args2);
	} else {
		puts("error @ PL0Args");
	}
}


void print_PL0Else(PL0Else els);
void print_PL0Stms(PL0Stms stms);
void print_PL0Stm(PL0Stm stm) {
	PL0Qid qid;
    GLS_Tok tok;
    PL0Expr e, e2;
	PL0Stms stms;
	PL0Args args;
	PL0Exprs es;
	PL0Stm stm2;
	PL0Else els;

	if (PL0Stm_begn(stm, &stms)) {
		puts("{");
		print_PL0Stms(stms);
		puts("}");
	} else if (PL0Stm_if(stm, &e, &stm2, &els)) {
		printf("if (");
		print_PL0Expr(e);
		printf(") ");
		print_PL0Stm(stm2);
		print_PL0Else(els);
	} else if (PL0Stm_asgn(stm, &qid, &e)) {
		print_PL0Qid(qid);
		printf(" = ");
		print_PL0Expr(e);
		puts(";");
	} else if (PL0Stm_stor(stm, &qid, &e, &e2)) {
		print_PL0Qid(qid);
		printf("[");
		print_PL0Expr(e);
		printf("] = ");
		print_PL0Expr(e2);
		puts(";");
	} else if (PL0Stm_rtrn(stm, &e)) {
		printf("return ");
		print_PL0Expr(e);
		puts(";");
	} else if (PL0Stm_call(stm, &qid, &args)) {
		print_PL0Qid(qid);
		printf("(");
		print_PL0Args(args);
		puts(");");
    } else {
        puts("error @ PL0Stm");
    }
}
void print_PL0Stms(PL0Stms stms) {
	PL0Stm stm;
	PL0Stms stms2;

	while (1) {
		if (PL0Stms_none(stms)) {
			break;
		} else if (PL0Stms_stm(stms, &stm)) {
			print_PL0Stm(stm);
			break;
		} else if (PL0Stms_stms(stms, &stm, &stms2)) {
			print_PL0Stm(stm);

			stms = stms2;
			continue;
		} else {
			puts("error @ PL0Stms");
			break;
		}
	}
}

void print_PL0Else(PL0Else els) {
	PL0Expr e;
	PL0Stm stm;
	PL0Else els2;

	while (1) {
		if (PL0Else_none(els)) {
			break;
		} else if (PL0Else_elif(els, &e, &stm, &els2)) {
			printf("else if(");
			print_PL0Expr(e);
			printf(") ");
			print_PL0Stm(stm);

			els = els2;
			continue;
		} else if (PL0Else_else(els, &stm)) {
			printf("else ");
			print_PL0Stm(stm);
			break;
		} else {
			puts("error @ PL0Else");
			break;
		}
	}
}

void print_PL0MainProc(PL0MainProc proc) {
	PL0Stms stms;

	if (PL0MainProc_main(proc, &stms)) {
		puts("begin");
		print_PL0Stms(stms);
		puts("end");
	}
	else {
		puts("error @ PL0Proc");
	}
}


void print_PL0Defs(PL0Defs defs);

void print_PL0Def(PL0Def def) {
    GLS_Tok tok;
	PL0OptPrms prms;
    PL0Defs defs;
    PL0Stms stms;
    PL0Ids ids;

    if (PL0Def_proc(def, &tok, &prms, &defs)) {
        printf("var ");
        print_escaped_name(GLS_Tok_string(tok));
        printf(" = function(");

        PL0Ids ids;

        if (PL0OptPrms_none(prms)) {
        } else if (PL0OptPrms_prms(prms, &ids)) {
            print_PL0Ids(ids);
		} else {
			puts("error @ PL0OptPrms(in PL0Def)");
			return;
		}

        puts(") {");

        print_PL0Defs(defs);

        puts("};");
    } else if (PL0Def_vars(def, &ids)) {
        printf("var ");

        print_PL0Ids(ids);

        puts(";");
    } else {
        puts("error @ PL0Def");
    }
}

void print_PL0Defs(PL0Defs defs) {
	PL0Stm stm;
	PL0Def def;
	PL0Defs defs2;

	while (1) {
		if (PL0Defs_none(defs)) {
			break;
		} else if (PL0Defs_proc(defs, &stm)) {
			print_PL0Stm(stm);
			break;
		} else if (PL0Defs_def(defs, &def)) {
			print_PL0Def(def);
			break;
		} else if (PL0Defs_defs(defs, &def, &defs2)) {
			print_PL0Def(def);

			defs = defs2;
			continue;
		} else {
			puts("error @ PL0Defs");
			break;
		}
	}
}

int main(int argc, char *argv[]) {
    c_string src = NULL;

    src = loadSrc(1 < argc ? argv[1] : (char*)NULL);

    MAP_init();
    initSymbols();
    PL0_initSymbols();

	{ PT_Term trm;
		{ Scn_T scn;
			Scn_get_PL0(&scn);

			{ PLR_Tab plr = PLR_get_PL0();
                { Scn_Stream strm = Stream_string(scn, src);
                    { PT_Cfg cfg = PT_init(plr, strm);       
                        trm = PT_PARSE(cfg, "Prog");

                        PT_quit(cfg);
                    }

					Stream_close(strm);
                    Stream_free(strm);
                }

				PLR_delTab(plr);
			}

			Scn_free(scn);
		}

		FreeMem(src);

        { PL0Prog prog;
            GLS_Tok tok_id;
            PL0Defs defs;

            if (!PL0_Start_Prog(trm, &prog))
                puts("error");
            else if (PL0Prog_prog(prog, &tok_id, &defs)) {
                c_string s_id = GLS_Tok_string(tok_id);

                printf("// program %s;\n", s_id);

                print_PL0Defs(defs);

                puts("// end.");
            } else
                puts("error @ prog");
        }
            
		PT_delT(trm);
	}

    PL0_quitSymbols();
    freeSymbols();
    MAP_quit();

    return 0;
}
