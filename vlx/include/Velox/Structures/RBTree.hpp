#pragma once

#include <memory>
#include <optional>

namespace vlx
{
	template<typename T>
	class RBTree
	{
	private:
		enum class Color : bool
		{
			Red		= false,
			Black	= true
		};

		struct Node;

		using NodePtr = std::unique_ptr<Node>;

		struct Node
		{
			T		key;
			Color	color	{Color::Red};
			NodePtr left	{nullptr};
			NodePtr right	{nullptr};
			Node*	parent	{nullptr};
		};

	public:
		RBTree() = default;

	public:
		const T* Search(const T& key) const;

		void Insert(const T& key);
		bool Delete(const T& key);

		void Clear();

	private:
		auto SearchImpl1(const T& key) const -> Node*;
		auto SearchImpl2(Node* ptr, const T& key) const -> Node*;

		void RotateLeft(NodePtr&& ptr);
		void RotateRight(NodePtr&& ptr);

		void InsertImpl(NodePtr ptr);
		void InsertFixup(NodePtr&& ptr);

		void DeleteImpl(Node* ptr);
		void DeleteFixup(Node* ptr, Node* parent);

		auto Transplant(Node* u, NodePtr&& v) -> NodePtr;
		auto Minimum(Node* ptr) -> Node*;

		auto GetUniquePtr(Node* ptr) -> NodePtr&;

	private:
		NodePtr m_root;
	};

	template<typename T>
	inline const T* RBTree<T>::Search(const T& key) const
	{
		const Node* node = SearchImpl1(key);
		return node ? &node->key : nullptr;
	}

	template<typename T>
	inline void RBTree<T>::Insert(const T& key)
	{
		InsertImpl(std::make_unique<Node>(key));
	}

	template<typename T>
	inline bool RBTree<T>::Delete(const T& key)
	{
		Node* node = SearchImpl1(key);
		if (node == nullptr)
			return false;

		DeleteImpl(node);

		return true;
	}

	template<typename T>
	inline void RBTree<T>::Clear()
	{
		m_root.reset();
	}

	template<typename T>
	inline auto RBTree<T>::SearchImpl1(const T& key) const -> Node*
	{
		return SearchImpl2(m_root.get(), key);
	}
	template<typename T>
	inline auto RBTree<T>::SearchImpl2(Node* ptr, const T& key) const -> Node*
	{
		if (!ptr || ptr->key == key)
			return ptr;

		return (key < ptr->key) ?
			SearchImpl2(ptr->left.get(), key) :
			SearchImpl2(ptr->right.get(), key);
	}

	template<typename T>
	inline void RBTree<T>::RotateLeft(NodePtr&& ptr)
	{
		assert(ptr->right != nullptr && "Right child should exist if rotating left");

		NodePtr right_child = std::move(ptr->right);
		ptr->right = std::move(right_child->left);

		if (ptr->right)
			ptr->right->parent = ptr.get();

		right_child->parent = ptr->parent;

		assert((ptr->parent && ptr == ptr->parent->left) ||
			   (ptr->parent && ptr == ptr->parent->right) ||
			   (!ptr->parent && ptr == m_root));

		Node* temp = ptr.release();
		ptr = std::move(right_child);

		assert(ptr->left == nullptr);

		ptr->left = NodePtr(temp);
		ptr->left->parent = ptr.get();
	}

	template<typename T>
	inline void RBTree<T>::RotateRight(NodePtr&& ptr)
	{
		assert(ptr->left != nullptr && "Left child should exist if rotating right");

		NodePtr left_child = std::move(ptr->left);
		ptr->left = std::move(left_child->right);

		if (ptr->left)
			ptr->left->parent = ptr.get();

		left_child->parent = ptr->parent;

		assert((ptr->parent && ptr == ptr->parent->left) ||
			   (ptr->parent && ptr == ptr->parent->right) ||
			   (!ptr->parent && ptr == m_root));

		Node* temp = ptr.release();
		ptr = std::move(left_child);

		assert(ptr->right == nullptr);

		ptr->right = NodePtr(temp);
		ptr->right->parent = ptr.get();
	}

	template<typename T>
	inline void RBTree<T>::InsertImpl(NodePtr ptr)
	{
		if (m_root == nullptr) // first insert
		{
			m_root			= std::move(ptr);
			m_root->color	= Color::Black;
			return;
		}

		Node* y = nullptr; 
		Node* x = m_root.get();

		while (x != nullptr) // find leaf
		{
			y = x; // save parent
			x = (ptr->key < x->key) ? x->left.get() : x->right.get();
		}

		assert(y != nullptr && "Should always exist a parent for a leaf (disregarding first insert)");

		ptr->parent = y;

		NodePtr& node = (ptr->key < y->key) ? y->left : y->right;
		assert(node == nullptr && "Leaf should not exist yet");

		node = std::move(ptr);
		InsertFixup(std::move(node));
	}

	template<typename T>
	inline void RBTree<T>::InsertFixup(NodePtr&& ptr)
	{
		Node* parent = ptr->parent;
		while (parent && parent->color == Color::Red)
		{
			Node* grandparent = parent->parent;
			if (parent == grandparent->left.get())
			{
				Node* uncle = grandparent->right.get();
				if (uncle && uncle->color == Color::Red)
				{
					uncle->color		= Color::Black;
					parent->color		= Color::Black;
					grandparent->color	= Color::Red;
					parent				= grandparent->parent;
				}
				else
				{
					if (ptr == parent->right)
					{
						RotateLeft(std::move(grandparent->left)); // use grandparent->left to access unique pointer for parent
						parent = grandparent->left.get();
					}

					parent->color		= Color::Black;
					grandparent->color	= Color::Red;

					RotateRight(std::move(GetUniquePtr(grandparent)));
				}
			}
			else
			{
				Node* uncle = grandparent->left.get();
				if (uncle && uncle->color == Color::Red)
				{
					uncle->color		= Color::Black;
					parent->color		= Color::Black;
					grandparent->color	= Color::Red;
					parent				= grandparent->parent;
				}
				else
				{
					if (ptr == parent->left)
					{
						RotateRight(std::move(grandparent->right));
						parent = grandparent->right.get();
					}

					parent->color		= Color::Black;
					grandparent->color	= Color::Red;

					RotateLeft(std::move(GetUniquePtr(grandparent)));
				}
			}
		}

		m_root->color = Color::Black;
	}

	template<typename T>
	inline void RBTree<T>::DeleteImpl(Node* ptr)
	{
		if (!ptr)
			return;

		Color original = ptr->color;

	}
	template<typename T>
	inline void RBTree<T>::DeleteFixup(Node* ptr, Node* parent)
	{

	}

	template<typename T>
	inline auto RBTree<T>::Transplant(Node* u, NodePtr&& v) -> NodePtr
	{
		assert(u != nullptr && v != nullptr);

		v->parent = u->parent;

		NodePtr& node = GetUniquePtr(u);

		Node* ptr = node.release();
		node = std::move(v);

		return NodePtr(ptr);
	}
	template<typename T>
	inline auto RBTree<T>::Minimum(Node* ptr) -> Node*
	{
		if (!ptr)
			return ptr;

		return Minimum(ptr->left.get());
	}

	template<typename T>
	inline auto RBTree<T>::GetUniquePtr(Node* ptr) -> NodePtr&
	{
		assert(ptr != nullptr);
		return (!ptr->parent) ? m_root : ((ptr == ptr->parent->left.get()) ?
			ptr->parent->left : ptr->parent->right);
	}
}