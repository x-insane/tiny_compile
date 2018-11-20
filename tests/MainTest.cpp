#include <gtest/gtest.h>
#include "common.h"
using namespace std;

TEST(Main, Test) {
    Log::setOutputStream(&std::cerr);
    ifstream fin("../../res/experiment2_test2_in.txt");
    try {
        auto list = Scanner::scan(fin);
        TokenStream stream(list);
        Parser parser(stream);
        MidCodeGenerator coder(parser.getRootTreeNode());

        // 输出
        cout << "\nAll tokens list:\n";
        for (auto &i : list)
            cout << "(" << Helper::getTokenTypeName(i.type) << ", " << i.token << ")" << endl;

        cout << endl << "symbol table:" << endl;
        parser.getSymbolTable()->print(cout);

        cout << endl << "syntax tree:" << endl;
        parser.getRootTreeNode()->print(cout);

        cout << endl << "mid codes(statement):" << endl;
        vector<MidCode*> codes = coder.getCodes();
        for (int i = 0; i < codes.size(); ++i)
            cout << i << ") " << codes[i]->statement() << endl;
        cout << endl << "mid codes(quaternary):" << endl;
        for (int i = 0; i < codes.size(); ++i)
            cout << i << ") " << codes[i]->quaternary() << endl;
    } catch (const string& err) {
        cerr << endl << err << endl;
    }
}
