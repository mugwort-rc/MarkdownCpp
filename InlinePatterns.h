/*
 * InlinePatterns.h
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#ifndef INLINEPATTERNS_H_
#define INLINEPATTERNS_H_

#include <boost/optional.hpp>
#include <boost/regex.hpp>

#include "ElementTree.h"
#include "odict.h"

namespace markdown{

/*!
 *  INLINE PATTERNS
 * =============================================================================
 *
 * Inline patterns such as *emphasis* are handled by means of auxiliary
 * objects, one per pattern.  Pattern objects must be instances of classes
 * that extend markdown.Pattern.  Each pattern object uses a single regular
 * expression and needs support the following methods:
 *
 *     pattern.getCompiledRegExp() # returns a regular expression
 *
 *     pattern.handleMatch(m) # takes a match object and returns
 *                            # an ElementTree element or just plain text
 *
 * All of python markdown's built-in patterns subclass from Pattern,
 * but you can add additional patterns that don't.
 *
 * Also note that all the regular expressions used by inline must
 * capture the whole block.  For this reason, they all start with
 * '^(.*)' and end with '(.*)!'.  In case with built-in expression
 * Pattern takes care of adding the "^(.*)" and "(.*)!".
 *
 * Finally, the order in which regular expressions are applied is very
 * important - e.g. if we first replace http://.../ links with <a> tags
 * and _then_ try to replace inline html, we would end up with a mess.
 * So, we apply the expressions in the following order:
 *
 * * escape and backticks have to go before everything else, so
 *   that we can preempt any markdown patterns by escaping them.
 *
 * * then we handle auto-links (must be done before inline html)
 *
 * * then we handle inline HTML.  At this point we will simply
 *   replace all inline HTML strings with a placeholder and add
 *   the actual HTML to a hash.
 *
 * * then inline images (must be done before links)
 *
 * * then bracketed links, first regular then reference-style
 *
 * * finally we apply strong and emphasis
 *
 */

std::wstring handleAttributes(const std::wstring &text, Element &parent);

class Markdown;  //!< forward declaration

/*!
 * Base class that inline patterns subclass.
 */
class Pattern
{
public:
    /*!
     * Create an instant of an inline pattern.
     *
     * Keyword arguments:
     *
     * * pattern: A regular expression that matches a pattern
     *
     */
    Pattern(const std::wstring& pattern, Markdown* markdown_instance=nullptr);

    /*!
     * Return a compiled regular expression.
     */
    boost::wregex getCompiledRegExp(void) const
    { return this->compiled_re; }

    /*!
     * Return a ElementTree element from the given match.
     *
     * Subclasses should override this method.
     *
     * Keyword arguments:
     *
     * * m: A re match object containing a match of the pattern.
     *
     */
    virtual boost::optional<std::wstring> handleMatch(const boost::wsmatch&)
    { return boost::none; }
    virtual Element handleMatch(const ElementTree&, const boost::wsmatch&)
    { return Element::InvalidElement; }

    /*!
     * Return class name, to define pattern type
     */
    virtual std::wstring type(void) const = 0;

    /*!
     * Return unescaped text given text with an inline placeholder.
     */
    virtual std::wstring unescape(const std::wstring& text);

protected:
    std::wstring pattern;
    boost::wregex compiled_re;
    bool safe_mode;
    Markdown* markdown;


};

typedef OrderedDict<Pattern> OrderedDictPatterns;

OrderedDictPatterns build_inlinepatterns(Markdown* md_instance);

} // end of namespace markdown

#endif // INLINEPATTERNS_H_
