/*
 * TreeProcessors.h
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#ifndef TREEPROCESSORS_H_
#define TREEPROCESSORS_H_

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>

#include "ElementTree.h"
#include "odict.h"

namespace markdown{

class Markdown;  //!< forward declaration

/*!
 * Treeprocessors are run on the ElementTree object before serialization.
 *
 * Each Treeprocessor implements a "run" method that takes a pointer to an
 * ElementTree, modifies it as necessary and returns an ElementTree
 * object.
 *
 * Treeprocessors must extend markdown.Treeprocessor.
 *
 */
class TreeProcessor
{
public:
    TreeProcessor(Markdown* md_instance);
    virtual ~TreeProcessor(void);

    virtual Element run(Element& root) = 0;

public:
    Markdown* markdown;
    typedef boost::tuples::tuple<boost::optional<std::wstring>, boost::optional<Element>> NodeItem;
    typedef std::map<std::wstring, NodeItem> StashNodes;
    StashNodes stashed_nodes;


};

typedef OrderedDict<TreeProcessor> OrderedDictTreeProcessors;

/*!
 * Build the default treeprocessors for Markdown.
 */
OrderedDictTreeProcessors build_treeprocessors(Markdown* md_instance);

} // end of namespace markdown

#endif // TREEPROCESSORS_H_
