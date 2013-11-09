/*
 * BlockProcessors.h
 *
 *  Created on: 2013/10/27
 *      Author: mugwort_rc
 */

#ifndef BLOCKPROCESSORS_H_
#define BLOCKPROCESSORS_H_

#include <boost/tuple/tuple.hpp>

#include "ElementTree.h"
#include "odict.h"

namespace markdown{

class Markdown;     //!< forward declaration
class BlockParser;  //!< forward declaration

class BlockProcessor
{
public:
    BlockProcessor(BlockParser *parser);
	virtual ~BlockProcessor();

    /*!
     * Return the last child of an etree element.
     */
    Element lastChild(const Element &parent);
    /*!
     * Remove a tab from the front of each line of the given text.
     */
    boost::tuples::tuple<std::wstring, std::wstring> detab(const std::wstring &text);
	/*!
	 * Remove a tab from front of lines but allowing dedented lines.
	 */
    std::wstring looseDetab(const std::wstring &text, unsigned int level=1);

	/*!
	 * Test for block type. Must be overridden by subclasses.
	 *
	 *   As the parser loops through processors, it will call the ``test`` method
	 *   on each to determine if the given block of text is of that type. This
	 *   method must return a boolean ``True`` or ``False``. The actual method of
	 *   testing is left to the needs of that particular block type. It could
	 *   be as simple as ``block.startswith(some_string)`` or a complex regular
	 *   expression. As the block type may be different depending on the parent
	 *   of the block (i.e. inside a list), the parent etree element is also
	 *   provided and may be used as part of the test.
     *
	 *   Keywords:
	 *
	 *   * ``parent``: A etree element which will be the parent of the block.
	 *   * ``block``: A block of text from the source which has been split at
	 *       blank lines.
	 */
    virtual bool test(Element &parent, const std::wstring &block) = 0;
	/*!
	 * Run processor. Must be overridden by subclasses.
	 *
	 *   When the parser determines the appropriate type of a block, the parser
	 *   will call the corresponding processor's ``run`` method. This method
	 *   should parse the individual lines of the block and append them to
	 *   the etree.
	 *
	 *   Note that both the ``parent`` and ``etree`` keywords are pointers
	 *   to instances of the objects which should be edited in place. Each
	 *   processor must make changes to the existing objects as there is no
	 *   mechanism to return new/different objects to replace them.
	 *
	 *   This means that this method should be adding SubElements or adding text
	 *   to the parent, and should remove (``pop``) or add (``insert``) items to
	 *   the list of blocks.
	 *
	 *   Keywords:
	 *
	 *   * ``parent``: A etree element which is the parent of the current block.
	 *   * ``blocks``: A list of all remaining blocks of the document.
	 */
    virtual void run(Element &parent, std::list<std::wstring>& block) = 0;

protected:
	BlockParser* parser;
	int tab_length;

};

typedef OrderedDict<BlockProcessor> OrderedDictBlockProcessors;

boost::shared_ptr<BlockParser> build_block_parser(Markdown* md_instance);

} // end of namespace markdown

#endif /* BLOCKPROCESSORS_H_ */
