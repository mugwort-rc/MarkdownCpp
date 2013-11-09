/*
 * tables.cpp
 *
 *  Created on: 2013/11/04
 *      Author: mugwort_rc
 */

#include "tables.h"

#include <boost/algorithm/string.hpp>

#include "../MarkdownCpp.h"
#include "../BlockParser.h"
#include "../BlockProcessors.h"

namespace markdown{

/*!
 * Process Tables.
 */
class TableProcessor : public BlockProcessor
{
public:
    TableProcessor(BlockParser* parser) :
        BlockProcessor(parser),
        CHECK_CHARS({L'|', L':', L'-'})
    {}

    bool test(Element&, const std::wstring& block)
    {
        std::vector<std::wstring> rows;
        boost::algorithm::split(rows, block, boost::is_any_of(L"\n"));
        return rows.size() > 2
                && rows[0].find(L'|') != std::wstring::npos
                && rows[1].find(L'|') != std::wstring::npos
                && rows[1].find(L'-') != std::wstring::npos
                && this->CHECK_CHARS.find(boost::algorithm::trim_copy(rows[1]).at(0)) != this->CHECK_CHARS.end();
    }

    /*!
     * Parse a table block and build table.
     */
    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring blocksTmp = blocks.front();
        blocks.pop_front();
        std::vector<std::wstring> block;
        boost::algorithm::split(block, blocksTmp, boost::is_any_of(L"\n"));
        std::wstring header = boost::algorithm::trim_copy(block[0]);
        std::wstring separator = boost::algorithm::trim_copy(block[1]);
        std::vector<std::wstring> rows;
        int counter = 0;
        for ( const std::wstring& row : block ) {
            if ( ++counter < 3 ) {
                continue;
            }
            rows.push_back(row);
        }
        //! Get format type (bordered by pipes or not)
        bool border = false;
        if ( header.at(0) == L'|' ) {
            border = true;
        }
        //! Get alignment of columns
        std::list<boost::optional<std::wstring>> align;
        for ( const std::wstring& c : this->split_row(separator, border) ) {
            if ( c.at(0) == L':' && c.at(c.size()-1) == L':' ) {
                align.push_back(std::wstring(L"center"));
            } else if ( c.at(0) == L':' ) {
                align.push_back(std::wstring(L"left"));
            } else if ( c.at(c.size()-1) == L':' ) {
                align.push_back(std::wstring(L"right"));
            } else {
                align.push_back(boost::none);
            }
        }
        //! Build table
        Element table(parent, L"table");
        parent.append(table);
        Element thead(table, L"thead");
        table.append(thead);
        this->build_row(header, thead, align, border);
        Element tbody(table, L"tbody");
        table.append(tbody);
        for ( const std::wstring& row : rows ) {
            this->build_row(boost::algorithm::trim_copy(row), tbody, align, border);
        }
    }

private:
    /*!
     * Given a row of text, build table cells.
     */
    void build_row(const std::wstring& row, Element &parent, const std::list<boost::optional<std::wstring>>& align, bool border)
    {
        Element tr(parent, L"tr");
        parent.append(tr);
        std::wstring tag = L"td";
        if ( parent.getTagName() == L"thead" ) {
            tag = L"th";
        }
        std::vector<std::wstring> cells = this->split_row(row, border);
        //! We use align here rather than cells to ensure every row
        //! contains the same number of columns.
        std::size_t i = 0;
        for ( boost::optional<std::wstring> a : align ) {
            Element c(tr, tag);
            tr.append(c);
            if ( cells.size() > i ) {
                std::wstring cell = boost::algorithm::trim_copy(cells.at(i));
                c.setText(cell);
            } else {
                c.setText(std::wstring());
            }
            if ( a ) {
                c.setAttribute(L"align", *a);
            }
            ++i;
        }
    }
    /*!
     * split a row of text into list of cells.
     */
    std::vector<std::wstring> split_row(const std::wstring& row, bool border)
    {
        std::wstring tmp = row;
        if ( border ) {
            if ( tmp.at(0) == L'|' ) {
                tmp = tmp.substr(1, tmp.size()-1);
            }
            if ( tmp.at(tmp.size()-1) == L'|' ) {
                tmp = tmp.substr(0, tmp.size()-1);
            }
        }
        std::vector<std::wstring> result;
        boost::algorithm::split(result, tmp, boost::is_any_of(L"|"));
        return result;
    }

private:
    std::set<wchar_t> CHECK_CHARS;

};

TableExtension::TableExtension() :
    Extension()
{}

void TableExtension::extendMarkdown(Markdown *md/*, md_globals*/)
{
    md->parser->blockprocessors.add("table", boost::shared_ptr<BlockProcessor>(new TableProcessor(md->parser.get())), "<hashheader");
}

Extension::Ptr TableExtension::generate(void)
{
    return boost::shared_ptr<Extension>(new TableExtension);
}

} // end of namespace markdown
