/*
 * PostProcessors.cpp
 *
 *  Created on: 2013/10/31
 *      Author: mugwort_rc
 */

#include "PostProcessors.h"

#include <QString>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "MarkdownCpp.h"

namespace markdown{

PostProcessor::PostProcessor(Markdown *markdown_instance) :
    markdown(markdown_instance)
{}

PostProcessor::~PostProcessor(void)
{}

/*!
 * Restore raw html to the document.
 */
class RawHtmlPostprocessor : public PostProcessor
{
public:
    RawHtmlPostprocessor(Markdown* markdown_instance) :
        PostProcessor(markdown_instance)
    {}

    /*!
     * Iterate over html stash and restore "safe" html.
     */
    std::wstring run(const std::wstring &text)
    {
        std::wstring result = text;
        for ( int i = 0; i < this->markdown->htmlStash.html_counter; ++i ) {
            HtmlStash::Item item = this->markdown->htmlStash.rawHtmlBlocks[i];
            std::wstring html = item.first;
            bool safe = item.second;
            if ( this->markdown->safeMode() != Markdown::default_mode && ! safe ) {
                if ( this->markdown->safeMode() == Markdown::escape_mode ) {
                    html = this->escape(html);
                } else if ( this->markdown->safeMode() == Markdown::remove_mode ) {
                    html = std::wstring();
                } else {
                    html = this->markdown->html_replacement_text();
                }
            }
            if ( this->isblocklevel(html) && ( safe || ! this->markdown->safeMode() ) ) {
                result = boost::algorithm::replace_all_copy(result, (boost::wformat(L"<p>%s</p>")%this->markdown->htmlStash.get_placeholder(i)).str(), html+L"\n");
            }
            result = boost::algorithm::replace_all_copy(result, this->markdown->htmlStash.get_placeholder(i), html);
        }
        return result;
    }

    /*!
     * Basic html escaping
     */
    std::wstring escape(const std::wstring& html)
    {
        std::wstring result = html;
        boost::algorithm::replace_all(result, L"&", L"&amp;");
        boost::algorithm::replace_all(result, L"<", L"&lt;");
        boost::algorithm::replace_all(result, L">", L"&gt;");
        boost::algorithm::replace_all(result, L"\"", L"&quot;");
        return result;
    }

    bool isblocklevel(const std::wstring& html)
    {
        boost::wsmatch m;
        if ( boost::regex_match(html, boost::wregex(L"^\\<\\/?([^ >]+)")) ) {
            wchar_t wch = m.str(1).at(0);
            // SPECIAL_CHARS: !, ?, @, %
            if ( SPECIAL_CHARS.find(wch) != SPECIAL_CHARS.end() ) {
                //! Comment, php etc...
                return true;
            }
            return util::isBlockLevel(m.str(1));
        }
        return false;
    }

private:
    static const std::set<wchar_t> SPECIAL_CHARS;

};

const std::set<wchar_t> RawHtmlPostprocessor::SPECIAL_CHARS = {L'!', L'?', L'@', L'%'};

/*!
 * Restore valid entities
 */
class AndSubstitutePostprocessor : public PostProcessor
{
public:
    AndSubstitutePostprocessor(Markdown* markdown_instance=nullptr) :
        PostProcessor(markdown_instance)
    {}

    std::wstring run(const std::wstring &text)
    {
        return boost::algorithm::replace_all_copy(text, util::AMP_SUBSTITUTE, L"&");
    }

};

/*!
 * Restore escaped chars
 */
class UnescapePostprocessor : public PostProcessor
{
public:
    UnescapePostprocessor(Markdown* markdown_instance=nullptr) :
        PostProcessor(markdown_instance),
        RE((boost::wformat(L"%s(\\d+)%s")%util::STX%util::ETX).str())
    {}

    std::wstring run(const std::wstring &text)
    {
        auto unescape = [](const boost::wsmatch& m) -> std::wstring {
            int i = QString::fromStdWString(m.str(1)).toInt();
            return QString(QChar(i)).toStdWString();
        };
        return boost::regex_replace(text, this->RE, unescape);
    }

private:
    boost::wregex RE;

};

OrderedDictPostProcessors build_postprocessors(Markdown* md_instance)
{
    OrderedDictPostProcessors postprocessors;
    postprocessors.append("raw_html", boost::shared_ptr<PostProcessor>(new RawHtmlPostprocessor(md_instance)));
    postprocessors.append("amp_substitute", boost::shared_ptr<PostProcessor>(new AndSubstitutePostprocessor()));
    postprocessors.append("unescape", boost::shared_ptr<PostProcessor>(new UnescapePostprocessor()));
    return postprocessors;
}

} // end of namespace markdown
