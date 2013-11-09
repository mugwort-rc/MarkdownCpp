/*
 * ElementTree.cpp
 *
 *  Created on: 2013/11/09
 *      Author: mugwort_rc
 */

#include "ElementTree.h"

#include <boost/scoped_ptr.hpp>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/util/XMLUTF8Transcoder.hpp>

#if defined(USE_CPP11)
#include <codecvt>
#elif defined(USE_QT)
#include <QString>
#endif

//! from XQilla

class XStr
{
public :
  XStr(const char* const toTranscode, XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm = 0)
      : str_(0),
        mm_(mm)
    {
      if(toTranscode == 0) {
        str_ = mm_ ? (XMLCh*)mm_->allocate(1 * sizeof(XMLCh)) : new XMLCh[1];
        str_[0] = 0;
      }
      else {
        xercesc::XMLUTF8Transcoder t(0, 512);
        size_t l = xercesc::XMLString::stringLen(toTranscode);
        const size_t needed = l * 2 + 1; // 2 chars per byte is the worst case, + '\0'
        str_ = mm_ ? (XMLCh*)mm_->allocate(needed * sizeof(XMLCh)) : new XMLCh[needed];

        boost::scoped_ptr<unsigned char> charSizes(new unsigned char[needed]);

        XMLSize_t bytesEaten = 0;
        t.transcodeFrom((const XMLByte*)toTranscode, l+1, str_,
                        needed, bytesEaten, charSizes.get());
      }
    }
  ~XStr()
  {
    if(str_) {
      if(mm_) mm_->deallocate(str_);
      else delete [] str_;
    }
  }

  const XMLCh *str() const
  {
    return str_;
  }

  XMLCh *adopt()
  {
    XMLCh *result = (XMLCh*)str_;
    str_ = 0;
    return result;
  }

private :
  XMLCh *str_;
  XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager *mm_;
};

#define X(strg) XStr(strg).str()

namespace markdown{

#if defined(USE_CPP11)

std::wstring wconvert(const std::string& src)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    return convert.from_bytes(src);
}

std::string convert(const std::wstring& src)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    return convert.to_bytes(src);
}

#elif defined(USE_QT)

std::wstring wconvert(const std::string& src)
{
    return QString::fromUtf8(src.data()).toStdWString();
}

std::string convert(const std::wstring& src)
{
    return QString::fromStdWString(src).toUtf8().constData();
}

#else

std::wstring wconvert(const std::string& src)
{
    std::wstring result;
    result.resize(src.size());
    mbstowcs(&result[0], &src[0], src.size());
    return result;
}

std::string convert(const std::wstring& src)
{
    std::string result;
    result.resize(src.size()*4);
    wcstombs(&result[0], &src[0], src.size());
    return std::string(result.c_str());
}

#endif

std::string convert(const XMLCh* src)
{
    const unsigned int length = 1024;
    xercesc::XMLUTF8Transcoder transcoder(nullptr, length);

    const XMLSize_t size = xercesc::XMLString::stringLen(src);
    XMLSize_t eaten = 0, eated = 0;
    std::string result;
    boost::scoped_ptr<unsigned char> buffer(new unsigned char[length+1]);
    while ( size > eated ) {
        XMLSize_t r_size = transcoder.transcodeTo(src+eated, size-eated, buffer.get(), length, eaten, xercesc::XMLTranscoder::UnRep_RepChar);
        result.append(reinterpret_cast<char*>(buffer.get()), r_size);
        eated += eaten;
    }
    return result;
}

std::wstring wconvert(const XMLCh* src)
{
    std::string buff = convert(src);
    return wconvert(buff);
}

class ElementImpl
{
public:
    ElementImpl(xercesc::DOMElement *elem) :
        _elem(elem)
    {}
    xercesc::DOMElement* ptr(void) const
    {
        return this->_elem;
    }
    void update(xercesc::DOMElement *repl)
    {
        this->_elem = repl;
    }

private:
    xercesc::DOMElement* _elem;

};

class ElementTreeImpl
{
public:
    ElementTreeImpl(xercesc::DOMDocument *doc) :
        _doc(doc)
    {}
    ~ElementTreeImpl(void)
    {
        if ( this->_doc ) {
            this->_doc->release();
        }
    }
    xercesc::DOMDocument* ptr(void) const
    {
        return this->_doc;
    }

public:
    static boost::shared_ptr<ElementTreeImpl> generate(const std::wstring &root)
    {
        const XMLCh LS[] = {xercesc::chLatin_L, xercesc::chLatin_S, xercesc::chNull};
        xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(LS);
        return boost::shared_ptr<ElementTreeImpl>(new ElementTreeImpl(impl->createDocument(nullptr, X(convert(root).c_str()), nullptr)));
    }

private:
    xercesc::DOMDocument* _doc;

};

ElementTree ElementTree::InvalidElementTree(ElementTree::Impl(new ElementTreeImpl(nullptr)));
Element Element::InvalidElement(Element::Impl(new ElementImpl(nullptr)));

Element::Element(const Element::Impl &impl) :
    _impl(impl)
{}
Element::Element(const ElementTree& doc) :
    _impl(new ElementImpl(doc._impl->ptr()->getDocumentElement()))
{}
Element::Element(const ElementTree& doc, const std::wstring &name) :
    _impl(new ElementImpl(doc._impl->ptr()->createElement(X(convert(name).c_str()))))
{}
Element::Element(const Element &parent, const std::wstring &name) :
    _impl(new ElementImpl(parent._impl->ptr()->getOwnerDocument()->createElement(X(convert(name).c_str()))))
{}
Element::Element(const Element &copy) :
    _impl(copy._impl)
{}
Element::Element(Element &&move) :
    _impl(move._impl)
{
    move._impl.reset();
}

Element& Element::operator =(const Element& rhs)
{
    this->_impl = rhs._impl;
    return *this;
}

bool Element::operator ==(const Element& rhs) const
{
    return this->_impl->ptr()->isSameNode(rhs._impl->ptr());
}

bool Element::operator !=(const Element& rhs) const
{
    return ! Element::operator ==(rhs);
}

ElementTree Element::getOwnerDocument(void) const
{
    return ElementTree(boost::shared_ptr<ElementTreeImpl>(new ElementTreeImpl(this->_impl->ptr()->getOwnerDocument())));
}

Element::List Element::child(void) const
{
    List result;
    xercesc::DOMElement* next = this->_impl->ptr()->getFirstElementChild();
    while ( next ) {
        result.push_back(Element(Impl(new ElementImpl(next))));
        next = next->getNextElementSibling();
    }
    return result;
}
Element::List Element::getElementsByTagName(const std::wstring &name) const
{
    List result;
    xercesc::DOMNodeList* list = this->_impl->ptr()->getElementsByTagName(X(convert(name).c_str()));
    for ( XMLSize_t i = 0; i < list->getLength(); ++i ) {
        result.push_back(Element(Impl(new ElementImpl(reinterpret_cast<xercesc::DOMElement*>(list->item(i))))));
    }
    return result;
}
Element Element::getFirstElementChild(void) const
{
    return Element(Impl(new ElementImpl(this->_impl->ptr()->getFirstElementChild())));
}
Element Element::getLastElementChild(void) const
{
    return Element(Impl(new ElementImpl(this->_impl->ptr()->getLastElementChild())));
}
Element Element::getNextElementSibling(void) const
{
    return Element(Impl(new ElementImpl(this->_impl->ptr()->getNextElementSibling())));
}

bool Element::isNull(void) const
{
    return this->_impl->ptr() == nullptr;
}
std::wstring Element::getTagName(void) const
{
    return wconvert(this->_impl->ptr()->getTagName());
}

void Element::setAttribute(const std::wstring &key, const std::wstring &val)
{
    this->_impl->ptr()->setAttribute(X(convert(key).c_str()), X(convert(val).c_str()));
}

Element::Attributes Element::getAttributes(void) const
{
    Attributes result;
    xercesc::DOMNamedNodeMap* attrs = this->_impl->ptr()->getAttributes();
    for ( XMLSize_t i = 0; i < attrs->getLength(); ++i ) {
        xercesc::DOMNode* attr = attrs->item(i);
        result[wconvert(attr->getNodeName())] = wconvert(attr->getNodeValue());
    }
    return result;
}

std::wstring Element::getNamespaceURI(void) const
{
    return wconvert(this->_impl->ptr()->getNamespaceURI());
}

std::wstring Element::getTextContent(void) const
{
    return wconvert(this->_impl->ptr()->getTextContent());
}

bool Element::hasText(void) const
{
    xercesc::DOMElement* elem = this->_impl->ptr();
    return elem->hasChildNodes() && elem->getFirstChild()->getNodeType() == xercesc::DOMNode::TEXT_NODE;
}
bool Element::hasTail(void) const
{
    xercesc::DOMElement* elem = this->_impl->ptr();
    return elem->getNextSibling() && elem->getNextSibling()->getNodeType() == xercesc::DOMNode::TEXT_NODE;
}

void Element::removeText(void)
{
    if ( this->hasText() ) {
        xercesc::DOMElement* elem = this->_impl->ptr();
        elem->removeChild(elem->getFirstChild());
    }
}
void Element::removeTail(void)
{
    if ( this->hasTail() ) {
        xercesc::DOMElement* elem = this->_impl->ptr();
        elem->getParentNode()->removeChild(elem->getNextSibling());
    }
}

void Element::setText(const std::wstring &text)
{
    if ( this->hasText() ) {
        this->_impl->ptr()->getFirstChild()->setNodeValue(X(convert(text).c_str()));
    } else {
        xercesc::DOMElement* elem = this->_impl->ptr();
        xercesc::DOMDocument* doc = elem->getOwnerDocument();
        xercesc::DOMText* node = doc->createTextNode(X(convert(text).c_str()));
        if ( elem->hasChildNodes() ) {
            elem->insertBefore(node, elem->getFirstChild());
        } else {
            elem->appendChild(node);
        }
    }
}
void Element::setTail(const std::wstring &tail)
{
    if ( this->hasTail() ) {
        this->_impl->ptr()->getNextSibling()->setNodeValue(X(convert(tail).c_str()));
    } else {
        xercesc::DOMElement* elem = this->_impl->ptr();
        xercesc::DOMDocument* doc = elem->getOwnerDocument();
        xercesc::DOMText* node = doc->createTextNode(X(convert(tail).c_str()));
        if ( elem->getNextSibling() ) {
            elem->getParentNode()->insertBefore(node, elem->getNextSibling());
        } else {
            if ( elem->getParentNode() ) {
                elem->getParentNode()->appendChild(node);
            } else {
                Element dummy(Impl(new ElementImpl(doc->createElement(X("dummy")))));
                dummy.append(*this);
                this->setTail(tail);
            }
        }
    }
}

std::wstring Element::text(void) const
{
    if ( this->hasText() ) {
        return wconvert(this->_impl->ptr()->getFirstChild()->getNodeValue());
    }
    return std::wstring();
}
std::wstring Element::tail(void) const
{
    if ( this->hasTail() ) {
        return wconvert(this->_impl->ptr()->getNextSibling()->getNodeValue());
    }
    return std::wstring();
}

void Element::append(Element &child)
{
    bool tail = child.hasTail();
    //! @warning: node is text!!!
    Impl node(new ElementImpl((xercesc::DOMElement*)child._impl->ptr()->getNextSibling()));
    xercesc::DOMElement* elem = this->_impl->ptr();
    child._impl->update(reinterpret_cast<xercesc::DOMElement*>(elem->appendChild(this->import(child._impl)->ptr())));
    if ( tail ) {
        elem->appendChild(this->import(node)->ptr());
    }
}
void Element::insertBefore(Element &child, const Element &ref)
{
    if ( ref.isNull() ) {
        //throw ...
        return;
    }
    bool tail = child.hasTail();
    //! @warning: node is text!!!
    Impl node(new ElementImpl((xercesc::DOMElement*)child._impl->ptr()->getNextSibling()));
    xercesc::DOMElement* elem = this->_impl->ptr();
    xercesc::DOMElement* refChild = ref._impl->ptr();
    child._impl->update(reinterpret_cast<xercesc::DOMElement*>(elem->insertBefore(this->import(child._impl)->ptr(), refChild)));
    if ( tail ) {
        elem->insertBefore(this->import(node)->ptr(), refChild);
    }
}
void Element::remove(Element &child)
{
    xercesc::DOMElement* elem = this->_impl->ptr();
    if ( elem->isSameNode(child._impl->ptr()->getParentNode()) ) {
        bool tail = child.hasTail();
        xercesc::DOMNode* node = child._impl->ptr()->getNextSibling();
        child._impl->update(reinterpret_cast<xercesc::DOMElement*>(elem->removeChild(child._impl->ptr())));
        if ( tail ) {
            elem->removeChild(node);
        }
    }
}

Element::Impl Element::import(Impl &target, bool deep)
{
    xercesc::DOMNode* self = this->_impl->ptr();
    if ( ! self->getOwnerDocument()->isSameNode(target->ptr()->getOwnerDocument()) ) {
        target->update(reinterpret_cast<xercesc::DOMElement*>(self->getOwnerDocument()->importNode(target->ptr(), deep)));
    }
    return target;
}

ElementTree::ElementTree(const std::wstring &root) :
    _impl(ElementTreeImpl::generate(root))
{}

ElementTree::ElementTree(const Impl &impl) :
    _impl(impl)
{}

ElementTree::ElementTree(const ElementTree &copy) :
    _impl(copy._impl)
{}

ElementTree::ElementTree(ElementTree &&move) :
    _impl(move._impl)
{
    move._impl.reset();
}

ElementTree& ElementTree::operator =(const ElementTree& rhs)
{
    this->_impl = rhs._impl;
    return *this;
}

} // end of namespace markdown
