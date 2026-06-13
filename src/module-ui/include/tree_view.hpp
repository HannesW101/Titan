#pragma once

// ============================================================================
// Includes
// ----------------------------------------------------------------------------

#include "module-ui/include/widget.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace titan::ui {

// ============================================================================
// Class Tree_view
// ----------------------------------------------------------------------------

/** @brief Expandable tree of labeled nodes. */
class Tree_view final : public Widget {

public:
// ============================================================================
// Class Node
// ----------------------------------------------------------------------------

    struct Node {
        std::string label;
        bool expanded=false;
        std::vector<std::unique_ptr<Node>> children;
        /**
         * @brief Add child.
         * @param l l
         */
        Node* add_child(std::string const& l){ auto n=std::make_unique<Node>(); n->label=l; Node* r=n.get(); children.push_back(std::move(n)); return r; }
    };
    /**
     * @brief Construct a Tree_view.
     * @param name Widget name (unique within its parent)
     */
    explicit Tree_view(std::string const& name);
    /**
     * @brief Get the root node.
     * @return The root node
     */
    Node& root_node();
    /**
     * @brief Set the row height.
     * @param h New row height
     */
    void set_row_height(float h);
    /**
     * @brief Set the indent.
     * @param i New indent
     */
    void set_indent(float i);
    /**
     * @brief Set the "on selected" callback.
     * @param cb Callback to invoke
     */
    void set_on_selected(std::function<void(Node const&)> cb);

protected:
    /**
     * @brief Handle the render event.
     * @param renderer renderer
     */
    void on_render(render::Renderer& renderer) override;
    /**
     * @brief Get the theme style key for this widget.
     * @return Style key string
     */
    std::string style_key() const override { return "tree_view"; }

private:
    Node _root; float _row=22.f,_indent=16.f;
    std::function<void(Node const&)> _on_selected;
    std::uint64_t _click=0;
    /**
     * @brief Render node.
     * @param n n
     * @param renderer renderer
     * @param y y
     * @param indent indent
     * @param font_ptr font ptr
     */
    void _render_node(Node const& n,render::Renderer& renderer,float& y,float indent,void* font_ptr);
    /**
     * @brief Hit.
     * @param n n
     * @param y y
     * @param my my
     */
    Node* _hit(Node& n,float& y,float my);
};
} // namespace titan::ui
