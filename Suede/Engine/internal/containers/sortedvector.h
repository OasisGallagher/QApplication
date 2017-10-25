#pragma once
#include <vector>
#include <algorithm>

template <class Ty, class Comp = std::less<Ty> > 
class sorted_vector {
public:
	typedef Ty value_type;
	typedef Comp comparer_type;

private:
	typedef std::vector<value_type> _Cont;

public:
	void insert(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container.begin(), container.end(), value, comp_);
		AssertX(ite == container.end() || comp_(*ite, value), "duplicate key");

		container_.insert(ite, value);
	}

	void remove(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		if (!comp(value, *ite)) {
			container_.erase(ite);
		}
	}

	void contains(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		return ite != container_.end() && !comp(value, *ite);
	}

private:
	_Cont container_;
	comparer_type comp_;
};
