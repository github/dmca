#ifndef _SCOPE_HPP
#define _SCOPE_HPP

class _Scope {};

template <typename Function_> class Scope : public _Scope {
  private:
	Function_ function_;

  public:
	Scope(const Function_ &function) : function_(function) {
	}

	~Scope() {
		function_();
	}
};

template <typename Function_> Scope<Function_> _scope(const Function_ &function) {
	return Scope<Function_>(function);
}

#define _scope__(counter, function) __attribute__((__unused__)) const _Scope &_scope##counter(_scope([&] function))
#define _scope_(counter, function) _scope__(counter, function)
#define _scope(function) _scope_(__COUNTER__, function)

#endif
