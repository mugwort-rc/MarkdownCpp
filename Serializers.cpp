/*
 * Serializers.cpp
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#include "Serializers.h"

#include <functional>
#include <set>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/tuple/tuple.hpp>

namespace markdown{

typedef enum {
    html,
    xhtml
} Format;

typedef std::map<std::wstring, std::wstring> NamespaceMap;

template<typename T, typename V>
std::list<T> keys(const std::map<T,V>& src)
{
    std::list<T> result;
    for ( auto it = src.begin(); it != src.end(); ++it ) {
        result.push_back(it->first);
    }
    return result;
}

static const std::set<std::wstring> HTML_EMPTY = {L"area", L"base", L"basefont", L"br", L"col", L"frame", L"hr",
                                                  L"img", L"input", L"isindex", L"link", L"meta", L"param"};

static const NamespaceMap namespace_map = {
    // "well-known" namespace prefixes
    std::make_pair(L"http://www.w3.org/XML/1998/namespace", L"xml"),
    std::make_pair(L"http://www.w3.org/1999/xhtml", L"html"),
    std::make_pair(L"http://www.w3.org/1999/02/22-rdf-syntax-ns#", L"rdf"),
    std::make_pair(L"http://schemas.xmlsoap.org/wsdl/", L"wsdl"),
    // xml schema
    std::make_pair(L"http://www.w3.org/2001/XMLSchema", L"xs"),
    std::make_pair(L"http://www.w3.org/2001/XMLSchema-instance", L"xsi"),
    // dublic core
    std::make_pair(L"http://purl.org/dc/elements/1.1/", L"dc")
};

std::wstring escape_cdata(const std::wstring& text)
{
    //! escape character data
    std::wstring result = text;

    //! it's worth avoiding do-nothing calls for strings that are
    //! shorter than 500 character, or so.  assume that's, by far,
    //! the most common case in most applications.
    if ( result.find(L'&') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"&", L"&amp;");
    }
    if ( result.find(L'<') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"<", L"&lt;");
    }
    if ( result.find(L'>') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L">", L"&gt;");
    }
    return result;
}

std::wstring escape_attrib(const std::wstring& text)
{
    //! escape attribute value
    std::wstring result = text;
    if ( result.find(L'&') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"&", L"&amp;");
    }
    if ( result.find(L'<') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"<", L"&lt;");
    }
    if ( result.find(L'>') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L">", L"&gt;");
    }
    if ( result.find(L'"') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"\"", L"&quot;");
    }
    if ( result.find(L'\n') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"\n", L"&#10;");
    }
    return result;
}

std::wstring escape_attrib_html(const std::wstring& text)
{
    //! escape attribute value
    std::wstring result = text;
    if ( result.find(L'&') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"&", L"&amp;");
    }
    if ( result.find(L'<') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"<", L"&lt;");
    }
    if ( result.find(L'>') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L">", L"&gt;");
    }
    if ( result.find(L'"') != std::wstring::npos ) {
        boost::algorithm::replace_all(result, L"\"", L"&quot;");
    }
    return result;
}

void serialize_html(const std::function<void(const std::wstring&)>& write, Element& elem, const NamespaceMap& qnames, const NamespaceMap& namespaces, Format format)
{
    /*
    if ( elem->getNodeType() == xercesc::DOMNode::COMMENT_NODE ) {
        xercesc::DOMComment* comment = reinterpret_cast<xercesc::DOMComment*>(elem);
        write((boost::wformat(L"<!--%s-->")%escape_cdata(wconvert(comment->getData()))).str());
    } else if ( elem->getNodeType() == xercesc::DOMNode::PROCESSING_INSTRUCTION_NODE ) {
        xercesc::DOMProcessingInstruction* pi = reinterpret_cast<xercesc::DOMProcessingInstruction*>(elem);
        write((boost::wformat(L"<?%s %s?>")%escape_cdata(wconvert(pi->getTarget()))%escape_cdata(wconvert(pi->getData()))).str());
    } else */{
        std::wstring tag = elem.getTagName();
        write(L"<"+tag);
        Element::Attributes attrs = elem.getAttributes();
        if ( attrs.size() > 0 ) {
            std::list<std::wstring> attrKeys = keys(attrs);
            attrKeys.sort();  //!< lexical order
            for ( const std::wstring& key : attrKeys ) {
                const std::wstring value = escape_attrib_html(attrs[key]);
                if ( format == html && qnames.find(key) != qnames.end() && qnames.at(key) == value ) {
                    //! handle boolean attributes
                    write((boost::wformat(L" %s")%value).str());
                } else if ( qnames.find(key) != qnames.end() ) {
                    write((boost::wformat(L" %s=\"%s\"")%qnames.at(key)%value).str());
                }
            }
            if ( ! namespaces.empty() ) {
                typedef std::pair<std::wstring, std::wstring> Pair;
                typedef std::vector<Pair> Pairs;
                Pairs ns_list;
                for ( NamespaceMap::const_iterator it = namespaces.begin(); it != namespaces.end(); ++it ) {
                    ns_list.push_back(Pair((*it).first, (*it).second));
                }
                boost::range::sort(ns_list, [](const Pair& a, const Pair& b) -> bool { return a.second < b.second; });  //!< sort on prefix
                for ( const Pair& pair : ns_list ) {
                    std::wstring key = pair.first;
                    if ( ! key.empty() ) {
                        key = L":"+key;
                    }
                    write((boost::wformat(L" xmlns%s=\"%s\"")%key%escape_attrib(pair.second)).str());
                }
            }
        }
        if ( format == xhtml && HTML_EMPTY.find(tag) != HTML_EMPTY.end() ) {
            write(L"/>");
        } else {
            write(L">");
            std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            if ( elem.hasText() ) {
                if ( tag == L"script" || tag == L"style" ) {
                    write(elem.text());
                } else {
                    write(escape_cdata(elem.text()));
                }
            }
            for ( Element& child : elem.child() ) {
                serialize_html(write, child, qnames, NamespaceMap(), format);
            }
            if ( HTML_EMPTY.find(tag) == HTML_EMPTY.end() ) {
                write((boost::wformat(L"</%s>")%tag).str());
            }
        }
    }
    if ( elem.hasTail() ) {
        write(escape_cdata(elem.tail()));
    }
}

boost::tuples::tuple<NamespaceMap, NamespaceMap> namespaces(Element &elem, const std::wstring& default_namespace=std::wstring())
{
    //! identify namespaces used in this tree

    //! maps qnames to *encoded* prefix:local names
    NamespaceMap qnames;

    //! maps uri:s to prefixes
    NamespaceMap nss;
    if ( ! default_namespace.empty() ) {
        nss[default_namespace] = std::wstring();
    }

    auto add_qname = [&](const std::wstring& qname){
        //! calculate serialized qname representation
        if ( qname.at(0) == L'{' ) {
            std::vector<std::wstring> datas;
            std::wstring buff = qname.substr(1, qname.size()-1);
            boost::algorithm::split(datas, buff, boost::is_any_of(L"}"));
            std::wstring uri = datas.at(0), tag = datas.at(1);
            std::wstring prefix;
            if ( nss.find(uri) == nss.end() ) {
                if ( namespace_map.find(uri) == namespace_map.end() ) {
                    prefix = (boost::wformat(L"ns%d")%nss.size()).str();
                }
                if ( prefix != L"xml" ) {
                    nss[uri] = prefix;
                }
            }
            if ( ! prefix.empty() ) {
                qnames[qname] = (boost::wformat(L"%s:%s")%prefix%tag).str();
            } else {
                qnames[qname] = tag;  //!< default element
            }
        } else {
            if ( ! default_namespace.empty() ) {
                throw std::invalid_argument("cannot use non-qualified names with default_namespace option");
            }
            qnames[qname] = qname;
        }
    };

    //! populate qname and namespaces table
    std::list<Element> iterNodes;
    std::function<void(const Element&)> iter = [&](const Element &it){
        if ( it.child().size() > 0 ) {
            for ( const Element& node : it.child() ) {
                iterNodes.push_back(node);
                iter(node);
            }
        }
    };
    iter(elem);

    if ( ! iterNodes.empty() ) {
        for ( Element &node : iterNodes ) {
            std::wstring nsURI = node.getNamespaceURI();
            std::wstring tag   = node.getTagName();
            if ( ! nsURI.empty() ) {
                tag = (boost::wformat(L"{%s}%s")%nsURI%tag).str();
            }
            if ( qnames.find(tag) != qnames.end() ) {
                add_qname(tag);
            }
            Element::Attributes attrs = node.getAttributes();
            for ( const std::wstring& key : keys(attrs) ) {
                if ( qnames.find(key) == qnames.end() ) {
                    add_qname(key);
                }
            }
        }
    }
    return boost::tuples::make_tuple(qnames, nss);
}

std::wstring write_html(Element &root, const Format& format)
{
    if ( root.isNull() ) {
        return std::wstring();
    }
    std::list<std::wstring> data;
    NamespaceMap qnames, namespaces_map;
    boost::tuples::tuple<NamespaceMap, NamespaceMap> result = namespaces(root);
    qnames = result.get<0>();
    namespaces_map = result.get<1>();
    serialize_html([&](const std::wstring& text){ data.push_back(text); }, root, qnames, namespaces_map, format);
    return boost::algorithm::join(data, std::wstring());
}

std::wstring to_html_string(Element &element)
{
    return write_html(element, html);
}

std::wstring to_xhtml_string(Element &element)
{
    return write_html(element, xhtml);
}

} // end of namespace markdown
