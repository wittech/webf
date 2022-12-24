/*
 * Copyright (C) 2019-2022 The Kraken authors. All rights reserved.
 * Copyright (C) 2022-present The WebF authors. All rights reserved.
 */
#ifndef BRIDGE_DOCUMENT_H
#define BRIDGE_DOCUMENT_H

#include "bindings/qjs/cppgc/local_handle.h"
#include "container_node.h"
#include "scripted_animation_controller.h"
#include "tree_scope.h"

namespace webf {

class HTMLBodyElement;
class HTMLHeadElement;
class HTMLHtmlElement;
class Text;
class Comment;

enum NodeListInvalidationType : int {
  kDoNotInvalidateOnAttributeChanges = 0,
  kInvalidateOnClassAttrChange,
  kInvalidateOnIdNameAttrChange,
  kInvalidateOnNameAttrChange,
  kInvalidateOnForAttrChange,
  kInvalidateForFormControls,
  kInvalidateOnHRefAttrChange,
  kInvalidateOnAnyAttrChange,
};
const int kNumNodeListInvalidationTypes = kInvalidateOnAnyAttrChange + 1;

// A document (https://dom.spec.whatwg.org/#concept-document) is the root node
// of a tree of DOM nodes, generally resulting from the parsing of a markup
// (typically, HTML) resource.
class Document : public ContainerNode, public TreeScope {
  DEFINE_WRAPPERTYPEINFO();

 public:
  using ImplType = Document*;

  explicit Document(ExecutingContext* context);

  static Document* Create(ExecutingContext* context, ExceptionState& exception_state);

  Element* createElement(const AtomicString& name, ExceptionState& exception_state);
  Element* createElement(const AtomicString& name, const ScriptValue& options, ExceptionState& exception_state);
  Text* createTextNode(const AtomicString& value, ExceptionState& exception_state);
  DocumentFragment* createDocumentFragment(ExceptionState& exception_state);
  Comment* createComment(const AtomicString& data, ExceptionState& exception_state);
  Event* createEvent(const AtomicString& type, ExceptionState& exception_state);
  HTMLAllCollection* all();

  [[nodiscard]] std::string nodeName() const override;
  [[nodiscard]] AtomicString nodeValue() const override;
  [[nodiscard]] NodeType nodeType() const override;
  [[nodiscard]] bool ChildTypeAllowed(NodeType) const override;

  Element* querySelector(const AtomicString& selectors, ExceptionState& exception_state);
  std::vector<Element*> querySelectorAll(const AtomicString& selectors, ExceptionState& exception_state);

  Element* getElementById(const AtomicString& id, ExceptionState& exception_state);
  std::vector<Element*> getElementsByClassName(const AtomicString& class_name, ExceptionState& exception_state);
  std::vector<Element*> getElementsByTagName(const AtomicString& tag_name, ExceptionState& exception_state);
  std::vector<Element*> getElementsByName(const AtomicString& name, ExceptionState& exception_state);

  // The following implements the rule from HTML 4 for what valid names are.
  static bool IsValidName(const AtomicString& name);

  Node* Clone(Document&, CloneChildrenFlag) const override;

  [[nodiscard]] HTMLHtmlElement* documentElement() const;

  // "body element" as defined by HTML5
  // (https://html.spec.whatwg.org/C/#the-body-element-2).
  // That is, the first body or frameset child of the document element.
  [[nodiscard]] HTMLBodyElement* body() const;
  void setBody(HTMLBodyElement* body, ExceptionState& exception_state);
  [[nodiscard]] HTMLHeadElement* head() const;
  void setHead(HTMLHeadElement* head, ExceptionState& exception_state);

  ScriptValue location() const;

  void IncrementNodeCount() { node_count_++; }
  void DecrementNodeCount() {
    assert(node_count_ > 0);
    node_count_--;
  }
  int NodeCount() const { return node_count_; }

  uint32_t RequestAnimationFrame(const std::shared_ptr<FrameCallback>& callback, ExceptionState& exception_state);
  void CancelAnimationFrame(uint32_t request_id, ExceptionState& exception_state);

  // Helper functions for forwarding LocalDOMWindow event related tasks to the
  // LocalDOMWindow if it exists.
  void SetWindowAttributeEventListener(const AtomicString& event_type,
                                       const std::shared_ptr<EventListener>& listener,
                                       ExceptionState& exception_state);
  std::shared_ptr<EventListener> GetWindowAttributeEventListener(const AtomicString& event_type);

  bool IsAttributeDefinedInternal(const AtomicString& key) const override;

  void Trace(GCVisitor* visitor) const override;

 private:
  int node_count_{0};
  ScriptAnimationController script_animation_controller_;
};

template <>
struct DowncastTraits<Document> {
  static bool AllowFrom(const Node& node) { return node.IsDocumentNode(); }
};

}  // namespace webf

#endif  // BRIDGE_DOCUMENT_H
