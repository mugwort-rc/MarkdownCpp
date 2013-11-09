/*
 * util.cpp
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */
#include "util.h"

#include <boost/format.hpp>

namespace markdown{

boost::wregex util::BLOCK_LEVEL_ELEMENTS(L"^(p|div|h[1-6]|blockquote|pre|table|dl|ol|ul"
                                         L"|script|noscript|form|fieldset|iframe|math"
                                         L"|hr|hr/|style|li|dt|dd|thead|tbody"
                                         L"|tr|th|td|section|footer|header|group|figure"
                                         L"|figcaption|aside|article|canvas|output"
                                         L"|progress|video)$", boost::regbase::icase);

const std::wstring util::STX = L"\u0002";
const std::wstring util::ETX = L"\u0003";
const std::wstring util::INLINE_PLACEHOLDER_PREFIX = util::STX+L"klzzwxh:";
const std::wstring util::INLINE_PLACEHOLDER = util::INLINE_PLACEHOLDER_PREFIX + L"%s" + util::ETX;
boost::wregex      util::INLINE_PLACEHOLDER_RE((boost::wformat(util::INLINE_PLACEHOLDER)%L"([0-9]{4})").str());
const std::wstring util::AMP_SUBSTITUTE = util::STX+L"amp"+util::ETX;

bool util::isBlockLevel(const std::wstring& tag)
{
	return boost::regex_match(tag, util::BLOCK_LEVEL_ELEMENTS);
}

HtmlStash::HtmlStash() :
    html_counter(0), rawHtmlBlocks()
{}

std::wstring HtmlStash::store(const std::wstring& html, bool safe)
{
    this->rawHtmlBlocks.push_back(Item(html, safe));
    std::wstring placeholder = this->get_placeholder(this->html_counter);
    this->html_counter += 1;
	return placeholder;
}

void HtmlStash::reset(void)
{
    this->html_counter = 0;
    this->rawHtmlBlocks = Items();
}

std::wstring HtmlStash::get_placeholder(int key)
{
	return (boost::wformat(L"%swzxhzdk:%d%s") % util::STX % key % util::ETX).str();
}

} // end of namespace markdown
