/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/QuickSort.h>
#include <LibWeb/DOM/Node.h>
#include <LibWeb/Layout/LayoutBox.h>
#include <LibWeb/Layout/LayoutDocument.h>
#include <LibWeb/Layout/StackingContext.h>

namespace Web {

StackingContext::StackingContext(LayoutBox& box, StackingContext* parent)
    : m_box(box)
    , m_parent(parent)
{
    ASSERT(m_parent != this);
    if (m_parent) {
        m_parent->m_children.append(this);

        // FIXME: Don't sort on every append..
        quick_sort(m_children, [](auto& a, auto& b) {
            return a->m_box.style().z_index().value_or(0) < b->m_box.style().z_index().value_or(0);
        });
    }
}

void StackingContext::render(RenderingContext& context)
{
    if (!m_box.is_root()) {
        m_box.render(context);
    } else {
        // NOTE: LayoutDocument::render() merely calls StackingContext::render()
        //       so we call its base class instead.
        to<LayoutDocument>(m_box).LayoutBlock::render(context);
    }
    for (auto* child : m_children) {
        child->render(context);
    }
}

void StackingContext::dump(int indent) const
{
    for (int i = 0; i < indent; ++i)
        dbgprintf(" ");
    dbgprintf("SC for %s{%s} %s [children: %zu]\n", m_box.class_name(), m_box.node() ? m_box.node()->node_name().characters() : "(anonymous)", m_box.absolute_rect().to_string().characters(), m_children.size());
    for (auto& child : m_children)
        child->dump(indent + 1);
}

}
