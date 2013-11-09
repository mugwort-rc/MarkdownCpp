#include <iostream>
#include <locale>

#include <xercesc/util/PlatformUtils.hpp>

#include "MarkdownCpp.h"
#include "extensions/tables.h"

class Initializer
{
public:
    Initializer(void)
    {
        xercesc::XMLPlatformUtils::Initialize();
    }
    ~Initializer(void)
    {
        xercesc::XMLPlatformUtils::Terminate();
    }

};

void markdown_test(const std::wstring& name, const std::wstring& test)
{
    std::wcout << L"=====" << name << L"=====" << std::endl;
    markdown::Markdown md;
    std::wcout << md.convert(test) << std::endl;
}

void markdown_table_test(const std::wstring& name, const std::wstring& test)
{
    std::wcout << L"=====" << name << L"=====" << std::endl;
    markdown::Markdown md({markdown::TableExtension::generate()});
    std::wcout << md.convert(test) << std::endl;
}

int main(int, char*[])
{
    Initializer init;

    setlocale(LC_CTYPE, "");

    markdown_test(L"emphasis1", L"This is *Example* Markdown.");
    markdown_test(L"em/strong", L"*this is test*\n"
                                L"**this is test too!**");
    //! Success !!
    markdown_test(L"link", L"This is [link](http://www.example.com).\n");
    markdown_test(L"reference", L"This is [referenced link][reference].\n"
                                L"\n"
                                L"\n[reference]: http://www.example.com \"Example\"");
    markdown_test(L"inline code", L"this is `inline-code` test.");
    markdown_test(L"automatic link 2", L"This is <http://www.example.com> link.");
    markdown_test(L"amp test", L"Copyright &copy; 2013. Alice&Bob.");
    markdown_test(L"image", L"![Text](./test.png)");
    markdown_test(L"headline test", L"# This is Test 1 \n"
                                    L"## This is Test 2 \n"
                                    L"### This is Test 3 \n"
                                    L"#### This is Test 4 \n"
                                    L"##### This is Test 5 \n"
                                    L"###### This is Test 6 \n"
                                    L"####### This is Test 7\n"
                                    L"\n"
                                    L"\n ## This is not headline.");
    markdown_test(L"line headline test", L"This is Headline 1\n"
                                         L"==================\n"
                                         L"\n"
                                         L"This is Headline 2\n"
                                         L"---------------------\n");
    markdown_test(L"blockquote", L"This is paragraph.\n"
                                 L"> This is quoted block.\n"
                                 L"> This is too.\n"
                                 L"> > This is double quoted...\n"
                                 L">\n"
                                 L"> markdown test\n"
                                 L">\n"
                                 L"> 1. list1\n"
                                 L"> 2. list2\n"
                                 L"\n"
                                 L"Here is paragraph.");
    markdown_test(L"horizontal", L"Horizontal line test.\n"
                                 L"\n"
                                 L"* * * *\n"
                                 L"Next paragraph.");
    markdown_test(L"ulists", L"* Item1\n"
                             L"* Item2\n"
                             L"* Item3\n"
                             L"---\n"
                             L"- ItemA\n"
                             L"- ItemB\n"
                             L"- ItemC\n"
                             L"---\n"
                             L"+ ItemX\n"
                             L"\n"
                             L"+ ItemY\n"
                             L"\n"
                             L"+ ItemZ");
    markdown_test(L"olists", L"1. Item1\n"
                             L"2. Item2\n"
                             L"3. Item3\n"
                             L"---\n"
                             L"1. ItemX\n"
                             L"\n"
                             L"2. ItemY\n"
                             L"\n"
                             L" 3. ItemZ");
    markdown_test(L"code block", L"This is normal paragraph:\n"
                                 L"\n"
                                 L"    This is code block.\n"
                                 L"   This is paragraph.");
    markdown_test(L"automatic link", L"<http://www.example.com>");

    markdown_table_test(L"table test", L"HEAD1 | HEAD2 \n"
                                       L"------|------:\n"
                                       L"aaa   | bbb\n"
                                       L"ccc   | ddd\n\n");

    return 0;
}
