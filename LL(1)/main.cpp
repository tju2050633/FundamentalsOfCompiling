#include "GrammarParser.h"


int main()
{
    GrammarParser parser;
    parser.getGrammar(); // 获取文法
    parser.init();       // 初始化
    parser.printParseTable();     // 打印预测分析表
    parser.printParseProcedure(); // 打印预测分析过程

    return 0;
}

// S->TE        FIRST(S)={(,i}  FOLLOW(S)={),#}
// E->+TE|e     FIRST(E)={+,e}  FOLLOW(E)={),#}
// T->FP        FIRST(T)={(,i}  FOLLOW(T)={+,),#}
// P->*FP|e     FIRST(P)={*,e}  FOLLOW(P)={+,),#}
// F->(S)|i     FIRST(F)={(,i}  FOLLOW(F)={*,+,),#}

// S->TE
// E->+S|e
// T->FH
// H->T|e
// F->PG
// G->*F|e
// P->(S)|a|b|^
// #