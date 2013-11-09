/*
 * BlockProcessors.cpp
 *
 *  Created on: 2013/10/27
 *      Author: mugwort_rc
 */

#include "BlockProcessors.h"

#include <QDebug>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include "MarkdownCpp.h"
#include "BlockParser.h"

namespace markdown{

BlockProcessor::BlockProcessor(BlockParser* parser) :
	parser(parser), tab_length(parser->markdown->tab_length())
{}

BlockProcessor::~BlockProcessor()
{}

Element BlockProcessor::lastChild(const Element &parent)
{
    if ( parent.child().size() > 0 ) {
        return parent.getLastElementChild();
    }
    return Element::InvalidElement;
}

boost::tuples::tuple<std::wstring, std::wstring> BlockProcessor::detab(const std::wstring &text)
{
    std::list<std::wstring> newtext, lines;
	boost::algorithm::split(lines, text, boost::is_any_of(L"\n"));
    for ( const std::wstring& line : lines ) {
        if ( line.substr(0, this->tab_length) == std::wstring(this->tab_length, L' ') ) {
			newtext.push_back(line.substr(this->tab_length, line.size()-this->tab_length));
		} else if ( boost::algorithm::trim_copy(line).empty() ) {
            newtext.push_back(std::wstring());
		} else {
			break;
		}
	}
    std::list<std::wstring> resultLines;
    unsigned int counter = 0;
    for ( std::list<std::wstring>::const_iterator it = lines.begin(); it != lines.end(); ++it, ++counter ) {
		if ( counter < newtext.size() ) {
			continue;
		}
		resultLines.push_back(*it);
    }
    return boost::tuples::make_tuple(boost::algorithm::join(newtext, L"\n"), boost::algorithm::join(resultLines, L"\n"));
}

std::wstring BlockProcessor::looseDetab(const std::wstring &text, unsigned int level)
{
	const unsigned int length = this->tab_length*level;
    std::vector<std::wstring> lines;
	boost::algorithm::split(lines, text, boost::is_any_of(L"\n"));
    for ( unsigned int i = 0; i < lines.size(); ++i ) {
        if ( lines[i].substr(0, length) == std::wstring(length, L' ') ) {
			lines[i] = lines[i].substr(length, lines[i].size()-length);
		}
	}
	return boost::algorithm::join(lines, L"\n");
}

/*!
 * Process children of list items.
 *
 *  Example:
 *      * a list item
 *          process this part
 *
 *          or this part
 *
 */
class ListIndentProcessor : public BlockProcessor
{
public:
    ListIndentProcessor(BlockParser *parser) :
		BlockProcessor(parser),
		INDENT_RE((boost::wformat(L"^(([ ]{%s})+)")%this->tab_length).str())
	{}
	~ListIndentProcessor(void)
	{}

    bool test(Element &parent, const std::wstring &block)
    {
        return block.substr(0, this->tab_length) == std::wstring(this->tab_length, L' ')
                && ( std::find(this->ITEM_TYPES.begin(), this->ITEM_TYPES.end(), parent.getTagName()) != this->ITEM_TYPES.end()
                || ( parent.child().size() > 0
                     && std::find(this->LIST_TYPES.begin(), this->LIST_TYPES.end(), parent.getLastElementChild().getTagName()) != this->LIST_TYPES.end() )
                );
	}

    void run(Element &parent, std::list<std::wstring> &blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        boost::tuples::tuple<int, Element> result = this->get_level(parent, block);
        int level = result.get<0>();
        Element sibling = result.get<1>();
        block = this->looseDetab(block, level);

        this->parser->state.set(L"detabbed");
        if ( std::find(this->ITEM_TYPES.begin(), this->ITEM_TYPES.end(), parent.getTagName()) != this->ITEM_TYPES.end() ) {
            //! It's possible that this parent has a 'ul' or 'ol' child list
            //! with a member.  If that is the case, then that should be the
            //! parent.  This is intended to catch the edge case of an indented
            //! list whose first member was parsed previous to this point
            //! see OListProcessor
            if ( parent.child().size() > 0 && std::find(this->LIST_TYPES.begin(), this->LIST_TYPES.end(), parent.getLastElementChild().getTagName()) != this->LIST_TYPES.end() ) {
                std::list<std::wstring> new_blocks = {block};
                Element new_parent = parent.getLastElementChild();
                this->parser->parseBlocks(new_parent, new_blocks);
            } else {
                std::list<std::wstring> new_blocks = {block};
                //! The parent is already a li. Just parse the child block.
                this->parser->parseBlocks(parent, new_blocks);
            }
        } else if ( std::find(this->ITEM_TYPES.begin(), this->ITEM_TYPES.end(), sibling.getTagName()) != this->ITEM_TYPES.end() ) {
            //! The sibling is a li. Use it as parent.
            std::list<std::wstring> new_blocks = {block};
            this->parser->parseBlocks(sibling, new_blocks);
        } else if ( sibling.child().size() > 0 && std::find(this->ITEM_TYPES.begin(), this->ITEM_TYPES.end(), sibling.getLastElementChild().getTagName()) != this->ITEM_TYPES.end() ) {
            //! The parent is a list (``ol`` or ``ul``) which has children.
            //! Assume the last child li is the parent of this block.
            if ( sibling.getLastElementChild().hasText() ) {
                //! If the parent li has text, that text needs to be moved to a p
                //! The p must be 'inserted' at beginning of list in the event
                //! that other children already exist i.e.; a nested sublist.
                Element elem = sibling.getLastElementChild();
                std::wstring text = elem.text();
                elem.removeText();
                Element p(elem, L"p");
                p.setText(text);
                if ( elem.child().size() > 0 ) {
                    elem.insertBefore(p, elem.getFirstElementChild());
                } else {
                    elem.append(p);
                }
            }
            Element new_parent = sibling.getLastElementChild();
            this->parser->parseChunk(new_parent, block);
        } else {
            this->create_item(sibling, block);
        }
        this->parser->state.reset();
    }

    /*!
     * Create a new li and parse the block with it as the parent.
     */
    void create_item(Element &parent, const std::wstring &block)
    {
        Element li(parent, L"li");
        parent.append(li);
        std::list<std::wstring> new_blocks = {block};
        this->parser->parseBlocks(li, new_blocks);
    }

    /*!
     * Get level of indent based on list level.
     */
    boost::tuples::tuple<int, Element> get_level(Element &parent, const std::wstring &block)
    {
        //! Get indent level
        int indent_level = 0;
        int level = 0;
        boost::wsmatch m;
        if ( boost::regex_match(block, m, this->INDENT_RE) ) {
            indent_level = m.str(1).size()/this->tab_length;
        }
        if ( this->parser->state.isstate(L"list") ) {
            //! We're in a tightlist - so we already are at correct parent.
            level = 1;
        } else {
            //! We're in a looselist - so we need to find parent.
            level = 0;
        }
        //! Step through children of tree to find matching indent level.
        while ( indent_level > level ) {
            Element child = this->lastChild(parent);
            if ( ! child.isNull() && ( this->LIST_TYPES.find(child.getTagName()) != this->LIST_TYPES.end() || this->ITEM_TYPES.find(child.getTagName()) != this->ITEM_TYPES.end() ) ) {
                if ( this->ITEM_TYPES.find(child.getTagName()) != this->ITEM_TYPES.end() ) {
                    level += 1;
                }
                parent = child;
            } else {
                //! No more child levels. If we're short of indent_level,
                //! we have a code block. So we stop here.
                break;
            }
        }
        return boost::tuples::make_tuple(level, parent);
    }

private:
	boost::wregex INDENT_RE;

private:
    static const std::set<std::wstring> ITEM_TYPES;
    static const std::set<std::wstring> LIST_TYPES;

};

const std::set<std::wstring> ListIndentProcessor::ITEM_TYPES = {L"li"};
const std::set<std::wstring> ListIndentProcessor::LIST_TYPES = {L"ul", L"ol"};

/*!
 * Process code blocks.
 */
class CodeBlockProcessor : public BlockProcessor
{
public:
    CodeBlockProcessor(BlockParser *parser) :
        BlockProcessor(parser)
    {}

    bool test(Element&, const std::wstring &block)
    {
        return block.substr(0, this->tab_length) == std::wstring(this->tab_length, L' ');
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        Element sibling = this->lastChild(parent);
        std::wstring block = blocks.front();
        blocks.pop_front();
        std::wstring theRest;
        if ( ! sibling.isNull() && sibling.getTagName() == L"pre" && sibling.child().size() > 0 && sibling.getFirstElementChild().getTagName() == L"code" ) {
            //! The previous block was a code block. As blank lines do not start
            //! new code blocks, append this block to the previous, adding back
            //! linebreaks removed from the split into a list.
            Element code = sibling.getFirstElementChild();
            boost::tuples::tuple<std::wstring, std::wstring> result = this->detab(block);
            block   = result.get<0>();
            theRest = result.get<1>();
            code.setText((boost::wformat(L"%s\n%s\n")%code.text()%boost::algorithm::trim_right_copy(block)).str());
        } else {
            Element pre(parent, L"pre");
            parent.append(pre);
            Element code(pre, L"code");
            pre.append(code);;
            boost::tuples::tuple<std::wstring, std::wstring> result = this->detab(block);
            block   = result.get<0>();
            theRest = result.get<1>();
            code.setText((boost::wformat(L"%s\n")%boost::algorithm::trim_right_copy(block)).str());
        }
        if ( ! theRest.empty() ) {
            //! This block contained unindented line(s) after the first indented
            //! line. Insert these lines as the first block of the master blocks
            //! list for future processing.
            blocks.push_front(theRest);
        }
    }

};

class BlockQuoteProcessor : public BlockProcessor
{
public:
    BlockQuoteProcessor(BlockParser *parser) :
        BlockProcessor(parser),
        RE(L"(^|\\n)[ ]{0,3}>[ ]?(.*)")
    {}

    bool test(Element&, const std::wstring &block)
    {
        return boost::regex_search(block, this->RE);
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        boost::wsmatch m;
        if ( boost::regex_search(block, m, this->RE) ) {
            std::wstring before = block.substr(0, m.position());  //!< Lines before blockquote
            //! Pass lines before blockquote in recursively for parsing forst.
            std::list<std::wstring> new_blocks = {before};
            this->parser->parseBlocks(parent, new_blocks);
            //! Remove ``> `` from begining of each line.
            std::wstring after = block.substr(m.position(), block.size()-m.position());
            std::list<std::wstring> lines;
            boost::algorithm::split(lines, after, boost::is_any_of(L"\n"));
            std::list<std::wstring> new_lines;
            for ( const std::wstring &line : lines ) {
                new_lines.push_back(this->clean(line));
            }
            block = boost::algorithm::join(new_lines, L"\n");
        }
        Element sibling = this->lastChild(parent);
        Element quote = Element::InvalidElement;
        if ( ! sibling.isNull() && sibling.getTagName() == L"blockquote" ) {
            //! Previous block was a blockquote so set that as this blocks parent
            quote = sibling;
        } else {
            //! This is a new blockquote. Create a new parent element.
            quote = Element(parent, L"blockquote");
            parent.append(quote);
        }
        //! Recursively parse block with blockquote as parent.
        //! change parser state so blockquotes embedded in lists use p tags
        this->parser->state.set(L"blockquote");
        this->parser->parseChunk(quote, block);
        this->parser->state.reset();
    }

    /*!
     * Remove ``>`` from beginning of a line.
     */
    std::wstring clean(const std::wstring &line)
    {
        boost::wsmatch m;
        if ( boost::algorithm::trim_copy(line) == L">" ) {
            return std::wstring();
        } else if ( boost::regex_match(line, m, this->RE) ) {
            return m.str(2);
        } else {
            return line;
        }
    }

private:
    const boost::wregex RE;

};

/*!
 * Process ordered list blocks.
 */
class OListProcessor : public BlockProcessor
{
public:
    OListProcessor(BlockParser *parser) :
        BlockProcessor(parser),
        TAG(L"ol"),
        RE(L"^[ ]{0,3}\\d+\\.[ ]+(.*)"),
        CHILD_RE(L"^[ ]{0,3}((\\d+\\.)|[*+-])[ ]+(.*)"),
        INDENT_RE(L"^[ ]{4,7}((\\d+\\.)|[*+-])[ ]+.*"),
        STARTSWITH(L"1"),
        SIBLING_TAGS({L"ol", L"ul"})
    {}
    virtual ~OListProcessor()
    {}

    bool test(Element&, const std::wstring &block)
    {
        return boost::regex_match(block, this->RE);
    }

    void run(Element& parent, std::list<std::wstring>& blocks)
    {
        //! Check fr multiple items in one block.
        std::wstring block = blocks.front();
        blocks.pop_front();
        std::list<std::wstring> items = this->get_items(block);
        Element sibling = this->lastChild(parent);
        Element lst = Element::InvalidElement;

        if ( ! sibling.isNull() && this->SIBLING_TAGS.find(sibling.getTagName()) != this->SIBLING_TAGS.end() ) {
            //! Previous block was a list item, so set that as parent
            lst = sibling;
            //! make sure previous item is in a p- if the item has text, then it
            //! it isn't in a p
            if ( lst.child().size() > 0 && lst.getLastElementChild().hasText() ) {
                //! since it's possible there are other children for this sibling,
                //! we can't just SubElement the p, we need to insert it as the
                //! first item
                Element elem = lst.getLastElementChild();
                Element p = Element(elem, L"p");
                std::wstring text = elem.text();
                elem.removeText();
                p.setText(text);
                if ( elem.child().size() > 0 ) {
                    elem.insertBefore(p, elem.getFirstElementChild());
                } else {
                    elem.append(p);
                }
            }
            //! if the last item has a tail, then the tail needs to be put in a p
            //! likely only when a header is not followed by a blank line
            Element lch = this->lastChild(lst.getLastElementChild());
            if ( ! lch.isNull() && lch.hasTail() ) {
                std::wstring tailText = boost::algorithm::trim_left_copy(lch.tail());
                lch.removeTail();
                Element p(lst, L"p");
                p.setText(tailText);
                lst.getLastElementChild().append(p);
            }

            //! parse first block differently as it gets wrapped in a p.
            Element li(lst, L"li");
            lst.append(li);
            this->parser->state.set(L"looselist");
            std::wstring firstitem = items.front();
            items.pop_front();
            std::list<std::wstring> new_blocks = {firstitem};
            this->parser->parseBlocks(li, new_blocks);
            this->parser->state.reset();
        } else if ( parent.getTagName() == L"ol" || parent.getTagName() == L"ul" ) {
            //! this catches the edge case of a multi-item indented list whose
            //! first item is in a blank parent-list item:
            //! * * subitem1
            //!     * subitem2
            //! see also ListIndentProcessor
            lst = parent;
        } else {
            //! This is a new list so create parent with appropriate tag.
            lst = Element(parent, this->TAG);
            parent.append(lst);
            //! Check if a custom start integer is set
            if ( ! this->parser->markdown->lazy_ol() && this->STARTSWITH != L"1" ) {
                lst.setAttribute(L"start", this->STARTSWITH);
            }
        }

        this->parser->state.set(L"list");
        //! Loop through items in block, recursively parsing each with the
        //! appropriate parent.
        for ( const std::wstring &item : items ) {
            std::list<std::wstring> new_blocks = {item};
            if ( item.substr(0, this->tab_length) == std::wstring(this->tab_length, L' ') ) {
                Element new_parent = lst.getLastElementChild();
                //! Item is indented. Parse with last item as parent
                this->parser->parseBlocks(new_parent, new_blocks);
            } else {
                //! New item. Create li and parse with it as parent
                Element li = Element(lst, L"li");
                lst.append(li);
                this->parser->parseBlocks(li, new_blocks);
            }
        }
        this->parser->state.reset();
    }

    /*!
     * Break a block into list items.
     */
    std::list<std::wstring> get_items(const std::wstring &block)
    {
        std::list<std::wstring> items;
        std::list<std::wstring> lines;
        boost::algorithm::split(lines, block, boost::is_any_of(L"\n"));
        for ( const std::wstring &line : lines ) {
            boost::wsmatch m;
            if ( boost::regex_match(line, m, this->CHILD_RE) ) {
                //! This is a new list item
                //! Check first item for the start index
                if ( items.empty() && this->TAG == L"ol" ) {
                    //! Detect the integer value of first list item
                    boost::wregex INTEGER_RE(L"(\\d+)");
                    boost::wsmatch im;
                    std::wstring regexTmp = m.str(1);
                    boost::regex_search(regexTmp, im, INTEGER_RE);
                    this->STARTSWITH = im.str();
                }
                //! Append to the list
                items.push_back(m.str(3));
            } else if ( boost::regex_match(line, this->INDENT_RE) ) {
                //! This is an indented (possibly nested) item.
                if ( ! items.empty() && items.back().substr(0, this->tab_length) == std::wstring(this->tab_length, L' ') ) {
                    //! Previous item was indented. Append to that item.
                    items.back() = (boost::wformat(L"%s\n%s")%items.back()%line).str();
                } else {
                    items.push_back(line);
                }
            } else {
                //! This is another line of previous item. Append to that item.
                items.back() = (boost::wformat(L"%s\n%s")%items.back()%line).str();
            }
        }
        return items;
    }

protected:
    std::wstring TAG;
    //! Detect an item (``1. item``). ``group(1)`` contains contents of item.
    boost::wregex RE;

private:
    //! Detect items on secondary lines. they can be of either list type.
    const boost::wregex CHILD_RE;
    //! Detect indented (nested) items of either type
    const boost::wregex INDENT_RE;
    //! The integer (python string) with which the lists starts (default=1)
    //! Eg: If list is intialized as)
    //!   3. Item
    //! The ol tag will get starts="3" attribute
    std::wstring STARTSWITH;
    //! List of allowed sibling tags.
    const std::set<std::wstring> SIBLING_TAGS;

};

class UListProcessor : public OListProcessor
{
public:
    UListProcessor(BlockParser* parser) :
        OListProcessor(parser)
    {
        OListProcessor::TAG = L"ul";
        OListProcessor::RE = boost::wregex(L"^[ ]{0,3}[*+-][ ]+(.*)");
    }

};

/*!
 * Process Hash Headers.
 */
class HashHeaderProcessor : public BlockProcessor
{
public:
    HashHeaderProcessor(BlockParser *parser) :
        BlockProcessor(parser),
        RE(L"(^|\\n)(?<level>#{1,6})(?<header>.*?)#*(\\n|$)")
    {}

    bool test(Element&, const std::wstring &block)
    {
        return boost::regex_search(block, this->RE);
    }

    void run(Element &parent, std::list<std::wstring> &blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        boost::wsmatch m;
        if ( boost::regex_search(block, m, this->RE) ) {
            std::wstring before = block.substr(0, m.position());  //!< All lines before header
            std::wstring after  = block.substr(m.position()+m.length(), block.size()-m.position()-m.length()); //!< All lines after header
            if ( ! before.empty() ) {
                //! As the header was not the first line of the block and the
                //! lines before the header must be parsed first,
                //! recursively parse this lines as a block.
                std::list<std::wstring> new_blocks = {before};
                this->parser->parseBlocks(parent, new_blocks);
            }
            //! Create header using named groups from RE
            Element h = Element(parent, (boost::wformat(L"h%d")%m.str(L"level").size()).str());
            parent.append(h);
            h.setText(boost::algorithm::trim_copy(m.str(L"header")));
            if ( ! after.empty() ) {
                //! Insert remaining lines as first block for future parsing.
                blocks.push_front(after);
            }
        } else {
            //! This should never happen, but just in case...
            qDebug() << "We've got a problem header: " << QString::fromStdWString(block);
        }
    }

private:
    //! Detect a header at start of any line in block
    boost::wregex RE;

};

/*!
 * Process Setext-style Headers.
 */
class SetextHeaderProcessor : public BlockProcessor
{
public:
    SetextHeaderProcessor(BlockParser *parser) :
        BlockProcessor(parser),
        RE(L"(?m)^.*?\\n[=-]+[ ]*(\\n|$)")
    {}

    bool test(Element&, const std::wstring &block)
    {
        return boost::regex_match(block, this->RE);
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        std::vector<std::wstring> lines;
        boost::algorithm::split(lines, block, boost::is_any_of(L"\n"));
        //! Determine level. ``=`` is 1 and ``-`` is 2.
        int level = 0;
        if ( lines.at(1).substr(0, 1) == L"=" ) {
            level = 1;
        } else {
            level = 2;
        }
        Element h(parent, (boost::wformat(L"h%d")%level).str());
        parent.append(h);
        h.setText(boost::algorithm::trim_copy(lines.at(0)));
        if ( lines.size() > 2 ) {
            //! Block contains additional lines. Add to  master blocks for later.
            std::list<std::wstring> buff;
            int counter = 0;
            for ( const std::wstring &line : lines ) {
                if ( counter++ < 2 ) {
                    continue;
                }
                buff.push_back(line);
            }
            blocks.push_front(boost::algorithm::join(buff, L"\n"));
        }
    }

private:
    //! Detect Setext-style header. Must be first 2 lines of block.
    const boost::wregex RE;

};

/*!
 * Process Horizontal Rules.
 */
class HRProcessor : public BlockProcessor
{
public:
    HRProcessor(BlockParser *parser) :
        BlockProcessor(parser),
        SEARCH_RE(L"(?m)^[ ]{0,3}((-+[ ]{0,2}){3,}|(_+[ ]{0,2}){3,}|(\\*+[ ]{0,2}){3,})[ ]*"),
        match()
    {}

    bool test(Element&, const std::wstring &block)
    {
        //! No atomic grouping in python so we simulate it here for performance.
        //! The regex only matches what would be in the atomic group - the HR.
        //! Then check if we are at end of block or if next char is a newline.
        boost::wsmatch m;
        if ( boost::regex_search(block, m, this->SEARCH_RE)
             && ( static_cast<unsigned int>( m.position()+m.length() ) == block.size()
                  || block.at(m.position()+m.length()) == L'\n' ) ) {
            //! Save match object on class instance so we can use it later.
            this->match = m;
            return true;
        }
        return false;
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        //! Check for lines in block before hr.
        std::wstring prelines = boost::algorithm::trim_right_copy_if(block.substr(0, this->match.position()), [](wchar_t ch) -> bool { return ch == L'\n'; });
        if ( ! prelines.empty() ) {
            //! Recursively parse lines before hr so they get parsed first.
            std::list<std::wstring> new_blocks = {prelines};
            this->parser->parseBlocks(parent, new_blocks);
        }
        //! create hr
        Element hr(parent, L"hr");
        parent.append(hr);
        //! check for lines in block after hr.
        int begin = this->match.position()+this->match.length();
        std::wstring postlines = boost::algorithm::trim_left_copy_if(block.substr(begin, block.size()-begin), [](wchar_t ch) -> bool { return ch == L'\n'; });
        if ( ! postlines.empty() ) {
            //! Add lines after hr to master blocks for later parsing.
            blocks.push_front(postlines);
        }
    }

private:
    //! Detect hr on any line of a block.
    boost::wregex SEARCH_RE;
    boost::wsmatch match;

};

/*!
 * Process blocks that are empty or start with an empty line.
 */
class EmptyBlockProcessor : public BlockProcessor
{
public:
    EmptyBlockProcessor(BlockParser *parser) :
        BlockProcessor(parser)
    {}

    bool test(Element&, const std::wstring &block)
    {
        return block.empty() || block.at(0) == L'\n';
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        std::wstring filler = L"\n\n";
        if ( ! block.empty() ) {
            //! Starts with empty line
            //! Only replace a single line.
            filler = L"\n";
            //! Save the rest for later.
            std::wstring theRest = block.substr(1, block.size()-1);
            if ( ! theRest.empty() ) {
                //! Add remaining lines to master blocks for later.
                blocks.push_front(theRest);
            }
        }
        Element sibling = this->lastChild(parent);
        if ( ! sibling.isNull() && sibling.getTagName() == L"pre" && sibling.child().size() > 0 && sibling.getFirstElementChild().getTagName() == L"code" ) {
            //! Last block is a codeblock. Append to preserve whitespace.
            std::wstring codeText = sibling.getFirstElementChild().text();
            sibling.getFirstElementChild().setText((boost::wformat(L"%s%s")%codeText%filler).str());
        }
    }

};

/*!
 * Process Paragraph blocks.
 */
class ParagraphProcessor : public BlockProcessor
{
public:
    ParagraphProcessor(BlockParser *parser) :
        BlockProcessor(parser)
    {}

    bool test(Element&, const std::wstring&)
    {
        return true;
    }

    void run(Element &parent, std::list<std::wstring>& blocks)
    {
        std::wstring block = blocks.front();
        blocks.pop_front();
        if ( ! boost::algorithm::trim_copy(block).empty() ) {
            //! Not a blank block. Add to parent, otherwise throw it away.
            if ( this->parser->state.isstate(L"list") ) {
                //! The parent is a tight-list.
                //!
                //! Check for any children. This will likely only happen in a
                //! tight-list when a header isn't followed by a blank line.
                //! For example:
                //!
                //!     * # Header
                //!     Line 2 of list item - not part of header.
                Element sibling = this->lastChild(parent);
                if ( ! sibling.isNull() ) {
                    //! Insetrt after sibling.
                    if ( sibling.hasTail() ) {
                        std::wstring tailText = sibling.tail();
                        sibling.setTail((boost::wformat(L"%s\n%s")%tailText%block).str());
                    } else {
                        sibling.setTail((boost::wformat(L"\n%s")%block).str());
                    }
                } else {
                    //! Append to parent.text
                    if ( parent.hasText() ) {
                        std::wstring parentText = parent.text();
                        parent.setText((boost::wformat(L"%s\n%s")%parentText%block).str());
                    } else {
                        parent.setText(boost::algorithm::trim_left_copy(block));
                    }
                }
            } else {
                //! Create a regular paragraph
                Element p(parent, L"p");
                parent.append(p);
                p.setText(boost::algorithm::trim_left_copy(block));
            }
        }
    }

};

boost::shared_ptr<BlockParser> build_block_parser(Markdown* md_instance)
{
	boost::shared_ptr<BlockParser> parser(new BlockParser(md_instance));
    parser->blockprocessors.append("empty", boost::shared_ptr<BlockProcessor>(new EmptyBlockProcessor(parser.get())));
    parser->blockprocessors.append("indent", boost::shared_ptr<BlockProcessor>(new ListIndentProcessor(parser.get())));
    parser->blockprocessors.append("code", boost::shared_ptr<BlockProcessor>(new CodeBlockProcessor(parser.get())));
    parser->blockprocessors.append("hashheader", boost::shared_ptr<BlockProcessor>(new HashHeaderProcessor(parser.get())));
    parser->blockprocessors.append("setextheader", boost::shared_ptr<BlockProcessor>(new SetextHeaderProcessor(parser.get())));
    parser->blockprocessors.append("hr", boost::shared_ptr<BlockProcessor>(new HRProcessor(parser.get())));
    parser->blockprocessors.append("olist", boost::shared_ptr<BlockProcessor>(new OListProcessor(parser.get())));
    parser->blockprocessors.append("ulist", boost::shared_ptr<BlockProcessor>(new UListProcessor(parser.get())));
    parser->blockprocessors.append("quote", boost::shared_ptr<BlockProcessor>(new BlockQuoteProcessor(parser.get())));
    parser->blockprocessors.append("paragraph", boost::shared_ptr<BlockProcessor>(new ParagraphProcessor(parser.get())));
	return parser;
}

} // end of namespace markdown
