/*
 * PostProcessors.h
 *
 *  Created on: 2013/10/31
 *      Author: mugwort_rc
 */

#ifndef POSTPROCESSORS_H_
#define POSTPROCESSORS_H_

/*!
 * POST-PROCESSORS
 * =============================================================================
 *
 * Markdown also allows post-processors, which are similar to preprocessors in
 * that they need to implement a "run" method. However, they are run after core
 * processing.
 *
 */

#include "odict.h"

namespace markdown{

class Markdown;  //!< forward declaration

/*!
 * Postprocessors are run after the ElementTree it converted back into text.
 *
 * Each Postprocessor implements a "run" method that takes a pointer to a
 * text string, modifies it as necessary and returns a text string.
 *
 * Postprocessors must extend markdown.Postprocessor.
 *
 */
class PostProcessor
{
public:
    PostProcessor(Markdown* markdown_instance);
    virtual ~PostProcessor(void);

    /*!
     * Subclasses of Postprocessor should implement a `run` method, which
     * takes the html document as a single text string and returns a
     * (possibly modified) string.
     *
     */
    virtual std::wstring run(const std::wstring& text) = 0;

public:
    Markdown* markdown;

};

typedef OrderedDict<PostProcessor> OrderedDictPostProcessors;

OrderedDictPostProcessors build_postprocessors(Markdown* md_instance);

} // end of namespace markdown

#endif // POSTPROCESSORS_H_
