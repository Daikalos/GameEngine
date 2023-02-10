#include <Velox/Algorithms/LQuadTree.hpp>

using namespace vlx;

LQuadTree::LQuadTree(const RectFloat& root_Rect, int max_elements, int max_depth)
	: m_root_rect(root_Rect), m_free_node(-1), m_max_elements(max_elements), m_max_depth(max_depth)
{
	m_nodes.emplace(QuadNode());
}

int LQuadTree::Insert(const int id, const RectFloat& rect)
{
	QuadEltPtr qen{};

	const auto ptr	= m_elements.emplace(id, rect);
	qen.element		= ptr;

	const auto elt_ptr = m_elements_ptr.insert(qen);

	NodeInsert(0, m_root_rect.Center(), elt_ptr, rect.Center(), 1);

	return ptr;
}

bool LQuadTree::Erase(const int id, const RectFloat& rect)
{
	int leaf_node_index = FindLeaf(rect.Center());

	if (m_nodes[leaf_node_index].count == 0)
		return false;

	QuadNode& node = m_nodes[leaf_node_index];

	int* cur = &node.first_child;
	while (*cur != -1) // search through all elements
	{
		QuadEltPtr& elt_ptr = m_elements_ptr[*cur];
		QuadElt& elt = m_elements[elt_ptr.element];

		if (elt.id == id)
		{
			m_elements.erase(elt_ptr.element);
			
			int temp = *cur;
			*cur = elt_ptr.next;

			m_elements_ptr.erase(temp);

			--node.count;

			// now that item has been erased, should there be recursive loop here to update all parenting branches aabb's?
			// loop could stop if no change was detected in rect, other promising option is for user to manually call an update function
			// that cleans and updates all aabb's.

			return true;
		}

		cur = &elt_ptr.next;
	}

	return false;
}

std::vector<int> vlx::LQuadTree::Query(const RectFloat& rect)
{
	std::vector<int> result{};

	SmallVector<int, 128> to_process;
	to_process.push_back(0); // push root

	while (!to_process.empty())
	{
		QuadNode& node = m_nodes[to_process.back()];
		to_process.pop_back();

		if (!rect.Overlap(node.rect))
			continue;

		if (node.count != -1)
		{
			int child = node.first_child;
			while (child != -1) // iterate over all children
			{
				QuadEltPtr& elt_ptr = m_elements_ptr[child];
				QuadElt& elt = m_elements[elt_ptr.element];

				if (elt.rect.Overlap(rect))
					result.push_back(elt.id);

				child = elt_ptr.next;
			}
		}
		else
		{
			for (int i = 0; i < CHILD_COUNT; ++i)
				to_process.push_back(node.first_child + i);
		}
	}

	return result;
}

std::vector<int> vlx::LQuadTree::Query(const sf::Vector2f& point)
{
	std::vector<int> result{};

	SmallVector<int, 128> to_process;
	to_process.push_back(0); // push root

	while (!to_process.empty())
	{
		QuadNode& node = m_nodes[to_process.back()];
		to_process.pop_back();

		if (!node.rect.Contains(point))
			continue;

		if (node.count != -1)
		{
			int child = node.first_child;
			while (child != -1) // iterate over all children
			{
				QuadEltPtr& elt_ptr = m_elements_ptr[child];
				QuadElt& elt = m_elements[elt_ptr.element];

				if (elt.rect.Contains(point))
					result.emplace_back(elt.id);

				child = elt_ptr.next;
			}
		}
		else
		{
			for (int i = 0; i < CHILD_COUNT; ++i)
				to_process.push_back(node.first_child + i);
		}
	}

	return result;
}

void LQuadTree::Cleanup()
{
	SmallVector<int, 128> to_process;
	to_process.push_back(0); // push root

	SmallVector<RectFloat*> reg;

	while (!to_process.empty())
	{
		QuadNode& node = m_nodes[to_process.back()];
		to_process.pop_back();


	}
}

void LQuadTree::UpdateLeafAABB(const int leaf_index)
{

}

void LQuadTree::SplitLeaf(QuadNode& node)
{
	if (m_free_node != -1)
	{
		node.first_child = m_free_node;
		m_free_node = m_nodes[node.first_child].first_child;

		for (int i = 0; i < CHILD_COUNT; ++i)
			m_nodes[node.first_child + i] = {};
	}
	else
	{
		node.first_child = m_nodes.size();
		for (int i = 0; i < CHILD_COUNT; ++i)
			m_nodes.emplace();
	}
}

void LQuadTree::NodeInsert(const int node_index, const sf::Vector2f& node_center, const int ptr_index, const sf::Vector2f& elt_center, const int depth)
{
	sf::Vector2f offset
	{
		m_root_rect.Width() / (2 * (depth + 1)),
		m_root_rect.Height() / (2 * (depth + 1))
	};

	auto& node = m_nodes[node_index];

	if (node.count == -1) // branch
	{
		if (elt_center.x > node_center.x)
		{
			(elt_center.y > node_center.y) ?
				NodeInsert(node.first_child + 3, { node_center.x + offset.x, node_center.y + offset.y }, ptr_index, elt_center, depth + 1) :
				NodeInsert(node.first_child + 2, { node_center.x + offset.x, node_center.y - offset.y }, ptr_index, elt_center, depth + 1);
		}
		else
		{
			(elt_center.y > node_center.y) ?
				NodeInsert(node.first_child + 1, { node_center.x - offset.x, node_center.y + offset.y }, ptr_index, elt_center, depth + 1) :
				NodeInsert(node.first_child + 0, { node_center.x - offset.x, node_center.y - offset.y }, ptr_index, elt_center, depth + 1);
		}
	}
	else if (depth == m_max_depth || node.count < m_max_elements) // normal insert if we have reached max depth
	{
		m_elements_ptr[ptr_index].next = node.first_child;
		node.first_child = ptr_index;
		++node.count;
	}
	else // we have reached max elements, convert this to branch
	{
		node.count = -1;
		m_elements_ptr[ptr_index].next = node.first_child;

		SplitLeaf(node);

		int next{}, i = ptr_index;
		while (i != -1)
		{
			next = m_elements_ptr[i].next;
			NodeInsert(node_index, node_center, i, m_elements[m_elements_ptr[i].element].rect.Center(), depth);
			i = next;
		}

		return;
	}

	const auto& elt_ptr = m_elements_ptr[ptr_index];
	const auto& elt		= m_elements[elt_ptr.element];

	node.rect = (node.count == 1) ? elt.rect : node.rect.Union(elt.rect); // update aabb
}

int LQuadTree::FindLeaf(const sf::Vector2f& point)
{
	sf::Vector2f offset { m_root_rect.Width() / 2, m_root_rect.Height() / 2 };
	sf::Vector2f node_center = m_root_rect.Center();

	int node_index = 0, count = m_nodes[node_index].count;
	while (count == -1) // continue on branches, will exit when leaf
	{
		QuadNode& quad_node = m_nodes[node_index];
		offset /= 2.0f;

		if (point.x > node_center.x)
		{
			if (point.y > node_center.y)
			{
				node_index = quad_node.first_child + 3;
				node_center += offset;
			}
			else
			{
				node_index = quad_node.first_child + 2;
				node_center += { offset.x, -offset.y };
			}
		}
		else
		{
			if (point.y > node_center.y)
			{
				node_index = quad_node.first_child + 1;
				node_center += { -offset.x, offset.y };
			}
			else
			{
				node_index = quad_node.first_child + 0;
				node_center -= offset;
			}

		}
	}

	return node_index;
}
