#pragma once

#include <memory>

#include <Velox/System/Concepts.h>
#include <Velox/Utility/NonCopyable.h>

namespace vlx
{
	template<typename T>
	class RBTree : private NonCopyable
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
		RBTree(RBTree&& rhs) noexcept;

		RBTree& operator=(RBTree&& rhs) noexcept;

	public:
		const T* Search(const T& key) const;

		void Insert(const T& key);
		bool Delete(const T& key);

		void Clear();

	private:
		auto SearchImpl1(const T& key) const -> Node*;
		auto SearchImpl2(Node* node, const T& key) const -> Node*;

		void RotateLeft(NodePtr&& ptr);
		void RotateRight(NodePtr&& ptr);

		void InsertImpl(NodePtr ptr);
		void InsertFixup(NodePtr&& ptr);

		void DeleteImpl(Node* node);
		void DeleteFixup(Node* node, Node* parent);

		auto Transplant(Node* u, NodePtr&& v) -> NodePtr;
		auto Minimum(Node* node) -> Node*;

		auto GetPtr(Node* node) -> NodePtr&;

	private:
		NodePtr m_root;
	};

	template<typename T>
	inline RBTree<T>::RBTree(RBTree&& rhs) noexcept
		: m_root(std::move(rhs.m_root))
	{

	}

	template<typename T>
	inline RBTree<T>& RBTree<T>::operator=(RBTree&& rhs) noexcept
	{
		m_root = std::move(rhs.m_root);
	}

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

		assert(node->key == key);
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
	inline auto RBTree<T>::SearchImpl2(Node* node, const T& key) const -> Node*
	{
		if (!node || node->key == key)
			return node;

		return (key < node->key) ?
			SearchImpl2(node->left.get(), key) :
			SearchImpl2(node->right.get(), key);
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

					RotateRight(std::move(GetPtr(grandparent)));
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

					RotateLeft(std::move(GetPtr(grandparent)));
				}
			}
		}

		m_root->color = Color::Black;
	}

	template<typename T>
	inline void RBTree<T>::DeleteImpl(Node* node)
	{
		if (!node)
			return;

		Color original	= node->color;
		Node* x			= nullptr;
		Node* xp		= nullptr;

		if (!node->left)
		{
			x	= node->right.get();
			xp	= node->parent;

			Transplant(node, std::move(node->right));
		}
		else if (!node->right)
		{
			x	= node->left.get();
			xp	= node->parent;

			Transplant(node, std::move(node->left));
		}
		else
		{
			Node* y = Minimum(node->right.get());
			original = y->color;

			x = y->right.get();
			xp = y;

			if (y->parent == node)
			{
				if (x != nullptr)
					x->parent = y;

				NodePtr pn		= Transplant(node, std::move(node->right));
				y->left			= std::move(node->left);
				y->left->parent = y;
				y->color		= node->color;
			}
			else 
			{
				xp = y->parent;

				NodePtr py			= Transplant(y, std::move(y->right));
				y->right			= std::move(node->right);
				y->right->parent	= y;

				NodePtr pn			= Transplant(node, std::move(py));
				y->left				= std::move(node->left);
				y->left->parent		= y;
				y->color			= node->color;
			}
		}

		if (original == Color::Black)
			DeleteFixup(x, xp);
	}
	template<typename T>
	inline void RBTree<T>::DeleteFixup(Node* node, Node* parent)
	{
		while (node != m_root.get() && (!node || node->color == Color::Black))
		{
			if (node == parent->left.get())
			{
				Node* sibling = parent->right.get();
				if (sibling && sibling->color == Color::Red)
				{
					sibling->color	= Color::Black;
					parent->color	= Color::Red;

					RotateLeft(std::move(GetPtr(parent)));

					sibling = parent->right.get();
				}

				if (sibling && (!sibling->left || sibling->left->color == Color::Black) && (!sibling->right || sibling->right->color == Color::Black)) 
				{
					sibling->color	= Color::Red;
					parent->color	= Color::Black;

					node = parent;
				}
				else if (sibling) 
				{
					if (!sibling->right || sibling->right->color == Color::Black)
					{
						sibling->color			= Color::Red;
						sibling->left->color	= Color::Black;

						RotateRight(std::move(GetPtr(sibling)));

						sibling = parent->right.get();
					}

					sibling->color			= parent->color;
					parent->color			= Color::Black;
					sibling->right->color	= Color::Black;

					RotateLeft(std::move(GetPtr(parent)));

					node = m_root.get();
				}
				else
				{
					node = m_root.get();
				}
			}
			else
			{
				Node* sibling = parent->left.get();
				if (sibling && sibling->color == Color::Red)
				{
					sibling->color		= Color::Black;
					parent->color	= Color::Red;

					RotateRight(std::move(GetPtr(parent)));

					sibling = parent->left.get();
				}

				if (sibling && (!sibling->left || sibling->left->color == Color::Black) && (!sibling->right || sibling->right->color == Color::Black)) 
				{
					sibling->color	= Color::Red;
					parent->color	= Color::Black;

					node = parent;
				}
				else if (sibling) 
				{
					if (!sibling->left || sibling->left->color == Color::Black)
					{
						sibling->color			= Color::Red;
						sibling->right->color	= Color::Black;

						RotateLeft(std::move(GetPtr(sibling)));

						sibling = parent->left.get();
					}

					sibling->color			= parent->color;
					parent->color			= Color::Black;
					sibling->left->color	= Color::Black;

					RotateRight(std::move(GetPtr(parent)));

					node = m_root.get();
				}
				else
				{
					node = m_root.get();
				}
			}
		}

		if (node)
			node->color = Color::Black;
	}

	template<typename T>
	inline auto RBTree<T>::Transplant(Node* u, NodePtr&& v) -> NodePtr
	{
		assert(u != nullptr);

		if (v != nullptr)
			v->parent = u->parent;

		NodePtr& node	= GetPtr(u);
		Node* temp		= node.release();
		node			= std::move(v);

		return NodePtr(temp);
	}
	template<typename T>
	inline auto RBTree<T>::Minimum(Node* node) -> Node*
	{
		if (!node)
			return node;

		while (node->left)
			node = node->left.get();

		return node;
	}

	template<typename T>
	inline auto RBTree<T>::GetPtr(Node* node) -> NodePtr&
	{
		assert(node != nullptr);
		return (!node->parent) ? m_root : ((node == node->parent->left.get()) ?
			node->parent->left : node->parent->right);
	}
}