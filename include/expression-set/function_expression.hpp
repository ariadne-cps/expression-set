/***************************************************************************
 *            expression-set/function_expression.hpp
 *
 *  Copyright  2008-20  Pieter Collins
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne Expression Set.
 *
 *  Ariadne Expression Set is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne Expression Set is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne Expression Set. If not, see <https://www.gnu.org/licenses/>.
 */

/** @file expression-set/function_expression.hpp
 *  @brief Function expressions and composition under coordinate projections.
 */

#ifndef ARIADNE_FUNCTION_EXPRESSION_HPP
#define ARIADNE_FUNCTION_EXPRESSION_HPP

#include "function/formula.hpp"
#include "function/function.hpp"
#include "function/function_model.hpp"
#include "function/projection.hpp"
#include "function/taylor_function.hpp"
#include "symbolic/expression.hpp"
#include "symbolic/space.hpp"

#include "expression-set/expression_set.hpp"

namespace Ariadne {

/** @brief Declare a symbolic wrapper around a function of a given signature. */
template<class P, class SIG>
class FunctionExpression;
/*
template<class P, class D> using ScalarFunctionExpression = FunctionExpression<P,D,IntervalDomainType>;
template<class P, class D> using VectorFunctionExpression = FunctionExpression<P,D,BoxDomainType>;
template<class P> using ScalarMultivariateFunctionExpression = FunctionExpression<P,BoxDomainType,IntervalDomainType>;
template<class P> using VectorMultivariateFunctionExpression = FunctionExpression<P,BoxDomainType,BoxDomainType>;

using ValidatedScalarFunctionExpression = FunctionExpression<ValidatedTag,BoxDomainType,IntervalDomainType>;
using ValidatedVectorFunctionExpression = FunctionExpression<ValidatedTag,BoxDomainType,BoxDomainType>;
*/

template<class P>
using ScalarMultivariateFunctionExpression = FunctionExpression<P, RealScalar(RealVector)>;
template<class P>
using VectorMultivariateFunctionExpression = FunctionExpression<P, RealVector(RealVector)>;

using VariableIntervalDomainType = VariableInterval<IntervalDomainType::UpperBoundType>;
using VariablesBoxDomainType = VariablesBox<IntervalDomainType>;

/** @brief Pull a box domain back through a coordinate projection. */
inline BoxDomainType preimage(const Projection& prj, const BoxDomainType& bx) {
    BoxDomainType dbx(prj.argument_size(), IntervalDomainType(-infty, +infty));
    for (SizeType i = 0; i != prj.result_size(); ++i) {
        IntervalDomainType& dbxj = dbx[prj.index(i)];
        dbxj = intersection(dbxj, bx[i]);
    }
    return dbx;
}

/** @brief Construct a named variable interval from a function domain. */
VariableIntervalDomainType _make_domain(const RealVariable& arg, const IntervalDomainType& dom) {
    return VariableIntervalDomainType(arg, dom);
}

/** @brief Convert a function box into named variable bounds. */
VariablesBoxDomainType _make_domain(const Vector<RealVariable>& args, const BoxDomainType& dom) {
    Map<RealVariable, FloatDPExactInterval> domain;
    for (SizeType i = 0; i != args.size(); ++i) {
        domain[args[i]] = dom[i];
    }
    return domain;
}

/** @brief Represent a function expression with explicitly named arguments. */
template<class P, class RES>
class FunctionExpression<P, RES(RealVariable)> {
    using ARG = RealVariable;
    using SIG = RES(ARG);
    typedef BoxDomainType D;

  public:
    template<class Y>
    using Argument = typename SignatureTraits<SIG>::template ArgumentType<Y>;
    template<class Y>
    using Result = typename SignatureTraits<SIG>::template ResultType<Y>;

  private:
    Argument<RealVariable> _vars;
    Function<P, SIG> _f;

  public:
    FunctionExpression(const Function<P, SIG>& f, const Argument<RealVariable>& vars) : _vars(vars), _f(f) {
        ARIADNE_PRECONDITION(_f.argument_size() == _vars.size());
    }
    const Function<P, SIG>& function() const { return this->_f; }
    const Argument<RealVariable>& variables() const { return this->_vars; }
    Set<RealVariable> arguments() const {
        Set<RealVariable> args;
        for (SizeType i = 0; i != _vars.size(); ++i) {
            args.insert(_vars[i]);
        }
        return args;
    }
    auto domain() const -> decltype(_make_domain(_vars, _f.domain())) { return _make_domain(_vars, _f.domain()); }
    template<class PP>
    friend OutputStream& operator<<(OutputStream& os, const FunctionExpression<PP, SIG>& sfe);
};

/** @brief Write a function expression and its named arguments. */
template<class P>
OutputStream& operator<<(OutputStream& os, const ScalarMultivariateFunctionExpression<P>& e) {
    os << e._f;
    for (SizeType i = 0; i != e._vars.size(); ++i) {
        os << (i == 0 ? '[' : ',') << "x[" << i << "]=" << e._vars[i];
    }
    return os << "]";
}

/** @brief Bind real variables to the input coordinates of a scalar function. */
template<class P>
ScalarMultivariateFunctionExpression<P> evaluate(const ScalarMultivariateFunction<P>& f, const Vector<RealVariable>& vars) {
    return ScalarMultivariateFunctionExpression<P>(f, vars);
}

/** @brief Compose a Taylor model with a coordinate projection. */
template<class F>
TaylorModel<ValidatedTag, F> compose(const TaylorModel<ValidatedTag, F>& x, const Projection& prj) {
    TaylorModel<ValidatedTag, F> r(prj.argument_size(), x.sweeper());
    r.expansion().reserve(x.number_of_nonzeros());
    MultiIndex ra(r.argument_size());
    for (auto xiter = x.begin(); xiter != x.end(); ++xiter) {
        UniformConstReference<MultiIndex> xa = xiter->index();
        UniformConstReference<F> xc = xiter->coefficient();
        for (SizeType i = 0; i != r.argument_size(); ++i) {
            ra[i] = xa[prj.index(i)];
        }
        r.expansion().append(ra, xc);
    }
    r.cleanup();
    return r;
}

/** @brief Compose a scaled function patch with a coordinate projection. */
template<class M>
ScaledFunctionPatch<M> compose(const ScaledFunctionPatch<M>& f, const Projection& prj) {
    auto f_dom = f.domain();
    BoxDomainType dom = preimage(prj, f_dom);

    bool has_strict_subdomain = false;
    for (SizeType i = 0; i != prj.result_size(); ++i) {
        if (not same(f_dom[i], dom[prj.index(i)])) {
            has_strict_subdomain = true;
        }
    }
    if (has_strict_subdomain) {
        Vector<ScaledFunctionPatch<M>> id = f.create_coordinates(dom);
        return compose(f, prj(id));
    } else {
        return ScaledFunctionPatch<M>(dom, compose(f.model(), prj));
    }
}

/** @brief Compose a scalar function model with a coordinate projection. */
template<class P, class PR, class PRE>
ScalarFunctionModel<P, PR, PRE> compose(const ScalarFunctionModel<P, PR, PRE>& f, const Projection& prj) {
    auto fpp = std::dynamic_pointer_cast<const ValidatedScalarMultivariateTaylorFunctionModelDP>(f.managed_pointer());
    if (fpp) {
        return compose(ValidatedScalarMultivariateTaylorFunctionModelDP(fpp), prj);
    }
    auto f_dom = f.domain();
    BoxDomainType dom = preimage(prj, f_dom);
    Vector<ScalarFunctionModel<P, PR, PRE>> id = f.create_coordinates(dom);
    Vector<ScalarFunctionModel<P, PR, PRE>> pid = prj(id);
    VectorFunctionModel<P, PR, PRE> vpid(pid.array());
    return compose(f, vpid);
}

template<class P>
ScalarFunction<P, BoxDomainType> compose(const ScalarFunction<P, BoxDomainType>& f, const Projection& prj) {
    auto fmp = std::dynamic_pointer_cast<const typename ScalarMultivariateFunctionModelDP<P>::Interface>(f.managed_pointer());
    if (fmp) {
        return compose(ScalarMultivariateFunctionModelDP<P>(fmp), prj);
    }
    auto f_dom = f.domain();
    BoxDomainType dom = preimage(prj, f_dom);
    Vector<ScalarFunction<P, BoxDomainType>> id(ScalarFunction<P, BoxDomainType>::coordinates(dom));
    VectorFunction<P, BoxDomainType> pid = prj(id);
    return compose(f, pid);
}

template<class P>
ScalarFunction<P, EuclideanDomain> compose(const ScalarFunction<P, EuclideanDomain>& f, const Projection& prj) {
    auto f_dom = f.domain();
    BoxDomainType dom = preimage(prj, f_dom);
    Vector<ScalarFunction<P, EuclideanDomain>> id(ScalarFunction<P, EuclideanDomain>::coordinates(dom));
    VectorFunction<P, EuclideanDomain> pid = prj(id);
    return compose(f, pid);
}

RealScalarMultivariateFunction compose(const RealScalarMultivariateFunction& f, const Projection& prj) {
    EuclideanDomain dom(prj.argument_size());
    Vector<RealScalarMultivariateFunction> id(RealScalarMultivariateFunction::coordinates(dom));
    RealVectorMultivariateFunction pid = prj(id);
    return compose(f, pid);
}

/** @brief Rebuild a scalar multivariate function in the specified variable order. */
template<class P>
ScalarMultivariateFunction<P> make_function(RealSpace args, const ScalarMultivariateFunctionExpression<P>& expr) {
    ARIADNE_PRECONDITION(subset(expr.arguments(), args.variables()));
    auto expr_vars = expr.variables();
    auto expr_func = expr.function();

    Array<SizeType> indices(expr_vars.size());
    for (SizeType i = 0; i != expr_vars.size(); ++i) {
        indices[i] = args.index(expr_vars[i]);
    }
    Projection projection(args.size(), indices);

    return compose(expr_func, projection);
}

} // namespace Ariadne

#endif /* ARIADNE_FUNCTION_EXPRESSION_HPP */
