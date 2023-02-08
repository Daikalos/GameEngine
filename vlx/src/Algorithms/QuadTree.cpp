#include <Velox/Algorithms/QuadTree.hpp>

using namespace vlx;

QuadTree::QuadTree(const sf::IntRect& root_Rect, int max_elements, int max_depth)
	: m_root_rect(root_Rect), m_free_node(-1), m_max_elements(max_elements), m_max_depth(max_depth)
{
	m_nodes.Emplace(QuadNode());
}

void QuadTree::Insert(const QuadEltNode& elt)
{
	//m_quads.emplace_back(id, sf::IntRect(
	//	{ (int)std::floor(rect.left), (int)std::floor(rect.top) },
	//	{ (int)std::floor(rect.width), (int)std::floor(rect.height) }));


}

void QuadTree::NodeInsert(int index, int depth, const sf::Vector2i& centre, const sf::Vector2i& half_size, int element)
{
	//const QuadElt& quad_elt = m_quads.at(element);

	//const int lft = quad_elt.rect.left;
	//const int top = quad_elt.rect.top;
	//const int rgt = lft + quad_elt.rect.width;
	//const int btm = top + quad_elt.rect.height;

}

std::vector<int> QuadTree::FindLeaves(int node, int depth, const sf::Vector2i& centre, const sf::Vector2i& half_size, const sf::IntRect& rect)
{
	return std::vector<int>();
}
