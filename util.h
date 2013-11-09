/*
 * util.h
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <boost/regex.hpp>

namespace markdown{

class util
{
public:

static boost::wregex BLOCK_LEVEL_ELEMENTS;

//! Placeholders
static const std::wstring STX;  //!< Use STX ("Start of text") for start-of-placeholder
static const std::wstring ETX;  //!< Use ETX ("End of text") for end-of-placeholder
static const std::wstring INLINE_PLACEHOLDER_PREFIX;
static const std::wstring INLINE_PLACEHOLDER;
static boost::wregex      INLINE_PLACEHOLDER_RE;
static const std::wstring AMP_SUBSTITUTE;

static bool isBlockLevel(const std::wstring& tag);

private:
	util(void);
	util(const util&);
	~util(void);
	util& operator=(const util&);

};

class HtmlStash
{
public:
    typedef std::pair<std::wstring, bool> Item;
	typedef std::vector<Item> Items;

public:
	HtmlStash(void);

    std::wstring store(const std::wstring& html, bool safe=false);
	void reset(void);

    std::wstring get_placeholder(int key);

public:
    int   html_counter;
    Items rawHtmlBlocks;

};

} // end of namespace markdown

#endif /* UTIL_H_ */
