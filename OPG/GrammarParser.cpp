#include "GrammarParser.h"

void GrammarParser::getGrammar()
{
    // 提示语
    cout << "====================请输入文法的产生式====================" << endl;
    cout << "要求如下：" << endl;
    cout << "【每个终结符、非终结符用单个字符表示】" << endl;
    cout << "【第一行的左部为文法开始符号】" << endl;
    cout << "【中间不要有空格】" << endl;
    cout << "【产生式用换行符分隔】" << endl;
    cout << "【箭头为-和>的组合】" << endl;
    cout << "【以#表示结束】" << endl;
    cout << "【空字符用e表示】" << endl;
    cout << "【非终结符不要含有分析程序临时符号N】" << endl;
    cout << "形如：" << endl;
    cout << "E->E+T|T|i*i" << endl;
    cout << "T->i" << endl;
    cout << "#" << endl;
    cout << "请输入：" << endl;

    // 循环处理每一行输入
    string line;
    while (cin >> line)
    {
        // 获取该产生式的非终结符
        char V_N = line[0];
        if (V_N == END)
            break;

        // 第一行的左部为开始符号
        if (S == EMPTY)
            S = V_N;

        // 获取该非终结符对应产生式的右部集合
        string temp = "";
        for (int i = 3; i < line.size(); i++)
        {
            if (line[i] == '|')
            {
                productions[V_N].insert(temp);
                temp = "";
            }
            else
            {
                temp += line[i];
            }
        }
        productions[V_N].insert(temp);
    }
}

// 初始化：根据输入的文法计算V_T V_N FIRST集 FOLLOW集 分析表

void GrammarParser::init()
{
    getV_T_V_N();              // 获取终结符集合和非终结符集合
    getFIRST_VT_AND_LAST_VT(); // 计算FIRST集和LAST集
    getRelationTable();        // 获取预测分析表

    // debug
    if (0)
    {
        cout << "production:" << endl;
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
        {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                cout << it->first << "->" << *it2 << endl;
            }
        }
        cout << endl;

        cout << "V_T:" << endl;
        for (set<char>::iterator it = V_T.begin(); it != V_T.end(); it++)
        {
            cout << *it << " ";
        }
        cout << endl
             << endl;

        cout << "V_N:" << endl;
        for (set<char>::iterator it = V_N.begin(); it != V_N.end(); it++)
        {
            cout << *it << " ";
        }
        cout << endl
             << endl;

        cout << "FIRST_VT:" << endl;
        for (map<char, set<char> >::iterator it = FIRST_VT.begin(); it != FIRST_VT.end(); it++)
        {
            cout << it->first << ": ";
            for (set<char>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                cout << *it2 << " ";
            }
            cout << endl;
        }
        cout << endl;

        cout << "LAST_VT:" << endl;
        for (map<char, set<char> >::iterator it = LAST_VT.begin(); it != LAST_VT.end(); it++)
        {
            cout << it->first << ": ";
            for (set<char>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                cout << *it2 << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

void GrammarParser::getV_T_V_N()
{
    // 产生式集合的所有左部符号为非终结符
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        V_N.insert(it->first);
    }

    // 遍历每一个产生式的右部
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            // 产生式右部字符串
            string right_side = *it2;

            // 遍历字符串每一个字符(空字除外)
            for (int j = 0; j < right_side.size() && right_side[j] != EMPTY; j++)
            {
                // 如果非终结符集合中不包含该字符，则该字符为终结符，加入终结符集合
                if (!inV_N(right_side[j]))
                {
                    V_T.insert(right_side[j]);
                }
            }
        }
    }
}

void GrammarParser::getFIRST_VT_AND_LAST_VT()
{
    bool modified = true;
    while (modified)
    {
        modified = false;

        // 遍历每一个产生式
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
        {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                string right_side = *it2;
                int length = right_side.size();

                /* 获取FIRST_VT */
                char first_c = right_side[0];

                // 若产生式右部第一个字符为终结符，则加入FIRST_VT集合
                if (inV_T(first_c))
                {
                    if (!inFIRST_VT(it->first, first_c))
                    {
                        FIRST_VT[it->first].insert(first_c);
                        modified = true;
                    }
                }
                // 若为非终结符，将其FIRST_VT加入当前非终结符的FIRST_VT集合；如后面还有一个非终结符，也加入
                else
                {
                    // 第一个非终结符的FIRST_VT集合加入
                    for (set<char>::iterator it3 = FIRST_VT[first_c].begin(); it3 != FIRST_VT[first_c].end(); it3++)
                    {
                        if (!inFIRST_VT(it->first, *it3))
                        {
                            FIRST_VT[it->first].insert(*it3);
                            modified = true;
                        }
                    }
                    // 后面的终结符加入
                    if (length > 1)
                    {
                        if (!inFIRST_VT(it->first, right_side[1]))
                        {
                            FIRST_VT[it->first].insert(right_side[1]);
                            modified = true;
                        }
                    }
                }

                /* 获取LAST_VT */
                char last_c = right_side[length - 1];

                // 若产生式右部最后一个字符为终结符，则加入LAST_VT集合
                if (inV_T(last_c))
                {
                    if (!inLAST_VT(it->first, last_c))
                    {
                        LAST_VT[it->first].insert(last_c);
                        modified = true;
                    }
                }
                // 若为非终结符，将其LAST_VT加入当前非终结符的LAST_VT集合；如后面还有一个非终结符，也加入
                else
                {
                    // 第一个非终结符的LAST_VT集合加入
                    for (set<char>::iterator it3 = LAST_VT[last_c].begin(); it3 != LAST_VT[last_c].end(); it3++)
                    {
                        if (!inLAST_VT(it->first, *it3))
                        {
                            LAST_VT[it->first].insert(*it3);
                            modified = true;
                        }
                    }
                    // 后面的终结符加入
                    if (right_side.size() > 1)
                    {
                        if (!inLAST_VT(it->first, right_side[length - 2]))
                        {
                            LAST_VT[it->first].insert(right_side[length - 2]);
                            modified = true;
                        }
                    }
                }
            }
        }
    }
}

void GrammarParser::getRelationTable()
{
    // 初始化分析表
    for (set<char>::iterator it = V_T.begin(); it != V_T.end(); it++)
    {
        for (set<char>::iterator it2 = V_T.begin(); it2 != V_T.end(); it2++)
        {
            relationTable[*it][*it2] = ' ';
        }
    }

    // 遍历产生式
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            // 获取当前产生式的左部和右部
            char left_side = it->first;
            string right_side = *it2;

            // 遍历产生式右部每个字符，若是终结符，进行检查
            for(int i = 0; i < right_side.size(); i++)
            {
                char c = right_side[i];
                if(!inV_T(c))
                    continue;

                // 若c前面是非终结符，则该非终结符的LAST_VT集合中的每个元素都>c
                if(i > 0 && inV_N(right_side[i - 1]))
                {
                    for(set<char>::iterator it3 = LAST_VT[right_side[i - 1]].begin(); it3 != LAST_VT[right_side[i - 1]].end(); it3++)
                    {
                        if(relationTable[*it3][c] == ' ')
                        {
                            relationTable[*it3][c] = '>';
                        }
                        else
                        {
                            cout << "Error: Conflict in relation table!" << endl;
                            exit(0);
                        }
                    }
                }
                // 若c后面是非终结符，则c<该非终结符的FIRST_VT集合中的每个元素
                if(i < right_side.size() - 1 && inV_N(right_side[i + 1]))
                {
                    for(set<char>::iterator it3 = FIRST_VT[right_side[i + 1]].begin(); it3 != FIRST_VT[right_side[i + 1]].end(); it3++)
                    {
                        if(relationTable[c][*it3] == ' ')
                        {
                            relationTable[c][*it3] = '<';
                        }
                        else
                        {
                            cout << "Error: Conflict in relation table!" << endl;
                            exit(0);
                        }
                    }
                }
                // 若c后面是终结符，则c=该终结符
                else if(i < right_side.size() - 1 && inV_T(right_side[i + 1]))
                {
                    if(relationTable[c][right_side[i + 1]] == ' ')
                    {
                        relationTable[c][right_side[i + 1]] = '=';
                    }
                    else
                    {
                        cout << "Error: Conflict in relation table!" << endl;
                        exit(0);
                    }
                }
                // 若c后两位是终结符，则c=该终结符(E)
                if(i < right_side.size() - 2 && inV_N(right_side[i + 1]) && inV_T(right_side[i + 2]))
                {
                    if(relationTable[c][right_side[i + 2]] == ' ')
                    {
                        relationTable[c][right_side[i + 2]] = '=';
                    }
                    else
                    {
                        cout << "Error: Conflict in relation table!" << endl;
                        exit(0);
                    }
                }
            }
        }
    }
}

// 打印分析表

void GrammarParser::printRelationTable()
{
    cout << endl;
    cout << "====================算符优先关系表====================" << endl;

    //上边框
    cout << "┌─────";
    for (set<char>::iterator it = V_T.begin(); it != V_T.end(); it++)
    {
        cout << "┬─────";
    }
    cout << "┐" << endl;

    // 表头
    cout << "│     ";
    for (set<char>::iterator it = V_T.begin(); it != V_T.end(); it++)
    {
        cout << "│";
        cout << "  " << *it << "  ";
    }
    cout << "│" << endl;

    // 表身
    for (set<char>::iterator it = V_T.begin(); it != V_T.end(); it++)
    {
        // 分隔线
        cout << "├─────";
        for (int i = 0; i < V_T.size(); i++)
        {
            cout << "┼─────";
        }
        cout << "┤" << endl;

        // 表格内容
        cout << "│  " << *it << "  ";
        for (set<char>::iterator it2 = V_T.begin(); it2 != V_T.end(); it2++)
        {
            cout << "│";
            cout << "  " << relationTable[*it][*it2]<< "  ";
        }
        cout << "│" <<endl;
    }

    //下边框
    cout << "└─────";
    for (int i = 0; i < V_T.size(); i++)
    {
        cout << "┴─────";
    }
    cout << "┘" << endl;
}

// 打印分析程序

void GrammarParser::printParseProcedure()
{
    // 获取输入串
    cout << "请输入要分析的字符串：";
    cin >> inputString;
    inputString.append("#");

    // #压入栈
    parseStack.push_back('#');

    // 分析过程
    cout << endl;
    cout << "====================分析程序====================" << endl;
    printf("%-7s%-18s%-7s%-18s%-12s\n", "步骤", "符号栈", "关系", "输入串", "动作");
    while (true)
    {
        // 判断分析成功条件：符号栈为#N#，输入串为空
        if(inputString.empty())
        {
            printStep(' ', ' ', "");
            cout << "分析成功！" << endl;
            break;
        }

        // 获取栈顶符号
        char X;
        for (int i = parseStack.size() - 1; i >= 0; i--)
        {
            if(inV_T(parseStack[i]) || parseStack[i] == '#')
            {
                X = parseStack[i];
                break;
            }
        }
        // 获取当前输入字符
        char c = inputString[0];

        // 获取两者关系
        char relation;
        if (X == '#')
        {
            relation = '<';
        }
        else if(c == '#')
        {
            relation = '>';
        }
        else
        {
            relation = relationTable[X][c];
        }

        // 关系为X<c或X=c，移进
        if (relation == '=' || relation == '<')
        {
            printStep(relation, c, "");
            parseStack.push_back(c);
            inputString.erase(0, 1);
            continue;
        }
        // 关系为X>c，规约
        else if(relation == '>')
        {
            string str;// 用于存储规约的产生式

            // 找到符号栈中最左素短语左边的符号，求规约串
            for(int i = parseStack.size() - 1; i >= 0; i--)
            {
                if (!inV_T(parseStack[i]) && parseStack[i] != '#')
                    continue;

                // 求后面一个非终结符ch
                char ch;
                int j;
                for (j = i + 1; j < parseStack.size(); j++)
                {
                    if (inV_T(parseStack[j]))
                    {
                        ch = parseStack[j];
                        break;
                    }
                }
                if(j == parseStack.size())
                    continue;

                // 求当前符号与ch的关系
                char relation_tmp;
                if (parseStack[i] == '#')
                {
                    relation_tmp = '<';
                }
                else
                {
                    relation_tmp = relationTable[parseStack[i]][ch];
                }

                // 当关系为<，规约最左素短语
                if(relation_tmp == '<')
                {
                    // 求规约串
                    for(int k = parseStack.size() - 1; k > i; k--)
                    {
                        str.insert(0, 1, parseStack[k]);
                    }

                    // 判断是否为产生式
                    if (!isP(str))
                    {
                        printStep(relation, ' ', "");
                        cout << "无可用于规约的产生式，分析失败！" << endl;
                        return;
                    }
                    printStep(relation, ' ',str);
                    for(int k = parseStack.size() - 1; k > i; k--)
                    {
                        parseStack.pop_back();
                    }
                    parseStack.push_back('N');
                    break;
                }
            }
        }
        // 关系为空，分析失败
        else if(relation == ' ')
        {
            printStep(relation, ' ', "");
            cout << "栈顶符号与输入符号无优先关系，分析失败！" << endl;
            break;
        }
    }
}

void GrammarParser::printStep(char relation, char c, string str)
{
    static int step = 0;

    // 获取符号栈字符串 
    vector<char> stack = parseStack;
    string s = "";
    while (!stack.empty())
    {
        s.insert(0, 1, stack.back());
        stack.pop_back();
    }

    // 获取描述动作的字符串
    string action = "";

    if(c != ' ')
    {
        action.append("移进，移进的符号：");
        action.append(1, c);
    }
    else if (str != "")
    {
        action.append("规约，最左素短语：");
        action.append(str);
    }

    printf("%-5d%-15s%-5c%-15s%-10s\n", step, s.c_str(), relation, inputString.c_str(), action.c_str());

    step++;
}

// 辅助函数

bool GrammarParser::inV_T(char c)
{
    return V_T.find(c) != V_T.end();
}

bool GrammarParser::inV_N(char c)
{
    return V_N.find(c) != V_N.end();
}

bool GrammarParser::inSet(set<char> set, char c)
{
    return set.find(c) != set.end();
}

bool GrammarParser::inFIRST_VT(char V_N, char c)
{
    return FIRST_VT[V_N].find(c) != FIRST_VT[V_N].end();
}

bool GrammarParser::inLAST_VT(char V_N, char c)
{
    return LAST_VT[V_N].find(c) != LAST_VT[V_N].end();
}

bool GrammarParser::isP(string str)
{
    // 遍历产生式
    for(map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for(set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            //产生式右部
            string right_side = *it2;

            // 长度要一样
            if (right_side.length() != str.length())
                continue;

            // 逐个字符比较，N与非终结符匹配，终结符与终结符匹配
            bool flag = true;
            for (int i = 0; i < str.size(); i++)
            {
                if (str[i] == 'N' && inV_N(right_side[i]))
                    continue;
                else if (inV_T(str[i]) && str[i] == right_side[i])
                    continue;
                else
                {
                    flag = false;
                    break;
                }
            }
            if(flag)
                return true;
        }
    }
    return false;
}