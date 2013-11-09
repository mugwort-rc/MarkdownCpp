/*
 * InlinePatterns.cpp
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#include "InlinePatterns.h"

#include <QUrl>
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
#include <QTextDocument>
#endif

#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "MarkdownCpp.h"

namespace markdown{

const std::wstring NOBRACKET(L"[^\\]\\[]*");

const std::wstring BRK = ( L"\\[("
        + (NOBRACKET + L"(\\[") + (NOBRACKET + L"(\\[") + (NOBRACKET + L"(\\[") + (NOBRACKET + L"(\\[") + (NOBRACKET + L"(\\[") + (NOBRACKET + L"(\\[")
        + (NOBRACKET+ L"\\])*") + (NOBRACKET+ L"\\])*") + (NOBRACKET+ L"\\])*") + (NOBRACKET+ L"\\])*") + (NOBRACKET+ L"\\])*") + (NOBRACKET+ L"\\])*")
        + NOBRACKET + L")\\]" );
const std::wstring NOIMG = L"(?<!\\!)";

const std::wstring BACKTICK_RE = L"(?<!\\\\)(`+)(.+?)(?<!`)\\2(?!`)";  //!< `e=f()` or ``e=f("`")``
const std::wstring ESCAPE_RE = L"\\\\(.)";                             //!< \<
const std::wstring EMPHASIS_RE = L"(\\*)([^\\*]+)\\2";                 //!< *emphasis*
const std::wstring STRONG_RE = L"(\\*{2}|_{2})(.+?)\\2";               //!< **strong**
const std::wstring STRONG_EM_RE = L"(\\*{3}|_{3})(.+?)\\2";            //!< ***strong***
const std::wstring SMART_EMPHASIS_RE = L"(?<!\\w)(_)(?!_)(.+?)(?<!_)\\2(?!\\w)";  //!< _smart_emphasis_
const std::wstring EMPHASIS_2_RE = L"(_)(.+?)\\2";                                //!< _emphasis_
const std::wstring LINK_RE = NOIMG + BRK + L"\\(\\s*(<.*?>|((?:(?:\\(.*?\\))|[^\\(\\)]))*?)\\s*((['\"])(.*?)\\12\\s*)?\\)";
//!< [text](url) or [text](<url>) or [text](url "title")

const std::wstring IMAGE_LINK_RE = L"\\!" + BRK + L"\\s*\\((<.*?>|([^\\)]*))\\)";
//!< ![alttxt](http://x.com/) or ![alttxt](<http://x.com/>)
const std::wstring REFERENCE_RE = NOIMG + BRK+ L"\\s?\\[([^\\]]*)\\]";           //!< [Google][3]
const std::wstring SHORT_REF_RE = NOIMG + L"\\[([^\\]]+)\\]";                    //!< [Google]
const std::wstring IMAGE_REFERENCE_RE = L"\\!" + BRK + L"\\s?\\[([^\\]]*)\\]";   //!< ![alt text][2]
const std::wstring NOT_STRONG_RE = L"((^| )(\\*|_)( |$))";                       //!< stand-alone * or _
const std::wstring AUTOLINK_RE = L"<((?:[Ff]|[Hh][Tt])[Tt][Pp][Ss]?://[^>]*)>";  //!<  <http://www.123.com>
const std::wstring AUTOMAIL_RE = L"<([^> \\!]*@[^> ]*)>";                        //!< <me@example.com>

const std::wstring HTML_RE = L"(<([a-zA-Z/][^>]*?|\\!--.*?--)>)";          //!< <...>
const std::wstring ENTITY_RE = L"(&[\\#a-zA-Z0-9]*;)";                           //!< &amp;
const std::wstring LINE_BREAK_RE = L"  \\n";                                     //!< two spaces at end of line

//! Remove quotes from around a string.
std::wstring dequote(const std::wstring &string)
{
    if ( ( string.at(0) == L'"' && string.at(string.size()-1) == L'"' ) || ( string.at(0) == L'\'' && string.at(string.size()-1) == L'\'' ) ) {
        return string.substr(1, string.size()-2);
    } else {
        return string;
    }
}

const boost::wregex ATTR_RE(L"\\{@([^\\}]*)=([^\\}]*)\\}");

std::wstring handleAttributes(const std::wstring &text, Element &parent)
{
    return boost::regex_replace(text, ATTR_RE, [&](const boost::wsmatch &m) -> std::wstring {
        parent.setAttribute(m.str(1), boost::algorithm::replace_all_copy(m.str(2), L"\n", L" "));
        return m.str(0);
    });
}

//! Set values of an element based on attribute definitions ({@id=123}).

Pattern::Pattern(const std::wstring &pattern, Markdown *markdown_instance) :
    pattern(pattern), compiled_re((boost::wformat(L"^(.*?)%s(.*?)$")%pattern).str(), boost::regex_constants::mod_s),
    //! Api for Markdown to pass safe_mode into instance
    safe_mode(false), markdown(markdown_instance)
{}

std::wstring Pattern::unescape(const std::wstring &text)
{
    TreeProcessor::StashNodes stash;
    if ( this->markdown->treeprocessors.exists("inline") ) {
        stash = this->markdown->treeprocessors["inline"]->stashed_nodes;
    } else {
        return text;
    }
    auto get_stash = [&](const boost::wsmatch &m) -> std::wstring {
        std::wstring id = m.str(1);
        if ( stash.find(id) != stash.end() ) {
            TreeProcessor::NodeItem value = stash[id];
            boost::optional<std::wstring> str = value.get<0>();
            boost::optional<Element> node = value.get<1>();
            if ( str ) {
                return *str;
            }
            return (*node).getTextContent();
        }
        return std::wstring();
    };
    return boost::regex_replace(text, util::INLINE_PLACEHOLDER_RE, get_stash);
}

/*!
 * Return a simple text of group(2) of a Pattern.
 */
class SimpleTextPattern : public Pattern
{
public:
    SimpleTextPattern(const std::wstring &pattern, Markdown *md=nullptr) :
        Pattern(pattern, md)
    {}

    boost::optional<std::wstring> handleMatch(const boost::wsmatch& m)
    {
        std::wstring text = m.str(2);
        if ( text == util::INLINE_PLACEHOLDER_PREFIX ) {
            return boost::none;
        }
        return text;
    }

    std::wstring type(void) const
    { return L"SimpleTextPattern"; }

};

/*!
 * Return an escaped character.
 */
class EscapePattern : public Pattern
{
public:
    EscapePattern(const std::wstring &pattern, Markdown *md) :
        Pattern(pattern, md)
    {}

    boost::optional<std::wstring> handleMatch(const boost::wsmatch &m)
    {
        std::wstring text = m.str(2);
        if ( text.size() > 1 ) {
            return (boost::wformat(L"\\%s")%text).str();
        }
        wchar_t ch = text.at(0);
        if ( this->markdown->ESCAPED_CHARS.find(ch) != this->markdown->ESCAPED_CHARS.end() ) {
            QString str = QString::fromStdWString(text);
            QChar c = str.at(0);
            return (boost::wformat(L"%s%s%s")%util::STX%c.unicode()%util::ETX).str();
        } else {
            return (boost::wformat(L"\\%s")%text).str();
        }
    }

    std::wstring type(void) const
    { return L"EscapePattern"; }

};

/*!
 * Return element of type `tag` with a text attribute of group(3)
 * of a Pattern.
 */
class SimpleTagPattern : public Pattern
{
public:
    SimpleTagPattern(const std::wstring &pattern, const std::wstring &tag, Markdown *md=nullptr) :
        Pattern(pattern, md),
        tag(tag)
    {}
    virtual ~SimpleTagPattern()
    {}

    virtual Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, this->tag);
        el.setText(m.str(3));
        return el;
    }

    virtual std::wstring type(void) const
    { return L"SimpleTagPattern"; }

protected:
    std::wstring tag;

};

/*!
 * Return an element of type `tag` with no children.
 */
class SubstituteTagPattern : public SimpleTagPattern
{
public:
    SubstituteTagPattern(const std::wstring &pattern, const std::wstring &tag, Markdown *md=nullptr) :
        SimpleTagPattern(pattern, tag, md)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch&)
    {
        return Element(doc, this->tag);
    }

    std::wstring type(void) const
    { return L"SubstituteTagPattern"; }

};

/*!
 * Return a `<code>` element containing the matching text.
 */
class BacktickPattern : public Pattern
{
public:
    BacktickPattern(const std::wstring &pattern, Markdown *md=nullptr) :
        Pattern(pattern, md),
        tag(L"code")
    {}

    virtual Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, this->tag);
        el.setText(boost::algorithm::trim_copy(m.str(3)));
        return el;
    }

    virtual std::wstring type(void) const
    { return L"BacktickPattern"; }

private:
    std::wstring tag;

};

/*!
 * Return a Element element nested in tag2 nested in tag1.
 *
 * Useful for strong emphasis etc.
 *
 */
class DoubleTagPattern : public SimpleTagPattern
{
public:
    DoubleTagPattern(const std::wstring &pattern, const std::wstring &tag, Markdown *md=nullptr) :
        SimpleTagPattern(pattern, tag, md)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        std::vector<std::wstring> tags;
        boost::algorithm::split(tags, this->tag, boost::is_any_of(L","));
        Element el1(doc, tags.at(0));
        Element el2(doc, tags.at(1));
        el2.setText(m.str(3));
        el1.append(el2);
        return el1;
    }

    std::wstring type(void) const
    { return L"DoubleTagPattern"; }

};

class HtmlPattern : public Pattern
{
public:
    HtmlPattern(const std::wstring &pattern, Markdown *md) :
        Pattern(pattern, md)
    {}

    boost::optional<std::wstring> handleMatch(const boost::wsmatch &m)
    {
        std::wstring rawHtml = this->unescape(m.str(2));
        return this->markdown->htmlStash.store(rawHtml);
    }

    std::wstring type(void) const
    { return L"HtmlPattern"; }

    std::wstring unescape(const std::wstring &text)
    {
        TreeProcessor::StashNodes stash;
        if ( this->markdown->treeprocessors.exists("inline") ) {
            stash = this->markdown->treeprocessors["inline"]->stashed_nodes;
        } else {
            return text;
        }
        auto get_stash = [&](const boost::wsmatch &m) -> std::wstring {
            std::wstring id = m.str(1);
            if ( stash.find(id) != stash.end() ) {
                TreeProcessor::NodeItem value = stash[id];
                boost::optional<std::wstring> str = value.get<0>();
                boost::optional<Element> node = value.get<1>();
                if ( str ) {
                    return L"\\" + *str;
                } else {
                    return this->markdown->serializer(*node);
                }
            }
            return std::wstring();
        };
        return boost::regex_replace(text, util::INLINE_PLACEHOLDER_RE, get_stash);
    }

};

/*!
 * Return a link element from the given match.
 */
class LinkPattern : public Pattern
{
public:
    LinkPattern(const std::wstring& pattern, Markdown* md) :
        Pattern(pattern, md)
    {}
    virtual ~LinkPattern(void)
    {}

    virtual Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, L"a");
        el.setText(m.str(2));
        std::wstring title = m.str(13);
        std::wstring href  = m.str(9);

        if ( ! href.empty() ) {
            if ( href.at(0) == L'<' ) {
                href = href.substr(1, href.size()-2);
            }
            el.setAttribute(L"href", this->sanitize_url(this->unescape(boost::algorithm::trim_copy(href))));
        } else {
            el.setAttribute(L"href", std::wstring());
        }

        if ( ! title.empty() ) {
            title = dequote(this->unescape(title));
            el.setAttribute(L"title", title);
        }

        return el;
    }

    /*!
     * Sanitize a url against xss attacks in "safe_mode".
     *
     * Rather than specifically blacklisting `javascript:alert("XSS")` and all
     * its aliases (see <http://ha.ckers.org/xss.html>), we whitelist known
     * safe url formats. Most urls contain a network location, however some
     * are known not to (i.e.: mailto links). Script urls do not contain a
     * location. Additionally, for `javascript:...`, the scheme would be
     * "javascript" but some aliases will appear to `urlparse()` to have no
     * scheme. On top of that relative links (i.e.: "foo/bar.html") have no
     * scheme. Therefore we must check "path", "parameters", "query" and
     * "fragment" for any literal colons. We don't check "scheme" for colons
     * because it *should* never have any and "netloc" must allow the form:
     * `username:password@host:port`.
     *
     */
    std::wstring sanitize_url(const std::wstring &url)
    {
        std::wstring result = url;
        boost::algorithm::replace_all(result, L" ", L"%20");
        if ( ! this->markdown->safeMode() ) {
            //! Return immediately bipassing parsing.
            return result;
        }
        QUrl qurl(QString::fromStdWString(result));
        std::set<std::wstring> locless_schemes = {L"", L"mailto", L"news"};
        std::set<std::wstring> allowed_schemes = {L"", L"mailto", L"news", L"http", L"https", L"ftp", L"ftps"};
        if ( allowed_schemes.find(qurl.scheme().toStdWString()) == allowed_schemes.end() ) {
            //! Not a known (allowed) scheme. Not safe.
            return std::wstring();
        }
        if ( qurl.host().isEmpty() && locless_schemes.find(qurl.scheme().toStdWString()) == locless_schemes.end() ) {
            //! This should not happen. Treat as suspect.
            return std::wstring();
        }
        //! A colon in "path", "parameters", "query" or "fragment" is suspect.
        if ( qurl.path().indexOf(":") != -1 || qurl.fragment().indexOf(":") != -1 ) {
            return std::wstring();
        }
        for ( const QPair<QString, QString>& item : qurl.queryItems() ) {
            if ( item.first.indexOf(":") != -1 || item.second.indexOf(":") != -1 ) {
                return std::wstring();
            }
        }
        //! Url passes all tests. Return url as-is.
        return qurl.toString().toStdWString();
    }

    virtual std::wstring type(void) const
    { return L"LinkPattern"; }

};

/*!
 * Return a img element from the given match.
 */
class ImagePattern : public LinkPattern
{
public:
    ImagePattern(const std::wstring &pattern, Markdown *md) :
        LinkPattern(pattern, md)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, L"img");
        std::wstring src_parts_source = m.str(9);
        std::vector<std::wstring> src_parts;
        boost::algorithm::split(src_parts, src_parts_source, boost::is_any_of(L" "));
        if ( ! src_parts.empty() ) {
            std::wstring src = src_parts.at(0);
            if ( src.at(0) == L'<' && src.at(src.size()-1) == L'>' ) {
                src = src.substr(1, src.size()-2);
            }
            el.setAttribute(L"src", src);
        } else {
            el.setAttribute(L"src", std::wstring());
        }
        if ( src_parts.size() > 1 ) {
            src_parts.erase(src_parts.begin());
            el.setAttribute(L"title", dequote(this->unescape(boost::algorithm::join(src_parts, L" "))));
        }

        std::wstring truealt;
        if ( this->markdown->enable_attributes() ) {
            truealt = handleAttributes(m.str(2), el);
        } else {
            truealt = m.str(2);
        }

        el.setAttribute(L"alt", this->unescape(truealt));
        return el;
    }

    std::wstring type(void) const
    { return L"ImagePattern"; }

};

class ReferencePattern : public LinkPattern
{
public:
    ReferencePattern(const std::wstring& pattern, Markdown* md) :
        LinkPattern(pattern, md),
        NEWLINE_CLEANUP_RE(L"(?m)[ ]?\\n")
    {}
    virtual ~ReferencePattern(void)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        std::wstring id;
        if ( m.size() > 8 ) {
            id = m.str(9);
        } else {
            //! if we got something like "[Google][]" or "[Goggle]"
            //! we'll use "google" as the id
            id = m.str(2);
        }
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);

        //! Clean up linebreaks in id
        id = boost::regex_replace(id, this->NEWLINE_CLEANUP_RE, L" ");
        if ( this->markdown->references.find(id) == this->markdown->references.end() ) {
            return Element::InvalidElement;
        }
        Markdown::ReferenceItem item = this->markdown->references.at(id);

        std::wstring text = m.str(2);
        return this->makeTag(doc, item.first, item.second, text);
    }

    virtual Element makeTag(const ElementTree& doc, const std::wstring& href, const std::wstring& title, const std::wstring& text)
    {
        Element el(doc, L"a");

        el.setAttribute(L"href", this->sanitize_url(href));
        if ( ! title.empty() ) {
            el.setAttribute(L"title", title);
        }

        el.setText(text);
        return el;
    }

    virtual std::wstring type(void) const
    { return L"ReferencePattern"; }

private:
    boost::wregex NEWLINE_CLEANUP_RE;

};

/*!
 * Match to a stored reference and return img element.
 */
class ImageReferencePattern : public ReferencePattern
{
public:
    ImageReferencePattern(const std::wstring& pattern, Markdown* md) :
        ReferencePattern(pattern, md)
    {}

    Element makeTag(const ElementTree& doc, const std::wstring& href, const std::wstring& title, const std::wstring& text)
    {
        Element el(doc, L"img");

        el.setAttribute(L"src", this->sanitize_url(href));
        if ( ! title.empty() ) {
            el.setAttribute(L"title", title);
        }

        std::wstring text_ = text;
        if ( this->markdown->enable_attributes() ) {
            text_ = handleAttributes(text, el);
        }

        el.setAttribute(L"alt", this->unescape(text_));
        return el;
    }

    virtual std::wstring type(void) const
    { return L"ImageReferencePattern"; }

};

/*!
 * Return a link Element given an autolink (`<http://example/com>`).
 */
class AutolinkPattern : public Pattern
{
public:
    AutolinkPattern(const std::wstring& pattern, Markdown* md) :
        Pattern(pattern, md)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, L"a");
        el.setAttribute(L"href", this->unescape(m.str(2)));
        el.setText(m.str(2));
        return el;
    }

    std::wstring type(void) const
    { return L"AutolinkPattern"; }

};

/*!
 * Return a mailto link Element given an automail link (`<foo@example.com>`).
 */
class AutomailPattern : public Pattern
{
public:
    AutomailPattern(const std::wstring& pattern, Markdown* md) :
        Pattern(pattern, md)
    {}

    Element handleMatch(const ElementTree& doc, const boost::wsmatch& m)
    {
        Element el(doc, L"a");
        std::wstring email = this->unescape(m.str(2));
        if ( email.substr(0, 7) == L"mailto:" ) {
            email = email.substr(7, email.size()-7);
        }

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        QString qstr = QString::fromStdWString(email);
        email = Qt::escape(qstr).toStdWString();
#else
        QString qstr = QString::fromStdWString(email);
        email = qstr.toHtmlEscaped().toStdWString();
#endif

        std::wstring mailto = L"mailto:"+email;

        QString tmp = QString::fromStdWString(mailto);
        QString buff;
        for ( int i = 0; i < tmp.size(); ++i ) {
            QChar ch = tmp.at(i);
            buff.append(QString("#%1%2;").arg(QString::fromStdWString(util::AMP_SUBSTITUTE), QString::number(ch.unicode())));
        }
        mailto = buff.toStdWString();

        el.setAttribute(L"href", this->unescape(mailto));
        el.setText(email);
        return el;
    }

    std::wstring type(void) const
    { return L"AutomailPattern"; }

};

OrderedDictPatterns build_inlinepatterns(Markdown* md_instance)
{
    OrderedDictPatterns inlinePatterns;
    inlinePatterns.append("backtick", boost::shared_ptr<Pattern>(new BacktickPattern(BACKTICK_RE)));
    inlinePatterns.append("escape", boost::shared_ptr<Pattern>(new EscapePattern(ESCAPE_RE, md_instance)));
    inlinePatterns.append("reference", boost::shared_ptr<Pattern>(new ReferencePattern(REFERENCE_RE, md_instance)));
    inlinePatterns.append("link", boost::shared_ptr<Pattern>(new LinkPattern(LINK_RE, md_instance)));
    inlinePatterns.append("image_link", boost::shared_ptr<Pattern>(new ImagePattern(IMAGE_LINK_RE, md_instance)));
    inlinePatterns.append("image_reference", boost::shared_ptr<Pattern>(new ImageReferencePattern(IMAGE_REFERENCE_RE, md_instance)));
    inlinePatterns.append("short_reference", boost::shared_ptr<Pattern>(new ReferencePattern(SHORT_REF_RE, md_instance)));
    inlinePatterns.append("autolink", boost::shared_ptr<Pattern>(new AutolinkPattern(AUTOLINK_RE, md_instance)));
    inlinePatterns.append("automail", boost::shared_ptr<Pattern>(new AutomailPattern(AUTOMAIL_RE, md_instance)));
    inlinePatterns.append("linebreak", boost::shared_ptr<Pattern>(new SubstituteTagPattern(LINE_BREAK_RE, L"br")));
    if ( md_instance->safeMode() != Markdown::escape_mode ) {
        inlinePatterns.append("html", boost::shared_ptr<Pattern>(new HtmlPattern(HTML_RE, md_instance)));
    }
    inlinePatterns.append("entity", boost::shared_ptr<Pattern>(new HtmlPattern(ENTITY_RE, md_instance)));
    inlinePatterns.append("not_strong", boost::shared_ptr<Pattern>(new SimpleTextPattern(NOT_STRONG_RE)));
    inlinePatterns.append("strong_em", boost::shared_ptr<Pattern>(new DoubleTagPattern(STRONG_EM_RE, L"strong,em")));
    inlinePatterns.append("strong", boost::shared_ptr<Pattern>(new SimpleTagPattern(STRONG_RE, L"strong")));
    inlinePatterns.append("emphasis", boost::shared_ptr<Pattern>(new SimpleTagPattern(EMPHASIS_RE, L"em")));
    if ( md_instance->smart_emphasis() ) {
        inlinePatterns.append("emphasis2", boost::shared_ptr<Pattern>(new SimpleTagPattern(SMART_EMPHASIS_RE, L"em")));
    } else {
        inlinePatterns.append("emphasis2", boost::shared_ptr<Pattern>(new SimpleTagPattern(EMPHASIS_2_RE, L"em")));
    }
    return inlinePatterns;
}

} // end of namespace markdown
