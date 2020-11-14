#ifndef _TREE_PRICE_STRATEGY_H_
#define _TREE_PRICE_STRATEGY_H_

class TreePriceStrategy {
	virtual double operator() (const TreeGenerator& treeGenerator, double pUp, double pDown) const = 0;
};

#endif