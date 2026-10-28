#include "pybind11.hpp"
#include "function-utilities.hpp"
#include "numeric_submodule.hpp"
#include "symbolic/expression.hpp"
#include "symbolic/space.hpp"
#include "expression-set/expression_set.hpp"
#include "set/function_set.hpp"
#include "function/function.hpp"
using namespace Ariadne;

Void export_sets(pybind11::module& module)
{
    auto real_variable_class = pybind11::reinterpret_borrow<pybind11::class_<RealVariable>>(module.attr("RealVariable"));
    pybind11::class_<RealVariableLowerInterval> real_variable_lower_interval_class(module,"RealVariableLowerInterval");
    real_variable_lower_interval_class.def(pybind11::init<Real,RealVariable>());
    real_variable_lower_interval_class.def("__str__",&__cstr__<RealVariableLowerInterval>);
    real_variable_lower_interval_class.def("__bool__", [](RealVariableLowerInterval const&){ARIADNE_THROW(std::runtime_error,"RealVariableLowerInterval.__bool__(self)","Cannot convert RealVariableLowerInterval to bool");});

    pybind11::class_<RealVariableUpperInterval> real_variable_upper_interval_class(module,"RealVariableUpperInterval");
    real_variable_upper_interval_class.def(pybind11::init<RealVariable,Real>());
    real_variable_upper_interval_class.def("__str__",&__cstr__<RealVariableUpperInterval>);
    real_variable_upper_interval_class.def("__bool__", [](RealVariableUpperInterval const&){ARIADNE_THROW(std::runtime_error,"RealVariableUpperInterval.__bool__(self)","Cannot convert RealVariableUpperInterval to bool");});

    real_variable_class.def("__le__", &__le__<Real,RealVariable , Return<RealVariableLowerInterval> >);
    real_variable_class.def("__ge__", &__ge__<RealVariable,Real , Return<RealVariableLowerInterval> >);
    real_variable_class.def("__le__", &__le__<RealVariable,Real , Return<RealVariableUpperInterval> >);
    real_variable_class.def("__or__", [](RealVariable const& x, RealInterval ivl){return RealVariableInterval(x,ivl);});
    real_variable_class.def("__lshift__",&__eq__<RealVariable,Real,Return<RealVariableInterval>>);

    real_variable_lower_interval_class.def("__le__", &__le__<RealVariableLowerInterval,Real , Return<RealVariableInterval> >);
//    real_variable_upper_interval_class.def("__le__", &__le__<Real,RealVariableUpperInterval , Return<RealVariableInterval> >);
    real_variable_upper_interval_class.def("__ge__", &__ge__<RealVariableUpperInterval,Real , Return<RealVariableInterval> >);


    // NOTE: This syntax allows creating a variable-interval using 'l<=x & x<=u'
    // It does not allow allows 'l<=x<=u' to define a variable-interval
    // since the Python short-circuit converts to  '(l<=x) and (x<=u)'
    real_variable_lower_interval_class.def("__and__",
        [] (RealVariableLowerInterval livl,RealVariableUpperInterval uivl) {
            assert(livl.variable()==uivl.variable());
            return RealVariableInterval(livl.lower_bound(),livl.variable(),uivl.upper_bound());});


    pybind11::class_<RealVariableInterval> real_variable_interval_class(module,"RealVariableInterval");
    real_variable_interval_class.def(pybind11::init<RealVariable,RealInterval>());
    real_variable_interval_class.def(pybind11::init<Real,RealVariable,Real>());
    real_variable_interval_class.def("variable", &RealVariableInterval::variable);
    real_variable_interval_class.def("interval", &RealVariableInterval::interval);
    real_variable_interval_class.def("lower", &RealVariableInterval::lower_bound);
    real_variable_interval_class.def("upper", &RealVariableInterval::upper_bound);
    real_variable_interval_class.def("__str__",&__cstr__<RealVariableInterval>);

    pybind11::class_<RealVariablesBox> real_variables_box_class(module,"RealVariablesBox");
    real_variables_box_class.def(pybind11::init([](pybind11::dict dct){return RealVariablesBox(pybind11::cast<Map<RealVariable,RealInterval>>(dct));}));
    real_variables_box_class.def(pybind11::init<Map<RealVariable,RealInterval>>());
    real_variables_box_class.def(pybind11::init<List<RealVariableInterval>>());
    real_variables_box_class.def("size", [](RealVariablesBox const& box){ return box.variables().size(); });
    real_variables_box_class.def("__len__", [](RealVariablesBox const& box){ return box.variables().size(); });
    real_variables_box_class.def("is_empty", &RealVariablesBox::is_empty);
    real_variables_box_class.def("euclidean_set", &RealVariablesBox::euclidean_set);
    real_variables_box_class.def("__getitem__", &RealVariablesBox::operator[]);
    real_variables_box_class.def("__str__",&__cstr__<RealVariablesBox>);
    pybind11::implicitly_convertible<pybind11::dict,RealVariablesBox>();

    typedef VariablesBox<FloatDPUpperInterval> FloatDPVariablesBox;
    pybind11::class_<FloatDPVariablesBox> floatdp_variables_box_class(module,"FloatDPVariablesBox");
    floatdp_variables_box_class.def("__getitem__", &FloatDPVariablesBox::operator[]);
    floatdp_variables_box_class.def("__str__",&__cstr__<FloatDPVariablesBox>);

    pybind11::class_<RealExpressionConstraintSet> real_expression_constraint_set_class(module,"RealExpressionConstraintSet");
    real_expression_constraint_set_class.def(pybind11::init<List<ContinuousPredicate>>());
    real_expression_constraint_set_class.def("number_of_variables", [](RealExpressionConstraintSet const& set){ return set.variables().size(); });
    real_expression_constraint_set_class.def("number_of_constraints", [](RealExpressionConstraintSet const& set){ return set.constraints().size(); });
    real_expression_constraint_set_class.def("euclidean_set", &RealExpressionConstraintSet::euclidean_set);
    real_expression_constraint_set_class.def("__str__",&__cstr__<RealExpressionConstraintSet>);

    pybind11::class_<RealExpressionBoundedConstraintSet> real_expression_bounded_constraint_set_class(module,"RealExpressionBoundedConstraintSet");
    real_expression_bounded_constraint_set_class.def(pybind11::init<List<RealVariableInterval>,List<ContinuousPredicate>>());
    real_expression_bounded_constraint_set_class.def(pybind11::init<RealVariablesBox,RealExpressionConstraintSet>());
    real_expression_bounded_constraint_set_class.def("number_of_variables", [](RealExpressionBoundedConstraintSet const& set){ return set.variables().size(); });
    real_expression_bounded_constraint_set_class.def("number_of_constraints", [](RealExpressionBoundedConstraintSet const& set){ return set.constraints().size(); });
    real_expression_bounded_constraint_set_class.def("euclidean_set", (BoundedConstraintSet(RealExpressionBoundedConstraintSet::*)(RealSpace const&)) &RealExpressionBoundedConstraintSet::euclidean_set);
    real_expression_bounded_constraint_set_class.def("__str__",&__cstr__<RealExpressionBoundedConstraintSet>);

    module.def("intersection", [](RealVariablesBox const& bx, RealExpressionConstraintSet const& cs){return intersection(bx,cs);});

    module.def("make_box", (RealBox(*)(RealSpace const&, RealVariablesBox const&)) &make_box);
    module.def("make_set", (RealBox(*)(RealSpace const&, RealVariablesBox const&)) &make_box);
    module.def("make_set", (ConstraintSet(*)(RealSpace const&, RealExpressionConstraintSet const&)) &make_set);
    module.def("make_set", (BoundedConstraintSet(*)(RealSpace const&, RealExpressionBoundedConstraintSet const&)) &make_set);
    module.def("make_set", (BoundedConstraintSet(*)(RealSpace const&, RealVariablesBox const&, RealExpressionConstraintSet const&)) &make_set);
    module.def("approximate_euclidean_set", &approximate_euclidean_set);

    pybind11::implicitly_convertible<Pair<Real,Real>,RealInterval>();
    pybind11::implicitly_convertible<std::map<RealVariable,RealInterval>,RealVariablesBox>();
//    pybind11::implicitly_convertible<List<RealVariableInterval>,RealVariablesBox>();
    pybind11::implicitly_convertible<List<ContinuousPredicate>,RealExpressionConstraintSet>();
}


Void expression_set_submodule(pybind11::module& module) { export_sets(module); }
