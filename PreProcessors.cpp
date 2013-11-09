/*
 * PreProcessors.cpp
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */

#include "PreProcessors.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>

#include "MarkdownCpp.h"
#include "util.h"

namespace markdown{

class PreProcessor : public Processor
{
public:
	PreProcessor(Markdown* markdown_instance) :
		Processor(markdown_instance)
	{}
	virtual ~PreProcessor(void)
	{}
};

/*!
 * Normalize whitespace for consistant parsing.
 */
class NormalizeWhitespace : public PreProcessor
{
public:
	NormalizeWhitespace(Markdown* markdown_instance) :
		PreProcessor(markdown_instance)
	{}
	virtual ~NormalizeWhitespace(void)
	{}

    std::list<std::wstring> run(const std::list<std::wstring>& lines)
	{
        std::wstring source = boost::algorithm::join(lines, L"\n");
        boost::algorithm::replace_all(source, util::STX, std::wstring());
        boost::algorithm::replace_all(source, util::ETX, std::wstring());
		boost::algorithm::replace_all(source, L"\r\n", L"\n");
		boost::algorithm::replace_all(source, L"\r", L"\n");
		source += L"\n\n";
        boost::algorithm::replace_all(source, L"\t", std::wstring(this->markdown->tab_length(), L' '));
		source = boost::regex_replace(source, boost::wregex(L"(?<=\n) +\n"), L"\n");
        std::list<std::wstring> result;
		boost::algorithm::split(result, source, boost::is_any_of(L"\n"));
		return result;
	}

};

/*!
 * Remove html blocks from the text and store them for later retrieval.
 */
class HtmlBlockProcessor : public PreProcessor
{
public:
    typedef std::map<std::wstring, std::wstring> Attributes;

public:
	HtmlBlockProcessor(Markdown* markdown_instance) :
		PreProcessor(markdown_instance),
		right_tag_patterns({L"</%s>", L"/%s>"}),
        attrs_pattern(L"\\s+(?<attr>[^>\"'/= ]+)=(?<q>['\"])(?<value>.*?)\\g{q}|\\s+(?<attr1>[^>\"'/= ]+)=(?<value1>[^> ]+)|\\s+(?<attr2>[^>\"'/= ]+)"),
        left_tag_pattern((boost::wformat(L"^<(?<tag>[^> ]+)(?<attrs>(%s)*)\\s*\\/?>?")%this->attrs_pattern).str()),
        attrs_re(this->attrs_pattern),
        left_tag_re(this->left_tag_pattern),
		markdown_in_raw(false)
	{}
	virtual ~HtmlBlockProcessor(void)
	{}

    std::list<std::wstring> run(const std::list<std::wstring>& lines)
	{
        std::wstring text = boost::algorithm::join(lines, L"\n");
        std::list<std::wstring> new_blocks;
        std::list<std::wstring> texts;
		// python str.rsplit()
		while ( true ) {
            std::wstring::size_type i = text.rfind(L"\n\n");
            if ( i == std::wstring::npos ) {
				texts.push_front(text);
				break;
			}
            std::wstring::size_type count = text.size() - (i + 2);
			texts.push_front(text.substr(i+2, count));
			text = text.substr(0, i);
		}
        std::list<std::wstring> items;
        std::wstring left_tag;
        std::wstring right_tag;
		Attributes attrs;
		int left_index;
        std::size_t data_index;
		bool in_tag = false;  //!< flag
		while ( texts.size() > 0 ) {
            std::wstring block = texts.front();
			if ( block[0] == L'\n' ) {
				block = block.substr(1, block.size()-1);
			}
			texts.pop_front();

			if ( block[0] == L'\n' ) {
				block = block.substr(1, block.size()-1);
			}

			if ( ! in_tag ) {
				if ( block[0] == L'<' && boost::algorithm::trim_copy(block).size() > 1 ) {
					if ( block[1] == L'!' ) {
						//! is a comment block
						left_tag   = L"--";
						left_index = 2;
						attrs      = Attributes();
					} else {
                        boost::tuples::tuple<std::wstring, int, Attributes> result = this->get_left_tag(block);
                        left_tag = result.get<0>();
                        left_index = result.get<1>();
                        attrs = result.get<2>();
					}
                    boost::tuples::tuple<std::wstring, std::size_t> result = this->get_right_tag(left_tag, left_index, block);
                    right_tag = result.get<0>();
                    data_index = result.get<1>();

					//! keep checking conditions below and maybe just append

					if ( data_index < block.size() && ( util::isBlockLevel(left_tag) || left_tag == L"--" ) ) {
						texts.push_front(block.substr(data_index, block.size()-data_index));
						block = block.substr(0, data_index);
					}

					wchar_t ch = block[1];
					if ( ! util::isBlockLevel(left_tag) || ( ch == L'!' || ch == L'?' || ch == L'@' || ch == L'%' ) ) {
						new_blocks.push_back(block);
						continue;
					}

					if ( this->is_oneliner(left_tag) ) {
						new_blocks.push_back(boost::algorithm::trim_copy(block));
						continue;
					}

                    std::wstring buff = boost::algorithm::trim_right_copy(block);
					if ( buff[buff.size()-1] == L'>' && this->equal_tags(left_tag, right_tag) ) {
                        if ( this->markdown_in_raw && attrs.find(L"markdown") != attrs.end() ) {
                            std::wstring start = boost::regex_replace(block.substr(0, left_index), boost::wregex(L"\\smarkdown(=['\"]?[^> ]*['\"]?)?"), std::wstring());
							int begin = block.size()-right_tag.size()-2;
                            std::wstring end   = block.substr(begin, block.size()-begin);
							block = block.substr(left_index, begin-left_index);
							new_blocks.push_back(this->markdown->htmlStash.store(start));
							new_blocks.push_back(block);
							new_blocks.push_back(this->markdown->htmlStash.store(end));
						} else {
							new_blocks.push_back(this->markdown->htmlStash.store(boost::algorithm::trim_copy(block)));
						}
						continue;
					} else {
						//! if is block level tag and is not complete

                        std::wstring buff = boost::algorithm::trim_right_copy(block);
						if ( ( util::isBlockLevel(left_tag) || left_tag == L"--" ) && ! buff[buff.size()-1] == L'>' ) {
							items.push_back(boost::algorithm::trim_copy(block));
							in_tag = true;
						} else {
							new_blocks.push_back(this->markdown->htmlStash.store(boost::algorithm::trim_copy(block)));
						}

						continue;
					}
				}

				new_blocks.push_back(block);

			} else {
				items.push_back(block);

                boost::tuples::tuple<std::wstring, std::size_t> result = this->get_right_tag(left_tag, 0, block);
                right_tag = result.get<0>();
                data_index = result.get<1>();

				if ( this->equal_tags(left_tag, right_tag) ) {
					//! if find closing tag

					if ( data_index < block.size() ) {
						//! we have more text after right_tag
						items.back() = block.substr(0, data_index);
						texts.push_front(block.substr(data_index, block.size()-data_index));
					}

					in_tag = false;
                    if (this->markdown_in_raw && attrs.find(L"markdown") != attrs.end() ) {
                        std::wstring start = boost::regex_replace(items.front().substr(0, left_index), boost::wregex(L"\\smarkdown(=['\"]?[^> ]*['\"]?)?"), std::wstring());
						items.front() = items.front().substr(left_index, items.front().size()-left_index);
						int begin = items.back().size()-right_tag.size()-2;
                        std::wstring end = items.back().substr(begin, items.back().size()-begin);
						items.back() = items.back().substr(0, begin);
						new_blocks.push_back(this->markdown->htmlStash.store(start));
                        for ( const std::wstring& item : items ) {
							new_blocks.push_back(item);
						}
						new_blocks.push_back(this->markdown->htmlStash.store(end));
					} else {
						new_blocks.push_back(boost::algorithm::join(items, L"\n\n"));
					}
                    items = std::list<std::wstring>();
				}
			}
		}
		if ( items.size() > 0 ) {
			if ( this->markdown_in_raw && attrs.find(L"markdown") != attrs.end() ) {
                std::wstring start = boost::regex_replace(items.front().substr(0, left_index), boost::wregex(L"\\smarkdown(=['\"]?[^> ]*['\"]?)?"), std::wstring());
				items.front() = items.front().substr(left_index, items.front().size()-left_index);
				int begin = items.back().size()-right_tag.size()-2;
                std::wstring end = items.back().substr(begin, items.back().size()-begin);
				items.back() = items.back().substr(0, begin);
				new_blocks.push_back(this->markdown->htmlStash.store(start));
                for ( const std::wstring& item : items ) {
					new_blocks.push_back(item);
				}
				new_blocks.push_back(this->markdown->htmlStash.store(end));
			} else {
				new_blocks.push_back(boost::algorithm::join(items, L"\n\n"));
			}
			//new_blocks.push_back(this->markdown->htmlStash.store(boost::algorithm::join(items, L"\n\n")));
			new_blocks.push_back(L"\n");
		}
        std::wstring new_text = boost::algorithm::join(new_blocks, L"\n\n");
        std::list<std::wstring> result;
		boost::algorithm::split(result, new_text, boost::is_any_of(L"\n"));
		return result;
	}

private:
    boost::tuples::tuple<std::wstring, int, Attributes> get_left_tag(const std::wstring& block)
	{
		boost::wsmatch m;
		if ( boost::regex_match(block, m, this->left_tag_re) ) {
            std::wstring tag = m[L"tag"];
            std::wstring raw_attrs = m[L"attrs"];
			Attributes attrs;
			if ( ! raw_attrs.empty() ) {
                std::wstring::const_iterator begin = raw_attrs.begin(), end = raw_attrs.end();
				boost::wsmatch ma;
				while ( boost::regex_search(begin, end, ma, this->attrs_re) ) {
                    if ( ! std::wstring(ma[L"attr"]).empty() ) {
                        std::wstring attr = ma[L"attr"];
						boost::algorithm::trim(attr);
                        if ( ! std::wstring(ma[L"value"]).empty() ) {
							attrs[attr] = ma[L"value"];
						} else {
                            attrs[attr] = std::wstring();
						}
                    } else if ( ! std::wstring(ma[L"attr1"]).empty() ) {
                        std::wstring attr1 = ma[L"attr1"];
						boost::algorithm::trim(attr1);
                        if ( ! std::wstring(ma[L"value1"]).empty() ) {
							attrs[attr1] = ma[L"value1"];
						} else {
                            attrs[attr1] = std::wstring();
						}
                    } else if ( ! std::wstring(ma[L"attr2"]).empty() ) {
                        std::wstring attr2 = ma[L"attr2"];
						boost::algorithm::trim(attr2);
                        attrs[attr2] = std::wstring();
					}
				}
            }
            return boost::tuples::make_tuple(tag, m.str(0).size(), attrs);
		} else {
            std::wstring tag = block.substr(1, block.find(L'>'));
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            return boost::tuples::make_tuple(tag, tag.size()+2, Attributes());
		}
	}
    int recursive_tagfind(const std::wstring& ltag, const std::wstring& rtag, int start_index, const std::wstring& block)
	{
		while ( true ) {
            std::wstring::size_type i = block.find(rtag, start_index);
            if ( i == std::wstring::npos ) {
				return -1;
			}
            std::wstring::size_type j = block.find(ltag, start_index);
			//! if no ltag, or rtag found before another ltag, return index
            if ( j > i || j == std::wstring::npos ) {
				return i + rtag.size();
			}
			//! another ltag found before rtag, use end of ltag as starting
			//! point and search again
			j = block.find(L'>', j);
			start_index = this->recursive_tagfind(ltag, rtag, j+1, block);
			if ( start_index == -1 ) {
				//! HTML potentially malformed- ltag has no corresponding
				//! rtag
				return -1;
			}
		}
	}
    boost::tuples::tuple<std::wstring, std::size_t> get_right_tag(const std::wstring& left_tag, int left_index, const std::wstring& block)
	{
        for ( const std::wstring& p : this->right_tag_patterns ) {
            std::wstring tag = (boost::wformat(p)%left_tag).str();
			int i = this->recursive_tagfind((boost::wformat(L"<%s")%left_tag).str(), tag, left_index, block);
			if ( i > 2 ) {
				boost::algorithm::trim_left_if(tag, [](wchar_t ch) -> bool { return ch == L'<'; });
				boost::algorithm::trim_right_if(tag, [](wchar_t ch) -> bool { return ch == L'>'; });
                return boost::tuples::make_tuple(tag, i);
			}
		}
        std::wstring result = boost::algorithm::trim_right_copy(block);
		int begin = result.size()-left_index;
		int count = result.size()-begin-1;
		result = result.substr(result.size()-left_index, count);
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return boost::tuples::make_tuple(result, block.size());
	}
    bool equal_tags(const std::wstring& left_tag, const std::wstring& right_tag)
	{
		wchar_t ch = left_tag[0];
		if ( ch == L'?' || ch == L'@' || ch == L'%' ) {  //!< handle PHP, etc.
			return true;
		}
		if ( (L"/"+left_tag) == right_tag ) {
			return true;
		}
		if ( right_tag == L"--" && left_tag == L"--" ) {
			return true;
		} else if ( left_tag == right_tag.substr(1, right_tag.size()-1)
					  && right_tag[0] == L'/' ) {
			return true;
		} else {
			return false;
		}
	}
    bool is_oneliner(const std::wstring& tag)
	{
		return tag == L"hr" || tag == L"hr/";
	}

private:
	std::vector<std::wstring> right_tag_patterns;
	std::wstring              attrs_pattern;
	std::wstring              left_tag_pattern;
	boost::wregex             attrs_re;
	boost::wregex             left_tag_re;
	bool                      markdown_in_raw;

};

/*!
 * Remove reference definitions from text and store for later use.
 */
class ReferencePreprocessor : public PreProcessor
{
public:
	ReferencePreprocessor(Markdown* markdown_instance) :
		PreProcessor(markdown_instance),
		TITLE(L"[ ]*(\\\"(.*)\\\"|\\'(.*)\\'|\\((.*)\\))[ ]*"),
		RE((boost::wformat(L"^[ ]{0,3}\\[([^\\]]*)\\]:\\s*([^ ]*)[ ]*(%s)?$")%TITLE).str(), boost::regex_constants::mod_s),
		TITLE_RE((boost::wformat(L"^%s$")%TITLE).str())
	{}

    std::list<std::wstring> run(const std::list<std::wstring>& lines)
	{
        std::list<std::wstring> buffer = lines;
        std::list<std::wstring> new_text;
		while ( buffer.size() > 0 ) {
            std::wstring line = buffer.front();
			buffer.pop_front();
			boost::wsmatch m;
			if ( boost::regex_match(line, m, this->RE) ) {
                std::wstring id = boost::algorithm::trim_copy(m.str(1));
				std::transform(id.begin(), id.end(), id.begin(), ::tolower);
                std::wstring link = m.str(2);
				boost::algorithm::trim_left_if(link, [](wchar_t ch) -> bool { return ch == L'<'; });
				boost::algorithm::trim_right_if(link, [](wchar_t ch) -> bool { return ch == L'>'; });
                std::wstring t;
				for ( int i = 5; i <= 7; ++i ) {
					t = m.str(i);
					if ( t.size() > 0 ) {
						break;
					}
				}
				if ( ! ( m.str(5).size() > 0 || m.str(6).size() > 0 || m.str(7).size() > 0 ) ) {
					//! Check next line for title
					boost::wsmatch tm;
                    std::wstring regexTmp = buffer.front();
                    if ( boost::regex_match(regexTmp, tm, this->TITLE_RE) ) {
						buffer.pop_front();
						for ( int i = 2; i <= 4; ++i ) {
							t = m.str(i);
							if ( t.size() > 0 ) {
								break;
							}
						}
					}
				}
                this->markdown->references[id] = Markdown::ReferenceItem(link, t);
			} else {
				new_text.push_back(line);
			}
		}
		return new_text;
	}

private:
    const std::wstring TITLE;
	const boost::wregex RE;
	const boost::wregex TITLE_RE;

};

OrderedDictProcessors build_preprocessors(Markdown* md_instance)
{
	OrderedDictProcessors preProcessors;
    preProcessors.append("normalize_whitespace", boost::shared_ptr<Processor>(new NormalizeWhitespace(md_instance)));
	if ( md_instance->safeMode() != Markdown::escape_mode ) {
        preProcessors.append("html_block", boost::shared_ptr<Processor>(new HtmlBlockProcessor(md_instance)));
	}
    preProcessors.append("reference", boost::shared_ptr<Processor>(new ReferencePreprocessor(md_instance)));
	return preProcessors;
}

} // end of namespace markdown
