/*
 * ElementTree.h
 *
 *  Created on: 2013/11/09
 *      Author: mugwort_rc
 */

#ifndef ELEMENTTREE_H_
#define ELEMENTTREE_H_

#include <list>
#include <map>
#include <string>

#include <boost/shared_ptr.hpp>

namespace markdown{

class ElementImpl;      //!< Pimpl
class ElementTreeImpl;  //!< PImpl

class Element;  //!< forward declaration for ElementTree

class ElementTree
{
    friend class Element;

private:
    typedef boost::shared_ptr<ElementTreeImpl> Impl;

public:
    ElementTree(const std::wstring& root);
    ElementTree(const Impl& impl);
    ElementTree(const ElementTree& copy);
    ElementTree(ElementTree &&move);
    ElementTree& operator =(const ElementTree& rhs);

public:
    static ElementTree InvalidElementTree;

private:
    Impl _impl;

};

class Element
{
public:
    typedef std::list<Element> List;
    typedef std::map<std::wstring, std::wstring> Attributes;

private:
    typedef boost::shared_ptr<ElementImpl> Impl;

public:
    Element(const ElementTree &doc);
    Element(const ElementTree &doc, const std::wstring &name);
    /*!
     * @code:
     * Element parent;
     * Element child(parent, L"child");  //!< @note: not appended
     *
     * parent.append(child);
     *
     * // or
     *
     * parent.insertBefore(child, parent.getFirstElementChild());
     *
     * @endcode
     */
    Element(const Element &parent, const std::wstring &name);
    Element(const Element &copy);
    Element(Element &&move);

    Element& operator =(const Element &rhs);

    bool operator ==(const Element &rhs) const;
    bool operator !=(const Element &rhs) const;

    ElementTree getOwnerDocument(void) const;

    List child(void) const;
    List getElementsByTagName(const std::wstring &name) const;
    Element getFirstElementChild(void) const;
    Element getLastElementChild(void) const;
    Element getNextElementSibling(void) const;

    bool isNull(void) const;
    std::wstring getTagName(void) const;

    void setAttribute(const std::wstring &key, const std::wstring &val);
    Attributes getAttributes(void) const;

    std::wstring getNamespaceURI(void) const;
    std::wstring getTextContent(void) const;

    bool hasText(void) const;
    bool hasTail(void) const;

    void removeText(void);
    void removeTail(void);

    void setText(const std::wstring &text);
    void setTail(const std::wstring &tail);

    std::wstring text(void) const;
    std::wstring tail(void) const;

    void append(Element &child);
    void insertBefore(Element &child, const Element &ref);
    void remove(Element &child);

public:
    static Element InvalidElement;

private:
    Element(const Impl &elem);
    Impl import(Impl &target, bool deep=true);
    void initialize(void);

private:
    Impl _impl;

};

} // end of namespace markdown

#endif // ELEMENTTREE_H_
