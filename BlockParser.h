/*
 * BlockParser.h
 *
 *  Created on: 2013/10/27
 *      Author: mugwort_rc
 */

#ifndef BLOCKPARSER_H_
#define BLOCKPARSER_H_

#include "BlockProcessors.h"

namespace markdown{

class Markdown;  //!< forward declaration

/*!
 * Track the current and nested state of the parser.
 *
 * This utility class is used to track the state of the BlockParser and
 * support multiple levels if nesting. It's just a simple API wrapped around
 * a list. Each time a state is set, that state is appended to the end of the
 * list. Each time a state is reset, that state is removed from the end of
 * the list.
 *
 * Therefore, each time a state is set for a nested block, that state must be
 * reset when we back out of that level of nesting or the state could be
 * corrupted.
 *
 * While all the methods of a list object are available, only the three
 * defined below need be used.
 */
class State
{
public:
    State()
    {}

    /*!
     * Set a new state.
     */
    void set(const std::wstring &state)
    {
        this->list.push_back(state);
    }
    /*!
     * Step back one step in nested state.
     */
    void reset(void)
    {
        this->list.pop_back();
    }
    /*!
     * Test that top (current) level is of given state.
     */
    bool isstate(const std::wstring &state)
    {
        if ( ! this->list.empty() ) {
            return this->list.back() == state;
        }
        return false;
    }

private:
    std::list<std::wstring> list;

};

/*!
 * Parse Markdown blocks into an ElementTree object.
 *
 *  A wrapper class that stitches the various BlockProcessors together,
 *  looping through them and creating an ElementTree object.
 */
class BlockParser {
public:
    BlockParser(Markdown *markdown);

	/*!
	 * Parse a markdown document into an ElementTree.
	 *
	 *   Given a list of lines, an ElementTree object (not just a parent Element)
	 *   is created and the root element is passed to the parser as the parent.
	 *   The ElementTree object is returned.
	 *
	 *   This should only be called on an entire document, not pieces.
	 */
    ElementTree parseDocument(const std::list<std::wstring> &lines);

	/*!
	 * Parse a chunk of markdown text and attach to given etree node.
	 *
	 *   While the ``text`` argument is generally assumed to contain multiple
	 *   blocks which will be split on blank lines, it could contain only one
	 *   block. Generally, this method would be called by extensions when
	 *   block parsing is required.
	 *
	 *   The ``parent`` etree Element passed in is altered in place.
	 *   Nothing is returned.
	 */
    void parseChunk(Element &parent, const std::wstring &text);

	/*!
	 * Process blocks of markdown text and attach to given etree node.
	 *
	 *   Given a list of ``blocks``, each blockprocessor is stepped through
	 *   until there are no blocks left. While an extension could potentially
	 *   call this method directly, it's generally expected to be used internally.
	 *
	 *   This is a public method as an extension may need to add/alter additional
	 *   BlockProcessors which call this method to recursively parse a nested
	 *   block.
     */
    void parseBlocks(Element &parent, std::list<std::wstring> &blocks);

public:
	Markdown* markdown;
	OrderedDictBlockProcessors blockprocessors;
    State state;
    ElementTree root;

};

} // end of namespace markdown

#endif /* BLOCKPARSER_H_ */
