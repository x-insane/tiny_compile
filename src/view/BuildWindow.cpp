#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "BuildWindow.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "../core/Scanner.h"
#include "../core/Helper.h"
#include "../core/Log.h"
#include "../core/TokenStream.h"
#include "../core/TreeNode.h"
#include "../core/MidCode.h"

BuildWindow::BuildWindow(QString filename, QWidget *parent) :
        QMainWindow(parent)
{
    resize(750, 500);

    QFont font;
    font.setFamily("Consolas");
    font.setFixedPitch(true);
    font.setPointSize(14);

    tab = new QTabWidget(this);
    QFont tabFont;
    tabFont.setFamily("Microsoft Yahei");
    tabFont.setFixedPitch(true);
    tabFont.setPointSize(12);
    tab->setFont(tabFont);

    output = new QTextBrowser(tab);
    output->setFrameShape(QFrame::NoFrame);
    output->setFont(font);
    tab->addTab(output, tr("输出"));

    QFile file(filename);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        output->setHtml("<p style=\"color: red; font-family: 'Microsoft Yahei'\">无法打开源文件: " + filename + "</p>");
        return;
    }

    QTextStream toText(&file);
    toText.setCodec("UTF-8");
    QString srcCode = toText.readAll();
    file.close();

    std::stringstream output_stream;
    std::stringstream token_stream;
    std::stringstream symbol_stream;
    std::stringstream code_stream;
    std::stringstream code_statement_stream;
    std::stringstream tree_stream;
    std::ifstream fin(filename.toStdString());
    try {
        Log log(output_stream);
        auto list = Scanner::scan(fin, log);
        TokenStream stream(list);
        Parser parser(stream, log);
        MidCodeGenerator coder(parser.getRootTreeNode());
        output_stream << "compile success" << std::endl;

        for (auto &i : list)
            token_stream << std::setw(15) << std::left << Helper::getTokenTypeName(i.type) << i.token << std::endl;
        parser.getSymbolTable()->print(symbol_stream);
        parser.getRootTreeNode()->print(tree_stream);

        std::vector<MidCode*> codes = coder.getCodes();
        for (unsigned int i = 0; i < codes.size(); ++i)
            code_stream << i << ") " << codes[i]->quaternary() << std::endl;
        for (unsigned int i = 0; i < codes.size(); ++i)
            code_statement_stream << i << ") " << codes[i]->statement() << std::endl;

        output->setText(output_stream.str().c_str());
    } catch (const std::string& err) {
        output_stream << std::endl << err << std::endl;
        output->setText(output_stream.str().c_str());
        return;
    }

    code = new QTextBrowser(tab);
    code->setFrameShape(QFrame::NoFrame);
    code->setFont(font);
    code->setText(code_stream.str().c_str());
    tab->addTab(code, tr("中间代码"));

    code_statement = new QTextBrowser(tab);
    code_statement->setFrameShape(QFrame::NoFrame);
    code_statement->setFont(font);
    code_statement->setText(code_statement_stream.str().c_str());
    tab->addTab(code_statement, tr("中间代码语句"));

    symbol = new QTextBrowser(tab);
    symbol->setFrameShape(QFrame::NoFrame);
    symbol->setFont(font);
    symbol->setText(symbol_stream.str().c_str());
    tab->addTab(symbol, tr("符号表"));

    tree = new QTextBrowser(tab);
    tree->setFrameShape(QFrame::NoFrame);
    tree->setFont(font);
    tree->setText(tree_stream.str().c_str());
    tab->addTab(tree, tr("语法树"));

    token = new QTextBrowser(tab);
    token->setFrameShape(QFrame::NoFrame);
    token->setFont(font);
    token->setText(token_stream.str().c_str());
    tab->addTab(token, tr("Tokens"));

    src = new QTextBrowser(tab);
    src->setFrameShape(QFrame::NoFrame);
    src->setFont(font);
    src->setText(srcCode);
    tab->addTab(src, tr("源代码"));
}

void BuildWindow::resizeEvent(QResizeEvent *event) {
    tab->resize(size());
}
