
/*
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file mcbasket.cpp
    \brief simple example of multi-factor Monte Carlo pricer

    \fullpath
    ql/Pricers/%mcbasket.cpp
*/

// $Id$

#include <ql/handle.hpp>
#include <ql/MonteCarlo/basketpathpricer.hpp>
#include <ql/MonteCarlo/mctypedefs.hpp>
#include <ql/Pricers/mcbasket.hpp>

namespace QuantLib {

    namespace Pricers {

        using MonteCarlo::PathPricer_old;
        using MonteCarlo::MultiPath;
        using MonteCarlo::GaussianMultiPathGenerator;
        using MonteCarlo::BasketPathPricer_old;
        using MonteCarlo::MultiFactorMonteCarloOption;
        using MonteCarlo::MonteCarloModel;
        using Math::Statistics;

        McBasket::McBasket(Option::Type type, const Array& underlying,
            double strike, const Array& dividendYield,
            const Math::Matrix& covariance,
            Rate riskFreeRate,  double residualTime,
            bool antitheticVariance, long seed) {

            QL_REQUIRE(covariance.rows() == covariance.columns(),
                "McBasket: covariance matrix not square");
            QL_REQUIRE(covariance.rows() == underlying.size(),
                "McBasket: underlying size does not match that of"
                " covariance matrix");
            QL_REQUIRE(covariance.rows() == dividendYield.size(),
                "McBasket: dividendYield size does not match"
                " that of covariance matrix");
            QL_REQUIRE(residualTime > 0,
                "McBasket: residual time must be positive");

            //! Initialize the path generator
            Array mu(riskFreeRate - dividendYield
                - 0.5 * covariance.diagonal());

            Handle<GaussianMultiPathGenerator> pathGenerator(
                new GaussianMultiPathGenerator(mu, covariance,
                std::vector<Time>(1, residualTime), seed));

            //! Initialize the pricer on the path pricer
            Handle<PathPricer_old<MultiPath> > pathPricer(new BasketPathPricer_old(
                type, underlying, strike,
                QL_EXP(-riskFreeRate*residualTime),
                antitheticVariance));

             //! Initialize the multi-factor Monte Carlo
            mcModel_ = Handle<MonteCarloModel<Statistics,
                GaussianMultiPathGenerator,
                PathPricer_old<MultiPath> > > (
                new MonteCarloModel<Math::Statistics,
                GaussianMultiPathGenerator,
                PathPricer_old<MultiPath> > (pathGenerator,
                pathPricer, Math::Statistics()));

        }

    }

}
