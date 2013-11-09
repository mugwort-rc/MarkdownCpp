/*
 * BlockParser.cpp
 *
 *  Created on: 2013/10/27
 *      Author: mugwort_rc
 */

#include "BlockParser.h"

#include <QString>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>

#include "MarkdownCpp.h"

namespace markdown{

BlockParser::BlockParser(Markdown *markdown) :
	markdown(markdown),
    blockprocessors(), root(ElementTree::InvalidElementTree)
{}

ElementTree BlockParser::parseDocument(const std::list<std::wstring> &lines)
{
    this->root = ElementTree(this->markdown->doc_tag());
    Element tmp(this->root);
    this->parseChunk(tmp, boost::algorithm::join(lines, L"\n"));
	return this->root;
}

void BlockParser::parseChunk(Element &parent, const std::wstring &text)
{
    std::list<std::wstring> buffer;
	boost::split_regex(buffer, text, boost::wregex(L"\n\n"));
	this->parseBlocks(parent, buffer);
}

void BlockParser::parseBlocks(Element &parent, std::list<std::wstring> &blocks)
{
	while ( blocks.size() > 0 ) {
        for (OrderedDictBlockProcessors::Ptr processor : this->blockprocessors.toList()) {
			if ( processor->test(parent, blocks.front()) ) {
				processor->run(parent, blocks);
				break;
			}
		}
	}
}

} // end of namespace markdown
