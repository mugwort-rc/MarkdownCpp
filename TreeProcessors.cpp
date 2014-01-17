/*
 * TreeProcessors.cpp
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#include "TreeProcessors.h"

#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "MarkdownCpp.h"

namespace markdown{

template<typename T>
void append(std::list<T>& dest, const std::list<T> &src)
{
    for ( const T& v : src ) {
        dest.push_back(v);
    }
}

TreeProcessor::TreeProcessor(Markdown* md_instance) :
    markdown(md_instance), stashed_nodes()
{}

TreeProcessor::~TreeProcessor(void)
{}

/*!
 * A Treeprocessor that traverses a tree, applying inline patterns.
 */
class InlineProcessor : public TreeProcessor
{
public:
    InlineProcessor(Markdown* md) :
        TreeProcessor(md),
        placeholder_prefix(util::INLINE_PLACEHOLDER_PREFIX),
        placeholder_suffix(util::ETX),
        placeholder_length(4 + this->placeholder_prefix.size() + this->placeholder_suffix.size()),
        placeholder_re(util::INLINE_PLACEHOLDER_RE),
        class_root(ElementTree::InvalidElementTree)
    {}

    ~InlineProcessor(void)
    {}

private:
    /*!
     * Generate a placeholder
     */
    boost::tuples::tuple<std::wstring, std::wstring> makePlaceholder(const std::wstring&/* type*/)
    {
        std::wstring id = (boost::wformat(L"%04d")%this->stashed_nodes.size()).str();
        std::wstring hash = (boost::wformat(util::INLINE_PLACEHOLDER)%id).str();
        return boost::tuples::make_tuple(hash, id);
    }

    /*!
     * Extract id from data string, start from index
     *
     * Keyword arguments:
     *
     * * data: string
     * * index: index, from which we start search
     *
     * Returns: placeholder id and string index, after the found placeholder.
     *
     */
    boost::tuples::tuple<boost::optional<std::wstring>, int> findPlaceholder(const std::wstring& data, std::wstring::size_type index)
    {
        std::wstring regexTmp = data;
        if ( index != std::wstring::npos ) {
            regexTmp = data.substr(index, data.size()-index);
        }
        boost::wsmatch m;
        if ( boost::regex_search(regexTmp, m, this->placeholder_re) ) {
            return boost::tuples::make_tuple(m.str(1), index+m.position()+m.length());
        } else {
            return boost::tuples::make_tuple(boost::none, index+1);
        }
    }

    /*!
     * Add node to stash
     */
    std::wstring stashNode(Element &node, const std::wstring& type)
    {
        std::wstring placeholder, id;
        boost::tuples::tuple<std::wstring, std::wstring> result = this->makePlaceholder(type);
        placeholder = result.get<0>();
        id = result.get<1>();
        this->stashed_nodes[id] = boost::tuples::make_tuple(boost::none, node);
        return placeholder;
    }
    std::wstring stashNode(const std::wstring& node, const std::wstring& type)
    {
        std::wstring placeholder, id;
        boost::tuples::tuple<std::wstring, std::wstring> result = this->makePlaceholder(type);
        placeholder = result.get<0>();
        id = result.get<1>();
        this->stashed_nodes[id] = boost::tuples::make_tuple(node, boost::none);
        return placeholder;
    }

    /*!
     * Process string with inline patterns and replace it
     * with placeholders
     *
     * Keyword arguments:
     *
     * * data: A line of Markdown text
     * * patternIndex: The index of the inlinePattern to start with
     *
     * Returns: String with placeholders.
     *
     */
    std::wstring handleInline(const std::wstring& data, std::size_t patternIndex = 0)
    {
        int startIndex = 0;
        std::wstring data_ = data;
        while ( patternIndex < this->markdown->inlinePatterns.size() ) {
            boost::shared_ptr<Pattern> pattern = this->markdown->inlinePatterns.at(patternIndex);
            boost::tuples::tuple<std::wstring, bool, int> result = this->applyPattern(pattern, data_, patternIndex, startIndex);
            data_ = result.get<0>();
            bool matched = result.get<1>();
            startIndex = result.get<2>();
            if ( ! matched ) {
                patternIndex += 1;
            }
        }
        return data_;
    }

    /*!
     * Process placeholders in Element.text or Element.tail
     * of Elements popped from self.stashed_nodes.
     *
     * Keywords arguments:
     *
     * * node: parent node
     * * subnode: processing node
     * * isText: bool variable, True - it's text, False - it's tail
     *
     * Returns: None
     *
     */
    void processElementText(Element &node, Element &subnode, bool isText=true)
    {
        std::wstring text;
        if ( isText ) {
            if ( subnode.hasText() ) {
                text = subnode.text();
                subnode.removeText();
            }
        } else {
            if ( subnode.hasTail() ) {
                text = subnode.tail();
                subnode.removeTail();
            }
        }

        Element::List childResult = this->processPlaceholders(text, subnode);

        Element pos = Element::InvalidElement;
        if ( ! isText && node != subnode ) {
            pos = subnode.getNextElementSibling();
            if ( subnode.hasTail() ) {
                subnode.removeTail();
            }
            node.remove(subnode);
        } else {
            pos = node.getFirstElementChild();
        }

        for ( Element &newChild : childResult ) {
            if ( ! pos.isNull() ) {
                node.insertBefore(newChild, pos);
                pos = newChild;
            } else {
                if ( node.child().size() > 0 ) {
                    node.insertBefore(newChild, node.getFirstElementChild());
                } else {
                    node.append(newChild);
                }
            }
        }
    }

    /*!
     * Process string with placeholders and generate ElementTree tree.
     *
     * Keyword arguments:
     *
     * * data: string with placeholders instead of ElementTree elements.
     * * parent: Element, which contains processing inline data
     *
     * Returns: list with ElementTree elements with applied inline patterns.
     *
     */
    Element::List processPlaceholders(const std::wstring& data, Element &parent)
    {
        Element::List result;
        auto linkText = [&](const std::wstring& text){
            if ( ! text.empty() ) {
                if ( ! result.empty() ) {
                    Element target = result.back();
                    if ( target.hasTail() ) {
                        target.setTail(target.tail()+text);
                    } else {
                        target.setTail(text);
                        result.pop_back();
                        result.push_back(target);
                    }
                } else {
                    if ( parent.hasText() ) {
                        parent.setText(parent.text()+text);
                    } else {
                        parent.setText(text);
                    }
                }
            }
        };
        int startIndex = 0;
        std::wstring data_ = data;
        while ( ! data_.empty() ) {
            std::wstring::size_type index = data_.find(this->placeholder_prefix, startIndex);
            if ( index != std::wstring::npos ) {
                boost::tuples::tuple<boost::optional<std::wstring>, int> ret = this->findPlaceholder(data_, index);
                boost::optional<std::wstring> id = ret.get<0>();
                int phEndIndex = ret.get<1>();
                if ( this->stashed_nodes.find(*id) != this->stashed_nodes.end() ) {
                    NodeItem node = this->stashed_nodes[*id];
                    boost::optional<std::wstring> str = node.get<0>();
                    boost::optional<Element> nodeptr = node.get<1>();
                    if ( index > 0 ) {
                        std::wstring text = data_.substr(startIndex, index-startIndex);
                        linkText(text);
                    }

                    if ( ! str ) {
                        //! it's Element
                        Element::List nodes = {*nodeptr};
                        for ( Element &e : (*nodeptr).child() ) {
                            nodes.push_back(e);
                        }
                        for ( Element &child : nodes ) {
                            if ( child.hasTail() ) {
                                if ( ! boost::algorithm::trim_copy(child.tail()).empty() ) {
                                    Element new_node = *nodeptr;
                                    this->processElementText(new_node, child, false);
                                }
                            }
                            if ( child.hasText() ) {
                                if ( ! boost::algorithm::trim_copy(child.text()).empty() ) {
                                    this->processElementText(child, child);
                                }
                            }
                        }
                    } else {
                        //! it's just a string
                        linkText(*str);
                        startIndex = phEndIndex;
                        continue;
                    }

                    startIndex = phEndIndex;
                    result.push_back(*nodeptr);

                } else {
                    //! wrong placeholder
                    int end = index + this->placeholder_prefix.size();
                    linkText(data_.substr(startIndex, end-startIndex));
                    startIndex = end;
                }
            } else {
                std::wstring text = data_.substr(startIndex, data_.size()-startIndex);
                linkText(text);
                data_ = std::wstring();
            }
        }
        return result;
    }

    /*!
     * Check if the line fits the pattern, create the necessary
     * elements, add it to stashed_nodes.
     *
     * Keyword arguments:
     *
     * * data: the text to be processed
     * * pattern: the pattern to be checked
     * * patternIndex: index of current pattern
     * * startIndex: string index, from which we start searching
     *
     * Returns: String with placeholders instead of ElementTree elements.
     *
     */
    boost::tuples::tuple<std::wstring, bool, int> applyPattern(boost::shared_ptr<Pattern> pattern, const std::wstring& data, int patternIndex, int startIndex=0)
    {
        std::wstring regexTmp = data.substr(startIndex, data.size()-startIndex);
        boost::wsmatch match;
        if ( ! boost::regex_match(regexTmp, match, pattern->getCompiledRegExp()) ) {
            return boost::tuples::make_tuple(data, false, 0);
        }
        std::wstring leftData = data.substr(0, startIndex);

        boost::optional<std::wstring> result = pattern->handleMatch(match);  //!< first handleMatch (case String)
        std::wstring placeholder;
        if ( ! result ) {
            Element node = pattern->handleMatch(this->class_root, match);     //!< second handleMatch (case Node)
            if ( node.isNull() ) {
                return boost::tuples::make_tuple(data, true, leftData.size()+match.position(match.size()-1));
            }
            if ( node.child().size() == 0 || node.hasText() ) {
                //! We need to process current node too
                Element::List nodes = {node};
                append(nodes, node.child());
                for ( Element& child : nodes ) {
                    if ( child.hasText() ) {
                        std::wstring text = child.text();
                        text = this->handleInline(text, patternIndex+1);
                        child.setText(text);
                    }
                    if ( child.hasTail() ) {
                        std::wstring tail = child.tail();
                        tail = this->handleInline(tail, patternIndex);
                        child.setTail(tail);
                    }
                }
            }

            placeholder = this->stashNode(node, pattern->type());
        } else {
            placeholder = this->stashNode(*result, pattern->type());
        }

        return boost::tuples::make_tuple((boost::wformat(L"%s%s%s%s")%leftData%match.str(1)%placeholder%match.str(match.size()-1)).str(), true, 0);
    }

    /*!
     * Apply inline patterns to a parsed Markdown tree.
     *
     * Iterate over ElementTree, find elements with inline tag, apply inline
     * patterns and append newly created Elements to tree.  If you don't
     * want to process your data with inline paterns, instead of normal string,
     * use subclass AtomicString:
     *
     *     node.text = markdown.AtomicString("This will not be processed.")
     *
     * Arguments:
     *
     * * tree: ElementTree object, representing Markdown tree.
     *
     * Returns: ElementTree object with applied inline patterns.
     *
     */
    Element run(Element &tree)
    {
        this->stashed_nodes = StashNodes();

        this->class_root = ElementTree(L"root");

        try{
            Element::List stack = {tree};
            while ( ! stack.empty() ) {
                Element currElement = stack.back();
                stack.pop_back();
                typedef std::pair<Element, Element::List> QueueItem;
                typedef std::list<QueueItem> Queue;
                Queue insertQueue;
                for ( Element& child : currElement.child() ) {
                    if ( child.hasText() ) {
                        std::wstring text = child.text();
                        child.removeText();
                        Element::List lst = this->processPlaceholders(this->handleInline(text), child);
                        append(stack, lst);
                        insertQueue.push_back(std::make_pair(child, lst));
                    }
                    if ( child.hasTail() ) {
                        std::wstring tail = this->handleInline(child.tail());
                        Element dumby_root(this->class_root, L"d_root");
                        Element dumby(dumby_root, L"d");
                        dumby_root.append(dumby);
                        Element::List tailResult = this->processPlaceholders(tail, dumby);
                        if ( dumby.hasTail() ) {
                            child.setTail(dumby.tail());
                        } else {
                            child.removeTail();
                        }
                        Element target = child.getNextElementSibling();
                        for ( Element::List::reverse_iterator it = tailResult.rbegin(); it != tailResult.rend(); ++it ) {
                            Element elem = (*it);
                            if ( ! target.isNull() ) {
                                currElement.insertBefore(elem, target);
                            } else {
                                currElement.append(elem);
                            }
                        }
                    }
                    if ( child.child().size() > 0 ) {
                        stack.push_back(child);
                    }
                }
                for ( const QueueItem& item : insertQueue ) {
                    Element element = item.first;
                    Element::List lst = item.second;
                    if ( this->markdown->enable_attributes() ) {
                        if ( element.hasText() ) {
                            std::wstring text = element.text();
                            text = handleAttributes(text, element);
                            element.setText(text);
                        }
                    }
                    Element ref = ( element.child().size() > 0 ) ? element.getFirstElementChild() : Element::InvalidElement;
                    for ( Element &newChild : lst ) {
                        if ( this->markdown->enable_attributes() ) {
                            //! Processing attributes
                            if ( newChild.hasTail() ) {
                                std::wstring text = newChild.tail();
                                text = handleAttributes(text, element);
                                newChild.setTail(text);
                            }
                            if ( newChild.hasText() ) {
                                std::wstring text = newChild.text();
                                text = handleAttributes(text, element);
                                newChild.setText(text);
                            }
                        }
                        if ( ! ref.isNull() ) {
                            element.insertBefore(newChild, ref);
                        } else {
                            element.append(newChild);
                        }
                    }
                }
            }
            return tree;
        } catch (...) {
            std::cerr << "TreeProcessor::run() exception." << std::endl;
        }
        this->class_root = ElementTree::InvalidElementTree;
        return Element::InvalidElement;
    }

private:
    std::wstring placeholder_prefix;
    std::wstring placeholder_suffix;
    unsigned int  placeholder_length;
    boost::wregex placeholder_re;

    ElementTree class_root;

};

/*!
 * Add linebreaks to the html document.
 */
class PrettifyTreeprocessor : public TreeProcessor
{
public:
    PrettifyTreeprocessor(Markdown* md) :
        TreeProcessor(md)
    {}

private:
    /*!
     * Recursively add linebreaks to ElementTree children.
     */
    void prettifyETree(Element &elem)
    {
        if ( util::isBlockLevel(elem.getTagName()) && elem.getTagName() != L"code" && elem.getTagName() != L"pre" ) {
            if ( ( ! elem.hasText() || boost::algorithm::trim_copy(elem.text()).empty() )
                 && elem.child().size() > 0 && util::isBlockLevel(elem.child().front().getTagName()) ) {
                elem.setText(L"\n");
            }
            for ( Element &e : elem.child() ) {
                if ( util::isBlockLevel(e.getTagName()) ) {
                    this->prettifyETree(e);
                }
            }
            if ( ! elem.hasTail() || boost::algorithm::trim_copy(elem.tail()).empty() ) {
                elem.setTail(L"\n");
            }
        }
        if ( ! elem.hasTail() || boost::algorithm::trim_copy(elem.tail()).empty() ) {
            elem.setTail(L"\n");
        }
    }

public:
    /*!
     * Add linebreaks to ElementTree root object.
     */
    Element run(Element &root)
    {
        this->prettifyETree(root);
        //! Do <br />'s seperately as they are often in the middle of
        //! inline content and missed by _prettifyETree.
        for ( Element& br : root.getElementsByTagName(L"br") ) {
            if ( ! br.hasTail() || boost::algorithm::trim_copy(br.tail()).empty() ) {
                br.setTail(L"\n");
            } else {
                br.setTail(L"\n"+br.tail());
            }
        }
        //! Clean up extra empty lines at end of code blocks.
        for ( Element& pre : root.getElementsByTagName(L"pre") ) {
            if ( pre.child().size() > 0 && pre.child().front().getTagName() == L"code" ) {
                pre.child().front().setText(boost::algorithm::trim_right_copy(pre.child().front().text())+L"\n");
            }
        }
        return Element::InvalidElement;
    }

};

OrderedDictTreeProcessors build_treeprocessors(Markdown* md_instance)
{
    OrderedDictTreeProcessors treeprocessors;
    treeprocessors.append("inline", boost::shared_ptr<TreeProcessor>(new InlineProcessor(md_instance)));
    treeprocessors.append("prettify", boost::shared_ptr<TreeProcessor>(new PrettifyTreeprocessor(md_instance)));
    return treeprocessors;
}

} // end of namespace markdown
