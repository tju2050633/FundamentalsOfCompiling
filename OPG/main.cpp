#include "GrammarParser.h"


int main()
{
    GrammarParser parser;
    parser.getGrammar(); // 获取文法
    parser.init();       // 初始化
    parser.printRelationTable();     // 打印预测分析表
    parser.printParseProcedure(); // 打印预测分析过程

    return 0;
}

// E->E+T|T
// T->T*F|F
// F->P^F|P
// P->(E)|i
// #

// i+i*i+i

// S->a|^|(T)
// T->T,S|S
// #