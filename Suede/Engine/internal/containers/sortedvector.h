#pragma once
#include <vector>
#include <algorithm>

template <class Ty, class Comp = std::less<Ty> > 
class sorted_vector {
public:

	typedef Ty value_type;
	typedef Comp comparer_type;

	typedef std::vector<value_type> _Cont;
	typedef typename _Cont::iterator iterator;
	typedef typename _Cont::const_iterator const_iterator;
	typedef typename _Cont::reference reference;
	typedef typename _Cont::const_reference const_reference;

public:
	void insert(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		if (ite != container_.end() && !comp_(*ite, value)) {
			*ite = value;
		}
		else {
			container_.insert(ite, value);
		}
	}

	iterator find(const value_type& value) {
		return std::lower_bound(container_.begin(), container_.end(), value, comp_);
	}

	bool get(value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		if (ite == container_.end()) { return false; }
		value = *ite;
		return true;
	}

	void remove(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		if (!comp_(value, *ite)) {
			container_.erase(ite);
		}
	}

	void contains(const value_type& value) {
		_Cont::iterator ite = std::lower_bound(container_.begin(), container_.end(), value, comp_);
		return ite != container_.end() && !comp(value, *ite);
	}

	iterator begin() { return container_.begin(); }
	iterator end() { return container_.end(); }

	size_t size() const { return container_.size(); }

	reference back() { return container_.back(); }
	reference front() { return container_.front(); }

	reference operator[] (size_t i) { return container_[i]; }

private:
	_Cont container_;
	comparer_type comp_;
};
