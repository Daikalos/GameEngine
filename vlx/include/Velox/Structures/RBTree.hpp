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
		std::optional<const T&> Search(const T& key) const;

		void Insert(const T& key);
		void Delete(const T& key);

	private:
		void RotateLeft(NodePtr&& ptr);
		void RotateRight(NodePtr&& ptr);

		auto SearchImpl1(const T& key) const -> const Node*;
		auto SearchImpl2(const Node* ptr, const T& key) const -> const Node*;

		void InsertImpl(NodePtr ptr);
		void InsertFixup(NodePtr&& ptr);

	private:
		NodePtr m_root;
	};

	template<typename T>
	inline std::optional<const T&> RBTree<T>::Search(const T& key) const
	{
		const Node* node = SearchHelper1(key);
		return node ? std::make_optional(node->key) : std::nullopt;
	}

	template<typename T>
	inline void RBTree<T>::Insert(const T& key)
	{
		InsertImpl(std::make_unique<Node>(key));
	}

	template<typename T>
	inline void RBTree<T>::Delete(const T& key)
	{
	}

	template<typename T>
	inline void RBTree<T>::RotateLeft(NodePtr&& ptr)
	{
		assert(ptr->right != nullptr);

		NodePtr right_child = std::move(ptr->right);
		ptr->right = std::move(right_child->left);

		if (ptr->right)
			ptr->right->parent = ptr.get();

		right_child->parent = ptr->parent;

		NodePtr& node = (!ptr->parent) ? m_root : ((ptr == ptr->parent->left) ?
			ptr->parent->left : ptr->parent->right);

		Node* temp = ptr.release();
		node = std::move(right_child);

		node->left = NodePtr(temp);
		node->left->parent = node.get();
	}

	template<typename T>
	inline void RBTree<T>::RotateRight(NodePtr&& ptr)
	{
		assert(ptr->left != nullptr);

		NodePtr left_child = std::move(ptr->left);
		ptr->left = std::move(left_child->right);

		if (ptr->left)
			ptr->left->parent = ptr.get();

		left_child->parent = ptr->parent;

		NodePtr& node = (!ptr->parent) ? m_root : ((ptr == ptr->parent->left) ?
			ptr->parent->left : ptr->parent->right);

		Node* temp = ptr.release();
		node = std::move(left_child);

		node->right = NodePtr(temp);
		node->right->parent = node.get();
	}

	template<typename T>
	inline auto RBTree<T>::SearchImpl1(const T& key) const -> const Node*
	{
		return SearchImpl2(m_root.get(), key);
	}
	template<typename T>
	inline auto RBTree<T>::SearchImpl2(const Node* ptr, const T& key) const -> const Node*
	{
		if (!ptr || ptr->key == key)
			return ptr;

		return (key < ptr->key) ?
			SearchImpl2(ptr->left.get(), key) :
			SearchImpl2(ptr->right.get(), key);
	}

	template<typename T>
	inline void RBTree<T>::InsertImpl(NodePtr ptr)
	{
		Node* y = nullptr; 
		Node* x = m_root.get();

		while (x != nullptr) // find leaf
		{
			y = x; // save parent
			x = (ptr->key < x->key) ? x->left.get() : x->right.get();
		}

		ptr->parent = y;

		NodePtr& node = !y ? m_root : ((ptr->key < y->key) ? y->left : y->right);
		assert(node == nullptr); // should be leaf

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

					NodePtr& node = (!grandparent->parent) ? m_root : ((grandparent == grandparent->parent->left.get()) ?
						grandparent->parent->left : grandparent->parent->right);

					RotateRight(std::move(node));
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

					NodePtr& node = (!grandparent->parent) ? m_root : ((grandparent == grandparent->parent->left.get()) ?
						grandparent->parent->left : grandparent->parent->right);

					RotateLeft(std::move(node));
				}
			}
		}

		m_root->color = Color::Black;
	}
}