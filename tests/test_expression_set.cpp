/***************************************************************************
 *            test_expression_set.cpp
 *
 *  Copyright  2009-21  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Ariadne.
 *
 *  Ariadne is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ariadne is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ariadne.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include "utility/container.hpp"
#include "utility/stlio.hpp"
#include "numeric/numeric.hpp"
#include "expression-set/expression_set.hpp"
#include "symbolic/templates.hpp"
#include "symbolic/assignment.hpp"
#include "symbolic/valuation.hpp"
#include "symbolic/space.hpp"
#include "function/formula.hpp"
#include "set/function_set.hpp"
#include "algebra/algebra.hpp"
#include "function/function.hpp"
#include "logging/logging.hpp"

#include "ariadne-testing.hpp"

using namespace Ariadne;

using RealVariablesPoint = VariablesPoint<Real>;

class TestExpressionSet : public Ariadne::Testing::ContextFixture {
  private:
    RealVariable x, y, z;

  public:
    explicit TestExpressionSet(Ariadne::Testing::Context& context)
        : ContextFixture(context), x("x"), y("y"), z("z") { }

    Void test_variables_point() {
        Map<RealVariable,Real> bindings;
        bindings.insert(x,0.1_dec);
        bindings.insert(y,-1);
        RealVariablesPoint p(bindings);
        ARIADNE_TESTING_EXPECT_EQUAL(p.variables().size(),2);
        ARIADNE_TESTING_EXPECT_EQUAL(p.euclidean_set(RealSpace({x,y})).dimension(),2);
        ARIADNE_TESTING_EXPECT_EXCEPTION(p.euclidean_set(RealSpace({x,z})));

    }

    Void test_template_streaming() {
        BoundedConstraintSet bounded_set(RealBox({{-1,1}}));
        ExactBoxType exact_box({ExactIntervalType(-1,1)});
        Symbolic<Separated,BoundedConstraintSet,ExactBoxType> separated_symbolic(
            Separated(),bounded_set,exact_box);
        std::ostringstream separated_stream;
        separated_stream << separated_symbolic;
        ARIADNE_TESTING_PROVE(not separated_stream.str().empty());
    }

    Void test_variables_box() {
        Real real_upper(2);
        auto real_upper_interval = real_upper >= x;
        ARIADNE_TESTING_PROVE(real_upper_interval.variable()==x);
        ARIADNE_TESTING_EXPECT_SAME(real_upper_interval.upper_bound(),real_upper);

        Real real_value(1);
        auto variable_equal_real = x == real_value;
        auto real_equal_variable = real_value == x;
        ARIADNE_TESTING_PROVE(variable_equal_real.variable()==x);
        ARIADNE_TESTING_EXPECT_SAME(variable_equal_real.lower_bound(),real_value);
        ARIADNE_TESTING_EXPECT_SAME(variable_equal_real.upper_bound(),real_value);
        ARIADNE_TESTING_PROVE(real_equal_variable.variable()==x);
        ARIADNE_TESTING_EXPECT_SAME(real_equal_variable.lower_bound(),real_value);
        ARIADNE_TESTING_EXPECT_SAME(real_equal_variable.upper_bound(),real_value);

        FloatDP float_lower(0,dp);
        auto float_lower_interval = float_lower <= x;
        ARIADNE_TESTING_PROVE(float_lower_interval.variable()==x);
        ARIADNE_TESTING_EXPECT_EQUAL(float_lower_interval.lower_bound(),float_lower);

        Dyadic dyadic_lower(1);
        auto dyadic_lower_interval = dyadic_lower <= x;
        ARIADNE_TESTING_PROVE(dyadic_lower_interval.variable()==x);
        ARIADNE_TESTING_EXPECT_EQUAL(dyadic_lower_interval.lower_bound(),dyadic_lower);

        double double_lower=0.25;
        auto double_lower_interval = double_lower <= x;
        ARIADNE_TESTING_PROVE(double_lower_interval.variable()==x);
        ARIADNE_TESTING_EXPECT_EQUAL(double_lower_interval.lower_bound(),double_lower);

        RealVariablesBox bx1 = {0<=x<=1.2_dec};
        ARIADNE_TESTING_PROVE(not bx1.is_empty());
        ARIADNE_TESTING_NOTE("bx1 = " << (bx1));

        RealVariablesBox empty_box(
            RealSpace({x}),RealBox({RealInterval(EmptyInterval())}));
        ARIADNE_TESTING_PROVE(empty_box.is_empty());

        ARIADNE_TESTING_EXPECT_EXCEPTION((void)RealVariablesBox(
            RealSpace({x,y}),RealBox({{0.1_dec,0.5_dec}})));

        RealVariablesBox bx2(RealSpace({x,y}),RealBox({{0.1_dec,0.5_dec},{-2.3_dec,1}}));
        ARIADNE_TESTING_EXPECT_EQUAL(bx2.variables().size(),2);
        ARIADNE_TESTING_NOTE("bx2 = " << (bx2));


        ARIADNE_TESTING_EXPECT_EQUAL(variables(bx2.bounds()).size(),2);

        ARIADNE_TESTING_NOTE("over_approximation(bx2) = " << (over_approximation(bx2)));
        ARIADNE_TESTING_NOTE("approximation(bx2) = " << (approximation(bx2)));
        ARIADNE_TESTING_NOTE("under_approximation(bx2) = " << (under_approximation(bx2)));

        ARIADNE_TESTING_NOTE("Executing: make_set(RealSpace({y,x}),bx2)"); make_set(RealSpace({y,x}),bx2);
        ARIADNE_TESTING_NOTE("Executing: make_box(RealSpace({x}),bx2)"); make_box(RealSpace({x}),bx2);
        ARIADNE_TESTING_EXPECT_EXCEPTION(make_box(RealSpace({x,z}),bx2));
    }

    Void test_real_expression_constraint_set() {
        RealExpressionConstraintSet set({y >= 0, z + y <= 2, sqr(z) + sqr(y) >= 0});
        ARIADNE_TESTING_NOTE("set = " << (set));
        ARIADNE_TESTING_EXPECT_EQUAL(set.variables().size(), 2);
        ARIADNE_TESTING_EXPECT_EQUAL(set.constraints().size(), 3);
        ARIADNE_TESTING_EXPECT_EXCEPTION(set.euclidean_set(RealSpace({y})));
        ARIADNE_TESTING_EXPECT_EXCEPTION(set.euclidean_set(RealSpace({x})));
        ARIADNE_TESTING_NOTE("Executing: set.euclidean_set(RealSpace({y, z}))"); set.euclidean_set(RealSpace({y, z}));
        ARIADNE_TESTING_NOTE("Executing: make_set(RealSpace({z, y}),set)"); make_set(RealSpace({z, y}),set);
    }

    Void test_real_expression_bounded_constraint_set() {
        RealExpressionConstraintSet unbounded_set({y>=0,z+y<=2,sqr(z)+sqr(y)>=0});
        ARIADNE_TESTING_NOTE("unbounded_set = " << (unbounded_set));
        RealVariable u("u");

        List<RealVariableInterval> bounds_list={0.1_dec<=y<=0.5_dec,-2.3_dec<=z<=1};
        List<ContinuousPredicate> constraints_list={y>=0,z+y<=2};
        ARIADNE_TESTING_EXPECT_EQUAL(variables(bounds_list).size(),2u);

        RealExpressionBoundedConstraintSet from_initializer({0.1_dec<=y<=0.5_dec,-2.3_dec<=z<=1});
        ARIADNE_TESTING_EXPECT_EQUAL(from_initializer.variables().size(),2u);

        RealExpressionBoundedConstraintSet from_initializer_constraints(
            {0.1_dec<=y<=0.5_dec,-2.3_dec<=z<=1},{y>=0,z+y<=2});
        ARIADNE_TESTING_EXPECT_EQUAL(from_initializer_constraints.constraints().size(),2u);

        RealExpressionBoundedConstraintSet from_list(bounds_list);
        ARIADNE_TESTING_EXPECT_EQUAL(from_list.bounds().size(),2u);

        RealExpressionBoundedConstraintSet from_list_constraints(bounds_list,constraints_list);
        ARIADNE_TESTING_EXPECT_EQUAL(from_list_constraints.constraints().size(),2u);

        Map<RealVariable,RealInterval> bounds_map=RealVariablesBox(bounds_list).bounds();
        RealExpressionBoundedConstraintSet from_map(bounds_map);
        ARIADNE_TESTING_EXPECT_EQUAL(from_map.variables().size(),2u);

        RealVariablesBox bounds_box(bounds_list);
        RealExpressionBoundedConstraintSet from_box(bounds_box);
        ARIADNE_TESTING_EXPECT_EQUAL(from_box.variables().size(),2u);

        List<RealVariableInterval> duplicate_bounds={0.1_dec<=y<=0.5_dec,-1<=y<=1};
        ARIADNE_TESTING_EXPECT_EXCEPTION((void)RealExpressionBoundedConstraintSet(duplicate_bounds));
        List<ContinuousPredicate> duplicate_compatible_constraints={y>=0};
        ARIADNE_TESTING_EXPECT_EXCEPTION((void)RealExpressionBoundedConstraintSet(duplicate_bounds,duplicate_compatible_constraints));
        ARIADNE_TESTING_EXPECT_EXCEPTION(RealExpressionBoundedConstraintSet(RealVariablesBox({0.1_dec<=x<=0.5_dec,-2.3_dec<=u<=1}),unbounded_set));
        ARIADNE_TESTING_NOTE("Executing: intersection(RealVariablesBox({0.1_dec<=z<=0.5_dec,-2.3_dec<=y<=1}),unbounded_set)"); intersection(RealVariablesBox({0.1_dec<=z<=0.5_dec,-2.3_dec<=y<=1}),unbounded_set);
        ARIADNE_TESTING_NOTE("Executing: make_set(RealSpace({y,z}),RealVariablesBox({0.1_dec<=z<=0.5_dec,-2.3_dec<=y<=1}),unbounded_set)"); make_set(RealSpace({y,z}),RealVariablesBox({0.1_dec<=z<=0.5_dec,-2.3_dec<=y<=1}),unbounded_set);

        RealExpressionBoundedConstraintSet set(RealVariablesBox({0.1_dec<=y<=0.5_dec,-2.3_dec<=z<=1}),unbounded_set);
        ARIADNE_TESTING_NOTE("set = " << (set));
        ARIADNE_TESTING_EXPECT_EXCEPTION(approximate_euclidean_set(set,RealSpace({x})));
        ARIADNE_TESTING_NOTE("Executing: approximate_euclidean_set(set,RealSpace({y,z}))"); approximate_euclidean_set(set,RealSpace({y,z}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(approximate_euclidean_set(set,RealSpace({x,y,z})));
        ARIADNE_TESTING_EXPECT_EXCEPTION(make_set(RealSpace({y,z,u}),set));

        RealExpressionBoundedConstraintSet set2(RealVariablesBox({0.3_dec<=x<=1,0.1_dec<=y<=0.5_dec,-2.3_dec<=z<=1}),unbounded_set);
        ARIADNE_TESTING_NOTE("set2 = " << (set2));
        ARIADNE_TESTING_EXPECT_EXCEPTION(approximate_euclidean_set(set2,RealSpace({x})));
        ARIADNE_TESTING_NOTE("Executing: approximate_euclidean_set(set2,RealSpace({y,z}))"); approximate_euclidean_set(set2,RealSpace({y,z}));
        ARIADNE_TESTING_NOTE("Executing: approximate_euclidean_set(set2,RealSpace({x,y,z}))"); approximate_euclidean_set(set2,RealSpace({x,y,z}));
        ARIADNE_TESTING_EXPECT_EXCEPTION(make_set(RealSpace({y,z,u}),set2));
    }
};

int main(int argc, char** argv)
{
    // Keep solver logging available without bypassing the test reporter.
#ifdef _WIN32
    Logger::instance().redirect_to_file("NUL");
#else
    Logger::instance().redirect_to_file("/dev/null");
#endif
    ARIADNE_TESTING_SUITE("test_expression_set");
    ARIADNE_TESTING_ADD_TEST(TestExpressionSet, test_variables_point);
    ARIADNE_TESTING_ADD_TEST(TestExpressionSet, test_template_streaming);
    ARIADNE_TESTING_ADD_TEST(TestExpressionSet, test_variables_box);
    ARIADNE_TESTING_ADD_TEST(TestExpressionSet, test_real_expression_constraint_set);
    ARIADNE_TESTING_ADD_TEST(TestExpressionSet, test_real_expression_bounded_constraint_set);
    return ARIADNE_TESTING_RUN_TESTS_WITH_ARGS(argc, argv);
}
