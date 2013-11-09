/*
 * MarkdownCpp.cpp
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */

#include "MarkdownCpp.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>

#include "PreProcessors.h"
#include "BlockParser.h"
#include "BlockProcessors.h"
#include "Serializers.h"

namespace markdown{

Markdown::Markdown(void) :
	_doc_tag(L"div"),
    _html_replacement_text(L"[HTML_REMOVED]"), _tab_length(4), _enable_attributes(true), _smart_emphasis(true), _lazy_ol(true),
	_safeMode(default_mode),
    //todo
    stripTopLevelTags(true),

    ESCAPED_CHARS({L'\\', L'`', L'*', L'_', L'{', L'}', L'[', L']',
                   L'(', L')', L'>', L'#', L'+', L'-', L'.', L'!'}),

    //! Initalize in build_parser()
    preprocessors(),
    parser(),
    inlinePatterns(),
    treeprocessors(),
    postprocessors(),

    references(),
    htmlStash(),

    serializer()
{
    this->build_parser();
    this->set_output_format();
    this->reset();
}

Markdown::Markdown(const Extensions &extensions) :
    Markdown()
{
    this->registerExtensions(extensions);
    this->reset();
}

Markdown& Markdown::build_parser(void)
{
	this->preprocessors = build_preprocessors(this);
    this->parser = build_block_parser(this);
    this->inlinePatterns = build_inlinepatterns(this);
    this->treeprocessors = build_treeprocessors(this);
    this->postprocessors = build_postprocessors(this);
	return *this;
}

Markdown& Markdown::registerExtensions(const Extensions &extensions/*, configs*/)
{
    for ( const Extension::Ptr& ext : extensions ) {
        ext->extendMarkdown(this);
    }
	return *this;
}

Markdown& Markdown::registerExtension(const Extension::Ptr&)
{
	return *this;
}

Markdown& Markdown::reset(void)
{
    this->htmlStash.reset();
    this->references.clear();
    //! TODO: extension
	return *this;
}

Markdown& Markdown::set_output_format(const output_formats format)
{
    if ( format == html || format == html4 || format == html5 ) {
        this->serializer = to_html_string;
    } else if ( format == xhtml || format == xhtml1 || format == xhtml5 ) {
        this->serializer = to_xhtml_string;
    }
	return *this;
}

std::wstring Markdown::convert(const std::wstring& source)
{
    //! Fixup the source text
    if ( boost::algorithm::trim_copy(source).empty() ) {
        return std::wstring();  //!< a blank unicode string
	}

    std::wstring source_ = source;

    //! Split into lines and run the line preprocessors.
    std::list<std::wstring> lines;
    boost::algorithm::split(lines, source_, boost::is_any_of(L"\n"));
    for ( OrderedDictProcessors::Ptr pre : this->preprocessors.toList() ) {
        lines = pre->run(lines);
    }

    //! Parse the high-level elements.
    ElementTree doc = this->parser->parseDocument(lines);
    Element root(doc);

    //! Run the tree-processors
    for ( OrderedDictTreeProcessors::Ptr tree : this->treeprocessors.toList() ) {
        Element newRoot = tree->run(root);
        if ( ! newRoot.isNull() ) {
            root = newRoot;
        }
    }

    //! Serialize _properly_.  Strip top-level tags.
    std::wstring output;
    output = this->serializer(root);
    if ( this->stripTopLevelTags ) {
        std::wstring::size_type begin = output.find((boost::wformat(L"<%s>")%this->doc_tag()).str());
        std::wstring::size_type end   = output.rfind((boost::wformat(L"</%s>")%this->doc_tag()).str());
        if ( begin != std::wstring::npos && end != std::wstring::npos ) {
            output = output.substr(begin+this->doc_tag().size()+2, end-this->doc_tag().size()-2);
        } else {
            return std::wstring();
        }
    }

    //! Run the text post-processors
    for ( OrderedDictPostProcessors::Ptr post : this->postprocessors.toList() ) {
        output = post->run(output);
    }

    return boost::algorithm::trim_copy(output);
}

Markdown& Markdown::convertFile(/*input, output, encoding=L"utf-8"*/)
{
	return *this;
}

} // end of namespace markdown
