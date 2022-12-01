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
    getV_T_V_N();    // 获取终结符集合和非终结符集合
    getFIRST();      // 获取FIRST集合
    getFOLLOW();     // 获取FOLLOW集合
    getParseTable(); // 获取预测分析表

    // debug
    if(0)
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

        cout << "FIRST:" << endl;
        for (map<char, set<char> >::iterator it = FIRST.begin(); it != FIRST.end(); it++)
        {
            cout << it->first << ": ";
            for (set<char>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                cout << *it2 << " ";
            }
            cout << endl;
        }
        cout << endl;

        cout << "FOLLOW:" << endl;
        for (map<char, set<char> >::iterator it = FOLLOW.begin(); it != FOLLOW.end(); it++)
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
                if(!inV_N(right_side[j]))
                {
                    V_T.insert(right_side[j]);
                }
            }
        }
    }
}

void GrammarParser::getFIRST()
{
    bool modified = true;
    while(modified)
    {
        modified = false;

        // 遍历每一个产生式，将其左部的FIRST集合、右部字符串作为参数调用重载函数getFIRST
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
        {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                getFIRST(*it2, FIRST[it->first], modified);
            }
        }
    }
}

void GrammarParser::getFIRST(string str, set<char> &first, bool &modified)
{
    // 遍历字符串的每一个字符
    for (int j = 0; j < str.size(); j++)
    {
        // 如果该字符为终结符或空字符，则将该字符加入FIRST集合
        if(inV_T(str[j]) || str[j] == EMPTY)
        {
            // 终结符加入FIRST集合，若FIRST集合发生改变，则modified置为true
            if(!inSet(first, str[j]))
            {
                first.insert(str[j]);
                modified = true;
            }
            break;
        }
        // 如果该字符为非终结符，则将该字符的FIRST集合加入当前产生式左部非终结符的FIRST集合
        else if (inV_N(str[j]))
        {
            // 非终结符的FIRST集合加入当前产生式左部非终结符的FIRST集合，若FIRST集合发生改变，则modified置为true
            for (set<char>::iterator it3 = FIRST[str[j]].begin(); it3 != FIRST[str[j]].end(); it3++)
            {
                if(!inSet(first, *it3))
                {
                    first.insert(*it3);
                    if(*it3 != EMPTY)
                        modified = true;
                }
            }

            // 如果该非终结符的FIRST集合中不包含空字符，则跳出循环
            if(!inFIRST(str[j], EMPTY))
            {
                break;
            }
            // 如果该非终结符的FIRST集合中包含空字符，且该非终结符不是字符串最后一个字符，则将空字符从FIRST集合中删除
            // 循环继续，找字符串后面可能的FIRST字符
            // else if(j != str.size() - 1 && str[j] != it->first)
            else if(j != str.size() - 1)
            {
                first.erase(EMPTY);
            }
        }
    }
}

void GrammarParser::getFOLLOW()
{
    // 将#加入文法开始符号的FOLLOW集合
    FOLLOW[S].insert(END);  

    bool modified = true;
    while(modified)
    {
        modified = false;

        // 遍历每个非终结符对应的每一个产生式的右部
        for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
        {
            for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                // 产生式右部字符串
                string right_side = *it2;

                // 遍历字符串的每一个字符
                for (int j = 0; j < right_side.size(); j++)
                {
                    // 如果该字符不为非终结符，跳过
                    if(!inV_N(right_side[j]))
                    {
                        continue;
                    }

                    // 如果该非终结符不是字符串最后一个字符
                    if (j != right_side.size() - 1)
                    {
                        // 遍历后面的字符
                        for(int k = j + 1; k < right_side.size(); k++)
                        {
                            // 如果该非终结符后面的字符为终结符，则将该终结符加入该非终结符的FOLLOW集合
                            if(inV_T(right_side[k]))
                            {
                                if(!inFOLLOW(right_side[j], right_side[k]))
                                {
                                    FOLLOW[right_side[j]].insert(right_side[k]);
                                    // cout << "ternimal insert " << right_side[k] << " from "<< right_side[k] << " into FOLLOW[" << right_side[j] << "]" << endl;
                                    modified = true;
                                }
                                break;
                            }
                            // 如果该非终结符后面的字符为非终结符，则将该非终结符的FIRST集合加入该非终结符的FOLLOW集合
                            else if(inV_N(right_side[k]))
                            {
                                for (set<char>::iterator it3 = FIRST[right_side[k]].begin(); it3 != FIRST[right_side[k]].end(); it3++)
                                {
                                    if(!inFOLLOW(right_side[j], *it3) && *it3 != EMPTY)
                                    {
                                        FOLLOW[right_side[j]].insert(*it3);
                                        // cout << "nonterminal insert " << *it3 << " from "<< right_side[k] << " into FOLLOW[" << right_side[j] << "]" << endl;
                                        modified = true;
                                    }
                                }

                                // 如果该非终结符的FIRST集合中不包含空字符，则跳出循环
                                if(!inFIRST(right_side[k], EMPTY))
                                {
                                    break;
                                }
                                // 如果该非终结符的FIRST集合中包含空字符，且该非终结符是最后一个字符，把产生式左部符号的FOLLOW加进当前非终结符的FOLLOW
                                else if(k == right_side.size() - 1)
                                {
                                    for (set<char>::iterator it3 = FOLLOW[it->first].begin(); it3 != FOLLOW[it->first].end(); it3++)
                                    {
                                        if(!inFOLLOW(right_side[j], *it3))
                                        {
                                            FOLLOW[right_side[j]].insert(*it3);
                                            // cout << "nonterminal insert " << *it3 << " from "<< it->first << " into FOLLOW[" << right_side[j] << "]" << endl;
                                            modified = true;
                                        }
                                    }
                                }
                                // 否则循环继续，找字符串后面可能的FIRST字符
                            }
                        }
                    }
                    // 如果该非终结符是字符串最后一个字符，把产生式左部符号的FOLLOW加进当前非终结符的FOLLOW
                    else
                    {
                        for (set<char>::iterator it3 = FOLLOW[it->first].begin(); it3 != FOLLOW[it->first].end(); it3++)
                        {
                            if(!inFOLLOW(right_side[j], *it3))
                            {
                                FOLLOW[right_side[j]].insert(*it3);
                                // cout << "nonterminal insert " << *it3 << " from "<< it->first << " into FOLLOW[" << right_side[j] << "]" << endl;
                                modified = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void GrammarParser::getParseTable()
{
    // 初始化分析表
    for (set<char>::iterator it = V_N.begin(); it != V_N.end(); it++)
    {
        for (set<char>::iterator it2 = V_T.begin(); it2 != V_T.end(); it2++)
        {
            parseTable[*it][*it2] = "error";
        }
        parseTable[*it]['#'] = "error";
    }

    // 遍历产生式
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            // 获取当前产生式的左部和右部
            char left_side = it->first;
            string right_side = *it2;

            // 获取右部的FIRST集合
            set<char> right_side_first_set;
            bool modified;  //unused
            getFIRST(right_side, right_side_first_set, modified);

            // 遍历右部的FIRST集合，将产生式加入分析表
            string temp = "";   // 当前产生式字符串
            temp.append(1, it->first);
            temp.append("->");
            temp.append(right_side);

            for (set<char>::iterator it3 = right_side_first_set.begin(); it3 != right_side_first_set.end(); it3++)
            {
                // 如果FIRST集合中不包含空字符，则将产生式加入该终结符的分析表中
                if (*it3 != EMPTY)
                {
                    parseTable[left_side][*it3] = temp;
                }
                // 如果FIRST集合中包含空字符，则将产生式加入该非终结符的FOLLOW集合中的每个终结符的分析表中
                else
                {
                    for (set<char>::iterator it4 = FOLLOW[left_side].begin(); it4 != FOLLOW[left_side].end(); it4++)
                    {
                        parseTable[left_side][*it4] = temp;
                    }
                }
            }
        }
    }
}

// 打印分析表

void GrammarParser::printParseTable()
{
    cout << endl;
    cout << "====================LL(1)分析表:====================" << endl;

    // 获取产生式中最长的右部长度
    int maxLen = 0;
    for (map<char, set<string> >::iterator it = productions.begin(); it != productions.end(); it++)
    {
        for (set<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            if (it2->size() > maxLen)
                maxLen = it2->size();
        }
    }
    maxLen += 3;// 产生式右部的长度加上左部和箭头

    // 上边框
    cout << "┌";
    for(int i = 0; i < maxLen; i++)
    {
        cout << "─";
    }
    for (int i = 0; i <= V_T.size(); i++)
    {
        cout << "┬";
        for(int i = 0; i < maxLen; i++)
        {
            cout << "─";
        }
    }
    cout << "┐" << endl;

    // 表头
    cout << "│";
    printTableCell(' ', maxLen);
    for(map<char, string>::iterator it = parseTable[S].begin(); it != parseTable[S].end(); it++)
    {
        cout << "│";
        printTableCell(it->first, maxLen);
    }
    cout << "│" << endl;

    // 表身
    for (map<char, map<char, string> >::iterator it = parseTable.begin(); it != parseTable.end(); it++)
    {
        // 分隔线
        cout << "├";
        for (int i = 0; i < maxLen; i++)
            cout << "─";
        for (map<char, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << "┼";
            for (int i = 0; i < maxLen; i++)
                cout << "─";
        }
        cout << "┤" << endl;

        // 表格内容
        cout << "│";
        printTableCell(it->first, maxLen);
        for (map<char, string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            cout << "│";
            printTableCell(it2->second, maxLen);
        }
        cout << "│" <<endl;
    }

    // 下边框
    cout << "└";
    for(int i = 0; i < maxLen; i++)
    {
        cout << "─";
    }
    for (int i = 0; i <= V_T.size(); i++)
    {
        cout << "┴";
        for(int i = 0; i < maxLen; i++)
        {
            cout << "─";
        }
    }
    cout << "┘" << endl;
}

void GrammarParser::printTableCell(char c, int len)
{
    // 字符居中
    int left = (len - 1) / 2;
    int right = len - left - 1;
    for (int i = 0; i < left; i++)
    {
        cout << " ";
    }
    cout << c;
    for (int i = 0; i < right; i++)
    {
        cout << " ";
    }
}

void GrammarParser::printTableCell(string s, int len)
{
    // 字符串居中
    int left = (len - s.size()) / 2;
    int right = len - left - s.size();
    for (int i = 0; i < left; i++)
    {
        cout << " ";
    }
    cout << s;
    for (int i = 0; i < right; i++)
    {
        cout << " ";
    }
}

// 打印分析程序

void GrammarParser::printParseProcedure()
{
    // 获取输入串
    cout << "请输入要分析的字符串：";
    cin >> inputString;
    inputString.append("#");

    // #和开始符号压入栈
    parseStack.push('#');
    parseStack.push(S);

    // 分析过程
    cout << endl;
    cout << "====================分析程序====================" << endl;
    printf("%-12s%-13s%-13s%-12s\n", "步骤", "符号栈", "输入串", "所用产生式");
    char c = inputString[0]; // 当前输入字符
    printStep("");
    while(true)
    {
        char X = parseStack.top();  // 栈顶符号

        if(inV_T(X))    // X为终结符
        {
            if (X == c) // 匹配
            {
                parseStack.pop();
                inputString = inputString.substr(1);
                c = inputString[0];
                printStep("");
            }
            else    // 不匹配
            {
                cout << "分析失败！" << endl;
                break;
            }
        }
        else if(inV_N(X) && parseTable[X][c] != "error")    // X为非终结符，且有产生式
        {
            parseStack.pop();
            // 产生式右部压入栈
            string right = parseTable[X][c].substr(3);
            for (int i = right.size() - 1; i >= 0; i--)
            {
                if(right[i] != EMPTY)
                    parseStack.push(right[i]);
            }
            printStep(parseTable[X][c]);
        }
        else if(X == '#')
        {
            if(c == '#')
            {
                cout << "分析成功！" << endl;
                break;
            }
            else
            {
                cout << "分析失败！" << endl;
                break;
            }
        }
        else
        {
            cout << "分析失败！" << endl;
            break;
        }
    }
}

void GrammarParser::printStep(string production)
{
    static int step = 0;
    
    // 获取符号栈字符串
    stack<char> stack = parseStack;
    string s = "";
    while (!stack.empty())
    {
        s.insert(0, 1, stack.top());
        stack.pop();
    }

    printf("%-10d%-10s%-10s%-10s\n", step, s.c_str(), inputString.c_str(), production.c_str());

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

bool GrammarParser::inFIRST(char V_N, char c)
{
    return FIRST[V_N].find(c) != FIRST[V_N].end();
}

bool GrammarParser::inFOLLOW(char V_N, char c)
{
    return FOLLOW[V_N].find(c) != FOLLOW[V_N].end();
}