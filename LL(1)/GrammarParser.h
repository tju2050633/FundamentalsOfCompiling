#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

using namespace std;

class GrammarParser
{
public:
    GrammarParser()
    {
        EMPTY = 'e';
        END = '#';
        S = EMPTY;
    }
    void getGrammar();          // 获取文法，拆解出产生式
    void init();                // 初始化
    void printParseTable();     // 打印预测分析表
    void printParseProcedure(); // 打印预测分析过程

private:
    char EMPTY; // 空字符
    char END;   // 结束符号

    char S; // 开始符号

    map<char, set<string> > productions; // 产生式

    set<char> V_T; // 终结符集合
    set<char> V_N; // 非终结符集合

    map<char, set<char> > FIRST;  // FIRST集合
    map<char, set<char> > FOLLOW; // FOLLOW集合

    map<char, map<char, string> > parseTable; // 预测分析表
    stack<char> parseStack;            // 预测分析过程栈
    string inputString;                // 预测分析过程输入串

    void getV_T_V_N();    // 获取终结符集合和非终结符集合
    void getFIRST();      // 获取FIRST集合
    void getFOLLOW();     // 获取FOLLOW集合
    void getParseTable(); // 获取预测分析表

    void getFIRST(string str, set<char> &first, bool &modified); // 获取FIRST集合

    // 辅助函数
    bool inV_T(char c); // 判断字符是否在终结符集合中
    bool inV_N(char c); // 判断字符是否在非终结符集合中
    bool inSet(set<char> set, char c);
    bool inFIRST(char V_N, char c);
    bool inFOLLOW(char V_N, char c);
    void printTableCell(char c, int len);
    void printTableCell(string s, int len);
    void printStep(string production);
};