//
// Created by max on 5/5/16.
//

#include "ss_tree.hpp"

using namespace std;

int librnary::Surface::PairI() const {
	return tree->PairI(id);
}
int librnary::Surface::PairJ() const {
	return tree->PairJ(id);
}

librnary::Surface librnary::Surface::Parent() const {
	return Surface(tree, tree->Parent(id));
}

vector<librnary::Surface> librnary::Surface::Children() const {
	vector<Surface> subsurfaces;
	for (const auto cid : tree->adj_list[id]) {
		subsurfaces.push_back(Surface(tree, cid));
	}
	return subsurfaces;
}

int librnary::Surface::NumChildren() const {
	return tree->NumChildren(id);
}

librnary::Surface librnary::Surface::Child(int idx) const {
	return Surface(tree, tree->Child(id, idx));
}

bool librnary::Surface::IsExternalLoop() const {
	return tree->IsExternalLoop(id);
}

int librnary::Surface::Unpaired() const {
	return tree->Unpaired(id);
}

bool librnary::Surface::operator==(const Surface &rhs) const {
	return rhs.id == id && rhs.tree == tree;
}

void librnary::SSTree::Construct(const Matching &matching, int i, int j) {
	unsigned num_pairings = 1; // Start at one to count i,j
	for (int k = 0; k < static_cast<int>(matching.size()); ++k) {
		if (k < matching[k]) {
			++num_pairings;
		}
	}
	next_id = RootId();
	adj_list.assign(num_pairings, std::vector<SSTreeNodeId>());
	parents.assign(num_pairings, next_id);
	bond_pairs.assign(num_pairings, BondPair());
	Construct(matching, next_id++, i, j);
}

void librnary::SSTree::Construct(const Matching &matching, SSTreeNodeId curr, int i, int j) {
	bond_pairs[curr].i = i;
	bond_pairs[curr].j = j;
	for (int k = i + 1; k < j; ++k) {
		if (matching[k] != k) { // k is paired.
			SSTreeNodeId child_id = next_id++;
			adj_list[curr].push_back(child_id);
			parents[child_id] = curr;
			Construct(matching, child_id, k, matching[k]);
			k = matching[k];
		}
	}
}

librnary::SSTree::SSTree(const Matching &matching) {
	Construct(matching, -1, static_cast<int>(matching.size()));
}

int librnary::SSTree::Unpaired(SSTreeNodeId node_id) const {
	int unpaired = PairJ(node_id) - PairI(node_id) - 1;
	for (auto child_id : adj_list[node_id]) {
		unpaired -= PairJ(child_id) - PairI(child_id) + 1;
	}
	return unpaired;
}

bool librnary::SSTree::IsExternalLoop(SSTreeNodeId node_id) const {
	return bond_pairs[node_id].i == -1;
}

int librnary::SSTree::NumChildren(SSTreeNodeId node_id) const {
	return static_cast<int>(adj_list[node_id].size());
}

std::vector<librnary::SSTreeNodeId> &librnary::SSTree::Children(librnary::SSTreeNodeId node_id) {
	return adj_list[node_id];
}

librnary::SSTreeNodeId librnary::SSTree::Child(SSTreeNodeId node_id, int child) const {
	return adj_list[node_id][child];
}

int librnary::SSTree::PairI(SSTreeNodeId node_id) const {
	return bond_pairs[node_id].i;
}

int librnary::SSTree::PairJ(SSTreeNodeId node_id) const {
	return bond_pairs[node_id].j;
}

librnary::SSTreeNodeId librnary::SSTree::RootId() const {
	return 0;
}

librnary::Surface librnary::SSTree::RootSurface() const {
	return Surface(this, RootId());
}
librnary::Surface librnary::SSTree::GetSurface(SSTreeNodeId id) const {
	return Surface(this, id);
}

librnary::SSTreeNodeId librnary::SSTree::Parent(SSTreeNodeId node_id) const {
	return parents[node_id];
}

int librnary::SSTree::NumNodes() const {
	return next_id;
}